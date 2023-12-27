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


use std::io::Write;
use std::ptr::write;
use rand::Rng;
use ptn::net::PetriNet;
use ptn::alg::DMatrix;
use ptn::modules::reachability::{CovType, Reachability, ReachabilityTree};

const DIR: &str = "train";
const SAMPLES_DIR: &str = "samples";
const LABELS_FILE: &str = "labels.csv";
const SAMPLES: usize = 10000;

#[derive(Debug)]
struct SampleConfig {
    n: u32,
    m: u32,
}

impl SampleConfig {
    fn new() -> Self {
        Self {
            n: rand::thread_rng().gen_range(1..=32),
            m: rand::thread_rng().gen_range(1..=32),
        }
    }
}

struct Sample {
    matrix: DMatrix<i32>,
    vector: Vec<i32>,
}

impl Sample {
    fn new(config: &SampleConfig) -> Self {
        Self {
            matrix: generate_matrix(config.n, config.m),
            vector: generate_vector(config.n),
        }
    }
}

fn generate_matrix(n: u32, m: u32) -> DMatrix<i32> {
    let mut matrix = DMatrix::zeros(n as usize, m as usize);
    let mut rng = rand::thread_rng();
    for i in 0..n {
        for j in 0..m {
            let value: i32 = rng.gen_range(-1..=1);
            matrix[(i as usize, j as usize)] = value;
        }
    }
    matrix
}

fn generate_vector(n: u32) -> Vec<i32> {
    let mut vector = Vec::new();
    let mut rng = rand::thread_rng();
    for _ in 0..n {
        let value: i32 = rng.gen_range(0..=1);
        vector.push(value);
    }
    vector
}

#[derive(serde::Serialize)]
struct SampleLabelsCsv {
    file_name: String,
    k_boundary: i32,
    safe: i32,
    dead: i32,
}

fn main() {
    let labels_file = format!("{}/{}", DIR, LABELS_FILE);
    let mut writer = csv::WriterBuilder::new()
        .has_headers(true)
        .from_path(labels_file)
        .unwrap();

    let mut n = 0;
    while n < SAMPLES {
        // as above
        print!("\rGenerating sample {}/{SAMPLES}", n);
        std::io::stdout().flush().unwrap();

        let config = SampleConfig::new();
        let sample = Sample::new(&config);

        // skip if matrix has free transitions (must have (min 1 and min -1) or only -1 in each col)
        let mut has_free = false;
        for col in sample.matrix.column_iter() {
            let mut has_min = false;
            let mut has_max = false;
            for val in col.iter() {
                if *val == -1 {
                    has_min = true;
                }
                if *val == 1 {
                    has_max = true;
                }
            }

            if !((has_min && has_max) || has_min) {
                has_free = true;
                break;
            }
        }
        if has_free {
            continue;
        }

        // skip if matrix has free positions (must have min 1 or min -1 in each row)
        let mut has_free = false;
        for row in sample.matrix.row_iter() {
            let mut has_min = false;
            let mut has_max = false;
            for val in row.iter() {
                if *val == -1 {
                    has_min = true;
                }
                if *val == 1 {
                    has_max = true;
                }
            }

            if !(has_min || has_max) {
                has_free = true;
                break;
            }
        }
        if has_free {
            continue;
        }


        let net = PetriNet::from_matrix(sample.matrix.clone(), sample.vector.clone());
        let tree = match Reachability::new(&net).compute() {
            Some(tree) => tree,
            None => continue,
        };
        let k_boundary = k_boundary(&tree);
        let labels = SampleLabelsCsv {
            file_name: format!("{}.txt", n),
            k_boundary: k_boundary.is_some() as i32,
            safe: k_boundary.map_or(false, |k| k == 1) as i32,
            dead: is_dead(&tree) as i32,
        };

        // save to file
        let path = format!("{}/{}/{}", DIR, SAMPLES_DIR, labels.file_name);
        save_to_file(&sample, &path);

        // save to csv
        writer.serialize(labels).unwrap();
        writer.flush().unwrap();

        n += 1;
    }
}

fn save_to_file(sample: &Sample, file_name: &str) {
    let mut file = std::fs::File::create(file_name).unwrap();
    writeln!(file, "{} {}", sample.matrix.nrows(), sample.matrix.ncols()).unwrap();
    for row in sample.matrix.row_iter() {
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
