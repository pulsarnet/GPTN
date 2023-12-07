use ptn::modules::reachability::{CovType, MarkerValue, Marking, ReachabilityTree};
use ptn::net::vertex::VertexIndex;
use vec::RustVec;

#[export_name = "ptn$modules$reachability$positions"]
extern "C" fn positions(this: &ReachabilityTree, vec: &mut RustVec<VertexIndex>) {
    unsafe { core::ptr::write_unaligned(vec, RustVec::from(this.positions().to_vec())) };
}

#[export_name = "ptn$modules$reachability$marking"]
extern "C" fn marking(this: &ReachabilityTree, vec: &mut RustVec<*const Marking>) {
    let result = this.markings().iter().map(|marking| marking as *const _).collect::<Vec<_>>();
    unsafe { core::ptr::write_unaligned(vec, RustVec::from(result)) };
}

#[export_name = "ptn$modules$reachability$drop"]
extern "C" fn drop(this: *mut ReachabilityTree) {
    if !this.is_null() {
        let _ = unsafe { Box::from_raw(this) };
    }
}

#[export_name = "ptn$modules$reachability$marking$values"]
extern "C" fn marking_values(this: &Marking, vec: &mut RustVec<i32>) {
    let result = this.data().row(0).iter().map(MarkerValue::as_number).collect::<Vec<_>>();
    unsafe { core::ptr::write_unaligned(vec, RustVec::from(result)) };
}

#[export_name = "ptn$modules$reachability$marking$type"]
extern "C" fn marking_type(this: &Marking) -> CovType {
    this.r#type()
}

#[export_name = "ptn$modules$reachability$marking$prev"]
extern "C" fn marking_prev(this: &Marking) -> i32 {
    this.prev().map(|(_, v)| *v as i32).unwrap_or(-1)
}

#[export_name = "ptn$modules$reachability$marking$transition"]
extern "C" fn marking_transition(this: &Marking) -> VertexIndex {
    this.prev().map(|(v, _)| v.clone()).unwrap_or(VertexIndex::transition(0))
}

