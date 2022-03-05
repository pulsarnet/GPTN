// extern "C" FFIPosition* add_position(PetriNet*);
// extern "C" FFIPosition* get_position(unsigned long);
// extern "C" unsigned long position_index(FFIPosition*);
// extern "C" void remove_position(FFIPosition*);

use ffi::vec::CVec;
use ::{PetriNet, Vertex};
use ffi::matrix::CNamedMatrix;
use net::Connection;
use net::vertex::{VertexIndex, VertexType};

#[no_mangle]
pub extern "C" fn create_net() -> *mut PetriNet {
    Box::into_raw(Box::new(PetriNet::new()))
}

#[no_mangle]
pub extern "C" fn net_get_vertex(net: &PetriNet, index: VertexIndex) -> *const Vertex {
    match index.type_ {
        VertexType::Position => net.get_position(index.id).unwrap() as *const Vertex,
        VertexType::Transition => net.get_transition(index.id).unwrap() as *const Vertex
    }
}

#[no_mangle]
pub unsafe extern "C" fn net_positions(net: &mut PetriNet, ret: &mut CVec<*const Vertex>) {
    let result = net.positions
        .values()
        .map(|p| p as *const Vertex).collect::<Vec<_>>();

    core::ptr::write_unaligned(ret, CVec::from(result));
}

#[no_mangle]
pub unsafe extern "C" fn net_transitions(net: &mut PetriNet, ret: &mut CVec<*const Vertex>) {
    let result = net.transitions
        .values()
        .map(|p| p as *const Vertex).collect::<Vec<_>>();

    core::ptr::write_unaligned(ret, CVec::from(result));
}

#[no_mangle]
pub unsafe extern "C" fn net_connections(net: &mut PetriNet, ret: &mut CVec<*const Connection>) {
    core::ptr::write_unaligned(ret, CVec::from(net.connections.iter().map(|c| c as *const Connection).collect::<Vec<_>>()));
}

#[no_mangle]
pub unsafe extern "C" fn delete_net(v: *mut PetriNet) {
    Box::from_raw(v);
}

#[no_mangle]
pub unsafe extern "C" fn add_position(net: &mut PetriNet) -> *const Vertex {
    net.add_position(net.next_position_index()) as *const Vertex
}

#[no_mangle]
pub unsafe extern "C" fn add_position_with(net: &mut PetriNet, index: usize) -> *const Vertex {
    net.add_position(index) as *const Vertex
}

#[no_mangle]
pub extern "C" fn add_position_with_parent(net: &mut PetriNet, index: usize, parent: usize) -> *const Vertex {
    let mut vertex = Vertex::position(index);
    vertex.set_parent(VertexIndex { type_: VertexType::Position, id: parent});
    net.insert(vertex) as *const Vertex
}

#[no_mangle]
pub unsafe extern "C" fn get_position(net: &mut PetriNet, index: usize) -> *const Vertex {
    net.get_position(index).unwrap() as *const Vertex
}

#[no_mangle]
pub unsafe extern "C" fn remove_position(net: &mut PetriNet, position: *mut Vertex) {
    net.remove_position((&*position).index().id);
}

#[no_mangle]
pub unsafe extern "C" fn add_transition(net: &mut PetriNet) -> *const Vertex {
    net.add_transition(net.next_transition_index()) as *const Vertex
}

#[no_mangle]
pub extern "C" fn add_transition_with(net: &mut PetriNet, index: usize) -> *const Vertex {
    net.add_transition(index) as *const Vertex
}

#[no_mangle]
pub extern "C" fn add_transition_with_parent(net: &mut PetriNet, index: usize, parent: usize) -> *const Vertex {
    let mut vertex = Vertex::transition(index);
    vertex.set_parent(VertexIndex { type_: VertexType::Transition, id: parent});
    net.insert(vertex) as *const Vertex
}

#[no_mangle]
pub extern "C" fn get_transition(net: &mut PetriNet, index: usize) -> *const Vertex {
    let transition = net.get_transition(index).unwrap();
    transition as *const Vertex
}

#[no_mangle]
pub extern "C" fn remove_transition(net: &mut PetriNet, transition: *mut Vertex) {
    net.remove_transition(unsafe { &*transition }.index().id);
}

#[no_mangle]
pub unsafe extern "C" fn connect_vertexes(
    net: &mut PetriNet,
    from: *const Vertex,
    to: *const Vertex,
)
{
    net.connect((&*from).index(), (&*to).index());
}

#[no_mangle]
pub unsafe extern "C" fn remove_connection(
    net: &mut PetriNet,
    from: *const Vertex,
    to: *const Vertex,
)
{
    let from = &*from;
    let to = &*to;
    net.connections
        .drain_filter(|c| c.first().eq(&from.index()) && c.second().eq(&to.index()));
}

#[no_mangle]
pub unsafe extern "C" fn petri_net_as_matrix(net: &PetriNet) -> *const CNamedMatrix {
    let matrix = net.as_matrix().0;
    Box::into_raw(Box::new(matrix))
}
