use std::ops::Deref;
use crate::Vertex;

pub struct Transition(pub Vertex);

impl Deref for Transition {
    type Target = Vertex;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

#[no_mangle]
extern "C" fn transition_index(transition: &Transition) -> usize {
    transition.index() as usize
}