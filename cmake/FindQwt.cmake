# Graphviz root must be defined in CMAKE_PREFIX_PATH
#
# Try to find Graphviz.
# This will define:
# GRAPHVIZ_FOUND - system has Graphviz
# GRAPHVIZ_INCLUDE_DIR - the Graphviz include directory
# GRAPHVIZ_XXX_LIBRARY - Graphviz xxx library
# GRAPHVIZ_LIBRARIES - Link these to use Graphviz (not cached)

# First, find include dir
find_path(QWT_INCLUDE_DIR qwt_global.h PATH_SUFFIXES include REQUIRED)

set(Qwt_ROOT ${QWT_INCLUDE_DIR}/..)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(QWT_LIB_NAMES qwt)
    set(QWT_DLL_NAMES qwt.dll)
else ()
    set(QWT_LIB_NAMES qwtd)
    set(QWT_DLL_NAMES qwtd.dll)
endif ()


find_library(QWT_LIBRARY NAMES ${QWT_LIB_NAMES} HINTS ${Qwt_ROOT} PATH_SUFFIXES lib NO_DEFAULT_PATH REQUIRED)
find_file(QWT_LIBRARY_DLL NAMES ${QWT_DLL_NAMES} HINTS ${Qwt_ROOT} PATH_SUFFIXES lib NO_DEFAULT_PATH REQUIRED)

add_library(Qwt::Qwt SHARED IMPORTED)
set_target_properties(Qwt::Qwt PROPERTIES
        IMPORTED_LOCATION "${QWT_LIBRARY_DLL}"
        IMPORTED_IMPLIB "${QWT_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${QWT_INCLUDE_DIR}"
)

file(STRINGS "${QWT_INCLUDE_DIR}/qwt_global.h" QWT_VERSION_STR REGEX "^#define[\t ]+QWT_VERSION_STR[\t ]+\".*\"")
string(REGEX REPLACE "^#define[\t ]+QWT_VERSION_STR[\t ]+\"([^\"]*)\".*" "\\1" QWT_VERSION_STRING ${QWT_VERSION_STR})

# handle the QUIETLY and REQUIRED arguments and set GRAPHVIZ_FOUND to TRUE if all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Qwt
        FOUND_VAR QWT_FOUND
        REQUIRED_VARS QWT_INCLUDE_DIR QWT_LIBRARY QWT_LIBRARY_DLL
        VERSION_VAR QWT_VERSION_STRING
)
