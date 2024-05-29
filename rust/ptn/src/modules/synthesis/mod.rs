mod decompose;

pub use self::decompose::*;
use net::{DirectedEdge, PetriNet};
use std::collections::HashMap;
use std::hint::black_box;
use std::sync::Arc;
use std::sync::atomic::{AtomicUsize, Ordering};
use std::thread::sleep;
use std::time::Duration;
use rayon::ThreadPoolBuilder;
use tracing::debug;

pub struct SynthesisProgram {
    data: Vec<u16>,

    transitions: usize,

    transitions_united: usize,
    positions_united: usize,
}

impl SynthesisProgram {

    /// Создает SynthesisProgram
    ///
    /// Принимает массив, максимальный элемент которого не превышает размер массива
    pub fn new_with(data: Vec<u16>, transitions: usize) -> Self {
        let transitions_united = count_duplicates(data[0..transitions].to_vec());
        let positions_united = count_duplicates(data[transitions..].to_vec());

        SynthesisProgram {
            data,
            transitions,
            transitions_united,
            positions_united
        }
    }

    pub fn transitions_united(&self) -> usize {
        self.transitions_united
    }

    pub fn positions_united(&self) -> usize {
        self.positions_united
    }

    pub fn sets(&self) -> (Vec<Vec<usize>>, Vec<Vec<usize>>) {
        (
            sets_of_duplicate_indexes(&self.data[0..self.transitions]),
            sets_of_duplicate_indexes(&self.data[self.transitions..])
        )
    }
}

pub fn synthesis_program(context: &DecomposeContext, index: usize) -> PetriNet {
    let mut pos_indexes_vec = context.positions().clone();
    let mut tran_indexes_vec = context.transitions().clone();

    let c_matrix = context.c_matrix.clone();
    let (mut adjacency_input, mut adjacency_output) = context.primitive_matrix.clone();

    let mut markers = context.marking();

    let program = SynthesisProgram::new_with(
        context.programs.get_partition(index),
        tran_indexes_vec.len(),
    );

    let (t_sets, p_sets) = program.sets();
    let (t_sets_size, p_sets_size) = (t_sets.len(), p_sets.len());

    for t_set in t_sets.into_iter() {
        context.transition_synthesis_program(&t_set, &mut adjacency_input, &mut adjacency_output);
    }

    for p_set in p_sets.into_iter() {
        context.position_synthesis_program(
            &p_set,
            &mut adjacency_input,
            &mut adjacency_output,
            &mut markers,
        );
    }

    if t_sets_size == 0 && p_sets_size == 0 {
        debug!("adjacency_input BEFORE => {}", adjacency_input);
        debug!("adjacency_output BEFORE => {}", adjacency_output);
    }

    adjacency_input = &c_matrix * adjacency_input;
    adjacency_output = &c_matrix * adjacency_output;
    markers = &c_matrix * markers;

    if t_sets_size == 0 && p_sets_size == 0 {
        debug!("adjacency_input => {}", adjacency_input);
        debug!("adjacency_output => {}", adjacency_output);
    }

    let mut fract = true;
    for element in adjacency_input
        .iter()
        .chain(adjacency_output.iter())
        .chain(markers.iter())
    {
        if element.fract() != 0. {
            fract = false;
            println!("FRACT");
            break;
        }
    }

    adjacency_input
        .iter()
        .zip(adjacency_output.iter())
        .for_each(|(a, b)| {
            if (*a > 0. && *b < 0.) || (*a < 0. && *b > 0.) {
                fract = false;
            }
        });

    if !fract {
        return PetriNet::new();
    }

    let mut remove_rows = vec![];
    for (index, (row_a, row_b)) in adjacency_input
        .row_iter()
        .zip(adjacency_output.row_iter())
        .enumerate()
    {
        if row_a.iter().all(|&e| e == 0.) && row_b.iter().all(|&e| e == 0.) {
            remove_rows.push(index);
            continue;
        }

        for (sub_index, (sub_a, sub_b)) in adjacency_input
            .row_iter()
            .zip(adjacency_output.row_iter())
            .enumerate()
            .skip(index + 1)
        {
            if row_a == sub_a && row_b == sub_b && markers.row(index) == markers.row(sub_index) {
                remove_rows.push(sub_index);
                markers.row_mut(index)[0] =
                    f64::max(markers.row_mut(index)[0], markers.row_mut(sub_index)[0]);
            }
        }
    }

    adjacency_input = adjacency_input.remove_rows_at(&remove_rows);
    adjacency_output = adjacency_output.remove_rows_at(&remove_rows);
    markers = markers.remove_rows_at(&remove_rows);
    pos_indexes_vec = pos_indexes_vec
        .into_iter()
        .enumerate()
        .filter(|i| !remove_rows.contains(&i.0))
        .map(|i| i.1.clone())
        .collect();

    let mut remove_cols = vec![];
    for (index, (col_a, col_b)) in adjacency_input
        .column_iter()
        .zip(adjacency_output.column_iter())
        .enumerate()
    {
        if col_a.iter().chain(col_b.iter()).all(|&e| e == 0.) {
            remove_cols.push(index);
            continue;
        }
        for (sub_index, (sub_a, sub_b)) in adjacency_input
            .column_iter()
            .zip(adjacency_output.column_iter())
            .enumerate()
            .skip(index + 1)
        {
            if col_a == sub_a && col_b == sub_b {
                remove_cols.push(sub_index);
            }
        }
    }

    adjacency_input = adjacency_input.remove_columns_at(&remove_cols);
    adjacency_output = adjacency_output.remove_columns_at(&remove_cols);
    tran_indexes_vec = tran_indexes_vec
        .into_iter()
        .enumerate()
        .filter(|i| !remove_cols.contains(&i.0))
        .map(|i| i.1.clone())
        .collect();

    let mut new_net = PetriNet::new();
    *new_net.positions_mut() = pos_indexes_vec
        .iter()
        .map(|v| (v.index(), v.clone()))
        .collect();
    *new_net.transitions_mut() = tran_indexes_vec
        .iter()
        .map(|v| (v.index(), v.clone()))
        .collect();

    let mut pos_new_indexes = HashMap::new();
    for (index, position) in new_net.positions_mut().values_mut().enumerate() {
        debug!("SET MARKERS: {} <= {}", index, markers.row(index)[0]);
        position.set_markers(markers.row(index)[0].max(0.) as usize);
        pos_new_indexes.insert(position.index(), index);
    }

    let mut trans_new_indexes = HashMap::new();
    for (index, transition) in new_net
        .transitions()
        .values()
        .filter(|e| e.is_transition())
        .enumerate()
    {
        trans_new_indexes.insert(transition.index(), index);
    }

    for (vtx_idx, idx) in trans_new_indexes {
        for i in 0..2 {
            let column = match i {
                0 => adjacency_input.column(idx),
                1 => adjacency_output.column(idx),
                _ => unreachable!()
            };

            for (index, &el) in column.iter().enumerate().filter(|e| e.1.ne(&0.)) {
                let pos = pos_indexes_vec[index].clone();
                if (el > 0. && i == 0) || (el < 0. && i == 1) {
                    new_net.add_directed(DirectedEdge::new_with(pos.index(), vtx_idx, el.abs() as u32));
                } else if (el > 0. && i == 1) || (el < 0. && i == 0) {
                    new_net.add_directed(DirectedEdge::new_with(vtx_idx, pos.index(), el.abs() as u32));
                }
            }
        }
    }

    new_net
}

pub fn synthesis_all_programs(ctx: Arc<DecomposeContext>) -> usize {
    let pool = ThreadPoolBuilder::new().num_threads(16).build().unwrap();
    let programs = ctx.programs.max();
    let counter = Arc::new(AtomicUsize::new(0));
    for i in 0..programs {
        let c = counter.clone();
        let ctx = ctx.clone();
        pool.spawn(move || {
            let net = synthesis_program(ctx.as_ref(), i);
            black_box(net);
            c.fetch_add(1, Ordering::SeqCst);
        });
    }

    while counter.load(Ordering::SeqCst) != programs {
        sleep(Duration::from_micros(1000))
    }

    counter.load(Ordering::SeqCst)
}

fn count_duplicates(mut data: Vec<u16>) -> usize {
    let len = data.len();
    for i in 0..len {
        let idx = data[i] as usize % len;
        data[idx] += len as u16;
    }
    data.iter().filter(|&&v| (v / len as u16) >= 2).count()
}

fn sets_of_duplicate_indexes(slice: &[u16]) -> Vec<Vec<usize>> {
    let mut set = vec![vec![]; slice.len()];
    for (idx, &search_number) in slice.iter().enumerate() {
        set[search_number as usize].push(idx);
    }
    set.retain(|array| array.len() > 1);
    set
}