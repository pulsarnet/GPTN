#![feature(let_else)]
#![feature(drain_filter)]
#![feature(option_result_contains)]

extern crate libc;
extern crate nalgebra;
extern crate ndarray_linalg;

use core::NamedMatrix;
use nalgebra::DMatrix;

pub mod ffi;
mod net;

mod core;

use net::{synthesis, PetriNet, Vertex, PetriNetVec};

pub struct CMatrix {
    inner: DMatrix<i32>
}

impl From<DMatrix<i32>> for CMatrix {
    fn from(inner: DMatrix<i32>) -> Self {
        Self { inner }
    }
}

pub struct SynthesisContext {
    pub positions: Vec<Vertex>,
    pub transitions: Vec<Vertex>,
    pub programs: Vec<Vec<usize>>,
    pub c_matrix: crate::CMatrix,
    pub primitive_matrix: crate::CMatrix,
    //pub linear_base_fragments: Vec<(crate::CMatrix, crate::CMatrix)>
}

impl SynthesisContext {

    pub fn positions(&self) -> &Vec<Vertex> {
        &self.positions
    }

    pub fn transitions(&self) -> &Vec<Vertex> {
        &self.transitions
    }

    pub fn programs(&self) -> &Vec<Vec<usize>> {
        &self.programs
    }

    pub fn c_matrix(&self) -> &crate::CMatrix {
        &self.c_matrix
    }

    pub fn primitive_matrix(&self) -> &crate::CMatrix {
        &self.primitive_matrix
    }

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

        synthesis(PetriNetVec(parts))
    }

    // pub fn linear_base_fragments(&self) -> &Vec<(crate::CMatrix, crate::CMatrix)> {
    //     &self.linear_base_fragments
    // }

}

#[no_mangle]
pub extern "C" fn synthesis_positions(ctx: &SynthesisContext) -> usize {
    ctx.positions().len()
}

#[no_mangle]
extern "C" fn synthesis_transitions(ctx: &SynthesisContext) -> usize {
    ctx.transitions().len()
}

#[no_mangle]
extern "C" fn synthesis_programs(ctx: &SynthesisContext) -> usize {
    ctx.programs().len()
}

#[no_mangle]
extern "C" fn synthesis_c_matrix(ctx: &SynthesisContext) -> *const CMatrix {
    ctx.c_matrix() as *const CMatrix
}

#[no_mangle]
extern "C" fn synthesis_primitive_matrix(ctx: &SynthesisContext) -> *const CMatrix {
    ctx.primitive_matrix() as *const CMatrix
}

#[no_mangle]
pub unsafe extern "C" fn synthesis_init(net: &PetriNet) -> *mut SynthesisContext {
    Box::into_raw(Box::new(SynthesisContext::init(net)))
}

#[no_mangle]
pub unsafe extern "C" fn synthesis_position_index(ctx: &SynthesisContext, index: usize) -> usize {
    ctx.positions[index].index() as usize
}

#[no_mangle]
pub unsafe extern "C" fn synthesis_transition_index(ctx: &SynthesisContext, index: usize) -> usize {
    ctx.transitions[index].index() as usize
}



#[no_mangle]
extern "C" fn matrix_index(matrix: &CMatrix, row: usize, column: usize) -> i32 {
    matrix.inner.row(row)[column]
}

#[no_mangle]
extern "C" fn matrix_rows(matrix: &CMatrix) -> usize {
    matrix.inner.nrows()
}

#[no_mangle]
extern "C" fn matrix_columns(matrix: &CMatrix) -> usize {
    matrix.inner.ncols()
}

// #[no_mangle]
// pub unsafe extern "C" fn eval_program(ctx: &mut SynthesisContext, index: usize) -> *mut CommonResult {
//     let result = synthesis_program(ctx, index);
//
//     let parents = program.get_parents();
//     let petri_net = Box::new(FFIBoxedSlice::from_net(result.result_net));
//     let c_matrix = Box::new(CMatrix::from_matrix(result.c_matrix));
//     let lbf_matrix = Box::new(FFINamedMatrix::from_matrix(result.lbf_matrix));
//     let fragments =
//         Box::new(FFILogicalBaseFragmentsVec::from_vec(program.linear_base_fragments.clone()));
//
//     Box::into_raw(Box::new(CommonResult {
//         petri_net: Box::into_raw(petri_net),
//         c_matrix: Box::into_raw(c_matrix),
//         lbf_matrix: Box::into_raw(lbf_matrix),
//         logical_base_fragments: Box::into_raw(fragments),
//         parents
//     }))
// }
