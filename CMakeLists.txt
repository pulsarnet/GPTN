cmake_minimum_required(VERSION 3.21)
project(ffi_rust)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_PREFIX_PATH "C:\\Qt\\6.2.2\\msvc2019_64\\lib\\cmake")

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
        src/mainwindow.cpp
        mainwindow.ui
        src/mainwindow.cpp src/graphics_view.cpp include/graphics_view.h src/position.cpp include/position.h src/arrow_line.cpp include/arrow_line.h
        resources/resources.qrc src/graphics_view_zoom.cpp include/graphics_view_zoom.h src/tab.cpp include/tab.h)

add_dependencies(ffi_rust rust)

target_link_libraries(ffi_rust
        debug "${CMAKE_SOURCE_DIR}/rust/target/debug/hello_world.lib" Qt6::Widgets
        optimized "${CMAKE_SOURCE_DIR}/rust/target/release/hello_world.lib" Qt6::Widgets
        )