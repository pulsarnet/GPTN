cmake_minimum_required(VERSION 3.21)
project(ffi_rust)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    set(CMAKE_PREFIX_PATH "C:\\Qt\\6.2.2\\msvc2019_64\\lib\\cmake")
else()
    set(CMAKE_PREFIX_PATH "/Users/nikolajmuravev/Qt/6.2.1/macos/lib/cmake")
endif()

add_compile_definitions(__cpp_lib_format=201907L)

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
find_package(fmt)

add_executable(ffi_rust
        main.cpp
        src/mainwindow.cpp
        mainwindow.ui
        src/mainwindow.cpp src/graphics_view.cpp include/graphics_view.h src/elements/position.cpp include/elements/position.h src/arrow_line.cpp include/arrow_line.h
        resources/resources.qrc src/graphics_view_zoom.cpp include/graphics_view_zoom.h src/tab.cpp include/tab.h include/ffi/rust.h include/split_list_model.h
        include/petri_net.h src/matrix_model.cpp include/matrix_model.h include/matrix.h include/named_matrix.h include/named_matrix_model.h
        include/synthesis/synthesis_programs_model.h include/synthesis/synthesis_program.h include/synthesis/synthesis_program_item_delegate.h include/synthesis/synthesis_view.h src/synthesis/synthesis_view.cpp src/ffi/rust.cpp include/connection_list.h include/ffi/net.h include/elements/transition.h include/elements/petri_object.h src/elements/transition.cpp src/elements/petri_object.cpp src/ffi/net.cpp include/ffi/methods.h src/ffi/transition.cpp src/ffi/position.cpp)

add_dependencies(ffi_rust rust)

target_link_libraries(ffi_rust PRIVATE
        debug "${CMAKE_SOURCE_DIR}/rust/target/debug/libhello_world.dylib" Qt6::Widgets fmt::fmt
        optimized "${CMAKE_SOURCE_DIR}/rust/target/release/libhello_world.dylib" Qt6::Widgets fmt::fmt
        )