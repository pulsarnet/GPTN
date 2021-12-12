#[no_mangle]
pub extern fn print(a: i32, b: f32, c: bool) {
    println!("int: {}, float: {}, bool: {}", a, b, c);
}

#[no_mangle]
pub extern fn update_i32(a: *mut i32) {
    unsafe { *a += 1; };
}