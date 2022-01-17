cmake_minimum_required(VERSION 3.21)
project(ffi_rust)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.2.2/msvc2019_64/lib/cmake")

    if (CMAKE_BUILD_TYPE EQUAL "Release")

    else()
        set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}C:/Program Files (x86)/QtADS/lib/cmake/qtadvanceddocking")
    endif()
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
        src/elements/arrow_line.cpp
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
        include/elements/arrow_line.h
        include/elements/position.h
        include/elements/transition.h
        include/elements/petri_object.h
        include/synthesis/synthesis_view.h
        include/ffi/net.h
        include/ffi/rust.h
        include/view/graphics_view.h
        include/view/graphics_view_zoom.h
        include/toolbox/toolbox.h include/toolbox/tool.h include/view/graphics_scene.h include/elements/graphicsitem.h)


add_executable(ffi_rust
        ${SRC_FILES}
        ${HEADERS}
        mainwindow.ui
        resources/resources.qrc
        )

target_include_directories(ffi_rust PRIVATE "/usr/local/include")

find_package(qtadvanceddocking)

add_dependencies(ffi_rust rust)


if(WIN32)
    target_link_directories(ffi_rust PRIVATE ${CMAKE_SOURCE_DIR}/lib)
    target_link_libraries(ffi_rust PRIVATE
        debug "${CMAKE_SOURCE_DIR}/rust/target/debug/hello_world.lib" Qt6::Widgets Qt6::Core Qt6::Gui ads::qtadvanceddocking liblapack
        optimized "${CMAKE_SOURCE_DIR}/rust/target/release/hello_world.lib" Qt6::Widgets Qt6::Core Qt6::Gui ads::qtadvanceddocking liblapack
        )
else()
        target_link_libraries(ffi_rust PRIVATE
            debug "${CMAKE_SOURCE_DIR}/rust/target/debug/libhello_world.a" Qt6::Widgets Qt6::Core Qt6::Gui lapack ads::qtadvanceddocking
            optimized "${CMAKE_SOURCE_DIR}/rust/target/release/libhello_world.a" Qt6::Widgets Qt6::Core Qt6::Gui lapack ads::qtadvanceddocking
            )
endif()
