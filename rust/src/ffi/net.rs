// extern "C" FFIPosition* add_position(PetriNet*);
// extern "C" FFIPosition* get_position(unsigned long);
// extern "C" unsigned long position_index(FFIPosition*);
// extern "C" void remove_position(FFIPosition*);

use ffi::position::Position;
use ffi::transition::Transition;
use PetriNet;

#[no_mangle]
pub extern "C" fn create_net() -> *mut PetriNet {
    Box::into_raw(Box::new(PetriNet::new()))
}

#[no_mangle]
pub unsafe extern "C" fn del(v: *mut PetriNet) {
    Box::from_raw(v);
}

#[no_mangle]
pub unsafe extern "C" fn add_position(net: &mut PetriNet) -> *mut Position {
    let added = net.add_position(net.next_position_index());
    Box::into_raw(Box::new(Position(added)))
}

#[no_mangle]
pub unsafe extern "C" fn add_position_with(net: &mut PetriNet, index: usize) -> *mut Position {
    let added = net.add_position(index as u64);
    Box::into_raw(Box::new(Position(added)))
}

#[no_mangle]
pub unsafe extern "C" fn get_position(net: &mut PetriNet, index: usize) -> *mut Position {
    let position = net.get_position(index as u64).cloned().unwrap();
    Box::into_raw(Box::new(Position(position)))
}

#[no_mangle]
pub unsafe extern "C" fn remove_position(net: &mut PetriNet, position: &mut Position) {
    net.remove_position(position.0.index());

    Box::from_raw(position);
}

#[no_mangle]
pub unsafe extern "C" fn add_transition(net: &mut PetriNet) -> *mut Transition {
    let added = net.add_transition(net.next_transition_index());
    Box::into_raw(Box::new(Transition(added)))
}

#[no_mangle]
pub extern "C" fn add_transition_with(net: &mut PetriNet, index: u64) -> *mut Transition {
    let added = net.add_transition(index);
    Box::into_raw(Box::new(Transition(added)))
}

#[no_mangle]
pub extern "C" fn get_transition(net: &mut PetriNet, index: usize) -> *mut Transition {
    let transition = net.get_transition(index as u64).cloned().unwrap();
    Box::into_raw(Box::new(Transition(transition)))
}

#[no_mangle]
pub extern "C" fn remove_transition(net: &mut PetriNet, transition: &mut Transition) {
    net.remove_transition(transition.0.index());
    unsafe { Box::from_raw(transition); }
}

#[no_mangle]
pub unsafe extern "C" fn connect_p(
    net: &mut PetriNet,
    position: &Position,
    transition: &Transition,
)
{
    net.connect(position.0.clone(), transition.0.clone());
}

#[no_mangle]
pub unsafe extern "C" fn connect_t(
    net: &mut PetriNet,
    transition: &Transition,
    position: &Position,
)
{
    net.connect(transition.0.clone(), position.0.clone());
}

#[no_mangle]
pub unsafe extern "C" fn remove_connection_p(
    net: &mut PetriNet,
    position: &mut Position,
    transition: &mut Transition,
)
{
    net.connections
        .drain_filter(|c| c.first().eq(&position.0) && c.second().eq(&transition.0));
}

#[no_mangle]
pub unsafe extern "C" fn remove_connection_t(
    net: &mut PetriNet,
    transition: &Position,
    position: &Transition,
)
{
    net.connections
        .drain_filter(|c| c.first().eq(&transition.0) && c.second().eq(&position.0));
}
