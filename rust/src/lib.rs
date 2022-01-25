#![feature(let_else)]
#![feature(drain_filter)]
#![feature(option_result_contains)]

extern crate libc;
extern crate nalgebra;
extern crate ndarray_linalg;

use std::ffi::CString;
use libc::c_char;
use core::NamedMatrix;
use nalgebra::DMatrix;

pub mod ffi;
mod net;

mod core;

use net::{synthesis, PetriNet, Vertex, PetriNetVec, synthesis_program};

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
    pub linear_base_fragments: Vec<(NamedMatrix, NamedMatrix)>
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

    pub fn add_program(&mut self) {
        self.programs.push(vec![0; self.positions.len() + self.transitions.len()])
    }

    pub fn remove_program(&mut self, index: usize) {
        self.programs.remove(index);
    }

    pub fn program_value(&self, program: usize, index: usize) -> usize {
        self.programs()[program][index]
    }

    pub fn set_program_value(&mut self, program: usize, index: usize, value: usize) {
        self.programs[program][index] = value;
    }

    pub fn program_header_name(&self, index: usize) -> String {
        if index < self.transitions.len() {
            self.transitions[index].name()
        }
        else {
            self.positions[index - self.transitions.len()].name()
        }
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

    pub fn primitive_net(&self) -> PetriNet {

        let mut result = PetriNet::new();

        result.elements.extend(self.positions.iter().cloned());
        result.elements.extend(self.transitions.iter().cloned());

        for column in 0..self.primitive_matrix.inner.ncols() {
            for row in 0..self.primitive_matrix.inner.nrows() {
                if self.primitive_matrix.inner.row(row)[column] > 0 {
                    result.connect(self.transitions[column].clone(), self.positions[row].clone());
                }
                else if self.primitive_matrix.inner.row(row)[column] < 0 {
                    result.connect(self.positions[row].clone(), self.transitions[column].clone())
                }
            }
        }

        result

    }

    pub fn linear_base_fragments(&self) -> PetriNet {

        // TODO: Установить максимальный индекс у позиции и перехода
        // TODO: Получение позиции по индексу
        // TODO: Получение перехода по индексу

        let mut result = PetriNet::new();

        for (d_input, d_output) in self.linear_base_fragments.iter() {
            result.elements.extend(d_input.rows.iter().map(|row| row.0.clone()).collect::<Vec<_>>());
            result.elements.extend(d_input.cols.iter().map(|column| column.0.clone()).collect::<Vec<_>>());

            for row in 0..d_input.rows.len() {
                for column in 0..d_input.cols.len() {

                    if d_input.matrix.row(row)[column] < 0 {
                        result.connect(
                            d_input.rows.iter().find(|(_, v)| **v == row).map(|(k, _)| k.clone()).unwrap(),
                            d_input.cols.iter().find(|(_, v)| **v == column).map(|(k, _)| k.clone()).unwrap(),
                        )
                    }

                    if d_output.matrix.column(column)[row] > 0 {
                        result.connect(
                            d_input.cols.iter().find(|(_, v)| **v == column).map(|(k, _)| k.clone()).unwrap(),
                            d_input.rows.iter().find(|(_, v)| **v == row).map(|(k, _)| k.clone()).unwrap(),
                        )
                    }

                }
            }
        }

        result
    }

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
extern "C" fn synthesis_add_program(ctx: &mut SynthesisContext) {
    ctx.add_program();
}

#[no_mangle]
extern "C" fn synthesis_remove_program(ctx: &mut SynthesisContext, index: usize) {
    ctx.remove_program(index);
}

#[no_mangle]
extern "C" fn synthesis_program_value(ctx: &SynthesisContext, program: usize, index: usize) -> usize {
    ctx.program_value(program, index)
}

#[no_mangle]
extern "C" fn synthesis_set_program_value(ctx: &mut SynthesisContext, program: usize, index: usize, value: usize) {
    ctx.set_program_value(program, index, value);
}

#[no_mangle]
extern "C" fn synthesis_program_header_name(ctx: &mut SynthesisContext, index: usize) -> *const c_char {
    // TODO: Исправить постоянное выделение памяти
    let c_str = CString::new(ctx.program_header_name(index)).unwrap();
    let pointer = c_str.as_ptr();
    std::mem::forget(c_str);
    pointer
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
extern "C" fn synthesis_primitive_net(ctx: &SynthesisContext) -> *const PetriNet {
    Box::into_raw(Box::new(ctx.primitive_net()))
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
extern "C" fn synthesis_linear_base_fragments(ctx: &SynthesisContext) -> *mut PetriNet {
    Box::into_raw(Box::new(ctx.linear_base_fragments()))
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

// Вычисление программ синтеза
#[no_mangle]
extern "C" fn synthesis_eval_program(ctx: &mut SynthesisContext, index: usize) -> *mut PetriNet {
    let result = synthesis_program(ctx, index);
    Box::into_raw(Box::new(result))
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
