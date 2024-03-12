# Add Qt-Advanced-Docking-System
set(BUILD_STATIC TRUE CACHE BOOL "ADS: Build shared library")
set(BUILD_EXAMPLES FALSE CACHE BOOL "ADS: Build examples")
set(ADS_VERSION "3.8.1" CACHE STRING "ADS: Version")
add_subdirectory(qtads EXCLUDE_FROM_ALL)
set_target_properties(qt6advanceddocking PROPERTIES
        INTERFACE_COMPILE_DEFINITIONS "ADS_STATIC"
        FOLDER thirdparty)

# Add Graphviz
set(CMAKE_PROGRAM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/graphviz/windows/dependencies/graphviz-build-utilities;${CMAKE_PROGRAM_PATH}")
set(CMAKE_PROGRAM_PATH "${CMAKE_CURRENT_SOURCE_DIR}/graphviz/windows/dependencies/graphviz-build-utilities/winflexbison;${CMAKE_PROGRAM_PATH}")
set(with_expat OFF CACHE STRING "")
set(enable_ltdl ON CACHE STRING "")
set(with_zlib OFF CACHE STRING "")
set(with_gvedit OFF CACHE STRING "")
set(with_smyrna OFF CACHE STRING "")
set(install_win_dependency_dlls OFF CACHE BOOL "")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/graphviz/build)
add_subdirectory(graphviz EXCLUDE_FROM_ALL)