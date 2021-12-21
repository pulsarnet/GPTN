#![feature(let_else)]
#![feature(drain_filter)]

extern crate nalgebra;

use std::collections::HashMap;
use std::ffi::CString;
use std::os::raw::c_char;

mod net;

pub mod core;

use net::{PetriNet, Vertex};

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
pub unsafe extern "C" fn count(v: *mut PetriNet) -> std::os::raw::c_ulong {
    0
}

#[no_mangle]
pub unsafe extern "C" fn add_position(v: *mut PetriNet, index: u64) {
    let v = &mut *v;
    v.add(Vertex::position(index));
}

#[no_mangle]
pub unsafe extern "C" fn add_transition(v: *mut PetriNet, index: u64) {
    let v = &mut *v;
    v.add(Vertex::transition(index));
}

#[no_mangle]
pub unsafe extern "C" fn connect_p(v: *mut PetriNet, position: u64, transition: u64) {
    let mut v = &mut *v;

    let position = Vertex::position(position);
    let transition = Vertex::transition(transition);

    v.connect(&position, &transition);
}

#[no_mangle]
pub unsafe extern "C" fn connect_t(v: *mut PetriNet, transition: u64, position: u64) {
    let v = &mut *v;

    let position = Vertex::position(position);
    let transition = Vertex::transition(transition);

    v.connect(&transition, &position);
}

#[repr(C)]
pub struct FFIBoxedSlice {
    ptr: *const *const c_char,
    partition_type: *const c_char,
    len: usize
}

impl FFIBoxedSlice {
    fn from_vec(vec: Vec<Vertex>, partition_type: String) -> Self {
        let mut cstr_vec: Vec<CString> = vec![];
        for s in vec {
            let c_str = CString::new(s.name()).unwrap();
            cstr_vec.push(c_str);
        }
        cstr_vec.shrink_to_fit();

        let mut c_char_vec: Vec<*const c_char> = vec![];
        for s in cstr_vec {
            c_char_vec.push(s.as_ptr());
            std::mem::forget(s);
        }

        let partition_type = CString::new(partition_type.as_str()).unwrap();

        let slice = FFIBoxedSlice {
            ptr: c_char_vec.as_ptr(),
            partition_type: partition_type.as_ptr() as *const c_char,
            len: c_char_vec.len()
        };

        std::mem::forget(c_char_vec);
        std::mem::forget(partition_type);
        slice
    }
}

#[repr(C)]
pub struct SplitNet {
    ptr: *const *mut FFIBoxedSlice,
    len: usize,
}

#[no_mangle]
pub unsafe extern "C" fn split(v: *mut PetriNet) -> *mut SplitNet {
    let v = &mut *v;

    let mut result = Vec::new();

    println!("V: {:?}", v);

    while let Some(l) = v.get_loop() {
        println!("LOOP: {:?}", l);
        v.remove_part(&l);
        let slice = Box::new(FFIBoxedSlice::from_vec(l, "loop".to_string()));
        result.push(Box::into_raw(slice));
    }

    while let Some(p) = v.get_part() {
        println!("PART: {:?}", p);
        v.remove_part(&p);
        let slice = Box::new(FFIBoxedSlice::from_vec(p, "part".to_string()));
        result.push(Box::into_raw(slice));
    }

    let boxed_result = Box::new(SplitNet { ptr: result.as_ptr() as *const *mut FFIBoxedSlice, len: result.len() });
    std::mem::forget(result);

    Box::into_raw(boxed_result)
}