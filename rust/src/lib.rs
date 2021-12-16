use std::collections::HashMap;
use std::ffi::CString;

#[derive(Default)]
pub struct PetriNet {
    positions: Vec<u8>
}

#[no_mangle]
pub extern "C" fn make() -> *mut PetriNet {
    let v = Box::new(PetriNet::default());
    Box::into_raw(v)
}

#[no_mangle]
pub unsafe extern "C" fn del(v: *mut PetriNet) {
    Box::from_raw(v);
}

#[no_mangle]
pub unsafe extern "C" fn count(v: *mut PetriNet) -> std::os::raw::c_ulong {
    (&*v).positions.len() as std::os::raw::c_ulong
}