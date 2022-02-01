use std::ffi::{CStr, CString};
use libc::c_char;
use Vertex;

#[repr(C)]
pub enum VertexType {
    Position,
    Transition
}

#[no_mangle]
pub extern "C" fn vertex_index(vertex: *const Vertex) -> u64 {
    unsafe { &*vertex }.index()
}

#[no_mangle]
pub extern "C" fn vertex_markers(vertex: *const Vertex) -> u64 {
    unsafe { &*vertex }.markers()
}

#[no_mangle]
pub extern "C" fn vertex_add_marker(vertex: *mut Vertex) {
    unsafe { &mut *vertex }.add_marker();
}

#[no_mangle]
pub extern "C" fn vertex_remove_marker(vertex: *const Vertex) {
    unsafe { &*vertex }.remove_marker();
}

#[no_mangle]
pub extern "C" fn vertex_get_name(vertex: *const Vertex) -> *const c_char {
    let name = unsafe { &*vertex }.get_name();
    let result = CString::new(name).unwrap();
    let ptr = result.as_ptr();
    std::mem::forget(result);
    ptr
}

#[no_mangle]
pub extern "C" fn vertex_set_name(vertex: *const Vertex, name: *mut c_char) {
    unsafe { &*vertex }.set_name(unsafe { CStr::from_ptr(name) }.to_string_lossy().to_string());
}

#[no_mangle]
pub extern "C" fn vertex_type(vertex: *const Vertex) -> VertexType {
    let vertex = unsafe { &*vertex };
    match vertex.is_position() {
        true => VertexType::Position,
        false => VertexType::Transition
    }
}