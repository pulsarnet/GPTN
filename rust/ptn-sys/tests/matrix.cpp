#include <ptn/matrix.h>
#include <ptn/types.h>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

namespace matrix = ptn::matrix;

TEST_CASE("Test matrix")
{
    matrix::RustMatrix<i32> mat(5, 10);
    REQUIRE(mat.nrows() == 5);
    REQUIRE(mat.ncols() == 10);

    REQUIRE(mat[{2, 3}] == 0);
    mat[{2, 3}] = 21;
    REQUIRE(mat[{2, 3}] == 21);
}

TEST_CASE("Matrix move semantic")
{
    matrix::RustMatrix<i32> mat;
    REQUIRE(mat.nrows() == 0);
    REQUIRE(mat.ncols() == 0);
}