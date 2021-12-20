#![feature(let_else)]
#![feature(drain_filter)]

extern crate nalgebra;

use std::collections::HashMap;
use std::ffi::CString;

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

#[no_mangle]
pub unsafe extern "C" fn split(v: *mut PetriNet) {
    let v = &mut *v;

    while let Some(l) = v.get_loop() {
        v.remove_part(&l);
        println!("{:?}", l);
    }

    while let Some(p) = v.get_part() {
        v.remove_part(&p);
        println!("{:?}", p);
    }
}