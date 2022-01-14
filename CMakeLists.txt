cmake_minimum_required(VERSION 3.21)
project(ffi_rust)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.2.2/msvc2019_64/lib/cmake")
else()
    set(CMAKE_PREFIX_PATH "/Users/nikolajmuravev/Qt/6.2.1/macos/lib/cmake")
    add_compile_definitions(__cpp_lib_format=201907L)
endif()

ExternalProject_Add(
        rust
        DOWNLOAD_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND cargo build COMMAND cargo build --release
        BINARY_DIR ${CMAKE_SOURCE_DIR}/rust
        INSTALL_COMMAND ""
        LOG_BUILD ON
)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

set(SRC_FILES
        main.cpp
        src/mainwindow.cpp
        src/tab.cpp
        src/matrix_model.cpp
        src/arrow_line.cpp
        src/elements/position.cpp
        src/elements/transition.cpp
        src/elements/petri_object.cpp
        src/synthesis/synthesis_view.cpp
        src/ffi/net.cpp
        src/ffi/position.cpp
        src/ffi/transition.cpp
        src/ffi/rust.cpp
        src/view/graphics_view.cpp
        src/view/graphics_view_zoom.cpp
        src/view/graphics_scene.cpp)

set(HEADERS
        include/mainwindow.h
        include/tab.h
        include/matrix_model.h
        include/arrow_line.h
        include/elements/position.h
        include/elements/transition.h
        include/elements/petri_object.h
        include/synthesis/synthesis_view.h
        include/ffi/net.h
        include/ffi/rust.h
        include/view/graphics_view.h
        include/view/graphics_view_zoom.h
        include/toolbox/toolbox.h include/toolbox/tool.h include/view/graphics_scene.h)


add_executable(ffi_rust
        ${SRC_FILES}
        ${HEADERS}
        mainwindow.ui
        resources/resources.qrc
        )

add_dependencies(ffi_rust rust)


target_link_libraries(ffi_rust PRIVATE
        debug "${CMAKE_SOURCE_DIR}/rust/target/debug/libhello_world.a" Qt6::Widgets lapack
        optimized "${CMAKE_SOURCE_DIR}/rust/target/release/libhello_world.a" Qt6::Widgets lapack
        )