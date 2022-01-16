#![feature(let_else)]
#![feature(drain_filter)]
#![feature(option_result_contains)]

extern crate libc;
extern crate nalgebra;
extern crate ndarray_linalg;

use core::NamedMatrix;
use libc::c_long;
use nalgebra::DMatrix;
use std::ffi::CString;
use std::os::raw::c_char;
use std::ptr::null;

pub mod ffi;
mod net;

mod core;

use net::{synthesis, synthesis_program, PetriNet, Vertex, PetriNetVec};

#[repr(C)]
pub struct FFIConnection {
    from: *const c_char,
    to: *const c_char,
}

#[repr(C)]
pub struct FFIBoxedSlice {
    elements: *const *const c_char,
    len_elements: usize,

    connections: *const *mut FFIConnection,
    len_connections: usize,

    markers: *const usize,
}

impl FFIBoxedSlice {
    fn from_net(net: PetriNet) -> Self {
        let mut elements_vec: Vec<CString> = vec![];
        let mut connections_vec: Vec<*mut FFIConnection> = vec![];
        let mut markers_vec: Vec<usize> = vec![];

        for s in net.elements.into_iter() {
            let c_str = CString::new(s.name()).unwrap();
            elements_vec.push(c_str);

            if s.is_position() {
                markers_vec.push(s.markers() as usize);
            }
        }
        elements_vec.shrink_to_fit();

        let mut c_char_vec: Vec<*const c_char> = vec![];
        for s in elements_vec {
            c_char_vec.push(s.as_ptr());
            std::mem::forget(s);
        }

        for s in net.connections.into_iter() {
            let s1 = CString::new(s.first().name()).unwrap();
            let s2 = CString::new(s.second().name()).unwrap();
            let conn = Box::new(FFIConnection {
                from: s1.as_ptr(),
                to: s2.as_ptr(),
            });
            let conn = Box::into_raw(conn);

            connections_vec.push(conn);

            std::mem::forget(s1);
            std::mem::forget(s2);
        }

        let slice = FFIBoxedSlice {
            elements: c_char_vec.as_ptr(),
            len_elements: c_char_vec.len(),
            connections: connections_vec.as_ptr(),
            len_connections: connections_vec.len(),
            markers: markers_vec.as_ptr(),
        };

        std::mem::forget(c_char_vec);
        std::mem::forget(connections_vec);
        std::mem::forget(markers_vec);
        slice
    }
}

#[repr(C)]
pub struct FFIMatrix {
    rows_len: usize,
    cols_len: usize,
    matrix: *const c_long,
}

impl FFIMatrix {
    fn from_matrix(matrix: DMatrix<i32>) -> FFIMatrix {
        let result_matrix_len = matrix.nrows() * matrix.ncols();
        let mut matrix_vector = Vec::with_capacity(result_matrix_len);
        matrix_vector.resize(result_matrix_len, 0 as c_long);

        for i in 0..matrix.nrows() {
            for j in 0..matrix.ncols() {
                matrix_vector[i * matrix.ncols() + j] = matrix.row(i)[j] as c_long;
            }
        }

        let result = FFIMatrix {
            rows_len: matrix.nrows(),
            cols_len: matrix.ncols(),
            matrix: matrix_vector.as_ptr(),
        };

        std::mem::forget(matrix_vector);

        result
    }
}

#[repr(C)]
pub struct FFINamedMatrix {
    rows: *const *const c_char,
    rows_len: usize,

    cols: *const *const c_char,
    cols_len: usize,
    matrix: *const c_long,
}

impl FFINamedMatrix {
    fn from_matrix(matrix: NamedMatrix) -> FFINamedMatrix {
        let rows = matrix.rows;
        let cols = matrix.cols;
        let matrix = matrix.matrix;

        let result_matrix_len = matrix.nrows() * matrix.ncols();
        let mut matrix_vector = Vec::with_capacity(result_matrix_len);
        matrix_vector.resize(result_matrix_len, 0 as c_long);

        for i in 0..matrix.nrows() {
            for j in 0..matrix.ncols() {
                matrix_vector[i * matrix.ncols() + j] = matrix.row(i)[j] as c_long;
            }
        }

        let mut rows_elements = vec![];
        rows_elements.resize(matrix.nrows(), null());
        for (s, i) in rows.into_iter() {
            let c_str = CString::new(s.name()).unwrap();
            rows_elements[i] = c_str.as_ptr();
            std::mem::forget(c_str);
        }

        let mut cols_elements = vec![];
        cols_elements.resize(matrix.ncols(), null());
        for (s, i) in cols.into_iter() {
            let c_str = CString::new(s.name()).unwrap();
            cols_elements[i] = c_str.as_ptr();
            std::mem::forget(c_str);
        }

        rows_elements.shrink_to_fit();
        cols_elements.shrink_to_fit();

        let result = FFINamedMatrix {
            rows: rows_elements.as_ptr(),
            rows_len: matrix.nrows(),
            cols: cols_elements.as_ptr(),
            cols_len: matrix.ncols(),
            matrix: matrix_vector.as_ptr(),
        };

        std::mem::forget(rows_elements);
        std::mem::forget(cols_elements);
        std::mem::forget(matrix_vector);

        result
    }
}

#[repr(C)]
pub enum FFIVertexType {
    Position,
    Transition
}

#[repr(C)]
pub struct FFIParent {
    vertex_type: FFIVertexType,
    child: usize,
    parent: usize
}

#[repr(C)]
pub struct FFIParentVec {
    inner: *const FFIParent,
    len: usize
}

#[repr(C)]
pub struct CommonResult {
    petri_net: *mut FFIBoxedSlice,
    c_matrix: *mut FFIMatrix,
    lbf_matrix: *mut FFINamedMatrix,

    d_input: *mut FFINamedMatrix,
    d_output: *mut FFINamedMatrix,
    parents: *mut FFIParentVec
}

#[no_mangle]
pub unsafe extern "C" fn synthesis_start(v: *mut PetriNet) -> *mut SynthesisProgram {
    let v = &mut *v;
    let mut v = v.clone();

    println!("V: {:?}", v);

    let mut parts = vec![];

    while let Some(l) = v.get_loop() {
        println!("LOOOOP:::::::::: {:?}", l);
        parts.push(v.remove_part(&l));
    }

    while let Some(p) = v.get_part() {
        println!("PART::::::::::: {:?}", p);
        parts.push(v.remove_part(&p));
    }

    // Добавим в каждую часть нормальную позицию
    parts.iter_mut().for_each(|net| net.normalize());
    parts.iter_mut().for_each(|net| println!("PART: {:?}", net));

    // VT => max(zt)
    // VP => max(zp)
    // D' => VP(N) * VT(N)
    // C * D' => D''
    // D'' убираем эквивалентные позиции и переходы

    Box::into_raw(Box::new(synthesis(PetriNetVec(parts))))
}

#[no_mangle]
pub unsafe extern "C" fn synthesis_end(v: *mut SynthesisProgram) -> *mut CommonResult {
    let program = &mut *v;
    let result = synthesis_program(program);

    let parents = program.get_parents();
    let petri_net = Box::new(FFIBoxedSlice::from_net(result.result_net));
    let c_matrix = Box::new(FFIMatrix::from_matrix(result.c_matrix));
    let lbf_matrix = Box::new(FFINamedMatrix::from_matrix(result.lbf_matrix));
    let d_input = Box::new(FFINamedMatrix::from_matrix(result.d_input));
    let d_output = Box::new(FFINamedMatrix::from_matrix(result.d_output));

    Box::into_raw(Box::new(CommonResult {
        petri_net: Box::into_raw(petri_net),
        c_matrix: Box::into_raw(c_matrix),
        lbf_matrix: Box::into_raw(lbf_matrix),
        d_input: Box::into_raw(d_input),
        d_output: Box::into_raw(d_output),
        parents
    }))
}

pub struct SynthesisResult {
    pub result_net: PetriNet,
    pub c_matrix: DMatrix<i32>,
    pub lbf_matrix: NamedMatrix,
    pub d_input: NamedMatrix,
    pub d_output: NamedMatrix,
}

pub struct SynthesisProgram {
    pub positions: Vec<Vertex>,
    pub transitions: Vec<Vertex>,
    pub programs: Vec<Vec<usize>>,
    pub c_matrix: DMatrix<i32>,
    pub lbf_matrix: DMatrix<i32>,
    pub d_input: DMatrix<i32>,
    pub d_output: DMatrix<i32>,
}

impl SynthesisProgram {

    pub fn get_parents(&self) -> *mut FFIParentVec {
        let mut parents = vec![];

        for pos in self.positions.iter() {
            if let Some(p) = pos.get_first_parent() {
                parents.push(FFIParent {
                    vertex_type: FFIVertexType::Position,
                    child: pos.index() as usize,
                    parent: p.index() as usize,
                });
            }
        }

        for tran in self.transitions.iter() {
            if let Some(t) = tran.get_first_parent() {
                parents.push(FFIParent {
                    vertex_type: FFIVertexType::Transition,
                    child: tran.index() as usize,
                    parent: t.index() as usize,
                });
            }
        }

        let ptr = Box::new(FFIParentVec { inner: parents.as_ptr(), len: parents.len() });
        std::mem::forget(parents);
        Box::into_raw(ptr)
    }

}

#[no_mangle]
extern "C" fn positions(p: *mut SynthesisProgram) -> usize {
    let program = unsafe { &mut *p };
    program.positions.len()
}

#[no_mangle]
extern "C" fn transitions(p: *mut SynthesisProgram) -> usize {
    let program = unsafe { &mut *p };
    program.transitions.len()
}

#[no_mangle]
extern "C" fn programs(p: *mut SynthesisProgram) -> usize {
    let program = unsafe { &mut *p };
    program.programs.len()
}

#[no_mangle]
extern "C" fn add_program(p: *mut SynthesisProgram) {
    let program = unsafe { &mut *p };

    let mut n = vec![];
    n.resize(transitions(p) + positions(p), 0);
    program.programs.push(n);
}

#[no_mangle]
extern "C" fn set_program_value(
    p: *mut SynthesisProgram,
    program_i: usize,
    index: usize,
    value: usize,
) {
    let program = unsafe { &mut *p };
    program.programs[program_i][index] = value;
}

#[no_mangle]
extern "C" fn get_program_value(p: *mut SynthesisProgram, program_i: usize, index: usize) -> usize {
    let program = unsafe { &mut *p };
    program.programs[program_i][index]
}

#[no_mangle]
extern "C" fn position(p: *mut SynthesisProgram, index: usize) -> *const c_char {
    let program = unsafe { &mut *p };
    let c_str = CString::new(program.positions[index].name()).unwrap();
    let ptr = c_str.as_ptr();

    std::mem::forget(c_str);

    ptr
}

#[no_mangle]
extern "C" fn transition(p: *mut SynthesisProgram, index: usize) -> *const c_char {
    let program = unsafe { &mut *p };
    let c_str = CString::new(program.transitions[index].name()).unwrap();
    let ptr = c_str.as_ptr();

    std::mem::forget(c_str);

    ptr
}

#[no_mangle]
extern "C" fn drop_string(s: *mut c_char) {
    unsafe { CString::from_raw(s) };
}
