fn main() {
    #[cfg(target_os = "windows")]
    {
        println!("cargo:rustc-link-lib=dylib=libopenblas");
        println!("cargo:rustc-link-search=native=../lib/");
    }
}
