// extern "C" FFIPosition* add_position(PetriNet*);
// extern "C" FFIPosition* get_position(unsigned long);
// extern "C" unsigned long position_index(FFIPosition*);
// extern "C" void remove_position(FFIPosition*);

use ffi::matrix::CNamedMatrix;
use ffi::vec::CVec;
use modules::reachability::{Reachability, ReachabilityTree};
use net::vertex::{VertexIndex, VertexType};
use net::Connection;
use {PetriNet, Vertex};

#[no_mangle]
pub extern "C" fn create_net() -> *mut PetriNet {
    Box::into_raw(Box::new(PetriNet::new()))
}

#[no_mangle]
pub extern "C" fn net_get_vertex(net: &PetriNet, index: VertexIndex) -> *const Vertex {
    match index.type_ {
        VertexType::Position => net.get_position(index.id).unwrap() as *const Vertex,
        VertexType::Transition => net.get_transition(index.id).unwrap() as *const Vertex,
    }
}

#[no_mangle]
pub unsafe extern "C" fn net_positions(net: &mut PetriNet, ret: &mut CVec<*const Vertex>) {
    let result = net
        .positions()
        .values()
        .map(|p| p as *const Vertex)
        .collect::<Vec<_>>();

    core::ptr::write_unaligned(ret, CVec::from(result));
}

#[no_mangle]
pub unsafe extern "C" fn net_transitions(net: &mut PetriNet, ret: &mut CVec<*const Vertex>) {
    let result = net
        .transitions()
        .values()
        .map(|p| p as *const Vertex)
        .collect::<Vec<_>>();

    core::ptr::write_unaligned(ret, CVec::from(result));
}

#[no_mangle]
pub unsafe extern "C" fn net_connections(net: &mut PetriNet, ret: &mut CVec<*const Connection>) {
    core::ptr::write_unaligned(
        ret,
        CVec::from(
            net.connections()
                .iter()
                .map(|c| c as *const Connection)
                .collect::<Vec<_>>(),
        ),
    );
}

#[no_mangle]
pub unsafe extern "C" fn clear_net(v: &mut PetriNet) {
    (*v) = PetriNet::new();
}

#[no_mangle]
pub unsafe extern "C" fn delete_net(v: *mut PetriNet) {
    let _ = Box::from_raw(v);
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
pub extern "C" fn add_position_with_parent(
    net: &mut PetriNet,
    index: usize,
    parent: usize,
) -> *const Vertex {
    let mut vertex = Vertex::position(index);
    vertex.set_parent(VertexIndex {
        type_: VertexType::Position,
        id: parent,
    });
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
pub extern "C" fn add_transition_with_parent(
    net: &mut PetriNet,
    index: usize,
    parent: usize,
) -> *const Vertex {
    let mut vertex = Vertex::transition(index);
    vertex.set_parent(VertexIndex {
        type_: VertexType::Transition,
        id: parent,
    });
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
) {
    net.connect((&*from).index(), (&*to).index(), 1);
}

/// Удаляет все соединения вершин, которые выходят из from и входят в to
#[no_mangle]
pub unsafe extern "C" fn remove_connection(
    net: &mut PetriNet,
    from: *const Vertex,
    to: *const Vertex,
) {
    // todo: pass VertexIndex
    let from = &*from;
    let to = &*to;
    net.disconnect(from.index(), to.index());
}

#[no_mangle]
pub unsafe extern "C" fn petri_net_as_matrix(
    net: &PetriNet,
    matrix1: &mut *const CNamedMatrix,
    matrix2: &mut *const CNamedMatrix,
) {
    let (matrix_1, matrix_2) = net.incidence_matrix();
    *matrix1 = Box::into_raw(Box::new(matrix_1));
    *matrix2 = Box::into_raw(Box::new(matrix_2));
}

#[no_mangle]
pub unsafe extern "C" fn petri_net_get_connection(
    net: &mut PetriNet,
    from: *const Vertex,
    to: *const Vertex,
) -> *mut Connection {
    let from = &*from;
    let to = &*to;
    let result = net
        .connections()
        .iter()
        .find(|c| c.first().eq(&from.index()) && c.second().eq(&to.index()));

    match result {
        Some(c) => c as *const Connection as *mut Connection,
        None => std::ptr::null_mut(),
    }
}

#[no_mangle]
pub unsafe extern "C" fn petri_net_input_positions(net: &PetriNet) -> usize {
    net.input_positions()
}

#[no_mangle]
pub unsafe extern "C" fn petri_net_output_positions(net: &PetriNet) -> usize {
    net.output_positions()
}

#[no_mangle]
pub unsafe extern "C" fn petri_net_connection_weight(
    net: &PetriNet,
    a: &Vertex,
    b: &Vertex,
) -> usize {
    net.connections()
        .iter()
        .find(|conn| conn.first() == a.index() && conn.second() == b.index())
        .map_or(0, |conn| conn.weight())
}

#[no_mangle]
pub extern "C" fn petri_net_reachability(net: &PetriNet) -> *mut ReachabilityTree {
    let reachability = Reachability::new(net);
    Box::into_raw(Box::new(reachability.compute()))
}
