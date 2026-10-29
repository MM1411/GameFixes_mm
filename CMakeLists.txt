cmake_minimum_required(VERSION 3.18)

if (POLICY CMP0092)
    cmake_policy(SET CMP0092 NEW) # No default /W3 on MSVC.
endif ()

project(game_fixes C CXX)

# Every target here, the SDK and vendor libraries included, against the static
# MSVC runtime the game links with.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Where the dependencies live: a -D cache variable, else the environment, else
# the external/ submodules.
#   SOURCESDK_DIR  Wend4r/sourcesdk (main), cloned recursively
#   METAMOD_DIR    alliedmodders/metamod-source at build 1411 (7ec0f16), the
#                  last one before KHook -- only its core/ headers are used
function(resolve_dependency_dir VAR_NAME ENV_NAMES DEFAULT_DIR)
    if (NOT ${VAR_NAME})
        foreach (ENV_NAME ${ENV_NAMES})
            if (DEFINED ENV{${ENV_NAME}} AND NOT "$ENV{${ENV_NAME}}" STREQUAL "")
                set(${VAR_NAME} "$ENV{${ENV_NAME}}")
                break()
            endif ()
        endforeach ()
    endif ()
    if (NOT ${VAR_NAME})
        set(${VAR_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/external/${DEFAULT_DIR}")
    endif ()
    file(TO_CMAKE_PATH "${${VAR_NAME}}" ${VAR_NAME})
    if (NOT EXISTS "${${VAR_NAME}}")
        message(FATAL_ERROR "${VAR_NAME} not found: \"${${VAR_NAME}}\" (pass -D${VAR_NAME}=..., set ${ENV_NAMES} or check out the submodule)")
    endif ()
    set(${VAR_NAME} "${${VAR_NAME}}" PARENT_SCOPE)
endfunction()

resolve_dependency_dir(SOURCESDK_DIR "SOURCESDK_DIR;HL2SDKCS2" "sourcesdk")
resolve_dependency_dir(METAMOD_DIR "METAMOD_DIR;MMSOURCE_DEV" "metamod-source")

include(makefiles/cmake/shared.cmake)
include(makefiles/cmake/sourcesdk.cmake)
include(makefiles/cmake/metamod.cmake)

# This plugin's own private SourceHook (vector and inline hooks), not
# metamod's shared one -- see src/plugin.h. Its own test suite stays out.
set(SOURCEHOOK_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/sourcehook)
add_subdirectory(vendor/dynlibutils)

if (LINUX)
    include(makefiles/cmake/linux.cmake)
elseif (WIN32)
    include(makefiles/cmake/windows.cmake)
endif ()

# The plugin binary.
include(makefiles/cmake/CMakeBuilder.cmake)

# Install layout: addons/plugins/ and addons/configs/.
include(makefiles/cmake/package.cmake)
