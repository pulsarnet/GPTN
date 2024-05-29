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

macro_rules! rust_vec_shims {
    ($segment:expr, $ty:ty) => {
        const _: () = {
            #[export_name = concat!("ptn$vec$", $segment, "$new")]
            unsafe extern "C" fn __new(this: *mut RustVec<$ty>) {
                ptr::write(this, RustVec::new())
            }

            #[export_name = concat!("ptn$vec$", $segment, "$drop")]
            unsafe extern "C" fn __drop(this: *mut RustVec<$ty>) {
                ptr::drop_in_place(this)
            }

            #[export_name = concat!("ptn$vec$", $segment, "$len")]
            unsafe extern "C" fn __len(this: *const RustVec<$ty>) -> usize {
                (&*this).len()
            }

            #[export_name = concat!("ptn$vec$", $segment, "$capacity")]
            unsafe extern "C" fn __capacity(this: *const RustVec<$ty>) -> usize {
                (&*this).capacity()
            }

            #[export_name = concat!("ptn$vec$", $segment, "$resize")]
            unsafe extern "C" fn __resize(this: *mut RustVec<$ty>, len: usize) {
                (&mut *this).set_len(len)
            }

            #[export_name = concat!("ptn$vec$", $segment, "$reserve")]
            unsafe extern "C" fn __reserve(this: *mut RustVec<$ty>, capacity: usize) {
                (&mut *this).reserve_total(capacity)
            }

            #[export_name = concat!("ptn$vec$", $segment, "$data")]
            unsafe extern "C" fn __data(this: *const RustVec<$ty>) -> *const $ty {
                (&*this).as_ptr()
            }
        };
    };
}

macro_rules! rust_vec {
    ($t:ident) => {
        rust_vec_shims! { stringify!($t), $t }
    }
}

rust_vec!(bool);
rust_vec!(u8);
rust_vec!(u16);
rust_vec!(u32);
rust_vec!(u64);
rust_vec!(i8);
rust_vec!(i16);
rust_vec!(i32);
rust_vec!(i64);
rust_vec!(usize);

rust_vec_shims!("const_vertex_ptr", *const ptn::net::Vertex);
rust_vec_shims!("const_directed_ptr", *const ptn::net::DirectedEdge);
rust_vec_shims!("const_inhibitor_ptr", *const ptn::net::InhibitorEdge);
rust_vec_shims!("const_marking_ptr", *const ptn::modules::reachability::Marking);
rust_vec_shims!("vertex_index", *const ptn::net::vertex::VertexIndex);