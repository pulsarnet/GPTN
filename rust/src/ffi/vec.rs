use crate::modules::reachability::Marking;
use net::vertex::VertexIndex;
use net::Connection;
use std::{mem, ptr};
use {PetriNet, Vertex};

#[repr(C)]
pub struct CVec<T> {
    ptr: ptr::NonNull<T>,
    len: usize,
    cap: usize,
}

impl<T> From<Vec<T>> for CVec<T> {
    fn from(v: Vec<T>) -> Self {
        let mut v = mem::ManuallyDrop::new(v);
        Self {
            ptr: unsafe { ptr::NonNull::new_unchecked(v.as_mut_ptr()).into() },
            len: v.len(),
            cap: v.capacity(),
        }
    }
}

impl<T> Into<Vec<T>> for CVec<T> {
    fn into(self) -> Vec<T> {
        let mut this = mem::ManuallyDrop::new(self);
        unsafe { Vec::from_raw_parts(this.ptr.as_mut(), this.len, this.cap) }
    }
}

impl<T> Drop for CVec<T> {
    fn drop(&mut self) {
        unsafe { drop::<Vec<T>>(ptr::read(self).into()) }
    }
}

macro_rules! generate_vec_type {
    ($t:ty) => {
        generate_vec_type!($t, $t);
    };
    ($t:ty, *const) => {
        generate_vec_type!($t, *const $t);
    };
    ($t:ty, $spec:ty) => {
        paste::item! {
            #[no_mangle]
            pub unsafe extern "C" fn [<vec_drop_ $t>](_vec: *mut CVec<$spec>) {
                // todo разобраться с ошибкой удаления (Runtime error)
                //let _ = Box::from_raw(vec);
            }
        }
    };
}

generate_vec_type!(i8);
generate_vec_type!(i16);
generate_vec_type!(i32);
generate_vec_type!(i64);

generate_vec_type!(u8);
generate_vec_type!(u16);
generate_vec_type!(u32);
generate_vec_type!(u64);

generate_vec_type!(Vertex, *const);
generate_vec_type!(Connection, *const);
generate_vec_type!(PetriNet, *const);
generate_vec_type!(Marking, *const);
generate_vec_type!(VertexIndex);
