#![feature(let_else)]
#![feature(drain_filter)]
#![feature(option_result_contains)]


extern crate nalgebra;
extern crate ndarray_linalg;
extern crate libc;

use std::ffi::CString;
use std::os::raw::c_char;
use std::ptr::null;
use libc::c_long;
use nalgebra::DMatrix;
use core::NamedMatrix;

mod net;

mod core;

use net::{PetriNet, synthesis, Vertex};

#[no_mangle]
pub extern "C" fn make() -> *mut PetriNet {
    let v = Box::new(PetriNet::new());
    Box::into_raw(v)
}

#[no_mangle]
pub unsafe extern "C" fn del(v: *mut PetriNet) {
    Box::from_raw(v);
}

#[no_mangle]
pub unsafe extern "C" fn count(_: *mut PetriNet) -> std::os::raw::c_ulong {
    0
}

#[no_mangle]
pub unsafe extern "C" fn add_position(v: *mut PetriNet, index: u64) {
    let v = &mut *v;
    v.add_position(index);
}

#[no_mangle]
pub unsafe extern "C" fn add_transition(v: *mut PetriNet, index: u64) {
    let v = &mut *v;
    v.add_transition(index);
}

#[no_mangle]
pub unsafe extern "C" fn connect_p(v: *mut PetriNet, position: u64, transition: u64) {
    let v = &mut *v;

    let position = Vertex::position(position);
    let transition = Vertex::transition(transition);

    v.connect(position, transition);
}

#[no_mangle]
pub unsafe extern "C" fn connect_t(v: *mut PetriNet, transition: u64, position: u64) {
    let v = &mut *v;

    let position = Vertex::position(position);
    let transition = Vertex::transition(transition);

    v.connect(transition, position);
}

#[repr(C)]
pub struct FFIConnection {
    from: *const c_char,
    to: *const c_char
}

#[repr(C)]
pub struct FFIBoxedSlice {
    elements: *const *const c_char,
    len_elements: usize,

    connections: *const *mut FFIConnection,
    len_connections: usize
}

impl FFIBoxedSlice {
    fn from_net(net: PetriNet) -> Self {
        let mut elements_vec: Vec<CString> = vec![];
        let mut connections_vec: Vec<*mut FFIConnection> = vec![];

        for s in net.elements.into_iter() {
            let c_str = CString::new(s.name()).unwrap();
            elements_vec.push(c_str);
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
            let conn = Box::new(FFIConnection { from: s1.as_ptr(), to: s2.as_ptr() });
            let conn = Box::into_raw(conn);

            connections_vec.push(conn);

            std::mem::forget(s1);
            std::mem::forget(s2);
        }

        let slice = FFIBoxedSlice {
            elements: c_char_vec.as_ptr(),
            len_elements: c_char_vec.len(),
            connections: connections_vec.as_ptr(),
            len_connections: connections_vec.len()
        };

        std::mem::forget(c_char_vec);
        std::mem::forget(connections_vec);
        slice
    }
}

#[repr(C)]
pub struct FFIMatrix {
    rows_len: usize,
    cols_len: usize,
    matrix: *const c_long
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
    matrix: *const c_long
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
pub struct CommonResult {
    petri_net: *mut FFIBoxedSlice,
    c_matrix: *mut FFIMatrix,
    d_input: *mut FFINamedMatrix,
    d_output: *mut FFINamedMatrix,
    lbf_matrix: *mut FFINamedMatrix
}


#[no_mangle]
pub unsafe extern "C" fn split(v: *mut PetriNet) -> *mut CommonResult {
    let v = &mut *v;

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

    let res = synthesis(parts);

    // VT => max(zt)
    // VP => max(zp)
    // D' => VP(N) * VT(N)
    // C * D' => D''
    // D'' убираем эквивалентные позиции и переходы


    let boxed_ffi_slice = Box::new(FFIBoxedSlice::from_net(res.result_net));
    let boxed_ffi_matrix = Box::new(FFIMatrix::from_matrix(res.c_matrix));
    let boxed_d_input = Box::new(FFINamedMatrix::from_matrix(res.d_input));
    let boxed_d_output = Box::new(FFINamedMatrix::from_matrix(res.d_output));
    let boxed_lbf_matrix = Box::new(FFINamedMatrix::from_matrix(res.lbf_matrix));

    Box::into_raw(Box::new(
        CommonResult {
            petri_net: Box::into_raw(boxed_ffi_slice),
            c_matrix: Box::into_raw(boxed_ffi_matrix),
            d_input: Box::into_raw(boxed_d_input),
            d_output: Box::into_raw(boxed_d_output),
            lbf_matrix: Box::into_raw(boxed_lbf_matrix)
        }
    ))

}