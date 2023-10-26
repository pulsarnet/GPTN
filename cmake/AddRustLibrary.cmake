find_program(CARGO cargo REQUIRED)

function(build_rust_library)
    set(options)
    set(oneValueArgs TARGET SOURCE_DIR LIB_FILENAME)
    set(multiValueArgs)
    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(_RUST_TARGET_DIR ${CMAKE_BINARY_DIR})

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        set(_RUST_BUILD_FLAG "--release")
        set(_RUST_ARTIFACT_DIR ${_RUST_TARGET_DIR}/release)
    else ()
        set(_RUST_ARTIFACT_DIR ${_RUST_TARGET_DIR}/debug)
    endif ()

    set(_RUST_ARTIFACT_PATH ${_RUST_ARTIFACT_DIR}/${ARGS_LIB_FILENAME})

    # Create a target out of the library compilation result
    add_custom_target(
            ${ARGS_TARGET}_target ALL
            COMMAND cargo build ${_RUST_BUILD_FLAG} --target-dir=${_RUST_TARGET_DIR}
            WORKING_DIRECTORY "${ARGS_SOURCE_DIR}"
            BYPRODUCTS ${_RUST_ARTIFACT_PATH}
    )

    add_library(${ARGS_TARGET} STATIC IMPORTED GLOBAL)
    add_dependencies(${ARGS_TARGET} ${ARGS_TARGET}_target)

    set_target_properties(${ARGS_TARGET} PROPERTIES IMPORTED_LOCATION "${_RUST_ARTIFACT_PATH}")
endfunction()