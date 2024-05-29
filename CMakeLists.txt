cmake_minimum_required(VERSION 3.21)
project(GPTN VERSION 0.1.2 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(TARGET_NAME GPTN)
set(TARGET_DIR ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TARGET_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TARGET_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${TARGET_DIR}/lib)
set(CMAKE_AUTORCC ON)
set(VERSION_COMPONENT_BUILD 1)
set(CMAKE_VERBOSE_MAKEFILE on)

if(WIN32)
    set(CMAKE_INSTALL_BINDIR ".")
    set(CMAKE_INSTALL_LIBDIR ".")
    set(CMAKE_INSTALL_INCLUDEDIR "include")
    set(CMAKE_INSTALL_DATADIR ".")
    set(CMAKE_INSTALL_DOCDIR ".")
endif ()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Svg DataVisualization)
get_target_property(QT_WIDGETS_LOCATION Qt6::Widgets IMPORTED_LOCATION)
get_filename_component(_qt_bin_path ${QT_WIDGETS_LOCATION} DIRECTORY)
set(QT_BIN_PATH "${_qt_bin_path}" CACHE INTERNAL "Path to qmake, macdeployqt, windeployqt, and other Qt utilities.")

add_subdirectory(thirdparty)
add_subdirectory(rust/ptn-sys)

if(WIN32)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/gptn.rc.cmake ${CMAKE_CURRENT_BINARY_DIR}/gptn.rc)
    set(WIN_RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/gptn.rc)
    add_definitions(-DGVDLL)
else()
    add_compile_definitions(__cpp_lib_format=201907L)
endif()

# configure config.h.cmake
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.cmake ${CMAKE_BINARY_DIR}/config.h)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE HEADER_FILES src/*.h)
file(GLOB_RECURSE QRC_FILES resources/*.qrc)

qt_add_resources(QT_RESOURCES ${QRC_FILES})
qt_add_executable(${TARGET_NAME}
        #WIN32
        MACOSX_BUNDLE
        ${SOURCE_FILES}
        ${HEADER_FILES}
        ${QT_RESOURCES}
        ${WIN_RC_FILE}
        src/Property/Spoiler.cpp
        src/Property/Spoiler.h
)
qt_import_plugins(${TARGET_NAME} INCLUDE Qt::Svg NO_DEFAULT)

target_link_libraries(${TARGET_NAME} PRIVATE
        ptn
        qt6advanceddocking
        gvc
        cgraph
        xdot
        gvplugin_core
        gvplugin_dot_layout
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        Qt6::Svg
        Qt6::DataVisualization
)

set_target_properties(${TARGET_NAME} PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        AUTOUIC ON
        CXX_STANDART 20
        CXX_STANDART_REQUIRED ON
        CXX_EXTENSIONS OFF
)


if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
endif()

include_directories(${CMAKE_BINARY_DIR})


if (WIN32)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ${CMAKE_INSTALL_BINDIR})
    include(InstallRequiredSystemLibraries)

    find_program(QT_WINDEPLOYQT_EXECUTABLE "windeployqt" HINTS ${QT_BIN_PATH} REQUIRED)
    add_custom_target(copy_qt_dlls ALL)
    set_target_properties(copy_qt_dlls PROPERTIES FOLDER "Copy Tasks")
    add_custom_command(TARGET copy_qt_dlls
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E rm -rRf "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
            COMMAND set "PATH=${QT_BIN_PATH};%PATH%"
            COMMAND "${QT_WINDEPLOYQT_EXECUTABLE}"
            --dir "${CMAKE_CURRENT_BINARY_DIR}/windeployqt"
            --no-compiler-runtime
            --no-opengl-sw
            --no-translations
            --no-system-d3d-compiler
            --no-quick-import
            --no-network
            --skip-plugin-types networkinformation,tls,iconengines,generic
            --verbose 0
            "$<TARGET_FILE:${TARGET_NAME}>"
            COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different "${CMAKE_CURRENT_BINARY_DIR}/windeployqt/" "$<TARGET_FILE_DIR:${TARGET_NAME}>"
            VERBATIM
    )

    add_custom_target(copy_graphviz_dlls ALL)
    set_target_properties(copy_graphviz_dlls PROPERTIES FOLDER "Copy Tasks")
    add_custom_command(
            TARGET copy_graphviz_dlls
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/thirdparty/graphviz/build/ $<TARGET_FILE_DIR:${TARGET_NAME}>
    )
    add_dependencies(copy_graphviz_dlls ${TARGET_NAME})


    add_custom_target(generate_graphviz_config ALL)
    set_target_properties(generate_graphviz_config PROPERTIES FOLDER "Generate Tasks")
    add_custom_command(
            TARGET generate_graphviz_config
            POST_BUILD
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/thirdparty/graphviz/build/
            COMMAND $<TARGET_FILE:dot> ARGS -c -v
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/thirdparty/graphviz/build/config6 $<TARGET_FILE_DIR:${TARGET_NAME}> # copy config6 to build directory
    )
    add_dependencies(generate_graphviz_config ${TARGET_NAME} xdot dot)

    install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/thirdparty/graphviz/build/ DESTINATION ${CMAKE_INSTALL_BINDIR} FILES_MATCHING REGEX ".*.dll|config6")
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/windeployqt/ DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()


# -----------------------------------------------------------------------------
# Packaging (CPack)
# -----------------------------------------------------------------------------
include(ConfigCPack.cmake)