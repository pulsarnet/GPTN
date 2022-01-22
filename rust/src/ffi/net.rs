// extern "C" FFIPosition* add_position(PetriNet*);
// extern "C" FFIPosition* get_position(unsigned long);
// extern "C" unsigned long position_index(FFIPosition*);
// extern "C" void remove_position(FFIPosition*);

use ffi::vec::CVec;
use ::{PetriNet, Vertex};
use net::Connection;

#[no_mangle]
pub extern "C" fn create_net() -> *mut PetriNet {
    Box::into_raw(Box::new(PetriNet::new()))
}

#[no_mangle]
pub unsafe extern "C" fn net_positions(net: &mut PetriNet, ret: &mut CVec<*const Vertex>) {
    let result = net.elements
        .iter()
        .filter(|p| p.is_position())
        .map(|p| p as *const Vertex).collect::<Vec<_>>();

    core::ptr::write_unaligned(ret, CVec::from(result));
}

#[no_mangle]
pub unsafe extern "C" fn net_transitions(net: &mut PetriNet, ret: &mut CVec<*const Vertex>) {
    let result = net.elements
        .iter()
        .filter(|p| p.is_transition())
        .map(|p| p as *const Vertex).collect::<Vec<_>>();

    core::ptr::write_unaligned(ret, CVec::from(result));
}

#[no_mangle]
pub unsafe extern "C" fn net_connections(net: &mut PetriNet, ret: &mut CVec<*const Connection>) {
    core::ptr::write_unaligned(ret, CVec::from(net.connections.iter().map(|c| c as *const Connection).collect::<Vec<_>>()));
}

#[no_mangle]
pub unsafe extern "C" fn del(v: *mut PetriNet) {
    Box::from_raw(v);
}

#[no_mangle]
pub unsafe extern "C" fn add_position(net: &mut PetriNet) -> *const Vertex {
    let added = net.add_position(net.next_position_index());
    Box::into_raw(Box::new(added))
}

#[no_mangle]
pub unsafe extern "C" fn add_position_with(net: &mut PetriNet, index: usize) -> *const Vertex {
    let added = net.add_position(index as u64);
    Box::into_raw(Box::new(added))
}

#[no_mangle]
pub unsafe extern "C" fn get_position(net: &mut PetriNet, index: usize) -> *const Vertex {
    let position = net.get_position(index as u64).unwrap();
    position as *const Vertex
}

#[no_mangle]
pub unsafe extern "C" fn remove_position(net: &mut PetriNet, position: *mut Vertex) {
    net.remove_position((&*position).index());
    Box::from_raw(position);
}

#[no_mangle]
pub unsafe extern "C" fn add_transition(net: &mut PetriNet) -> *const Vertex {
    let added = net.add_transition(net.next_transition_index());
    Box::into_raw(Box::new(added))
}

#[no_mangle]
pub extern "C" fn add_transition_with(net: &mut PetriNet, index: u64) -> *const Vertex {
    let added = net.add_transition(index);
    Box::into_raw(Box::new(added))
}

#[no_mangle]
pub extern "C" fn get_transition(net: &mut PetriNet, index: usize) -> *const Vertex {
    let transition = net.get_transition(index as u64).unwrap();
    transition as *const Vertex
}

#[no_mangle]
pub extern "C" fn remove_transition(net: &mut PetriNet, transition: *mut Vertex) {
    net.remove_transition(unsafe { &*transition }.index());
    unsafe { Box::from_raw(transition); }
}

#[no_mangle]
pub unsafe extern "C" fn connect_vertexes(
    net: &mut PetriNet,
    from: *const Vertex,
    to: *const Vertex,
)
{
    net.connect((&*from).clone(), (&*to).clone());
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
        .drain_filter(|c| c.first().eq(from) && c.second().eq(to));
}
