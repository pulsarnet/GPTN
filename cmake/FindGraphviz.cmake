# Graphviz root must be defined in CMAKE_PREFIX_PATH
#
# Try to find Graphviz.
# This will define:
# GRAPHVIZ_FOUND - system has Graphviz
# GRAPHVIZ_INCLUDE_DIR - the Graphviz include directory
# GRAPHVIZ_XXX_LIBRARY - Graphviz xxx library
# GRAPHVIZ_LIBRARIES - Link these to use Graphviz (not cached)

# First, find include dir
find_path(GRAPHVIZ_INCLUDE_DIR graphviz/gvc.h PATH_SUFFIXES include)

set(Graphviz_ROOT ${GRAPHVIZ_INCLUDE_DIR}/..)
find_path(GRAPHVIZ_LIB_DIR gvc.lib HINTS ${Graphviz_ROOT} PATH_SUFFIXES lib NO_DEFAULT_PATH)
find_path(GRAPHVIZ_BIN_DIR gvc.dll HINTS ${Graphviz_ROOT} PATH_SUFFIXES bin NO_DEFAULT_PATH)
find_file(GRAPHVIZ_CONFIG NAMES config config6 HINTS ${Graphviz_ROOT} PATH_SUFFIXES bin NO_DEFAULT_PATH)

list(PREPEND Graphviz_FIND_COMPONENTS cdt cgraph)
list(REMOVE_DUPLICATES Graphviz_FIND_COMPONENTS)
foreach (LIB ${Graphviz_FIND_COMPONENTS})
    find_library(Graphviz_${LIB}_LIBRARY NAMES ${LIB} HINTS ${GRAPHVIZ_LIB_DIR} NO_DEFAULT_PATH)
    find_file(Graphviz_${LIB}_LIBRARY_DLL NAMES "${LIB}.dll" HINTS ${GRAPHVIZ_BIN_DIR} NO_DEFAULT_PATH)
    if (Graphviz_${LIB}_LIBRARY AND Graphviz_${LIB}_LIBRARY_DLL)
        set(Graphviz_${LIB}_FOUND TRUE)
        add_library(Graphviz::${LIB} SHARED IMPORTED)
        set_target_properties(Graphviz::${LIB} PROPERTIES
                IMPORTED_LOCATION "${Graphviz_${LIB}_LIBRARY_DLL}"
                IMPORTED_IMPLIB "${Graphviz_${LIB}_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${GRAPHVIZ_INCLUDE_DIR}"
        )
    endif ()
endforeach ()

if(GRAPHVIZ_INCLUDE_DIR AND EXISTS "${GRAPHVIZ_INCLUDE_DIR}/graphviz/graphviz_version.h")
    file(STRINGS "${GRAPHVIZ_INCLUDE_DIR}/graphviz/graphviz_version.h" graphviz_version_str
            REGEX "^#define[\t ]+PACKAGE_VERSION[\t ]+\".*\"")

    string(REGEX REPLACE "^#define[\t ]+PACKAGE_VERSION[\t ]+\"([^\"]*)\".*" "\\1"
            Graphviz_VERSION_STRING "${graphviz_version_str}")
    unset(graphviz_version_str)
endif()


# handle the QUIETLY and REQUIRED arguments and set GRAPHVIZ_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Graphviz
        FOUND_VAR GRAPHVIZ_FOUND
        REQUIRED_VARS Graphviz_ROOT GRAPHVIZ_INCLUDE_DIR GRAPHVIZ_BIN_DIR Graphviz_cgraph_LIBRARY Graphviz_cdt_LIBRARY
        VERSION_VAR Graphviz_VERSION_STRING
)
