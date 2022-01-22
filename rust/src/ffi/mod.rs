pub mod net;
pub mod position;
pub mod transition;
pub mod vec;

use ffi::position::Position;

#[no_mangle]
pub extern "C" fn size_of_position() -> usize {
    std::mem::size_of::<Position>()
}

#[no_mangle]
pub extern "C" fn size_of_transition() -> usize {
    std::mem::size_of::<Position>()
}