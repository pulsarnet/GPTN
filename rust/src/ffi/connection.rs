use net::Connection;
use net::vertex::VertexIndex;

#[no_mangle]
pub extern "C" fn connection_from(connection: *const Connection) -> VertexIndex {
    unsafe { &*connection }.first()
}

#[no_mangle]
pub extern "C" fn connection_to(connection: *const Connection) -> VertexIndex {
    unsafe { &*connection }.second()
}

#[no_mangle]
pub extern "C" fn connection_weight(connection: *const Connection) -> usize {
    unsafe { &*connection }.weight()
}

#[no_mangle]
pub extern "C" fn connection_set_weight(connection: *mut Connection, weight: usize) {
    unsafe { &mut *connection }.set_weight(weight);
}