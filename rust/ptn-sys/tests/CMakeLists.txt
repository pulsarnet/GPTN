Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0 # or a later release
)
FetchContent_MakeAvailable(Catch2)

add_executable(ptn_tests
        net.cpp
        vec.cpp
        matrix.cpp
)

target_link_libraries(ptn_tests PRIVATE ptn Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

set(PARSE_CATCH_TESTS_VERBOSE ON)
catch_discover_tests(ptn_tests)