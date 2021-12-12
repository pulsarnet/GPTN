cmake_minimum_required(VERSION 3.21)
project(ffi_rust)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_PREFIX_PATH /Users/nikolajmuravev/Qt6/6.2.1/macos/lib/cmake)

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

add_executable(ffi_rust
        main.cpp
        mainwindow.cpp
        mainwindow.ui
        mainwindow.cpp scene.cpp scene.h)

add_dependencies(ffi_rust rust)

target_link_libraries(ffi_rust
        debug "${CMAKE_SOURCE_DIR}/rust/target/debug/libhello_world.a" Qt6::Widgets
        optimized "${CMAKE_SOURCE_DIR}/rust/target/release/libhello_world.a" Qt6::Widgets
        )