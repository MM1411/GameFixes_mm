# Settings every platform shares. The per-platform ones are in linux.cmake /
# windows.cmake.

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif ()

if (WIN32 AND NOT MSVC)
    message(FATAL_ERROR "MSVC restricted.")
endif ()

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif ()

set(CMAKE_STATIC_LIBRARY_PREFIX "")

# The version the plugin reports: the release tag and the commit.
if (DEFINED ENV{GITHUB_SHA_SHORT})
    set(PLUGIN_GITHUB_SHA "$ENV{GITHUB_SHA_SHORT}")
else ()
    set(PLUGIN_GITHUB_SHA "Local")
endif ()

if (DEFINED ENV{SEMVER} AND NOT "$ENV{SEMVER}" STREQUAL "")
    set(PLUGIN_SEMVER "$ENV{SEMVER}")
else ()
    set(PLUGIN_SEMVER "Local")
endif ()

set(PLUGIN_COMPILE_DEFINITIONS
    GITHUB_SHA="${PLUGIN_GITHUB_SHA}"
    SEMVER="${PLUGIN_SEMVER}"
)

# vendor/sourcehook/include ahead of metamod's core/: "sourcehook/sourcehook.h"
# would otherwise find metamod's own (older) core/sourcehook/ copy.
set(PLUGIN_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/vendor/sourcehook/include
    ${CMAKE_SOURCE_DIR}/public
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/vendor
    ${CMAKE_SOURCE_DIR}/vendor/dynlibutils/include
    ${CMAKE_SOURCE_DIR}/vendor/dynlibutils/include/dynlibutils
)
