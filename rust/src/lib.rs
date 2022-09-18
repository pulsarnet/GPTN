#![feature(let_else)]
#![feature(drain_filter)]
#![feature(option_result_contains)]

extern crate libc;
extern crate nalgebra;
extern crate ndarray_linalg;
extern crate log4rs;
extern crate log;
extern crate chrono;
extern crate indexmap;

use std::cmp::{max_by, min_by};
use std::collections::{HashMap, HashSet};
use std::ffi::CString;
use std::ops::Deref;
use libc::c_char;
use log4rs::append::file::FileAppender;
use log4rs::{Config, config::Logger};
use log4rs::config::{Appender, Root};
use log4rs::encode::pattern::PatternEncoder;
use log::{info, LevelFilter};
use nalgebra::DMatrix;
use ndarray_linalg::Solve;
use core::Counter;
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

    info!("GOOD");

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

    pub fn calculate_c_matrix(positions: usize, transitions: usize, linear_base_fragments: &(CMatrix, CMatrix), primitive_matrix: &DMatrix<i32>) -> CMatrix {
        let mut c_matrix = nalgebra::DMatrix::<i32>::zeros(positions, positions);
        let (equivalent_input, equivalent_output) = linear_base_fragments;
        let mut d_matrix = nalgebra::DMatrix::<i32>::zeros(positions, transitions);
        for i in 0..d_matrix.nrows() {
            for j in 0..d_matrix.ncols() {
                if (equivalent_input.row(i)[j] + equivalent_output.row(i)[j]) == 0 && equivalent_input.row(i)[j] != 0 {
                    d_matrix.row_mut(i)[j] = 0;
                } else {
                    d_matrix.row_mut(i)[j] = equivalent_input.row(i)[j] + equivalent_output.row(i)[j];
                }
            }
        }

        let mut entries = vec![];
        let mut result_entries = vec![];
        for i in 0..c_matrix.nrows() {
            let mut entry = ndarray::Array2::zeros((primitive_matrix.nrows(), c_matrix.nrows()));
            let mut result_entry = ndarray::Array1::zeros(c_matrix.nrows());
            for j in 0..primitive_matrix.ncols() {
                for d in 0..primitive_matrix.nrows() {
                    entry.row_mut(j)[d] = primitive_matrix.row(d)[j] as f64;
                }
                result_entry[j] = d_matrix.row(i)[j] as f64;
            }

            for j in 0..entry.nrows() {
                for d in 0..entry.ncols() {
                    if entry.row(j)[d] == -1.0 {
                        entry.row_mut(j + primitive_matrix.ncols())[d] = 1.0;
                        result_entry[j + primitive_matrix.ncols()] = 0.0; //(rand::random::<u8>() % 4).into();
                        //result_entry[j + lbf_matrix.ncols()] = 1.0;//(rand::random::<u8>() % 4).into();
                    }
                }
            }

            entries.push(entry);
            result_entries.push(result_entry);
        }

        for (index, (a, b)) in entries
            .into_iter()
            .zip(result_entries.into_iter())
            .enumerate()
        {
            let solve = a.solve(&b).unwrap();
            solve
                .into_iter()
                .enumerate()
                .for_each(|v| c_matrix.row_mut(index)[v.0] = v.1 as i32);
        }

        CMatrix::from(c_matrix)
    }

    pub fn build(mut self) -> DecomposeContext {
        self.parts.sort();

        let parts = self.parts;
        let positions = parts.0.iter().flat_map(|net| net.positions.values()).cloned().collect::<Vec<_>>();
        let transitions = parts.0.iter().flat_map(|net| net.transitions.values()).cloned().collect::<Vec<_>>();
        let (primitive_net, primitive_matrix) = parts.primitive();
        let linear_base_fragments_matrix = parts.equivalent_matrix();

        let markers = parts.0.iter()
            .flat_map(|net| net.positions.values()).map(|pos| pos.markers())
            .collect::<Vec<usize>>();
        let c_matrix = DecomposeContextBuilder::calculate_c_matrix(positions.len(), transitions.len(), &linear_base_fragments_matrix, &primitive_matrix);

        DecomposeContext {
            parts,
            positions,
            transitions,
            primitive_net,
            primitive_matrix: CMatrix::from(primitive_matrix),
            linear_base_fragments_matrix,
            c_matrix,
            programs: vec![]
        }
    }

}

pub struct DecomposeContext {
    pub parts: PetriNetVec,
    pub positions: Vec<Vertex>,
    pub transitions: Vec<Vertex>,
    pub primitive_net: PetriNet,
    pub primitive_matrix: CMatrix,
    pub linear_base_fragments_matrix: (CMatrix, CMatrix),

    pub programs: Vec<SynthesisProgram>,
    pub c_matrix: crate::CMatrix,
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

        let mut res = DecomposeContextBuilder::new(parts).build();
        res.add_synthesis_programs();
        res
    }

    pub fn add_synthesis_programs(&mut self) {
        let transitions = self.transitions.len();
        let positions = self.positions.len();

        let mut t_counter = Counter::new(transitions);
        let mut t_programs = Vec::with_capacity(Counter::new(transitions).count());
        while let Some(c) = t_counter.next() {
            t_programs.push(c);
        }

        let mut p_counter = Counter::new(positions);
        let mut p_programs = Vec::with_capacity(Counter::new(positions).count());
        while let Some(c) = p_counter.next() {
            p_programs.push(c);
        }

        println!("T: {}", t_programs.len());
        println!("P: {}", p_programs.len());

        for t_program in t_programs {
            for p_program in p_programs.iter() {
                let mut program = SynthesisProgram::new(transitions + positions, transitions);
                t_program.iter()
                    .chain(p_program.iter())
                    .enumerate()
                    .for_each(|(i, v)| program.data[i] = *v);

                self.programs.push(program);
                synthesis_program(self, self.programs.len() - 1);
            }
        }
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

                if d_input.row(row)[column] < 0 {
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

    pub fn programs(&self) -> &Vec<SynthesisProgram> {
        &self.programs
    }

    pub fn add_program(&mut self) {
        self.programs.push(SynthesisProgram::new(self.positions().len() + self.transitions().len(), self.transitions().len()))
    }

    pub fn remove_program(&mut self, index: usize) {
        self.programs.remove(index);
    }

    pub fn program_value(&self, program: usize, index: usize) -> usize {
        self.programs()[program].data[index] as usize
    }

    pub fn set_program_value(&mut self, program: usize, index: usize, value: usize) {
        self.programs[program].data[index] = value as u16;
    }

    pub fn program_equation(&self, index: usize) -> String {
        let pos_indexes_vec = self.positions();
        let tran_indexes_vec = self.transitions();
        let (t_sets, p_sets) = self.programs[index].sets(pos_indexes_vec, tran_indexes_vec);
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

    pub fn c_matrix(&self) -> &crate::CMatrix {
        &self.c_matrix
    }

    pub fn primitive_matrix(&self) -> &crate::CMatrix {
        &self.primitive_matrix
    }

    pub fn primitive_net(&self) -> &PetriNet {
        &self.primitive_net
    }

    pub fn transition_synthesis_program(
        &self,
        t_set: &Vec<usize>,
        d_input: &mut DMatrix<i32>,
        d_output: &mut DMatrix<i32>,
    ) {

        let mut input = DMatrix::<i32>::zeros(d_input.nrows(), 1);
        let mut output = DMatrix::<i32>::zeros(d_input.nrows(), 1);
        for t in t_set.iter() {
            d_input.column(*t).iter().enumerate().for_each(|(index, &b)| {
                let a = input.row(index)[0];
                input.row_mut(index)[0] = min_by(a, b, |&ra, &rb| ra.cmp(&rb));
            });

            d_output.column(*t).iter().enumerate().for_each(|(index, &b)| {
                let a = output.row(index)[0];
                output.row_mut(index)[0] = max_by(a, b, |&ra, &rb| ra.cmp(&rb));
            });
        }

        for t in t_set.iter() {
            d_input.set_column(*t, &input.column(0));
            d_output.set_column(*t, &output.column(0));
        }

    }

    pub fn position_synthesis_program(
        &self,
        p_set: &Vec<usize>,
        d_input: &mut DMatrix<i32>,
        d_output: &mut DMatrix<i32>,
        d_markers: &mut DMatrix<i32>,
    ) {

        let mut input = DMatrix::<i32>::zeros(1, d_input.ncols());
        let mut output = DMatrix::<i32>::zeros(1, d_input.ncols());
        let mut markers = 0;
        for p in p_set.iter() {
            d_input.row(*p).iter().enumerate().for_each(|(index, &b)| {
                let a = input.column(index)[0];
                input.column_mut(index)[0] = min_by(a, b, |&ra, &rb| ra.cmp(&rb));
            });

            d_output.row(*p).iter().enumerate().for_each(|(index, &b)| {
                let a = output.column(index)[0];
                output.column_mut(index)[0] = max_by(a, b, |&ra, &rb| ra.cmp(&rb));
            });

            markers += d_markers.row(*p)[0];
        }

        for p in p_set.iter() {
            d_input.set_row(*p, &input.row(0));
            d_output.set_row(*p, &output.row(0));
            d_markers.row_mut(*p)[0] = markers;
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
pub extern "C" fn decompose_context_primitive_matrix(ctx: &DecomposeContext) -> *const CMatrix {
    &ctx.primitive_matrix as *const CMatrix
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
    match ctx.programs.get(index) {
        Some(_) => {
            let c_str = CString::new(ctx.program_equation(index)).unwrap();
            let pointer = c_str.as_ptr();
            std::mem::forget(c_str);
            pointer
        },
        None => std::ptr::null()
    }
}

#[no_mangle]
extern "C" fn synthesis_programs(ctx: &DecomposeContext) -> usize {
    ctx.programs().len()
}

#[no_mangle]
extern "C" fn synthesis_program_size(ctx: &DecomposeContext, index: usize) -> usize {
    ctx.programs()[index].data.len()
}

#[no_mangle]
extern "C" fn synthesis_program_transition_united(ctx: &DecomposeContext, index: usize) -> usize {
    ctx.programs()[index].transitions_united()
}

#[no_mangle]
extern "C" fn synthesis_program_position_united(ctx: &DecomposeContext, index: usize) -> usize {
    ctx.programs()[index].positions_united()
}

#[no_mangle]
extern "C" fn synthesis_c_matrix(ctx: &DecomposeContext) -> *const CMatrix {
    ctx.c_matrix() as *const CMatrix
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