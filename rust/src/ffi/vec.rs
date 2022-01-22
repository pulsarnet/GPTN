use std::ffi::CString;
use std::marker::PhantomData;
use std::mem;
use std::mem::MaybeUninit;
use std::ops::Deref;
use libc::{c_char, c_void};
use net::Connection;
use Vertex;

#[repr(C)]
pub struct CVec<T> {
    repr: [MaybeUninit<usize>; mem::size_of::<Vec<c_void>>() / mem::size_of::<usize>()],
    marker: PhantomData<Vec<T>>,
}

impl<T> CVec<T> {
    pub fn new() -> Self {
        Self::from(vec![])
    }

    pub fn as_vec(&self) -> &Vec<T> {
        unsafe { &*(self as *const CVec<T> as *mut Vec<T>) }
    }

    pub fn len(&self) -> usize {
        self.as_vec().len()
    }

    pub fn raw(&self) -> *const T {
        self.as_vec().as_ptr()
    }
}

impl<T> From<Vec<T>> for CVec<T> {
    fn from(v: Vec<T>) -> Self {
        unsafe { std::mem::transmute::<Vec<T>, CVec<T>>(v) }
    }
}

#[no_mangle]
pub unsafe extern "C" fn vec_len_u64(vec: *mut CVec<u64>) -> usize {
    (&mut *vec).len()
}

#[no_mangle]
pub unsafe extern "C" fn vec_data_u64(vec: *mut CVec<u64>) -> *const u64 {
    (&mut *vec).raw()
}

#[no_mangle]
pub unsafe extern "C" fn vec_len_vertex(vec: *mut CVec<*const Vertex>) -> usize {
    (&mut *vec).len()
}

#[no_mangle]
pub unsafe extern "C" fn vec_data_vertex(vec: *mut CVec<*const Vertex>) -> *const *const Vertex {
    (&mut *vec).raw()
}

#[no_mangle]
pub unsafe extern "C" fn vec_len_connection(vec: *mut CVec<*const Connection>) -> usize {
    (&mut *vec).len()
}

#[no_mangle]
pub unsafe extern "C" fn vec_data_connection(vec: *mut CVec<*const Connection>) -> *const *const Connection {
    (&mut *vec).raw()
}