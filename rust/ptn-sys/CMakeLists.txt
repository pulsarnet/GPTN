if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CARGO_CMD cargo build --release)
    set(TARGET_DIR release)
else ()
    # https://www.ralphminderhoud.com/blog/rust-cxx-corrosion-msvc-debug-crt/
    set(CARGO_CMD CFLAGS=-MDd CXXFLAGS=-MDd cargo build --verbose)
    set(TARGET_DIR debug)
endif ()


set(PTN_SYS_LIB_ARTIFACT "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_DIR}/ptn_sys.lib")
set(PTN_SYS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cxx/include")
file(GLOB PTN_SYS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/cxx/src/*.cpp")

add_custom_target(
        ptn_target ALL
        COMMAND ${CMAKE_COMMAND} -E env CARGO_TARGET_DIR=${CMAKE_CURRENT_BINARY_DIR} ${CARGO_CMD}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        BYPRODUCTS ${PTN_SYS_LIB_ARTIFACT}
)

add_library(ptn STATIC ${PTN_SYS_SOURCES})
add_dependencies(ptn ptn_target)

target_include_directories(ptn PUBLIC ${PTN_SYS_INCLUDE_DIR})
target_link_libraries(ptn ${PTN_SYS_LIB_ARTIFACT})

# tests
add_subdirectory(tests)
