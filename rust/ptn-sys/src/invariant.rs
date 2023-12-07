use ptn::modules::invariant::{p_invariant, t_invariant};
use ptn::net::PetriNet;

#[export_name = "ptn$modules$invariant$as_position"]
pub extern "C" fn petri_net_p_invariant(net: &PetriNet) {
    p_invariant(net)
}

#[export_name = "ptn$modules$invariant$as_transition"]
pub extern "C" fn petri_net_t_invariant(net: &PetriNet) {
    t_invariant(net);
}
