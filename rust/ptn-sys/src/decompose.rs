use ptn::modules::synthesis::DecomposeContext;
use ptn::net::PetriNet;

#[export_name = "ptn$modules$decompose$init"]
pub extern "C" fn decompose_context_init(net: &PetriNet) -> *mut DecomposeContext {
    Box::into_raw(Box::new(DecomposeContext::init(net)))
}

#[export_name = "ptn$modules$decompose$positions"]
pub extern "C" fn decompose_context_positions(ctx: &DecomposeContext) -> usize {
    ctx.positions.len()
}

#[export_name = "ptn$modules$decompose$transitions"]
pub extern "C" fn decompose_context_transitions(ctx: &DecomposeContext) -> usize {
    ctx.transitions.len()
}

#[export_name = "ptn$modules$decompose$primitive"]
extern "C" fn decompose_context_primitive_net(ctx: &DecomposeContext) -> *const PetriNet {
    &ctx.primitive_net as *const PetriNet
}

#[export_name = "ptn$modules$decompose$lbf"]
extern "C" fn decompose_context_linear_base_fragments(ctx: &DecomposeContext) -> *mut PetriNet {
    Box::into_raw(Box::new(ctx.linear_base_fragments()))
}

#[export_name = "ptn$modules$decompose$position_at"]
extern "C" fn decompose_context_position_index(ctx: &DecomposeContext, index: usize) -> usize {
    ctx.positions[index].index().id
}

#[export_name = "ptn$modules$decompose$transition_at"]
extern "C" fn decompose_context_transition_index(ctx: &DecomposeContext, index: usize) -> usize {
    ctx.transitions[index].index().id as usize
}

#[export_name = "ptn$modules$decompose$drop"]
pub unsafe extern "C" fn decompose_context_delete(ctx: *mut DecomposeContext) {
    let _ = Box::from_raw(ctx);
}


