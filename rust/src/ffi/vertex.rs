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
pub extern "C" fn vertex_type(vertex: *const Vertex) -> VertexType {
    let vertex = unsafe { &*vertex };
    match vertex.is_position() {
        true => VertexType::Position,
        false => VertexType::Transition
    }
}