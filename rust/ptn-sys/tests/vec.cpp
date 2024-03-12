#include <ptn/vec.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

namespace vec = ptn::alloc::vec;

TEST_CASE("Test RustVec") {
    auto arr = vec::RustVec<bool>();
    REQUIRE(arr.capacity() == 0);
    arr.reserve(10);
    REQUIRE(arr.capacity() == 10);
}