use net::Connection;
use Vertex;

#[no_mangle]
pub extern "C" fn connection_from(connection: *const Connection) -> *const Vertex {
    unsafe { (&*connection) }.first() as *const Vertex
}

#[no_mangle]
pub extern "C" fn connection_to(connection: *const Connection) -> *const Vertex {
    unsafe { (&*connection) }.second() as *const Vertex
}