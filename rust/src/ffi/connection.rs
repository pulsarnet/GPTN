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