use std::ffi::CStr;
use libc::c_char;
use tracing::{debug, error, info, warn};
use tracing_subscriber::{filter, Layer};
use tracing_subscriber::layer::SubscriberExt;
use tracing_subscriber::util::SubscriberInitExt;

#[export_name = "ptn$logger$init"]
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

#[export_name = "ptn$logger$debug"]
extern "C" fn external_debug_log(msg: *const c_char) {
    let msg = unsafe { CStr::from_ptr(msg) }.to_str().unwrap_or_default();
    debug!(target: "external", "{msg}");
}

#[export_name = "ptn$logger$info"]
extern "C" fn external_info_log(msg: *const c_char) {
    let msg = unsafe { CStr::from_ptr(msg) }.to_str().unwrap_or_default();
    info!(target: "external", "{msg}");
}

#[export_name = "ptn$logger$warn"]
extern "C" fn external_warn_log(msg: *const c_char) {
    let msg = unsafe { CStr::from_ptr(msg) }.to_str().unwrap_or_default();
    warn!(target: "external", "{msg}");
}

#[export_name = "ptn$logger$error"]
extern "C" fn external_error_log(msg: *const c_char) {
    let msg = unsafe { CStr::from_ptr(msg) }.to_str().unwrap_or_default();
    error!(target: "external", "{msg}");
}