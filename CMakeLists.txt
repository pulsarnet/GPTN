cmake_minimum_required(VERSION 3.17.3)
project(GPTN VERSION 0.1.1)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(TARGET_NAME GPTN)
set(TARGET_DIR ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TARGET_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TARGET_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${TARGET_DIR}/lib)
set(CMAKE_AUTORCC ON)
set(VERSION_COMPONENT_BUILD 1)

find_package(Graphviz REQUIRED COMPONENTS gvc pathplan)
find_package(qt6advanceddocking REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Svg Charts DataVisualization)
find_package(Qwt REQUIRED)

add_compile_definitions(QWT_DLL)

set(QT6_MAIN_DIR ${Qt6_DIR})

if(NOT WIN32)
    add_compile_definitions(__cpp_lib_format=201907L)
endif()

if(WIN32)
    configure_file(cmake/gptn.rc.cmake ${CMAKE_CURRENT_BINARY_DIR}/gptn.rc)
    set(WIN_RC_FILE ${CMAKE_CURRENT_BINARY_DIR}/gptn.rc)
endif()

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
            resources/resources.qrc
            )
else()
    add_executable(${TARGET_NAME}
            WIN32
            ${SRC_FILES}
            ${HEADERS}
            ${WIN_RC_FILE}
            resources/resources.qrc
            )
endif()

# RUST {
include(AddRustLibrary)
build_rust_library(
        TARGET       ptn_lib
        SOURCE_DIR   "${CMAKE_SOURCE_DIR}/rust"
        LIB_FILENAME ptn_lib.lib
)
# } RUST


target_link_libraries(${TARGET_NAME} PRIVATE
        ptn_lib
        ads::qt6advanceddocking
        Graphviz::cgraph
        Graphviz::gvc
        Graphviz::pathplan
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        Qt6::Svg
        Qt6::DataVisualization
        Qwt::Qwt
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


if (WIN32)
    include(InstallRequiredSystemLibraries)
    install(IMPORTED_RUNTIME_ARTIFACTS
            Qt6::Core
            Qt6::Widgets
            Qt6::Gui
            Qt6::Svg
            Qt6::Charts
            Qt6::DataVisualization
            Graphviz::cgraph
            Graphviz::cdt
            Graphviz::gvc
            Graphviz::pathplan
            Qwt::Qwt
            ads::qt6advanceddocking
            RUNTIME DESTINATION .)

    # Copy Qt6 platform plugins
    install(FILES "${QT6_MAIN_DIR}/plugins/platforms/qwindows.dll" DESTINATION ./plugins/platforms)
    # Copy imageformats Qt
    install(FILES "${QT6_MAIN_DIR}/plugins/imageformats/qsvg.dll" DESTINATION ./plugins/imageformats)
    # Copy Qt6 styles
    install(FILES "${QT6_MAIN_DIR}/plugins/styles/qwindowsvistastyle.dll" DESTINATION ./plugins/styles)

    set(CPACK_PACKAGE_NAME "${TARGET_NAME}")
    set(CPACK_PACKAGE_DIRECTORY "packages")
    set(CPACK_PACKAGE_VENDOR "Nickolay Muraviev")
    set(CPACK_PACKAGE_VERSION_MAJOR ${CMAKE_PROJECT_VERSION_MAJOR})
    set(CPACK_PACKAGE_VERSION_MINOR ${CMAKE_PROJECT_VERSION_MINOR})
    set(CPACK_PACKAGE_VERSION_PATCH ${CMAKE_PROJECT_VERSION_PATCH})
    set(CPACK_WIX_UPGRADE_GUID 193923F9-F4F0-4F30-B2CA-6C97C2D3FA45)
    set(CPACK_CREATE_DESKTOP_LINKS "GPTN")
    set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "GPTN")
    set(CPACK_GENERATOR WIX)
    set(CPACK_PACKAGE_EXECUTABLES ${TARGET_NAME} "GPTN")
    # todo: разобраться с расширением
    set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE.txt)
    install(TARGETS ${TARGET_NAME} RUNTIME DESTINATION .)
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
            $<TARGET_FILE:Graphviz::gvc>
            $<TARGET_FILE:Graphviz::cgraph>
            $<TARGET_FILE:Graphviz::cdt>
            $<TARGET_FILE:Graphviz::pathplan>
            ${GRAPHVIZ_CONFIG}
            $<TARGET_FILE_DIR:${TARGET_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:Qwt::Qwt>
            $<TARGET_FILE_DIR:${TARGET_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:ads::qt6advanceddocking>
            $<TARGET_FILE_DIR:${TARGET_NAME}>
    )
endif()

if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
endif()