use std::ffi::CStr;
use libc::c_char;
use tracing::{debug, error, info, warn};
use tracing_subscriber::{filter, Layer};
use tracing_subscriber::layer::SubscriberExt;
use tracing_subscriber::util::SubscriberInitExt;

#[no_mangle]
extern "C" fn init() {
    let stdout_write = tracing_subscriber::fmt::layer()
        .with_ansi(true)
        .with_file(true)
        .with_target(true)
        .pretty();

    tracing_subscriber::registry()
        .with(
            stdout_write
                .with_filter(filter::LevelFilter::TRACE)
        )
        .init();
}

#[no_mangle]
extern "C" fn external_debug_log(msg: *const c_char) {
    let msg = unsafe { CStr::from_ptr(msg) }.to_str().unwrap_or_default();
    debug!(target: "external", "{msg}");
}

#[no_mangle]
extern "C" fn external_info_log(msg: *const c_char) {
    let msg = unsafe { CStr::from_ptr(msg) }.to_str().unwrap_or_default();
    info!(target: "external", "{msg}");
}

#[no_mangle]
extern "C" fn external_warn_log(msg: *const c_char) {
    let msg = unsafe { CStr::from_ptr(msg) }.to_str().unwrap_or_default();
    warn!(target: "external", "{msg}");
}

#[no_mangle]
extern "C" fn external_error_log(msg: *const c_char) {
    let msg = unsafe { CStr::from_ptr(msg) }.to_str().unwrap_or_default();
    error!(target: "external", "{msg}");
}