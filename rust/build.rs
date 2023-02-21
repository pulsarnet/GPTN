fn main() {
    println!("cargo:rustc-link-lib=dylib=libopenblas");
    println!("cargo:rustc-link-search=native=../lib/");
}
