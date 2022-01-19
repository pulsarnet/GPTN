cmake_minimum_required(VERSION 3.21)
project(ffi_rust)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    #set(Qt6_DIR "C:/Qt/6.2.2/msvc2019_64/lib/cmake")
    #set(qtadvanceddocking_DIR "C:/Program Files (x86)/QtADS/lib/cmake/qtadvanceddocking")
    #set(CMAKE_PREFIX_PATH ${qtadvanceddocking_DIR})
    include_directories("C:/Program Files/Graphviz/include")
    link_directories("C:/Program Files/Graphviz/lib")

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

set(CMAKE_PREFIX_PATH "C:/Qt/6.2.2/msvc2019_64/lib/cmake")
find_package(Qt6 COMPONENTS Widgets REQUIRED)

set(CMAKE_PREFIX_PATH "C:/Program Files (x86)/QtADS/lib/cmake/qtadvanceddocking")
find_package(qtadvanceddocking)

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
        src/view/graphics_scene.cpp
        src/graphviz/graphviz_wrapper.cpp)

set(HEADERS
        src/mainwindow.h
        src/tab.h
        src/matrix_model.h
        src/elements/arrow_line.h
        src/elements/position.h
        src/elements/transition.h
        src/elements/petri_object.h
        src/synthesis/synthesis_view.h
        src/ffi/net.h
        src/ffi/rust.h
        src/view/graphics_view.h
        src/view/graphics_view_zoom.h
        src/toolbox/toolbox.h src/toolbox/tool.h src/view/graphics_scene.h src/elements/graphicsitem.h
        src/graphviz/graphviz_wrapper.h)


add_executable(ffi_rust
        ${SRC_FILES}
        ${HEADERS}
        mainwindow.ui
        resources/resources.qrc
        )

#target_include_directories(ffi_rust PRIVATE "/usr/local/include")

add_dependencies(ffi_rust rust)

if(WIN32)
    target_link_directories(ffi_rust PRIVATE ${CMAKE_SOURCE_DIR}/lib )
    target_link_libraries(ffi_rust PRIVATE
        debug "${CMAKE_SOURCE_DIR}/rust/target/release/hello_world.lib" Qt6::Widgets Qt6::Core Qt6::Gui ads::qtadvanceddocking liblapack gvc cgraph
        optimized "${CMAKE_SOURCE_DIR}/rust/target/release/hello_world.lib" Qt6::Widgets Qt6::Core Qt6::Gui ads::qtadvanceddocking liblapack gvc cgraph
        )
else()
        target_link_libraries(ffi_rust PRIVATE
            debug "${CMAKE_SOURCE_DIR}/rust/target/debug/libhello_world.a" Qt6::Widgets Qt6::Core Qt6::Gui lapack ads::qtadvanceddocking gvc cgraph cdt
            optimized "${CMAKE_SOURCE_DIR}/rust/target/release/libhello_world.a" Qt6::Widgets Qt6::Core Qt6::Gui lapack ads::qtadvanceddocking gvc cgraph
            )
endif()

if (CMAKE_BUILD_TYPE EQUAL Release)
    set_target_properties(ffi_rust PROPERTIES RUNTIME_OUTPUT_DIRECTORY
            ${CMAKE_SOURCE_DIR}/build_release)
else()
    set_target_properties(ffi_rust PROPERTIES RUNTIME_OUTPUT_DIRECTORY
            ${CMAKE_SOURCE_DIR}/build_debug)
endif()