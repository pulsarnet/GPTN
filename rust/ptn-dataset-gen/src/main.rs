// Generate dataset for ptn:
//
// File structure:
// - data/
//   - {number}.txt
// - labels.csv
//
// {number}.txt format:
// n m - n is the number of lines, m is the number of columns of matrix on the next line
// {matrix} - one line representation of n x m matrix (possible values: 0, 1, -1)
// {vector} - one line representation of n x 1 vector (possible values: 0, 1)
//
// labels.csv format:
// file_name,k-boundary,safe,boundary,1-conservative,conservative,active,stable
// - file_name - name of the file in data/ folder
// other values are 0 or 1 is labels (1 - true, 0 - false)
//
//
// Generate algorithm:
// for k samples
//  random n number (1 <= n <= 32)
//  random m number (1 <= m <= 32)
//  generate matrix (n x m) with 0, 1, -1 values
//  generate vector (n x 1) with 0, 1 values
//  create PetriNet from matrix and vector of initial marking
//  generate ReachabilityTree from PetriNet
//  generate labels from ReachabilityTree
//  save matrix and vector to file
//  add row to labels.csv


mod semaphore;

use std::io::Write;
use std::sync::{Arc, RwLock};
use std::sync::atomic::{AtomicUsize, Ordering};
use rand::Rng;
use ptn::net::PetriNet;
use ptn::alg::DMatrix;
use ptn::modules::reachability::{CovType, Reachability, ReachabilityTree};

const DIR: &str = "train";
const SAMPLES_DIR: &str = "samples";
const LABELS_FILE: &str = "labels.csv";

const SAMPLES: usize = 10000;
const REQUIRED_K_BOUNDARY: usize = SAMPLES / 2;
const REQUIRED_SAFE: usize = REQUIRED_K_BOUNDARY / 2;
const REQUIRED_DEAD: usize = SAMPLES / 2;

const TRANSITIONS: usize = 16;
const POSITIONS: usize = 16;

struct Sample {
    matrix: DMatrix<i32>,
    vector: Vec<i32>,
}

impl Sample {
    fn new() -> Self {
        let mut rng = rand::thread_rng();
        let (n, m) = (rng.gen_range(1..=POSITIONS), rng.gen_range(1..=TRANSITIONS));

        Self {
            matrix: generate_matrix(n, m),
            vector: generate_vector(n),
        }
    }

    fn get_matrix(&self) -> DMatrix<i32> {
        let mut result = DMatrix::zeros(POSITIONS + TRANSITIONS, POSITIONS + TRANSITIONS);
        for i in 0..self.matrix.nrows() {
            for j in 0..self.matrix.ncols() {
                if self.matrix[(i, j)] == -1 {
                    result[(i, POSITIONS + j)] = 1;
                } else if self.matrix[(i, j)] == 1 {
                    result[(POSITIONS + j, i)] = 1;
                }
            }
        }
        result
    }
}

fn generate_matrix(n: usize, m: usize) -> DMatrix<i32> {
    let mut matrix = DMatrix::zeros(n, m);
    let mut rng = rand::thread_rng();
    for i in 0..n {
        for j in 0..m {
            let value: i32 = rng.gen_range(-1..=1);
            matrix[(i, j)] = value;
        }
    }
    matrix
}

fn generate_vector(n: usize) -> Vec<i32> {
    let mut vector = Vec::new();
    let mut rng = rand::thread_rng();
    for _ in 0..n {
        let value: i32 = rng.gen_range(0..=1);
        vector.push(value);
    }
    vector
}

#[derive(serde::Serialize, Debug)]
struct SampleLabelsCsv {
    file_name: String,
    k_boundary: i32,
    safe: i32,
    dead: i32,
}

fn main() {
    let labels_file = format!("{}/{}", DIR, LABELS_FILE);
    let writer = csv::WriterBuilder::new()
        .has_headers(true)
        .from_path(labels_file)
        .unwrap();

    let writer = Arc::new(RwLock::new(writer));

    let sem = semaphore::Semaphore::new(12);

    let k_boundary_count = Arc::new(AtomicUsize::new(0));
    let safe_count = Arc::new(AtomicUsize::new(0));
    let dead_count = Arc::new(AtomicUsize::new(0));
    let n = Arc::new(AtomicUsize::new(0));

    let pool = rayon::ThreadPoolBuilder::new()
        .num_threads(12)
        .build()
        .unwrap();

    loop {
        let n = Arc::clone(&n);
        let writer = Arc::clone(&writer);
        let k_boundary_count = Arc::clone(&k_boundary_count);
        let safe_count = Arc::clone(&safe_count);
        let dead_count = Arc::clone(&dead_count);

        let samples = n.load(Ordering::SeqCst);
        if samples >= SAMPLES {
            break;
        }

        let permit = sem.acquire();

        pool.spawn(move || {
            let _permit = permit;
            let sample = Sample::new();

            // for col in sample.matrix.column_iter() {
            //     let has_max = col.iter().any(|s| *s == 1);
            //     let has_min = col.iter().any(|s| *s == -1);
            //
            //     if (has_max && !has_min) || (!has_max && !has_min) {
            //         return;
            //     }
            // }

            let net = PetriNet::from_matrix(sample.matrix.clone(), sample.vector.clone());
            let tree = match Reachability::new(&net).compute() {
                Some(tree) => tree,
                None => return,
            };

            let k_boundary = k_boundary(&tree);
            let safe = k_boundary.map_or(false, |k| k == 1) as i32;
            let dead = is_dead(&tree) as i32;

            let mut writer = writer.write().unwrap();
            let k_boundary_count_load = k_boundary_count.load(Ordering::SeqCst);
            let safe_count_load = safe_count.load(Ordering::SeqCst);
            let dead_count_load = dead_count.load(Ordering::SeqCst);
            if k_boundary_count_load > REQUIRED_K_BOUNDARY && k_boundary.is_some() {
                return;
            } else if safe_count_load > REQUIRED_SAFE && safe == 1 {
                return;
            } else if dead_count_load > REQUIRED_DEAD && dead == 1 {
                return;
            }

            let labels = SampleLabelsCsv {
                file_name: format!("{}.txt", n.fetch_add(1, Ordering::SeqCst)),
                k_boundary: k_boundary.is_some() as i32,
                safe,
                dead,
            };

            let samples = n.load(Ordering::SeqCst);
            print!("\rGenerating sample {samples}/{SAMPLES} of {dead_count_load} : {labels:?}");
            std::io::stdout().flush().unwrap();

            k_boundary_count.fetch_add(labels.k_boundary as usize, Ordering::SeqCst);
            safe_count.fetch_add(labels.safe as usize, Ordering::SeqCst);
            dead_count.fetch_add(labels.dead as usize, Ordering::SeqCst);

            // save to file
            let path = format!("{}/{}/{}", DIR, SAMPLES_DIR, labels.file_name);
            save_to_file(&sample, &path);

            // save to csv
            writer.serialize(labels).unwrap();
            writer.flush().unwrap();
        });
    }
}

fn save_to_file(sample: &Sample, file_name: &str) {
    let mut file = std::fs::File::create(file_name).unwrap();
    writeln!(file, "{}", POSITIONS + TRANSITIONS).unwrap();
    let transform = sample.get_matrix();
    for row in transform.row_iter() {
        for val in row.iter() {
            write!(file, "{} ", val).unwrap();
        }
    }

    writeln!(file).unwrap();
    for val in sample.vector.iter() {
        write!(file, "{} ", val).unwrap();
    }
}

fn k_boundary(tree: &ReachabilityTree) -> Option<i32> {
    let mut max = 0;
    for marking in tree.markings() {
        let val = marking.data().as_slice().iter().max().unwrap().as_number();
        if val == -1 {
            return None;
        }
        max = std::cmp::max(max, val);
    }
    Some(max)
}

fn is_dead(tree: &ReachabilityTree) -> bool {
    tree.markings()
        .iter()
        .find(|marking| marking.r#type() == CovType::DeadEnd)
        .is_some()
}
