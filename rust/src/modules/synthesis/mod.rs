mod decompose;

pub use self::decompose::*;
use net::{Connection, PetriNet, Vertex};
use std::collections::{HashMap, HashSet};

pub struct SynthesisProgram {
    data: Vec<u16>,

    transitions: usize,
    //net_after: Option<PetriNet>
}

impl SynthesisProgram {
    pub fn new_with(data: Vec<u16>, transitions: usize) -> Self {
        SynthesisProgram {
            data,
            transitions,
            //net_after: None
        }
    }

    pub fn transitions_united(&self) -> usize {
        let mut counts = HashMap::with_capacity(self.transitions);
        self.data
            .iter()
            .take(self.transitions)
            .for_each(|el| *counts.entry(*el).or_insert_with(|| 0 as usize) += 1);
        counts.values().filter(|v| **v > 1).count()
    }

    pub fn positions_united(&self) -> usize {
        let mut counts = HashMap::with_capacity(self.data.len() - self.transitions);
        self.data
            .iter()
            .skip(self.transitions)
            .for_each(|el| *counts.entry(*el).or_insert_with(|| 0 as usize) += 1);
        counts.values().filter(|v| **v > 1).count()
    }

    pub fn sets(
        &self,
        pos_indexes_vec: &Vec<Vertex>,
        tran_indexes_vec: &Vec<Vertex>,
    ) -> (Vec<Vec<usize>>, Vec<Vec<usize>>) {
        let mut t_sets = vec![];
        let mut p_sets = vec![];
        let mut searched = vec![].into_iter().collect::<HashSet<u16>>();

        for index_a in 0..tran_indexes_vec.len() {
            let search_number = self.data[index_a];
            if searched.contains(&search_number) {
                continue;
            }
            let mut indexes = vec![];
            for index_b in (0..tran_indexes_vec.len()).filter(|e| self.data[*e] == search_number) {
                if index_a == index_b {
                    continue;
                }
                indexes.push(index_b);
            }
            if indexes.len() > 0 {
                indexes.push(index_a);
                t_sets.push(indexes);
            }
            searched.insert(search_number);
        }

        let offset = tran_indexes_vec.len();
        let mut searched = vec![].into_iter().collect::<HashSet<u16>>();
        for index_a in offset..(offset + pos_indexes_vec.len()) {
            let search_number = self.data[index_a];
            if searched.contains(&search_number) {
                continue;
            }
            let mut indexes = vec![];
            for index_b in (offset..(offset + pos_indexes_vec.len()))
                .filter(|e| self.data[*e] == search_number)
            {
                if index_a == index_b {
                    continue;
                }
                indexes.push(index_b - offset);
            }
            if indexes.len() > 0 {
                indexes.push(index_a - offset);
                p_sets.push(indexes);
            }
            searched.insert(search_number);
        }

        (t_sets, p_sets)
    }
}

pub fn synthesis_program(programs: &mut DecomposeContext, index: usize) -> PetriNet {
    let mut pos_indexes_vec = programs.positions().clone();
    let mut tran_indexes_vec = programs.transitions().clone();

    let c_matrix = programs.c_matrix.clone();
    let (mut adjacency_input, mut adjacency_output) = programs.primitive_matrix.clone();

    let mut markers = programs.marking();

    let program = SynthesisProgram::new_with(
        programs.programs.get_partition(index),
        tran_indexes_vec.len(),
    );

    let (t_sets, p_sets) = program.sets(&pos_indexes_vec, &tran_indexes_vec);
    let (t_sets_size, p_sets_size) = (t_sets.len(), p_sets.len());

    log::error!("PSET => {:?}", p_sets);
    log::error!("TSET => {:?}", t_sets);

    for t_set in t_sets.into_iter() {
        programs.transition_synthesis_program(&t_set, &mut adjacency_input, &mut adjacency_output);
    }

    for p_set in p_sets.into_iter() {
        programs.position_synthesis_program(
            &p_set,
            &mut adjacency_input,
            &mut adjacency_output,
            &mut markers,
        );
    }

    if t_sets_size == 0 && p_sets_size == 0 {
        println!("adjacency_input BEFORE => {}", adjacency_input);
        println!("adjacency_output BEFORE => {}", adjacency_output);
    }

    adjacency_input = &c_matrix * adjacency_input;
    adjacency_output = &c_matrix * adjacency_output;
    markers = &c_matrix * markers;

    if t_sets_size == 0 && p_sets_size == 0 {
        println!("adjacency_input => {}", adjacency_input);
        println!("adjacency_output => {}", adjacency_output);
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
    new_net.positions = pos_indexes_vec
        .iter()
        .map(|v| (v.index(), v.clone()))
        .collect();
    new_net.transitions = tran_indexes_vec
        .iter()
        .map(|v| (v.index(), v.clone()))
        .collect();

    let mut pos_new_indexes = HashMap::new();
    for (index, position) in new_net.positions.values_mut().enumerate() {
        log::info!("SET MARKERS: {} <= {}", index, markers.row(index)[0]);
        position.set_markers(markers.row(index)[0].max(0.) as usize);
        pos_new_indexes.insert(position.index(), index);
    }

    let mut trans_new_indexes = HashMap::new();
    for (index, transition) in new_net
        .transitions
        .values()
        .filter(|e| e.is_transition())
        .enumerate()
    {
        trans_new_indexes.insert(transition.index(), index);
    }

    let mut connections = vec![];
    for transition in new_net.transitions.values() {
        for i in 0..2 {
            let column = match i {
                0 => adjacency_input.column(*trans_new_indexes.get(&transition.index()).unwrap()),
                1 => adjacency_output.column(*trans_new_indexes.get(&transition.index()).unwrap()),
                _ => panic!("Invalid column index"),
            };

            for (index, &el) in column.iter().enumerate().filter(|e| e.1.ne(&0.)) {
                let pos = pos_indexes_vec[index].clone();
                if el < 0. {
                    connections.push(Connection::new(pos.index(), transition.index()));
                } else {
                    connections.push(Connection::new(transition.index(), pos.index()));
                }

                connections
                    .last_mut()
                    .unwrap()
                    .set_weight(el.abs() as usize);
            }
        }
    }

    new_net.connections = connections;

    new_net
}
