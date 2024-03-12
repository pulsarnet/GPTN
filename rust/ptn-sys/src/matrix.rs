use std::ffi::c_void;
use std::fmt::Debug;
use std::marker::PhantomData;
use std::{mem, ptr};
use std::mem::MaybeUninit;
use num_traits::Zero;
use ptn::nalgebra::{DMatrix, Scalar};

#[repr(C)]
pub(crate) struct RustMatrix<T> {
    repr: [MaybeUninit<usize>; mem::size_of::<DMatrix<c_void>>() / mem::size_of::<usize>()],
    marker: PhantomData<DMatrix<T>>,
}

impl<T> RustMatrix<T>
    where
        T: Scalar + Debug + Zero
{
    pub fn new(n: usize, m: usize) -> Self {
        Self::from(DMatrix::<T>::zeros(n, m))
    }

    #[allow(unused)]
    pub fn into_matrix(self) -> DMatrix<T> {
        unsafe { mem::transmute::<RustMatrix<T>, DMatrix<T>>(self) }
    }

    pub fn as_matrix(&self) -> &DMatrix<T> {
        unsafe { &*(self as *const RustMatrix<T> as *const DMatrix<T>) }
    }

    #[allow(unused)]
    pub fn as_mut_matrix(&mut self) -> &mut DMatrix<T> {
        unsafe { &mut *(self as *mut RustMatrix<T> as *mut DMatrix<T> )}
    }

    pub fn nrows(&self) -> usize {
        self.as_matrix().nrows()
    }

    pub fn ncols(&self) -> usize {
        self.as_matrix().ncols()
    }

    pub fn index(&self, row: usize, col: usize) -> *const T {
        unsafe { self.as_matrix().get_unchecked((row, col)) as *const T }
    }
}

impl<T> From<DMatrix<T>> for RustMatrix<T> {
    fn from(v: DMatrix<T>) -> Self {
        unsafe { mem::transmute::<DMatrix<T>, RustMatrix<T>>(v) }
    }
}

macro_rules! rust_matrix_shims {
    ($segment:expr, $ty:ty) => {
        const _: () = {
            #[export_name = concat!("ptn$matrix$", $segment, "$new")]
            unsafe extern "C" fn __new(this: *mut RustMatrix<$ty>, n: usize, m: usize) {
                ptr::write(this, RustMatrix::new(n, m))
            }

            #[export_name = concat!("ptn$matrix$", $segment, "$drop")]
            unsafe extern "C" fn __drop(this: *mut RustMatrix<$ty>) {
                ptr::drop_in_place(this)
            }

            #[export_name = concat!("ptn$matrix$", $segment, "$nrows")]
            unsafe extern "C" fn __nrows(this: *const RustMatrix<$ty>) -> usize {
                (&*this).nrows()
            }

            #[export_name = concat!("ptn$matrix$", $segment, "$ncols")]
            unsafe extern "C" fn __ncols(this: *const RustMatrix<$ty>) -> usize {
                (&*this).ncols()
            }

            #[export_name = concat!("ptn$matrix$", $segment, "$index")]
            unsafe extern "C" fn __index(this: *const RustMatrix<$ty>, row: usize, col: usize) -> *const $ty {
                (&*this).index(row, col)
            }
        };
    };
}

macro_rules! rust_matrix {
    ($t:ident) => {
        rust_matrix_shims! { stringify!($t), $t }
    }
}

rust_matrix!(i32);
rust_matrix!(i64);