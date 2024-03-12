use ptn::modules::synthesis::DecomposeContext;
use ptn::net::PetriNet;

struct PetriNetContext {
    // Основная сеть петри
    pub net: PetriNet,

    // Контекст декомпозиции
    pub decompose_context: Option<Box<DecomposeContext>>,
}

impl PetriNetContext {
    pub fn new() -> Self {
        Self {
            net: PetriNet::new(),
            decompose_context: None,
        }
    }

    pub fn set_decompose_context(&mut self, decompose_context: Option<Box<DecomposeContext>>) {
        self.decompose_context = decompose_context;
    }
}

#[export_name = "ptn$net$context$new"]
extern "C" fn new_context() -> *const PetriNetContext {
    Box::into_raw(Box::new(PetriNetContext::new()))
}

#[export_name = "ptn$net$context$net"]
extern "C" fn ctx_net(ctx: &PetriNetContext) -> *const PetriNet {
    &(ctx.net) as *const PetriNet
}

#[export_name = "ptn$net$context$decompose_ctx"]
extern "C" fn ctx_decompose_context(ctx: &PetriNetContext) -> *const DecomposeContext {
    match ctx.decompose_context.as_ref() {
        Some(decompose_context) => decompose_context.as_ref() as *const DecomposeContext,
        None => std::ptr::null(),
    }
}

#[export_name = "ptn$net$context$decompose"]
extern "C" fn ctx_decompose(ctx: &mut PetriNetContext) {
    ctx.decompose_context = Some(Box::new(DecomposeContext::init(&ctx.net)));
}

#[export_name = "ptn$net$context$set_decompose_ctx"]
extern "C" fn ctx_set_decompose_context(ctx: &mut PetriNetContext, d_ctx: *mut DecomposeContext) {
    match d_ctx.is_null() {
        true => ctx.set_decompose_context(None),
        false => ctx.set_decompose_context(Some(unsafe { Box::from_raw(d_ctx) })),
    }
}

#[export_name = "ptn$net$context$drop"]
extern "C" fn drop_context(ctx: *mut PetriNetContext) {
    unsafe { let _ = Box::from_raw(ctx); }
}
