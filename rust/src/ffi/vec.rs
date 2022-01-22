use std::ffi::CString;
use std::marker::PhantomData;
use std::mem;
use std::mem::MaybeUninit;
use std::ops::Deref;
use libc::{c_char, c_void};
use ffi::position::Position;
use ffi::transition::Transition;

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
pub unsafe extern "C" fn vec_len_position(vec: *mut CVec<Position>) -> usize {
    (&mut *vec).len()
}

#[no_mangle]
pub unsafe extern "C" fn vec_data_position(vec: *mut CVec<Position>) -> *const Position {
    (&mut *vec).raw()
}

#[no_mangle]
pub unsafe extern "C" fn vec_len_transition(vec: *mut CVec<Transition>) -> usize {
    (&mut *vec).len()
}

#[no_mangle]
pub unsafe extern "C" fn vec_data_transition(vec: *mut CVec<Transition>) -> *const Transition {
    (&mut *vec).raw()
}