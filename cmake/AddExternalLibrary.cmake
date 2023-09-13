function(add_external_library)
    set(options)
    set(oneValueArgs TARGET WORKING_DIRECTORY OUTPUT COMMENT)
    set(multiValueArgs COMMAND INCLUDE_DIRS)
    cmake_parse_arguments(ARGS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Specify how to build the result of the library
    add_custom_command(OUTPUT "${ARGS_OUTPUT}"
        COMMAND ${ARGS_COMMAND}
        WORKING_DIRECTORY "${ARGS_WORKING_DIRECTORY}"
        COMMENT "${ARGS_COMMENT}")

    # Create a target out of the library compilation result
    add_custom_target(${ARGS_TARGET}_target DEPENDS ${ARGS_OUTPUT})

    # Create an library target out of the library compilation result
    add_library(${ARGS_TARGET} STATIC IMPORTED GLOBAL)
    add_dependencies(${ARGS_TARGET} ${ARGS_TARGET}_target)

    # Specify where the library is and where to find the headers
    set_target_properties(${ARGS_TARGET}
        PROPERTIES
        IMPORTED_LOCATION "${ARGS_OUTPUT}"
        INTERFACE_INCLUDE_DIRECTORIES "${ARGS_INCLUDE_DIRS}"
    )
endfunction()