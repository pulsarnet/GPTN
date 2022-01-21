use std::ops::Deref;
use crate::Vertex;

pub struct Position(pub Vertex);

impl Deref for Position {
    type Target = Vertex;

    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

#[no_mangle]
extern "C" fn position_index(position: &Position) -> usize {
    position.index() as usize
}

#[no_mangle]
extern "C" fn position_markers(position: &Position) -> usize {
    position.markers() as usize
}

#[no_mangle]
extern "C" fn position_add_marker(position: &Position) {
    position.add_marker();
}


#[no_mangle]
extern "C" fn position_remove_marker(position: &mut Position) {
    position.remove_marker();
}