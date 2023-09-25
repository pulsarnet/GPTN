cmake_minimum_required(VERSION 3.17.3)
project(GPTN VERSION 0.1)

message(STATUS ${CMAKE_CXX_FLAGS})
set(CMAKE_CXX_STANDARD 20)

set(TARGET_NAME GPTN)
set(TARGET_DIR ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TARGET_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TARGET_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${TARGET_DIR}/lib)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)
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

find_library(QWT_LIBRARY_D
        NAMES qwtd
        HINTS ${QWT_DIR}/lib
        REQUIRED)

if(QWT_LIBRARY)
    include_directories(${QWT_DIR}/include)
    message("QWT optimized found: ${QWT_LIBRARY}")
endif()

if(QWT_LIBRARY_D)
    message("QWT debug found: ${QWT_LIBRARY_D}")
endif()

if(NOT WIN32)
    #set(CMAKE_PREFIX_PATH "/Users/nikolajmuravev/Qt/6.2.1/macos/lib/cmake")
    add_compile_definitions(__cpp_lib_format=201907L)
endif()

set(CMAKE_PREFIX_PATH ${Qt6_DIR})
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Svg Charts DataVisualization)

set(CMAKE_PREFIX_PATH ${Qt_ADVANCED_DOCKING_SYSTEM_DIR})
find_package(qt6advanceddocking)

set(SRC_FILES
        main.cpp
        src/ActionTabWidget/ActionTabWidget.cpp
        src/ActionTabWidget/DecomposeModelTab.cpp
        src/ActionTabWidget/NetModelingTab.cpp
        src/ActionTabWidget/WrappedLayoutWidget.cpp
        src/Core/FFI/reachability.cpp
        src/Core/FFI/rust.cpp
        src/Core/FFI/simulation.cpp
        src/Core/FFI/vec.cpp
        src/Core/Graphviz/GraphvizWrapper.cpp
        src/Core/ProjectMetadata.cpp
        src/DataVisualization/InputHandler3D.cpp
        src/DockSystem/DockToolbar.cpp
        src/DockSystem/DockWidget.cpp
        src/DockSystem/SplittableComponentsFactory.cpp
        src/MainTree/MainTreeItem.cpp
        src/MainTree/MainTreeModel.cpp
        src/QwtExt/CanvasPicker.cpp
        src/QwtExt/TimeLineThreadActivity/QwtTimeLineTransitionActivity.cpp
        src/QwtExt/TimeLineThreadActivity/QwtVertexScaleDraw.cpp
        src/QwtExt/QwtExtPlotCurveLabels.cpp
        src/Settings/RecentProjects.cpp
        src/WindowWidgets/NewProjectWindow.cpp
        src/MainWindow.cpp
        src/modules/reachability/ReachabilityLine.cpp
        src/modules/reachability/ReachabilityNode.cpp
        src/modules/reachability/ReachabilityTreeScene.cpp
        src/modules/reachability/ReachabilityView.cpp
        src/modules/reachability/ReachabilityWindow.cpp
        src/overrides/MatrixWindow.cpp
        src/Synthesis/SynthesisTable.cpp
        src/Synthesis/SynthesisWindow.cpp
        src/Synthesis/SynthesisModel.cpp
        src/Editor/Commands/AddCommand.cpp
        src/Editor/Commands/ConnectCommand.cpp
        src/Editor/Commands/MarkCommand.cpp
        src/Editor/Commands/MoveCommand.cpp
        src/Editor/Commands/RemoveCommand.cpp
        src/Editor/Commands/RotateCommand.cpp
        src/Editor/GraphicsScene.cpp
        src/Editor/GraphicsView.cpp
        src/Editor/GraphicsViewZoom.cpp
        src/Editor/Simulation/SimulationWidget.cpp
        src/Editor/Elements/ArrowLine.cpp
        src/Editor/Elements/PetriObject.cpp
        src/Editor/Elements/Position.cpp
        src/Editor/Elements/Transition.cpp
)

set(HEADERS
        src/ActionTabWidget/ActionTabWidget.h
        src/ActionTabWidget/ActionTabWidgetController.cpp
        src/ActionTabWidget/ActionTabWidgetController.h
        src/ActionTabWidget/DecomposeModelTab.h
        src/ActionTabWidget/NetModelingTab.h
        src/ActionTabWidget/WrappedLayoutWidget.h
        src/Core/ApplicationProjectController.cpp
        src/Core/ApplicationProjectController.h
        src/Core/FFI/reachability.h
        src/Core/FFI/rust.h
        src/Core/FFI/simulation.h
        src/Core/Graphviz/GraphvizWrapper.h
        src/Core/ProjectMetadata.h
        src/DataVisualization/InputHandler3D.h
        src/DockSystem/DockToolbar.h
        src/DockSystem/DockWidget.h
        src/DockSystem/SplittableComponentsFactory.h
        src/Editor/Commands.h
        src/Editor/Commands/AddCommand.h
        src/Editor/Commands/ConnectCommand.h
        src/Editor/Commands/MarkCommand.h
        src/Editor/Commands/MoveCommand.h
        src/Editor/Commands/RemoveCommand.h
        src/Editor/Commands/RotateCommand.h
        src/Editor/Elements/ArrowLine.h
        src/Editor/Elements/PetriObject.h
        src/Editor/Elements/Position.h
        src/Editor/Elements/Transition.h
        src/Editor/GraphicsSceneActions.cpp
        src/Editor/GraphicsSceneActions.h
        src/Editor/GraphicsScene.h
        src/Editor/GraphicsView.h
        src/Editor/GraphicsViewZoom.h
        src/Editor/Simulation/SimulationWidget.h
        src/Editor/ToolBox/Tool.h
        src/Editor/ToolBox/ToolBox.h
#        src/MainTree/MainTreeController.cpp
#        src/MainTree/MainTreeController.h
#        src/MainTree/MainTreeItem.h
#        src/MainTree/MainTreeModel.h
#        src/MainTree/MainTreeView.cpp
#        src/MainTree/MainTreeView.h
        src/QwtExt/CanvasPicker.h
        src/QwtExt/TimeLineThreadActivity/QwtTimeLineTransitionActivity.h
        src/QwtExt/TimeLineThreadActivity/QwtVertexScaleDraw.h
        src/QwtExt/QwtExtPlotCurveLabels.h
        src/Settings/RecentProjects.h
        src/WindowWidgets/NewProjectWindow.h
        src/MainWindow.h
        src/modules/reachability/ReachabilityLine.h
        src/modules/reachability/ReachabilityNode.h
        src/modules/reachability/ReachabilityTreeScene.h
        src/modules/reachability/ReachabilityView.h
        src/modules/reachability/ReachabilityWindow.h
        src/NamedMatrixModel.h
        src/overrides/MatrixWindow.h
        src/Synthesis/SynthesisModel.h
        src/Synthesis/SynthesisTable.h
        src/Synthesis/SynthesisWindow.h
)

if (UNIX)
    add_executable(${TARGET_NAME}
            ${SRC_FILES}
            ${HEADERS}
            mainwindow.ui
            resources/resources.qrc
            )
else()
    add_executable(${TARGET_NAME}
            WIN32
            ${SRC_FILES}
            ${HEADERS}
            mainwindow.ui
            resources/resources.qrc
            )
endif()

# RUST {
include(AddExternalLibrary)
if(APPLE)
    set(FFI_LIBRARY_NAME "libptn_lib.a")
else()
    set(FFI_LIBRARY_NAME "ptn_lib.lib")
endif()
set(FFI_SOURCE_DIR "${CMAKE_SOURCE_DIR}/rust")
set(FFI_TARGET_DIR ${FFI_SOURCE_DIR}/target/release)
set(FFI_OUTPUT_PATH "${FFI_TARGET_DIR}/${FFI_LIBRARY_NAME}")
add_external_library(
        TARGET            ptn_lib
        COMMAND           cargo build --release
        WORKING_DIRECTORY "${FFI_SOURCE_DIR}"
        OUTPUT            "${FFI_OUTPUT_PATH}"
        COMMENT           "Cargo build ptn_lib"
)
# } RUST

# https://github.com/microsoft/ELL/wiki/Building-OpenBlas-on-Windows
find_package(BLAS)
message( STATUS BLAS found: ${BLAS_LIBRARIES} )

target_link_libraries(${TARGET_NAME} PRIVATE
        ptn_lib
        ${BLAS_LIBRARIES}
        ads::qt6advanceddocking
        gvc
        cgraph
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        Qt6::Svg
        Qt6::DataVisualization
)
target_link_libraries(${TARGET_NAME} PRIVATE
        optimized ${QWT_LIBRARY}
        debug ${QWT_LIBRARY_D}
        )

qt6_import_plugins(${TARGET_NAME} INCLUDE_BY_TYPE imageformats)

set_target_properties(${TARGET_NAME} PROPERTIES
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
    set_target_properties(${TARGET_NAME} PROPERTIES
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
            "${QT6_BIN_DIR}/Qt6DataVisualization.dll"

            # Copy Graphviz
            "${GRAPHVIZ_BIN_DIR}/cgraph.dll"
            "${GRAPHVIZ_BIN_DIR}/gvc.dll"
            "${GRAPHVIZ_BIN_DIR}/pathplan.dll"
            "${GRAPHVIZ_BIN_DIR}/expat.dll"
            "${GRAPHVIZ_BIN_DIR}/cdt.dll"
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

    set(CPACK_PACKAGE_NAME "${TARGET_NAME}")
    set(CPACK_PACKAGE_DIRECTORY "packages")
    set(CPACK_PACKAGE_VENDOR "Nickolay Muraviev")
    set(CPACK_PACKAGE_VERSION_MAJOR "0")
    set(CPACK_PACKAGE_VERSION_MINOR "1")
    set(CPACK_PACKAGE_VERSION_PATCH "1")
    set(CPACK_WIX_UPGRADE_GUID 193923F9-F4F0-4F30-B2CA-6C97C2D3FA45)
    set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "GPTN")
    set(CPACK_GENERATOR WIX)
    set(CPACK_PACKAGE_EXECUTABLES ${TARGET_NAME} "GPTN")
    install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION bin)
    include(CPack)

    add_custom_command(
            TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${TARGET_NAME}>/plugins/platforms
            COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${TARGET_NAME}>/plugins/styles
            COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${TARGET_NAME}>/plugins/imageformats
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt6::Widgets>
            $<TARGET_FILE:Qt6::Core>
            $<TARGET_FILE:Qt6::Gui>
            $<TARGET_FILE:Qt6::Svg>
            $<TARGET_FILE:Qt6::DataVisualization>
            $<TARGET_FILE:Qt6::OpenGL>
            $<TARGET_FILE:Qt6::OpenGLWidgets>
            $<TARGET_FILE_DIR:${TARGET_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt6::QWindowsIntegrationPlugin>
            $<TARGET_FILE_DIR:${TARGET_NAME}>/plugins/platforms/
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt6::QSvgPlugin>
            $<TARGET_FILE_DIR:${TARGET_NAME}>/plugins/imageformats/
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qt6::QWindowsVistaStylePlugin>
            $<TARGET_FILE_DIR:${TARGET_NAME}>/plugins/styles/
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${GRAPHVIZ_BIN_DIR}/cgraph.dll"
            "${GRAPHVIZ_BIN_DIR}/gvc.dll"
            "${GRAPHVIZ_BIN_DIR}/pathplan.dll"
            "${GRAPHVIZ_BIN_DIR}/expat.dll"
            "${GRAPHVIZ_BIN_DIR}/cdt.dll"
            "${GRAPHVIZ_BIN_DIR}/gvplugin_core.dll"
            "${GRAPHVIZ_BIN_DIR}/gvplugin_dot_layout.dll"
            "${GRAPHVIZ_BIN_DIR}/gvplugin_neato_layout.dll"
            "${GRAPHVIZ_BIN_DIR}/gvplugin_pango.dll"
            "${GRAPHVIZ_BIN_DIR}/xdot.dll"
            "${GRAPHVIZ_BIN_DIR}/config6"
            $<TARGET_FILE_DIR:${TARGET_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${QWT_BIN_DIR}/qwt$<$<CONFIG:Debug>:d>.dll"
            $<TARGET_FILE_DIR:${TARGET_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:ads::qt6advanceddocking>"
            $<TARGET_FILE_DIR:${TARGET_NAME}>
    )
endif()

if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
endif()