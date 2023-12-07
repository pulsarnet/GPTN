extern crate cxx_build;

fn main() {
    cxx_build::bridge("src/vec.rs").compile("vec.rss");
}