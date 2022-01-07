#![feature(let_else)]
#![feature(drain_filter)]
#![feature(option_result_contains)]


extern crate nalgebra;

use std::ffi::CString;
use std::os::raw::c_char;

mod net;

pub mod core;

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


#[no_mangle]
pub unsafe extern "C" fn split(v: *mut PetriNet) -> *mut FFIBoxedSlice {
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

    let res = synthesis(parts).downgrade_transitions().downgrade_pos();

    let boxed_result = Box::new(FFIBoxedSlice::from_net(res));
    //std::mem::forget(result);

    Box::into_raw(boxed_result)
}