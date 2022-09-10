cmake_minimum_required(VERSION 3.17.3)
project(ffi_rust)
include(ExternalProject)

message(STATUS ${CMAKE_CXX_FLAGS})
set(CMAKE_CXX_STANDARD 17)

set(TARGET_DIR ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TARGET_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TARGET_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${TARGET_DIR}/lib)
set(CMAKE_AUTORCC ON)

if (NOT GRAPHVIZ_DIR)
    message(FATAL_ERROR "Graphviz dir not set")
endif()
#
if (NOT Qt6_DIR)
    message(FATAL_ERROR "Qt6_DIR not set")
endif()

if (NOT Qt_ADVANCED_DOCKING_SYSTEM_DIR)
    message(FATAL_ERROR "Qt_ADVANCED_DOCKING_SYSTEM_DIR not set")
endif()

include_directories(${GRAPHVIZ_DIR}/include/)
link_directories(${GRAPHVIZ_DIR}/lib/)

if(NOT WIN32)
    #set(CMAKE_PREFIX_PATH "/Users/nikolajmuravev/Qt/6.2.1/macos/lib/cmake")
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

set(CMAKE_PREFIX_PATH ${Qt6_DIR})
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui OpenGLWidgets Svg Charts)

set(CMAKE_PREFIX_PATH ${Qt_ADVANCED_DOCKING_SYSTEM_DIR})
find_package(qtadvanceddocking)

set(SRC_FILES
        main.cpp
        src/mainwindow.cpp
        src/matrix_model.cpp
        src/elements/arrow_line.cpp
        src/elements/position.cpp
        src/elements/transition.cpp
        src/elements/petri_object.cpp
        src/ffi/rust.cpp
        src/view/graphics_view.cpp
        src/view/graphics_view_zoom.cpp
        src/view/graphic_scene.cpp
        src/overrides/switch.cpp
        src/graphviz/graphviz_wrapper.cpp
        src/synthesis/synthesis_model.cpp
        src/synthesis/synthesis_item_delegate.cpp
        src/ActionTabWidget/ActionTabWidget.cpp
        src/ActionTabWidget/NetModelingTab.cpp
        src/overrides/MatrixWindow.cpp
        src/ActionTabWidget/WrappedLayoutWidget.cpp
        src/ActionTabWidget/DecomposeModelTab.cpp
        src/DockSystem/SplittableComponentsFactory.cpp
        src/DockSystem/DockToolbar.cpp
        src/DockSystem/DockWidget.cpp
        src/synthesis/synthesis_table.cpp
        )

set(HEADERS
        src/mainwindow.h
        src/matrix_model.h
        src/elements/arrow_line.h
        src/elements/position.h
        src/elements/transition.h
        src/elements/petri_object.h
        src/view/graphics_view.h
        src/view/graphics_view_zoom.h
        src/toolbox/toolbox.h
        src/toolbox/tool.h
        src/ffi/rust.h
        src/view/graphic_scene.h
        src/graphviz/graphviz_wrapper.h
        src/overrides/switch.h
        src/synthesis/synthesis_table.h
        src/synthesis/synthesis_model.h
        src/synthesis/synthesis_item_delegate.h
        src/windows_types/close_on_inactive.h
        src/named_matrix_model.h
        src/overrides/MatrixWindow.h
        src/ActionTabWidget/ActionTabWidget.h
        src/ActionTabWidget/NetModelingTab.h
        src/ActionTabWidget/DecomposeModelTab.h
        src/ActionTabWidget/WrappedLayoutWidget.h
        src/DockSystem/SplittableComponentsFactory.h
        src/DockSystem/DockToolbar.h
        src/DockSystem/DockWidget.h
        )

link_directories("${CMAKE_SOURCE_DIR}/lib")

add_executable(ffi_rust
        ${SRC_FILES}
        ${HEADERS}
        mainwindow.ui
        resources/resources.qrc
        )

add_dependencies(ffi_rust rust)
target_link_libraries(ffi_rust PRIVATE
        "${CMAKE_SOURCE_DIR}/rust/target/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>/hello_world.lib"
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        Qt6::OpenGLWidgets
        Qt6::Charts
        Qt6::Svg
        ads::qtadvanceddocking
        libopenblas
        gvc
        cgraph
        )

qt6_import_plugins(ffi_rust
        INCLUDE_BY_TYPE imageformats
        )

set_target_properties(ffi_rust PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        AUTOUIC ON
        CXX_STANDART 20
        CXX_STANDART_REQUIRED ON
        CXX_EXTENSIONS OFF
        )