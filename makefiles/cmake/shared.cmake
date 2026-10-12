# Settings every platform shares -- CMake twin of makefiles/ambuild/shared.ambuild.
# The per-platform ones are in linux.cmake / windows.cmake.

if (UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif ()

if (WIN32 AND NOT MSVC)
    message(FATAL "MSVC restricted.")
endif ()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING
        "Only do Release and Debug"
        FORCE
)

set(CMAKE_CXX_STANDARD 20)


set(CMAKE_STATIC_LIBRARY_PREFIX "")

# s2sdk's include paths, defines and libraries come from s2sdk-manifests, as
# s2sdk::s2sdk (linked in CMakeBuilder.cmake) -- SdkHelpers.configureCxx() on the
# AMBuild side.

# META_IS_SOURCE2 and position independent code come from s2sdk-manifests.

if (DEFINED ENV{GITHUB_SHA_SHORT})
    add_definitions(-DGITHUB_SHA="$ENV{GITHUB_SHA_SHORT}")
else ()
    add_definitions(-DGITHUB_SHA="Local")
endif ()

if (DEFINED ENV{SEMVER})
    add_definitions(-DSEMVER="$ENV{SEMVER}")
else ()
    add_definitions(-DSEMVER="Local")
endif ()

include_directories(
    ${CMAKE_SOURCE_DIR}/public
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/sdk
    ${METAMOD_DIR}/core
    # KHook: metamod's detour library. Header-only on the plugin side, the
    # engine behind it lives in metamod.so and is handed over by PLUGIN_SAVEVARS().
    ${METAMOD_DIR}/third_party/khook/include
    ${CMAKE_SOURCE_DIR}/vendor
    ${CMAKE_SOURCE_DIR}/vendor/dynlibutils/include
    ${CMAKE_SOURCE_DIR}/vendor/dynlibutils/include/dynlibutils
)
