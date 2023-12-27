extern crate chrono;
extern crate indexmap;
extern crate libc;
pub extern crate nalgebra;
extern crate ndarray;
extern crate ndarray_linalg;
extern crate num;
extern crate num_traits;
extern crate rand;
extern crate rayon;
extern crate tracing;
extern crate tracing_subscriber;
extern crate intel_mkl_src;

pub use nalgebra as alg;
use std::fmt::Debug;
use std::ops::{Deref, DerefMut};
use nalgebra::DMatrix;

pub mod modules;
pub mod net;

mod core;


#[derive(Debug, Clone)]
pub struct CMatrix {
    inner: DMatrix<i32>,
}

impl Deref for CMatrix {
    type Target = DMatrix<i32>;

    fn deref(&self) -> &Self::Target {
        &self.inner
    }
}

impl DerefMut for CMatrix {
    fn deref_mut(&mut self) -> &mut Self::Target {
        &mut self.inner
    }
}

impl From<DMatrix<i32>> for CMatrix {
    fn from(inner: DMatrix<i32>) -> Self {
        Self { inner }
    }
}

#[cfg(test)]
mod tests {
    use modules::synthesis::SynthesisProgram;

    use super::*;

    #[test]
    fn test_synthesis_program() {
        let program = SynthesisProgram::new_with(vec![0, 1, 2, 3, 0, 1, 2, 3, 4, 5, 6, 7], 4);
        let pos_indexes = vec![
            Vertex::position(1),
            Vertex::position(2),
            Vertex::position(3),
            Vertex::position(4),
            Vertex::position(5),
            Vertex::position(6),
            Vertex::position(7),
            Vertex::position(8),
        ];

        let tran_indexes = vec![
            Vertex::transition(1),
            Vertex::transition(2),
            Vertex::transition(3),
            Vertex::transition(4),
        ];

        dbg!(program.sets(&pos_indexes, &tran_indexes));
    }
}
