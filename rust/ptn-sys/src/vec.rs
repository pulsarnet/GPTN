// Source: https://github.com/dtolnay/cxx/blob/09b4c0c1e4bcd32d37f75404edbb5032dc60d685/src/rust_vec.rs
use std::{mem, ptr};
use std::ffi::c_void;
use std::marker::PhantomData;
use std::mem::MaybeUninit;

#[repr(C)]
pub struct RustVec<T> {
    repr: [MaybeUninit<usize>; mem::size_of::<Vec<c_void>>() / mem::size_of::<usize>()],
    marker: PhantomData<Vec<T>>
}

impl<T> RustVec<T> {
    pub fn new() -> Self {
        Self::from(Vec::new())
    }

    pub fn into_vec(self) -> Vec<T> {
        unsafe { mem::transmute::<RustVec<T>, Vec<T>>(self) }
    }

    pub fn as_vec(&self) -> &Vec<T> {
        unsafe { &*(self as *const RustVec<T> as *const Vec<T>) }
    }

    pub fn as_mut_vec(&mut self) -> &mut Vec<T> {
        unsafe { &mut *(self as *mut RustVec<T> as *mut Vec<T>) }
    }

    pub fn len(&self) -> usize {
        self.as_vec().len()
    }

    pub fn capacity(&self) -> usize {
        self.as_vec().capacity()
    }

    pub fn as_ptr(&self) -> *const T {
        self.as_vec().as_ptr()
    }

    pub fn reserve_total(&mut self, new_cap: usize) {
        let vec = self.as_mut_vec();
        if new_cap > vec.capacity() {
            let additional = new_cap - vec.len();
            vec.reserve(additional);
        }
    }

    pub unsafe fn set_len(&mut self, len: usize) {
        unsafe { self.as_mut_vec().set_len(len) }
    }

    pub fn truncate(&mut self, len: usize) {
        self.as_mut_vec().truncate(len);
    }
}

impl<T> From<Vec<T>> for RustVec<T> {
    fn from(v: Vec<T>) -> Self {
        unsafe { mem::transmute::<Vec<T>, RustVec<T>>(v) }
    }
}

impl<T> From<&Vec<T>> for &RustVec<T> {
    fn from(v: &Vec<T>) -> Self {
        unsafe { &*(v as *const Vec<T> as *const RustVec<T>) }
    }
}

impl<T> From<&mut Vec<T>> for &mut RustVec<T> {
    fn from(v: &mut Vec<T>) -> Self {
        unsafe { &mut *(v as *mut Vec<T> as *mut RustVec<T>) }
    }
}

impl<T> Drop for RustVec<T> {
    fn drop(&mut self) {
        unsafe { ptr::drop_in_place(self.as_mut_vec()) }
    }
}
