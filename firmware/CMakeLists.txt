# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

#If IOT_SOLUTION_PATH is not defined, use relative path as default value
if(NOT DEFINED ENV{IOT_SOLUTION_PATH})
    get_filename_component(IOT_SOLUTION_PATH "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)
    set(ENV{IOT_SOLUTION_PATH} ${IOT_SOLUTION_PATH})
    message(WARNING "Can't detect IOT_SOLUTION_PATH in your environment, we infer it is $ENV{IOT_SOLUTION_PATH}")
endif()


include($ENV{IOT_SOLUTION_PATH}/component.cmake)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

list(APPEND EXTRA_COMPONENT_DIRS EXTRA_COMPONENT_DIRS "$ENV{ESP_AZURE_PATH}/port")



set(VERSION_MAJOR_REGEX "[\t]*#define[ \t]+VERSION_MAJOR[ \t]+[\"]?([0-9]+)[\"]?")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/main/inc/version.h" VERSION_MAJOR REGEX ${VERSION_MAJOR_REGEX})
string(REGEX REPLACE ${VERSION_MAJOR_REGEX} "\\1" VERSION_MAJOR "${VERSION_MAJOR}")

set(VERSION_MINOR_REGEX "[\t]*#define[ \t]+VERSION_MINOR[ \t]+[\"]?([0-9]+)[\"]?")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/main/inc/version.h" VERSION_MINOR REGEX ${VERSION_MINOR_REGEX})
string(REGEX REPLACE ${VERSION_MINOR_REGEX} "\\1" VERSION_MINOR "${VERSION_MINOR}")

set(VERSION_PATCH_REGEX "[\t]*#define[ \t]+VERSION_PATCH[ \t]+[\"]?([0-9]+)[\"]?")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/main/inc/version.h" VERSION_PATCH REGEX ${VERSION_PATCH_REGEX})
string(REGEX REPLACE ${VERSION_PATCH_REGEX} "\\1" VERSION_PATCH "${VERSION_PATCH}")

add_compile_options(-fdiagnostics-color=always)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w -Wno-dev -fstack-protector-strong")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -Wno-dev -fstack-protector-strong")
# set(PROJECT_VER "${VERSION_MAJOR}-${VERSION_MINOR}-${VERSION_PATCH}-${VERSION_BUILD}")
set(PROJECT_VER "${VERSION_MAJOR}-${VERSION_MINOR}-${VERSION_PATCH}")

project("NewRangeOCS_UI_v${PROJECT_VER}")
