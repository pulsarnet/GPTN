use libc::c_char;
use ptn::net::vertex::{VertexIndex, VertexType, Vertex};
use std::ffi::{CStr, CString};

#[export_name = "ptn$net$vertex$index"]
extern "C" fn vertex_index(vertex: &Vertex) -> VertexIndex {
    vertex.index()
}

#[export_name = "ptn$net$vertex$markers"]
extern "C" fn vertex_markers(vertex: &Vertex) -> usize {
    vertex.markers()
}

#[export_name = "ptn$net$vertex$add_marker"]
extern "C" fn vertex_add_marker(vertex: &mut Vertex) {
    vertex.add_marker()
}

#[export_name = "ptn$net$vertex$remove_marker"]
extern "C" fn vertex_remove_marker(vertex: &mut Vertex) {
    vertex.remove_marker();
}

#[export_name = "ptn$net$vertex$label"]
extern "C" fn vertex_label(vertex: &Vertex, show_parent: bool) -> *const c_char {
    let name = vertex.label(show_parent);
    let result = CString::new(name).unwrap();
    let ptr = result.as_ptr();
    std::mem::forget(result);
    ptr
}

#[export_name = "ptn$net$vertex$set_label"]
extern "C" fn vertex_set_label(vertex: &mut Vertex, name: *const c_char) {
    vertex.set_label(
        unsafe { CStr::from_ptr(name) }
            .to_string_lossy()
            .to_string(),
    );
}

#[export_name = "ptn$net$vertex$type"]
extern "C" fn vertex_type(vertex: &Vertex) -> VertexType {
    match vertex.is_position() {
        true => VertexType::Position,
        false => VertexType::Transition,
    }
}

#[export_name = "ptn$net$vertex$parent"]
extern "C" fn vertex_parent(vertex: &Vertex) -> usize {
    vertex.get_parent().map_or(0, |parent| parent.id as usize)
}

#[export_name = "ptn$net$vertex$set_parent"]
extern "C" fn vertex_set_parent(vertex: &mut Vertex, index: usize) {
    vertex.set_parent(index);
}

#[export_name = "ptn$net$vertex$set_markers"]
pub extern "C" fn vertex_set_markers(vertex: &mut Vertex, markers: usize) {
    vertex.set_markers(markers)
}
