extern crate libc;
extern crate ptn;
extern crate tracing;
extern crate tracing_subscriber;
extern crate cxx;

use ptn::modules::synthesis::synthesis_all_programs;
use std::ffi::CString;
use std::sync::Arc;
use libc::c_char;
use ptn::CMatrix;
use ptn::modules::synthesis::{DecomposeContext, synthesis_program, SynthesisProgram};
use ptn::net::PetriNet;

mod vertex;
mod net;
mod vec;
mod edge;
mod context;
mod reachability;
mod simulation;
mod logger;
mod invariant;
mod decompose;

macro_rules! profile {
    ($($token:tt)+) => {
        {
            let _instant = std::time::Instant::now();
            let _result = {
                $($token)+
            };

            (_instant.elapsed(), _result)
        }
    }
}





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
    matrix.row(row)[column]
}

#[no_mangle]
extern "C" fn matrix_set_value(matrix: &mut CMatrix, row: usize, column: usize, value: i32) {
    matrix.row_mut(row)[column] = value;
}

#[no_mangle]
extern "C" fn matrix_rows(matrix: &CMatrix) -> usize {
    matrix.nrows()
}

#[no_mangle]
extern "C" fn matrix_columns(matrix: &CMatrix) -> usize {
    matrix.ncols()
}

// Вычисление программ синтеза
#[no_mangle]
extern "C" fn synthesis_eval_program(ctx: &mut DecomposeContext, index: usize) -> *const PetriNet {
    Box::into_raw(Box::new(synthesis_program(ctx, index))) as *const PetriNet
}

#[no_mangle]
unsafe  extern "C" fn synthesis_all_programs_ext(ctx: &mut DecomposeContext) {
    let ctx = Arc::new(ctx.clone());
    println!("Start calc {} programs", ctx.programs.max());
    let (dur, result) = profile!(synthesis_all_programs(ctx));
    println!("Synthesis time {:?} of {} programs Rust parallel", dur, result);
}