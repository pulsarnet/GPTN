#![feature(drain_filter)]

extern crate chrono;
extern crate indexmap;
extern crate libc;
extern crate log;
extern crate log4rs;
extern crate nalgebra;
extern crate ndarray;
extern crate ndarray_linalg;
extern crate num;
extern crate num_traits;
extern crate rand;

use libc::c_char;
use log::LevelFilter;
use log4rs::append::file::FileAppender;
use log4rs::config::{Appender, Root};
use log4rs::encode::pattern::PatternEncoder;
use log4rs::{config::Logger, Config};
use nalgebra::DMatrix;
use std::ffi::CString;
use std::ops::Deref;

use ffi::vec::CVec;
use modules::synthesis::{
    synthesis_program, DecomposeContext, DecomposeContextBuilder, SynthesisProgram,
};

pub mod ffi;
mod modules;
mod net;

mod core;

use net::{PetriNet, PetriNetVec, Vertex};

#[no_mangle]
extern "C" fn init() {
    let f = format!(
        "log/{}.log",
        chrono::Local::now().format("%d-%m-%YT%H_%M_%S")
    );
    let requests = FileAppender::builder()
        .encoder(Box::new(PatternEncoder::new("{m}{n}")))
        .build(f)
        .unwrap();

    let config = Config::builder()
        .appender(Appender::builder().build("requests", Box::new(requests)))
        .logger(
            Logger::builder()
                .appender("requests")
                .build("app::requests", LevelFilter::Error),
        )
        .build(
            Root::builder()
                .appender("requests")
                .build(LevelFilter::Error),
        )
        .unwrap();

    log4rs::init_config(config).unwrap();
}

pub struct CMatrix {
    inner: DMatrix<i32>,
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

#[no_mangle]
pub extern "C" fn decompose_context_parts(
    ctx: &DecomposeContext,
    parts: &mut CVec<*const PetriNet>,
) {
    let result = ctx
        .parts
        .0
        .iter()
        .map(|p| p as *const PetriNet)
        .collect::<Vec<_>>();

    unsafe { std::ptr::write_unaligned(parts, CVec::from(result)) };
}

#[no_mangle]
pub extern "C" fn decompose_context_init(net: &PetriNet) -> *mut DecomposeContext {
    Box::into_raw(Box::new(DecomposeContext::init(net)))
}

#[no_mangle]
pub extern "C" fn decompose_context_from_nets(
    nets: *mut *mut PetriNet,
    len: usize,
) -> *mut DecomposeContext {
    let mut parts_m = vec![];
    for i in 0..len {
        parts_m.push(unsafe { &**nets.offset(i as isize) }.clone());
    }
    Box::into_raw(Box::new(
        DecomposeContextBuilder::new(PetriNetVec(parts_m)).build(),
    ))
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
pub unsafe extern "C" fn decompose_context_position_index(
    ctx: &DecomposeContext,
    index: usize,
) -> usize {
    ctx.positions[index].index().id as usize
}

#[no_mangle]
pub unsafe extern "C" fn decompose_context_transition_index(
    ctx: &DecomposeContext,
    index: usize,
) -> usize {
    ctx.transitions[index].index().id as usize
}

#[no_mangle]
pub unsafe extern "C" fn decompose_context_delete(ctx: *mut DecomposeContext) {
    let _ = Box::from_raw(ctx);
}

// #[no_mangle]
// extern "C" fn synthesis_add_program(ctx: &mut DecomposeContext) {
//     ctx.add_program();
// }

// #[no_mangle]
// extern "C" fn synthesis_remove_program(ctx: &mut DecomposeContext, index: usize) {
//     ctx.remove_program(index);
// }

#[no_mangle]
extern "C" fn synthesis_program_value(
    ctx: &DecomposeContext,
    program: usize,
    index: usize,
) -> usize {
    ctx.program_value(program, index)
}

#[no_mangle]
extern "C" fn synthesis_set_program_value(
    ctx: &mut DecomposeContext,
    program: usize,
    index: usize,
    value: usize,
) {
    ctx.set_program_value(program, index, value);
}

#[no_mangle]
extern "C" fn synthesis_program_header_name(
    ctx: &mut DecomposeContext,
    index: usize,
    label: bool,
) -> *const c_char {
    // TODO: Исправить постоянное выделение памяти
    let c_str = CString::new(ctx.program_header_name(index, label)).unwrap();
    let pointer = c_str.as_ptr();
    std::mem::forget(c_str);
    pointer
}

#[no_mangle]
extern "C" fn synthesis_program_equations(
    ctx: &mut DecomposeContext,
    index: usize,
) -> *const c_char {
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
extern "C" fn synthesis_program_size(ctx: &DecomposeContext, _: usize) -> usize {
    ctx.programs().len()
}

#[no_mangle]
extern "C" fn synthesis_program_transition_united(ctx: &DecomposeContext, index: usize) -> usize {
    SynthesisProgram::new_with(ctx.programs.get_partition(index), ctx.transitions.len())
        .transitions_united()
}

#[no_mangle]
extern "C" fn synthesis_program_position_united(ctx: &DecomposeContext, index: usize) -> usize {
    SynthesisProgram::new_with(ctx.programs.get_partition(index), ctx.transitions.len())
        .positions_united()
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
