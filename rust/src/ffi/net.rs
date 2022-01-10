// extern "C" FFIPosition* add_position(PetriNet*);
// extern "C" FFIPosition* get_position(unsigned long);
// extern "C" unsigned long position_index(FFIPosition*);
// extern "C" void remove_position(FFIPosition*);

use PetriNet;
use Vertex;

#[repr(C)]
pub struct FFIPosition(Vertex);

#[repr(C)]
pub struct FFITransition(Vertex);

#[no_mangle]
pub extern "C" fn make() -> *mut PetriNet {
    let v = Box::new(PetriNet::new());
    Box::into_raw(v)
}

#[no_mangle]
pub unsafe extern "C" fn del(v: *mut PetriNet) {
    Box::from_raw(v);
}

#[no_mangle]
pub unsafe extern "C" fn add_position(net: *mut PetriNet) -> *mut FFIPosition {
    let net = &mut *net;
    let added = net.add_position(net.next_position_index());
    Box::into_raw(Box::new(FFIPosition(added)))
}

#[no_mangle]
pub unsafe extern "C" fn markers(net: *mut FFIPosition) -> usize {
    let position = &mut *net;
    position.0.markers() as usize
}

#[no_mangle]
pub unsafe extern "C" fn add_marker(net: *mut FFIPosition) {
    let position = &mut *net;
    position.0.add_marker();
}

#[no_mangle]
pub unsafe extern "C" fn remove_marker(net: *mut FFIPosition) {
    let position = &mut *net;
    position.0.remove_marker();
}

#[no_mangle]
pub unsafe extern "C" fn set_markers(net: *mut FFIPosition, markers: usize) {
    let position = &mut *net;
    position.0.set_markers(markers as u64);
}

#[no_mangle]
pub unsafe extern "C" fn add_position_with(net: *mut PetriNet, index: usize) -> *mut FFIPosition {
    let net = &mut *net;
    let added = net.add_position(index as u64);
    Box::into_raw(Box::new(FFIPosition(added)))
}

#[no_mangle]
pub unsafe extern "C" fn get_position(net: *mut PetriNet, index: usize) -> *mut FFIPosition {
    let net = &mut *net;
    let position = net.get_position(index as u64).cloned().unwrap();
    Box::into_raw(Box::new(FFIPosition(position)))
}

#[no_mangle]
pub unsafe extern "C" fn position_index(position: *mut FFIPosition) -> usize {
    let position = &mut *position;
    position.0.index() as usize
}

#[no_mangle]
pub unsafe extern "C" fn remove_position(net: *mut PetriNet, position: *mut FFIPosition) {
    let net = &mut *net;
    let position = &mut *position;

    net.remove_position(position.0.index());

    Box::from_raw(position);
}

#[no_mangle]
pub unsafe extern "C" fn add_transition(net: *mut PetriNet) -> *mut FFITransition {
    let net = &mut *net;
    let added = net.add_transition(net.next_transition_index());
    Box::into_raw(Box::new(FFITransition(added)))
}

#[no_mangle]
pub unsafe extern "C" fn add_transition_with(net: *mut PetriNet, index: u64) -> *mut FFITransition {
    let net = &mut *net;
    let added = net.add_transition(index);
    Box::into_raw(Box::new(FFITransition(added)))
}

#[no_mangle]
pub unsafe extern "C" fn get_transition(net: *mut PetriNet, index: usize) -> *mut FFITransition {
    let net = &mut *net;
    let transition = net.get_transition(index as u64).cloned().unwrap();
    Box::into_raw(Box::new(FFITransition(transition)))
}

#[no_mangle]
pub unsafe extern "C" fn transition_index(transition: *mut FFITransition) -> usize {
    let transition = &mut *transition;
    transition.0.index() as usize
}

#[no_mangle]
pub unsafe extern "C" fn remove_transition(net: *mut PetriNet, transition: *mut FFITransition) {
    let net = &mut *net;
    let transition = &mut *transition;

    net.remove_transition(transition.0.index());

    Box::from_raw(transition);
}

#[no_mangle]
pub unsafe extern "C" fn connect_p(
    v: *mut PetriNet,
    position: *mut FFIPosition,
    transition: *mut FFITransition,
) {
    let v = &mut *v;
    let position = &mut *position;
    let transition = &mut *transition;

    v.connect(position.0.clone(), transition.0.clone());
}

#[no_mangle]
pub unsafe extern "C" fn connect_t(
    v: *mut PetriNet,
    transition: *mut FFITransition,
    position: *mut FFIPosition,
) {
    let v = &mut *v;
    let position = &mut *position;
    let transition = &mut *transition;

    v.connect(transition.0.clone(), position.0.clone());
}

#[no_mangle]
pub unsafe extern "C" fn remove_connection_p(
    v: *mut PetriNet,
    position: *mut FFIPosition,
    transition: *mut FFITransition,
) {
    let v = &mut *v;
    let position = &mut *position;
    let transition = &mut *transition;

    v.connections
        .drain_filter(|c| c.first().eq(&position.0) && c.second().eq(&transition.0));
}

#[no_mangle]
pub unsafe extern "C" fn remove_connection_t(
    v: *mut PetriNet,
    transition: *mut FFITransition,
    position: *mut FFIPosition,
) {
    let v = &mut *v;
    let position = &mut *position;
    let transition = &mut *transition;

    v.connections
        .drain_filter(|c| c.first().eq(&transition.0) && c.second().eq(&position.0));
}
