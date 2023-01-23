cmake_minimum_required(VERSION 3.17.3)
project(ffi_rust VERSION 0.1)
include(ExternalProject)

message(STATUS ${CMAKE_CXX_FLAGS})
set(CMAKE_CXX_STANDARD 20)

set(TARGET_DIR ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TARGET_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TARGET_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${TARGET_DIR}/lib)
set(CMAKE_AUTORCC ON)

if (NOT GRAPHVIZ_DIR)
    message(FATAL_ERROR "Graphviz dir not set")
endif()

if (NOT Qt6_DIR)
    message(FATAL_ERROR "Qt6_DIR not set")
endif()

if (NOT Qt_ADVANCED_DOCKING_SYSTEM_DIR)
    message(FATAL_ERROR "Qt_ADVANCED_DOCKING_SYSTEM_DIR not set")
endif()

if (NOT QWT_DIR)
    message(FATAL_ERROR "QWT_DIR not set")
endif()

set(QT6_MAIN_DIR ${Qt6_DIR})
set(QT6_BIN_DIR "${Qt6_DIR}/bin")
set(GRAPHVIZ_BIN_DIR "${GRAPHVIZ_DIR}/bin")
set(QWT_BIN_DIR "${QWT_DIR}/lib")
set(Qt_ADVANCED_DOCKING_SYSTEM_BIN_DIR "${Qt_ADVANCED_DOCKING_SYSTEM_DIR}/bin")

include_directories(${GRAPHVIZ_DIR}/include/)
link_directories(${GRAPHVIZ_DIR}/lib/)

add_compile_definitions(QWT_DLL)
find_library(QWT_LIBRARY
        NAMES qwt
        HINTS ${QWT_DIR}/lib
        REQUIRED)

if(QWT_LIBRARY)
    include_directories(${QWT_DIR}/include)
    message("QWT found:${QWT_LIBRARY}")
endif()

if(NOT WIN32)
    #set(CMAKE_PREFIX_PATH "/Users/nikolajmuravev/Qt/6.2.1/macos/lib/cmake")
    add_compile_definitions(__cpp_lib_format=201907L)
endif()

set(CMAKE_PREFIX_PATH ${Qt6_DIR})
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Svg Charts DataVisualization)

set(CMAKE_PREFIX_PATH ${Qt_ADVANCED_DOCKING_SYSTEM_DIR})
find_package(qtadvanceddocking)

set(SRC_FILES
        main.cpp
        src/mainwindow.cpp
        src/matrix_model.cpp
        src/view/elements/arrow_line.cpp
        src/view/elements/position.cpp
        src/view/elements/transition.cpp
        src/view/elements/petri_object.cpp
        src/ffi/rust.cpp
        src/view/GraphicsView.cpp
        src/view/GraphicsViewZoom.cpp
        src/view/GraphicScene.cpp
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
        src/QwtExt/qwt_ext_plot_curve_labels.cpp
        src/QwtExt/CanvasPicker.cpp
        src/synthesis/synthesis_window.cpp
        src/modules/reachability/reachability_tree_scene.cpp
        src/view/Simulation/SimulationWidget.cpp
        src/MainTree/DecomposeTreeItem.cpp
        src/modules/reachability/reachability_node.cpp
        src/ffi/simulation.cpp
        src/ffi/reachability.cpp
        src/ffi/vec.cpp
        src/modules/reachability/reachability_line.cpp
        src/modules/reachability/reachability_view.cpp
        src/modules/reachability/reachability_window.cpp
        src/MainTree/ReachabilityTreeItem.cpp
        src/view/Commands/AddCommand.cpp
        src/view/Commands/MarkCommand.cpp
        src/view/Commands/MoveCommand.cpp
        src/view/Commands/RotateCommand.cpp
        src/view/Commands/ConnectCommand.cpp
        src/view/Commands/RemoveCommand.cpp
        src/QwtExt/TimeLineThreadActivity/QwtVertexScaleDraw.cpp
        src/QwtExt/TimeLineThreadActivity/QwtTimeLineTransitionActivity.cpp
        src/WindowWidgets/NewProjectWindow.cpp
        src/DataVisualization/InputHandler3D.cpp
        src/MainTree/MainTreeItem.cpp
        src/MainTree/MainTreeModel.cpp
        src/MainTree/ProjectTreeItem.cpp
        )

set(HEADERS
        src/mainwindow.h
        src/matrix_model.h
        src/view/elements/arrow_line.h
        src/view/elements/position.h
        src/view/elements/transition.h
        src/view/elements/petri_object.h
        src/view/GraphicsView.h
        src/view/GraphicsViewZoom.h
        src/view/toolbox/toolbox.h
        src/view/toolbox/tool.h
        src/ffi/rust.h
        src/view/GraphicScene.h
        src/graphviz/graphviz_wrapper.h
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
        src/QwtExt/qwt_ext_plot_curve_labels.h
        src/QwtExt/CanvasPicker.h
        src/synthesis/synthesis_window.h
        src/view/Commands.h
        src/modules/reachability/reachability_tree_scene.h
        src/ffi/reachability.h
        src/modules/reachability/reachability_node.h
        src/modules/reachability/reachability_line.h
        src/modules/reachability/reachability_view.h
        src/modules/reachability/reachability_window.h
        src/MainTree/MainTreeItem.h
        src/MainTree/MainTreeModel.h
        src/MainTree/ProjectTreeItem.h
        src/MainTree/AnalysisTreeItem.cpp
        src/MainTree/AnalysisTreeItem.h
        src/MainTree/ModelTreeItem.cpp
        src/MainTree/ModelTreeItem.h
        src/MainTree/MainTreeView.cpp
        src/MainTree/MainTreeView.h
        src/MainTree/DecomposeTreeItem.h
        src/MainTree/ReachabilityTreeItem.h
        src/view/Commands/AddCommand.h
        src/view/Commands/MarkCommand.h
        src/view/Commands/MoveCommand.h
        src/view/Commands/RotateCommand.h
        src/view/Commands/ConnectCommand.h
        src/view/Commands/RemoveCommand.h
        src/view/Simulation/SimulationWidget.h
        src/ffi/simulation.h
        src/QwtExt/TimeLineThreadActivity/QwtVertexScaleDraw.h
        src/QwtExt/TimeLineThreadActivity/QwtTimeLineTransitionActivity.h
        src/WindowWidgets/NewProjectWindow.h
        src/DataVisualization/InputHandler3D.h)

link_directories("${CMAKE_SOURCE_DIR}/lib")

if (APPLE)
    add_executable(ffi_rust
            ${SRC_FILES}
            ${HEADERS}
            mainwindow.ui
            resources/resources.qrc
            )
else()
    add_executable(ffi_rust
            #WIN32
            ${SRC_FILES}
            ${HEADERS}
            mainwindow.ui
            resources/resources.qrc
            )
endif()

if (APPLE)
    set(FFI_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/rust/target/release/libhello_world.a")
else()
    set(FFI_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/rust/target/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>/hello_world.lib")
endif()

target_link_libraries(ffi_rust PRIVATE
        ${FFI_LIBRARY_PATH}
        #"${CMAKE_SOURCE_DIR}/rust/target/debug/hello_world.lib"
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        Qt6::Svg
        Qt6::DataVisualization
        ads::qtadvanceddocking
        libopenblas
        gvc
        cgraph
        ${QWT_LIBRARY}
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

if (APPLE)
    set(APPS ${CMAKE_INSTALL_PREFIX_DIR}/${PROJECT_NAME}.app)

    if(CMAKE_PREFIX_PATH)
        foreach(dir ${CMAKE_PREFIX_PATH})
            list(APPEND DIRS "${dir}/bin" "${dir}/lib")
        endforeach()
    endif()
    set_target_properties(ffi_rust PROPERTIES
            BUNDLE True
            )

    install(CODE "
        include(BundleUtilites)
        fixup_bundle(\"\" \"${QWT_LIBRARY}\" \"${QWT_LIBRARY}\")
    ")
endif()

if (WIN32)
    include(InstallRequiredSystemLibraries)
    install(FILES
            # Copy Qt6 dlls
            "${QT6_BIN_DIR}/Qt6Core.dll"
            "${QT6_BIN_DIR}/Qt6Gui.dll"
            "${QT6_BIN_DIR}/Qt6Widgets.dll"
            "${QT6_BIN_DIR}/Qt6Svg.dll"
            "${QT6_BIN_DIR}/Qt6OpenGLWidgets.dll"
            "${QT6_BIN_DIR}/Qt6OpenGL.dll"

            # Copy Graphviz
            "${GRAPHVIZ_BIN_DIR}/cgraph.dll"
            "${GRAPHVIZ_BIN_DIR}/gvc.dll"
            "${GRAPHVIZ_BIN_DIR}/gvplugin_core.dll"
            "${GRAPHVIZ_BIN_DIR}/gvplugin_dot_layout.dll"
            "${GRAPHVIZ_BIN_DIR}/gvplugin_neato_layout.dll"
            "${GRAPHVIZ_BIN_DIR}/gvplugin_pango.dll"
            "${GRAPHVIZ_BIN_DIR}/xdot.dll"
            "${GRAPHVIZ_BIN_DIR}/config6"

            # Copy Qwt
            "${QWT_BIN_DIR}/qwt.dll"

            # Copy QtAdvancedDockingSystem
            "${Qt_ADVANCED_DOCKING_SYSTEM_BIN_DIR}/qtadvanceddocking.dll"
            DESTINATION bin)

    # Copy Qt6 platform plugins
    install(FILES "${QT6_MAIN_DIR}/plugins/platforms/qwindows.dll" DESTINATION bin/plugins/platforms)
    # Copy imageformats Qt
    install(FILES "${QT6_MAIN_DIR}/plugins/imageformats/qsvg.dll" DESTINATION bin/plugins/imageformats)
    # Copy Qt6 styles
    install(FILES "${QT6_MAIN_DIR}/plugins/styles/qwindowsvistastyle.dll" DESTINATION bin/plugins/styles)

    set(CPACK_PACKAGE_NAME "GPTN")
    set(CPACK_PACKAGE_DIRECTORY "packages")
    set(CPACK_PACKAGE_VENDOR "Nickolay Muraviev")
    set(CPACK_PACKAGE_VERSION_MAJOR "0")
    set(CPACK_PACKAGE_VERSION_MINOR "1")
    install(TARGETS ffi_rust RUNTIME DESTINATION bin)
    include(CPack)
endif()
