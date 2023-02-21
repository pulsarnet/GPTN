#![feature(drain_filter)]
#![feature(option_result_contains)]

extern crate libc;
extern crate nalgebra;
extern crate ndarray_linalg;
extern crate log4rs;
extern crate log;
extern crate chrono;
extern crate indexmap;
extern crate ndarray;
extern crate num_traits;
extern crate num;
extern crate rand;


use std::collections::{HashMap, HashSet};
use std::ffi::CString;
use std::ops::Deref;
use libc::c_char;
use log4rs::append::file::FileAppender;
use log4rs::{Config, config::Logger};
use log4rs::config::{Appender, Root};
use log4rs::encode::pattern::PatternEncoder;
use log::LevelFilter;
use nalgebra::DMatrix;
use ndarray::{Array1, Array2};
use ndarray_linalg::Solve;
use core::{Counter, logical_column_add, logical_row_add, SetPartitionMesh};
use ffi::vec::CVec;


pub mod ffi;
mod modules;
mod net;

mod core;

use net::{PetriNet, Vertex, PetriNetVec, synthesis_program};

#[no_mangle]
extern "C" fn init() {
    let f = format!("log/{}.log", chrono::Local::now().format("%d-%m-%YT%H_%M_%S"));
    let requests = FileAppender::builder()
        .encoder(Box::new(PatternEncoder::new("{m}{n}")))
        .build(f)
        .unwrap();

    let config = Config::builder()
        .appender(Appender::builder().build("requests", Box::new(requests)))
        .logger(Logger::builder()
            .appender("requests")
            .build("app::requests", LevelFilter::Error))
        .build(Root::builder().appender("requests").build(LevelFilter::Error))
        .unwrap();

    log4rs::init_config(config).unwrap();

}

pub struct CMatrix {
    inner: DMatrix<i32>
}

impl Deref for CMatrix {
    type Target = DMatrix<i32>;

    fn deref(&self) -> &Self::Target {
        &self.inner
    }
}

impl From<DMatrix<i32>> for CMatrix {
    fn from(inner: DMatrix<i32>) -> Self {
        Self { inner }
    }
}

/// Контекст декомпозиции
///
/// Содержит информацию о разделении сети Петри на состовляющие компоненты
/// и сеть Петри в примитивной системе координат

pub struct DecomposeContextBuilder {
    pub parts: PetriNetVec,
}

impl DecomposeContextBuilder {

    pub fn new(parts: PetriNetVec) -> Self {
        DecomposeContextBuilder {
            parts
        }
    }

    fn solve_with_mu(mut a: Array2<f64>, mut b: Array1<f64>) -> Array1<f64> {
        // find all negative in n/2 eqution
        let mu_equation_index = a.nrows() / 2;
        for i in 0..mu_equation_index {
            for j in 0..a.ncols() {
                if a[(i, j)] == -1. && a[(mu_equation_index, j)] != 0. {
                    a[(mu_equation_index, j)] = 0.;
                    for k in 0..a.ncols() {
                        if a[(i, k)] == 1. {
                            a[(mu_equation_index, k)] += 1.;
                            b[mu_equation_index] += b[i];
                        }
                    }
                }
            }
        }

        // Set free variable
        let mut free_variable = 0;
        for j in 0..a.ncols() {
            if a[(mu_equation_index, j)] > 0. {
                free_variable = j;
                break;
            }
        }


        // Set other negative to zero
        let mut next_equation_index = mu_equation_index + 1;
        'outer: for i in 0..mu_equation_index {
            for j in 0..a.ncols() {
                if a[(i, j)] == 1. && j == free_variable {
                    continue 'outer;
                }
            }

            for j in 0..a.ncols() {
                if b[i] > 0. && a[(i, j)] == 1. {
                    a[(next_equation_index, j)] = 1.;
                    b[next_equation_index] = b[i];
                    next_equation_index += 1;
                    break
                } else if b[i] < 0. && a[(i, j)] == -1. {
                    a[(next_equation_index, j)] = 1.;
                    b[next_equation_index] -= b[i];
                    next_equation_index += 1;
                    break
                } else if a[(i, j)] == -1. {
                    a[(next_equation_index, j)] = 1.;
                    next_equation_index += 1;
                    break
                }
                // if a[(i, j)] == -1.{
                //     a[(next_equation_index, j)] = 1.;
                //     next_equation_index += 1;
                // }
            }
        }

        println!("a: {}", a);
        // solve
        a.solve(&b).unwrap()
    }

    fn solve_without_mu(mut a: Array2<f64>, mut b: Array1<f64>) -> Array1<f64> {
        let mut mu_equation_index = a.nrows() / 2;
        for i in 0..mu_equation_index {
            for j in 0..a.ncols() {
                if b[i] > 0. && a[(i, j)] == 1. {
                    a[(mu_equation_index, j)] = 1.;
                    b[mu_equation_index] = b[i];
                    mu_equation_index += 1;
                    break
                } else if b[i] < 0. && a[(i, j)] == -1. {
                    a[(mu_equation_index, j)] = 1.;
                    b[mu_equation_index] -= b[i];
                    mu_equation_index += 1;
                    break
                } else if a[(i, j)] == -1. {
                    a[(mu_equation_index, j)] = 1.;
                    mu_equation_index += 1;
                    break
                }

                // if a[(i, j)] == -1. {
                //     a[(mu_equation_index, j)] = 1.;
                //     mu_equation_index += 1;
                // }
            }
        }

        a.solve(&b).unwrap()
    }

    fn solve(a: Array2<f64>, b: Array1<f64>) -> Array1<f64> {
        let mu_equation_index = a.nrows() / 2;
        let mu_equation = a.row(mu_equation_index);
        if mu_equation.iter().any(|&x| x != 0.) {
            DecomposeContextBuilder::solve_with_mu(a, b)
        } else {
            DecomposeContextBuilder::solve_without_mu(a, b)
        }
    }

    pub fn calculate_c_matrix(positions: usize,
                              transitions: usize,
                              linear_base_fragments: &(DMatrix<i32>, DMatrix<i32>),
                              primitive_matrix: DMatrix<i32>,
                              mu: &DMatrix<i32>) -> DMatrix<f64>
    {
        let mut c_matrix = nalgebra::DMatrix::<f64>::zeros(positions, positions);
        let d_matrix =
            linear_base_fragments.1.clone() - linear_base_fragments.0.clone();

        for row in 0..positions {
            let mut array_a = Array2::<f64>::zeros((positions, positions));
            let mut array_b = Array1::<f64>::zeros(positions);

            for col in 0..transitions {
                array_a.row_mut(col).assign(
                    &primitive_matrix.column(col)
                        .iter()
                        .map(|&x| x as f64)
                        .collect::<Array1<f64>>()
                );
                array_b[col] = d_matrix[(row, col)] as f64;
            }

            // set mu equation
            array_a.row_mut(transitions).assign(
                &mu.row(0)
                    .iter()
                    .map(|&x| x as f64)
                    .collect::<Array1<f64>>()
            );
            array_b[transitions] = mu[(0, row)] as f64;

            let solution = DecomposeContextBuilder::solve(array_a, array_b);
            c_matrix.row_mut(row).copy_from_slice(solution.as_slice().unwrap());
        }

        println!("{}", c_matrix);

        c_matrix.iter_mut().for_each(|x| {
            if x.fract() == 0. && *x == 0. {
                *x = 0.
            }
        });

        println!("{}", c_matrix);

        c_matrix
    }

    pub fn build(mut self) -> DecomposeContext {
        self.parts.sort();

        let parts = self.parts;
        let positions = parts.0.iter().flat_map(|net| net.positions.values()).cloned().collect::<Vec<_>>();
        let transitions = parts.0.iter().flat_map(|net| net.transitions.values()).cloned().collect::<Vec<_>>();

        let primitive_net = parts.primitive_net();
        //let adjacency_primitive = primitive_net.adjacency_matrix();
        let adjacency_primitive = primitive_net.adjacency_matrices();
        println!("adjacency_primitive 0: {}", adjacency_primitive.0);
        println!("adjacency_primitive 1: {}", adjacency_primitive.1);

        let (primitive_input, primitive_output) = parts.primitive_matrix();
        let linear_base_fragments_matrix = parts.equivalent_matrix();
        let mu = DMatrix::from_row_slice(1, positions.len(), &positions.iter().map(|x| x.markers() as i32).collect::<Vec<_>>());
        let c_matrix = DecomposeContextBuilder::calculate_c_matrix(
            positions.len(),
            transitions.len(),
            &linear_base_fragments_matrix,
            primitive_output.clone() - primitive_input.clone(),
            &mu
        );

        let (pos, tran) = (positions.len(), transitions.len());
        DecomposeContext {
            parts,
            positions,
            transitions,
            primitive_net,
            primitive_matrix: adjacency_primitive,
            linear_base_fragments_matrix: (CMatrix::from(linear_base_fragments_matrix.0), CMatrix::from(linear_base_fragments_matrix.1)),
            c_matrix,
            programs: SetPartitionMesh::new(pos, tran),
        }
    }

}

pub struct DecomposeContext {
    pub parts: PetriNetVec,
    pub positions: Vec<Vertex>,
    pub transitions: Vec<Vertex>,
    pub primitive_net: PetriNet,
    pub primitive_matrix: (DMatrix<f64>, DMatrix<f64>),
    pub linear_base_fragments_matrix: (CMatrix, CMatrix),

    pub programs: SetPartitionMesh,

    pub c_matrix: DMatrix<f64>,
}

impl DecomposeContext {

    pub fn init(net: &PetriNet) -> Self {
        let mut net = net.clone();
        let mut parts = vec![];

        while let Some(l) = net.get_loop() {
            parts.push(net.remove_part(&l));
        }

        while let Some(p) = net.get_part() {
            parts.push(net.remove_part(&p));
        }

        parts.iter_mut().for_each(|net| net.normalize());

        let parts = PetriNetVec(parts);

        DecomposeContextBuilder::new(parts).build()
        //res.add_synthesis_programs();
        //res
    }

    // pub fn add_synthesis_programs(&mut self) {
    //     let transitions = self.transitions.len();
    //     let positions = self.positions.len();
    //
    //     let mut t_counter = Counter::new(transitions);
    //     let mut t_programs = vec![];
    //     while let Some(c) = t_counter.next() {
    //         t_programs.push(c);
    //     }
    //
    //     let mut p_counter = Counter::new(positions);
    //     let mut p_programs = vec![];
    //     while let Some(c) = p_counter.next() {
    //         p_programs.push(c);
    //     }
    //
    //     println!("T: {}", t_programs.len());
    //     println!("P: {}", p_programs.len());
    //
    //     for t_program in t_programs {
    //         for p_program in p_programs.iter() {
    //             let mut program = SynthesisProgram::new(transitions + positions, transitions);
    //             t_program.iter()
    //                 .chain(p_program.iter())
    //                 .enumerate()
    //                 .for_each(|(i, v)| program.data[i] = *v);
    //
    //             self.programs.push(program);
    //
    //             //synthesis_program(self, self.programs.len() - 1);
    //         }
    //     }
    // }

    pub fn marking(&self) -> DMatrix<f64> {
        let mut marking = DMatrix::zeros(self.positions.len(), 1);
        for (i, p) in self.positions.iter().enumerate() {
            marking[(i, 0)] = p.markers() as f64;
        }

        marking
    }

    pub fn linear_base_fragments(&self) -> PetriNet {

        // TODO: Установить максимальный индекс у позиции и перехода
        // TODO: Получение позиции по индексу
        // TODO: Получение перехода по индексу

        let mut result = PetriNet::new();
        let (d_input, d_output) = &self.linear_base_fragments_matrix;

        result.positions.extend(self.positions.iter().map(|v| (v.index(), v.clone())));
        result.transitions.extend(self.transitions.iter().map(|v| (v.index(), v.clone())));

        for row in 0..d_input.nrows() {
            for column in 0..d_input.ncols() {

                if d_input.row(row)[column] > 0 {
                    result.connect(
                        self.positions.iter().enumerate().find(|(k, _)| *k == row).map(|(_, k)| k.index()).unwrap(),
                        self.transitions.iter().enumerate().find(|(k, _)| *k == column).map(|(_, k)| k.index()).unwrap(),
                    )
                }

                if d_output.column(column)[row] > 0 {
                    result.connect(
                        self.transitions.iter().enumerate().find(|(k, _)| *k == column).map(|(_, k)| k.index()).unwrap(),
                        self.positions.iter().enumerate().find(|(k, _)| *k == row).map(|(_, k)| k.index()).unwrap(),
                    )
                }

            }
        }

        result
    }

    pub fn positions(&self) -> &Vec<Vertex> {
        &self.positions
    }

    pub fn transitions(&self) -> &Vec<Vertex> {
        &self.transitions
    }

    pub fn programs(&self) -> &SetPartitionMesh {
        &self.programs
    }

    pub fn add_program(&mut self) {
        //self.programs.push(SynthesisProgram::new(self.positions().len() + self.transitions().len(), self.transitions().len()))
    }

    pub fn remove_program(&mut self, index: usize) {
        //self.programs.remove(index);
    }

    pub fn program_value(&self, program: usize, index: usize) -> usize {
        //self.programs()[program].data[index] as usize
        0
    }

    pub fn set_program_value(&mut self, program: usize, index: usize, value: usize) {
        //self.programs[program].data[index] = value as u16;
    }

    pub fn program_equation(&self, index: usize) -> String {
        let pos_indexes_vec = self.positions();
        let tran_indexes_vec = self.transitions();

        let program = SynthesisProgram::new_with(
            self.programs.get_partition(index),
            tran_indexes_vec.len()
        );

        let (t_sets, p_sets) = program.sets(pos_indexes_vec, tran_indexes_vec);

        log::error!("{:?}\n{:?}\n{:?}\n{:?}", t_sets, p_sets, pos_indexes_vec, tran_indexes_vec);

        let mut result = String::new();
        for set in t_sets {
            if set.is_empty() {
                continue
            }

            result += tran_indexes_vec[*set.last().unwrap()].name().as_str();
            result += " = ";
            for i in (0..set.len()).rev() {
                result += &tran_indexes_vec[set[i]].name();
                if i > 0 {
                    result += " + ";
                }
            }

            result += "\n";
        }

        for set in p_sets {
            if set.is_empty() {
                continue
            }

            result += &pos_indexes_vec[*set.last().unwrap()].name().as_str();
            result += " = ";
            for i in (0..set.len()).rev() {
                result += &pos_indexes_vec[set[i]].name();
                if i > 0 {
                    result += " + ";
                }
            }

            result += "\n";
        }

        result
    }

    pub fn program_header_name(&self, index: usize, label: bool) -> String {
        if index < self.transitions().len() {
            match label {
                true => self.transitions()[index].label(false),
                false => self.transitions()[index].full_name()
            }
        }
        else {
            match label {
                true => self.positions()[index - self.transitions().len()].label(false),
                false => self.positions()[index - self.transitions().len()].full_name()
            }
        }
    }

    pub fn primitive_net(&self) -> &PetriNet {
        &self.primitive_net
    }

    pub fn transition_synthesis_program(
        &self,
        t_set: &Vec<usize>,
        adjacency_input: &mut DMatrix<f64>,
        adjacency_output: &mut DMatrix<f64>)
    {
        assert!(t_set.len() > 1);

        let first = t_set[0];
        for t in t_set.iter().skip(1) {
            logical_column_add(adjacency_input, first, *t);
            logical_column_add(adjacency_output, first, *t);
            // let column_input = adjacency_input.column(*t).clone_owned();
            // adjacency_input.column_mut(first).add_assign(column_input);
            //
            // let column_output = adjacency_output.column(*t).clone_owned();
            // adjacency_output.column_mut(first).add_assign(column_output);
        }

        for t in t_set.iter().skip(1) {
            logical_column_add(adjacency_input, *t, first);
            logical_column_add(adjacency_output, *t, first);
            // let vector = Vector::from_data(adjacency_input.column(first).clone_owned().data);
            // adjacency_input.set_column(*t, &vector);
            //
            // let vector = Vector::from_data(adjacency_output.column(first).clone_owned().data);
            // adjacency_output.set_column(*t, &vector);
        }
    }

    pub fn position_synthesis_program(
        &self,
        p_set: &Vec<usize>,
        adjacency_input: &mut DMatrix<f64>,
        adjacency_output: &mut DMatrix<f64>,
        d_markers: &mut DMatrix<f64>,
    ) {

        let first = p_set[0];
        for p in p_set.iter().skip(1) {
            logical_row_add(adjacency_input, first, *p);
            logical_row_add(adjacency_output, first, *p);
            // let row = adjacency_input.row(*p).clone_owned();
            // adjacency_input.row_mut(first).add_assign(row);
            //
            // let row = adjacency_output.row(*p).clone_owned();
            // adjacency_output.row_mut(first).add_assign(row);

            d_markers[(first, 0)] = d_markers[(*p, 0)].max(d_markers[(first, 0)]);
        }

        for p in p_set.iter().skip(1) {
            logical_row_add(adjacency_input, *p, first);
            logical_row_add(adjacency_output, *p, first);
            // let row = RowVector::from_data(adjacency_input.row(first).clone_owned().data);
            // adjacency_input.set_row(*p, &row);
            //
            // let row = RowVector::from_data(adjacency_output.row(first).clone_owned().data);
            // adjacency_output.set_row(*p, &row);

            d_markers[(*p, 0)] = d_markers[(first, 0)];
        }

    }
}

#[no_mangle]
pub extern "C" fn decompose_context_parts(ctx: &DecomposeContext, parts: &mut CVec<*const PetriNet>) {
    let result = ctx.parts
        .0
        .iter()
        .map(|p| p as *const PetriNet).collect::<Vec<_>>();

    unsafe { std::ptr::write_unaligned(parts, CVec::from(result)) };
}

#[no_mangle]
pub extern "C" fn decompose_context_init(net: &PetriNet) -> *mut DecomposeContext {
    Box::into_raw(Box::new(DecomposeContext::init(net)))
}

#[no_mangle]
pub extern "C" fn decompose_context_from_nets(nets: *mut *mut PetriNet, len: usize) -> *mut DecomposeContext {
    let mut parts_m = vec![];
    for i in 0..len {
        parts_m.push(unsafe { &**nets.offset(i as isize) }.clone());
    }
    Box::into_raw(Box::new(DecomposeContextBuilder::new(PetriNetVec(parts_m)).build()))
}

#[no_mangle]
pub extern "C" fn decompose_context_positions(ctx: &DecomposeContext) -> usize {
    ctx.positions.len()
}

#[no_mangle]
pub extern "C" fn decompose_context_transitions(ctx: &DecomposeContext) -> usize {
    ctx.transitions.len()
}

#[no_mangle]
extern "C" fn decompose_context_primitive_net(ctx: &DecomposeContext) -> *const PetriNet {
    &ctx.primitive_net as *const PetriNet
}

#[no_mangle]
extern "C" fn decompose_context_linear_base_fragments(ctx: &DecomposeContext) -> *mut PetriNet {
    Box::into_raw(Box::new(ctx.linear_base_fragments()))
}

#[no_mangle]
pub unsafe extern "C" fn decompose_context_position_index(ctx: &DecomposeContext, index: usize) -> usize {
    ctx.positions[index].index().id as usize
}

#[no_mangle]
pub unsafe extern "C" fn decompose_context_transition_index(ctx: &DecomposeContext, index: usize) -> usize {
    ctx.transitions[index].index().id as usize
}

#[no_mangle]
pub unsafe extern "C" fn decompose_context_delete(ctx: *mut DecomposeContext) {
    let _ = Box::from_raw(ctx);
}

pub struct SynthesisProgram {
    data: Vec<u16>,

    transitions: usize,

    //net_after: Option<PetriNet>
}

impl SynthesisProgram {
    pub fn new(size: usize, transitions: usize) -> Self {
        SynthesisProgram {
            data: vec![0; size],
            transitions,
            //net_after: None
        }
    }

    pub fn new_with(data: Vec<u16>, transitions: usize) -> Self {
        SynthesisProgram {
            data,
            transitions,
            //net_after: None
        }
    }

    pub fn transitions_united(&self) -> usize {
        let mut counts = HashMap::with_capacity(self.transitions);
        self.data.iter().take(self.transitions).for_each(|el| {
            *counts.entry(*el).or_insert_with(|| 0 as usize) += 1
        });
        counts.values().filter(|v| **v > 1).count()
    }

    pub fn positions_united(&self) -> usize {
        let mut counts = HashMap::with_capacity(self.data.len() - self.transitions);
        self.data.iter().skip(self.transitions).for_each(|el| {
            *counts.entry(*el).or_insert_with(|| 0 as usize) += 1
        });
        counts.values().filter(|v| **v > 1).count()
    }

    pub fn sets(&self, pos_indexes_vec: &Vec<Vertex>, tran_indexes_vec: &Vec<Vertex>) -> (Vec<Vec<usize>>, Vec<Vec<usize>>) {
        let mut t_sets = vec![];
        let mut p_sets = vec![];
        let mut searched = vec![].into_iter().collect::<HashSet<u16>>();

        for index_a in 0..tran_indexes_vec.len() {
            let search_number = self.data[index_a];
            if searched.contains(&search_number) {
                continue;
            }
            let mut indexes = vec![];
            for index_b in (0..tran_indexes_vec.len()).filter(|e| self.data[*e] == search_number)
            {
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

#[no_mangle]
extern "C" fn synthesis_add_program(ctx: &mut DecomposeContext) {
    ctx.add_program();
}

#[no_mangle]
extern "C" fn synthesis_remove_program(ctx: &mut DecomposeContext, index: usize) {
    ctx.remove_program(index);
}

#[no_mangle]
extern "C" fn synthesis_program_value(ctx: &DecomposeContext, program: usize, index: usize) -> usize {
    ctx.program_value(program, index)
}

#[no_mangle]
extern "C" fn synthesis_set_program_value(ctx: &mut DecomposeContext, program: usize, index: usize, value: usize) {
    ctx.set_program_value(program, index, value);
}

#[no_mangle]
extern "C" fn synthesis_program_header_name(ctx: &mut DecomposeContext, index: usize, label: bool) -> *const c_char {
    // TODO: Исправить постоянное выделение памяти
    let c_str = CString::new(ctx.program_header_name(index, label)).unwrap();
    let pointer = c_str.as_ptr();
    std::mem::forget(c_str);
    pointer
}

#[no_mangle]
extern "C" fn synthesis_program_equations(ctx: &mut DecomposeContext, index: usize) -> *const c_char {
    let c_str = CString::new(ctx.program_equation(index)).unwrap();
    let pointer = c_str.as_ptr();
    std::mem::forget(c_str);
    pointer
}

#[no_mangle]
extern "C" fn synthesis_programs(ctx: &DecomposeContext) -> usize {
    ctx.programs().max()
}

#[no_mangle]
extern "C" fn synthesis_program_size(ctx: &DecomposeContext, index: usize) -> usize {
    ctx.programs().len()
}

#[no_mangle]
extern "C" fn synthesis_program_transition_united(ctx: &DecomposeContext, index: usize) -> usize {
    SynthesisProgram::new_with(
        ctx.programs.get_partition(index),
        ctx.transitions.len(),
    ).transitions_united()
}

#[no_mangle]
extern "C" fn synthesis_program_position_united(ctx: &DecomposeContext, index: usize) -> usize {
    SynthesisProgram::new_with(
        ctx.programs.get_partition(index),
        ctx.transitions.len(),
    ).positions_united()
}

#[no_mangle]
extern "C" fn matrix_index(matrix: &CMatrix, row: usize, column: usize) -> i32 {
    matrix.inner.row(row)[column]
}

#[no_mangle]
extern "C" fn matrix_set_value(matrix: &mut CMatrix, row: usize, column: usize, value: i32) {
    matrix.inner.row_mut(row)[column] = value;
}

#[no_mangle]
extern "C" fn matrix_rows(matrix: &CMatrix) -> usize {
    matrix.inner.nrows()
}

#[no_mangle]
extern "C" fn matrix_columns(matrix: &CMatrix) -> usize {
    matrix.inner.ncols()
}

// Вычисление программ синтеза
#[no_mangle]
extern "C" fn synthesis_eval_program(ctx: &mut DecomposeContext, index: usize) -> *const PetriNet {
    // TODO: Comment
    // if ctx.programs[index].net_after.is_some() {
    //     return ctx.programs[index].net_after.as_ref().unwrap() as *const PetriNet
    // }

    Box::into_raw(Box::new(synthesis_program(ctx, index))) as *const PetriNet
    //ctx.programs[index].net_after.as_ref().unwrap() as *const PetriNet
}

#[cfg(test)]
mod tests {
    // test SynthesisProgram
    use super::*;

    #[test]
    fn test_synthesis_program() {
        let mut program = SynthesisProgram::new(12, 4);
        program.data = vec![0, 1, 2, 3, 0, 1, 2, 3, 4, 5, 6, 7];

        let pos_indexes = vec![
            Vertex::position(1),
            Vertex::position(2),
            Vertex::position(3),
            Vertex::position(4),
            Vertex::position(5),
            Vertex::position(6),
            Vertex::position(7),
            Vertex::position(8),
        ];

        let tran_indexes = vec![
            Vertex::transition(1),
            Vertex::transition(2),
            Vertex::transition(3),
            Vertex::transition(4),
        ];

        dbg!(program.sets(&pos_indexes, &tran_indexes));
    }
}