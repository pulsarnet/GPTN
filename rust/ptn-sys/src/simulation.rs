use ptn::net::PetriNet;
use ptn::modules::reachability::simulation::Simulation;
use ptn::net::vertex::VertexIndex;
use vec::RustVec;

#[export_name = "ptn$modules$simulation$new"]
extern "C" fn new(net: &PetriNet) -> *mut Simulation {
    Box::into_raw(Box::new(Simulation::new(net)))
}

#[export_name = "ptn$modules$simulation$step"]
extern "C" fn step(simulation: &mut Simulation) -> i32 {
    simulation.simulate()
}

#[export_name = "ptn$modules$simulation$markers_at"]
extern "C" fn place_marking(simulation: &Simulation, index: VertexIndex) -> usize {
    simulation.markers_at(index)
}

// todo deprecate
#[export_name = "ptn$modules$simulation$cycles"]
extern "C" fn cycles(simulation: &Simulation) -> usize {
    simulation.cycles()
}

#[export_name = "ptn$modules$simulation$fired"]
unsafe extern "C" fn fired(simulation: &Simulation, fired: *mut RustVec<VertexIndex>) {
    // todo return slice
    std::ptr::write_unaligned(fired, RustVec::from(simulation.fired().to_vec()));
}

#[export_name = "ptn$modules$simulation$is_fired"]
extern "C" fn is_fired(simulation: &Simulation, transition: VertexIndex) -> bool {
    simulation.fired().contains(&transition)
}

#[export_name = "ptn$modules$simulation$net"]
extern "C" fn net(simulation: &Simulation) -> *const PetriNet {
    simulation.net()
}

#[export_name = "ptn$modules$simulation$drop"]
extern "C" fn drop(simulation: *mut Simulation) {
    let _ = unsafe { Box::from_raw(simulation) };
}