use ::{DecomposeContext, PetriNet};
pub mod net;
pub mod vec;
pub mod vertex;
pub mod connection;
pub mod matrix;

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
            decompose_context: None
        }
    }

    pub fn set_decompose_context(&mut self, decompose_context: Option<Box<DecomposeContext>>) {
        self.decompose_context = decompose_context;
    }
}

#[no_mangle]
extern "C" fn new_context() -> *const PetriNetContext {
    Box::into_raw(Box::new(PetriNetContext::new()))
}

#[no_mangle]
extern "C" fn ctx_net(ctx: *const PetriNetContext) -> *const PetriNet {
    &(unsafe { &*ctx }.net) as *const PetriNet
}

#[no_mangle]
extern "C" fn ctx_decompose_context(ctx: *const PetriNetContext) -> *const DecomposeContext {
    match unsafe { &*ctx }.decompose_context.as_ref() {
        Some(decompose_context) => decompose_context.as_ref() as *const DecomposeContext,
        None => std::ptr::null()
    }
}

#[no_mangle]
extern "C" fn ctx_decompose(ctx: &mut PetriNetContext) {
    ctx.decompose_context = Some(Box::new(DecomposeContext::init(&ctx.net)));
}

#[no_mangle]
extern "C" fn ctx_set_decompose_context(ctx: *mut PetriNetContext, d_ctx: *mut DecomposeContext) {
    let ctx = unsafe { &mut *ctx };
    match d_ctx.is_null() {
        true => ctx.set_decompose_context(None),
        false => ctx.set_decompose_context(Some(unsafe { Box::from_raw(d_ctx) }))
    }
}

#[no_mangle]
extern "C" fn delete_context(ctx: *mut PetriNetContext) {
    unsafe { Box::from_raw(ctx) };
}