use libc::c_char;
use net::vertex::{VertexIndex, VertexType};
use std::ffi::{CStr, CString};
use Vertex;

#[no_mangle]
pub extern "C" fn vertex_index(vertex: *const Vertex) -> VertexIndex {
    unsafe { &*vertex }.index()
}

#[no_mangle]
pub extern "C" fn vertex_markers(vertex: *const Vertex) -> usize {
    unsafe { &*vertex }.markers()
}

#[no_mangle]
pub extern "C" fn vertex_add_marker(vertex: *mut Vertex) {
    unsafe { &mut *vertex }.add_marker();
}

#[no_mangle]
pub extern "C" fn vertex_remove_marker(vertex: *mut Vertex) {
    unsafe { &mut *vertex }.remove_marker();
}

#[no_mangle]
pub extern "C" fn vertex_label(vertex: *const Vertex, show_parent: bool) -> *const c_char {
    let name = unsafe { &*vertex }.label(show_parent);
    let result = CString::new(name).unwrap();
    let ptr = result.as_ptr();
    std::mem::forget(result);
    ptr
}

#[no_mangle]
pub extern "C" fn vertex_set_label(vertex: *mut Vertex, name: *mut c_char) {
    unsafe { &mut *vertex }.set_label(
        unsafe { CStr::from_ptr(name) }
            .to_string_lossy()
            .to_string(),
    );
}

#[no_mangle]
pub extern "C" fn vertex_type(vertex: *const Vertex) -> VertexType {
    let vertex = unsafe { &*vertex };
    match vertex.is_position() {
        true => VertexType::Position,
        false => VertexType::Transition,
    }
}

#[no_mangle]
pub extern "C" fn vertex_parent(vertex: &Vertex) -> usize {
    vertex.get_parent().map_or(0, |parent| parent.id as usize)
}

#[no_mangle]
pub extern "C" fn vertex_set_parent(vertex: &mut Vertex, index: VertexIndex) {
    vertex.set_parent(index);
}

#[no_mangle]
pub extern "C" fn vertex_set_markers(vertex: &mut Vertex, markers: usize) {
    vertex.set_markers(markers)
}
