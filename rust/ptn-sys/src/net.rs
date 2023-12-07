use crate::vec::RustVec;

use ptn::net::PetriNet;
use ptn::net::vertex::{VertexIndex, VertexType, Vertex};
use ptn::net::Connection;


#[export_name = "ptn$net$make"]
extern "C" fn make_net() -> *mut PetriNet {
    Box::into_raw(Box::new(PetriNet::new()))
}

#[export_name = "ptn$net$vertex"]
extern "C" fn net_get_vertex(net: &PetriNet, index: VertexIndex) -> *const Vertex {
    match index.type_ {
        VertexType::Position => net.get_position(index.id).unwrap() as *const Vertex,
        VertexType::Transition => net.get_transition(index.id).unwrap() as *const Vertex,
    }
}

#[export_name = "ptn$net$positions"]
extern "C" fn net_positions(net: &mut PetriNet, ret: &mut RustVec<*const Vertex>) {
    let result = net
        .positions()
        .values()
        .map(|p| p as *const Vertex)
        .collect::<Vec<_>>();

    unsafe { core::ptr::write_unaligned(ret, RustVec::from(result)) };
}

#[export_name = "ptn$net$transitions"]
extern "C" fn net_transitions(net: &mut PetriNet, ret: &mut RustVec<*const Vertex>) {
    let result = net
        .transitions()
        .values()
        .map(|p| p as *const Vertex)
        .collect::<Vec<_>>();

    unsafe { core::ptr::write_unaligned(ret, RustVec::from(result)) };
}

#[export_name = "ptn$net$edges"]
unsafe extern "C" fn net_edges(net: &mut PetriNet, ret: &mut RustVec<*const Connection>) {
    core::ptr::write_unaligned(
        ret,
        RustVec::from(
            net.connections()
                .iter()
                .map(|c| c as *const Connection)
                .collect::<Vec<_>>(),
        ),
    );
}

#[export_name = "ptn$net$clear"]
extern "C" fn net_clear(v: &mut PetriNet) {
    (*v) = PetriNet::new();
}

#[export_name = "ptn$net$drop"]
unsafe extern "C" fn delete_net(v: *mut PetriNet) {
    let _ = Box::from_raw(v);
}

#[export_name = "ptn$net$add_position"]
extern "C" fn add_position(net: &mut PetriNet) -> *const Vertex {
    net.add_position(net.next_position_index()) as *const Vertex
}

#[export_name = "ptn$net$insert_position"]
extern "C" fn insert_position(net: &mut PetriNet, index: usize, parent: isize) -> *const Vertex {
    let mut vertex = Vertex::position(index);
    if parent >= 0 {
        vertex.set_parent(parent as usize)
    }
    net.add_position(index) as *const Vertex
}

#[export_name = "ptn$net$position"]
extern "C" fn get_position(net: &mut PetriNet, index: usize) -> *const Vertex {
    net.get_position(index).unwrap() as *const Vertex
}

#[export_name = "ptn$net$remove"]
extern "C" fn remove(net: &mut PetriNet, vertex: VertexIndex) {
    net.remove(vertex);
}

#[export_name = "ptn$net$add_transition"]
extern "C" fn add_transition(net: &mut PetriNet) -> *const Vertex {
    net.add_transition(net.next_transition_index()) as *const Vertex
}

#[export_name = "ptn$net$insert_transition"]
extern "C" fn insert_transition(net: &mut PetriNet, index: usize, parent: isize) -> *const Vertex {
    let mut vertex = Vertex::transition(index);
    if parent >= 0 {
        vertex.set_parent(parent as usize)
    }
    net.add_transition(index) as *const Vertex
}

#[export_name = "ptn$net$transition"]
extern "C" fn get_transition(net: &mut PetriNet, index: usize) -> *const Vertex {
    let transition = net.get_transition(index).unwrap();
    transition as *const Vertex
}

#[export_name = "ptn$net$make_edge"]
extern "C" fn make_edge(net: &mut PetriNet, from: VertexIndex, to: VertexIndex) {
    net.connect(from, to, 1);
}

/// Удаляет все соединения вершин, которые выходят из from и входят в to
#[export_name = "ptn$net$remove_edge"]
extern "C" fn remove_edge(net: &mut PetriNet, from: VertexIndex, to: VertexIndex) {
    net.disconnect(from, to);
}
