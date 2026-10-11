cmake_minimum_required(VERSION 3.18)
project(game_fixes C CXX ASM)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

if(NOT DEFINED ENV{MMSOURCE_DEV})
    message(FATAL_ERROR "Environment variable MMSOURCE_DEV is not set!")
endif()

# s2sdk (alliedmodders/s2sdk, formerly hl2sdk's cs2 branch): the first of
# these variables that is set wins.
foreach(var S2SDK_CS2 S2SDKCS2 S2SDK HL2SDK_CS2 HL2SDKCS2)
    if(DEFINED ENV{${var}} AND NOT "$ENV{${var}}" STREQUAL "")
        file(TO_CMAKE_PATH "$ENV{${var}}" SOURCESDK_DIR)
        break()
    endif()
endforeach()
if(NOT SOURCESDK_DIR)
    message(FATAL_ERROR "s2sdk not found: set S2SDK (or HL2SDKCS2) to your checkout!")
endif()

set(METAMOD_DIR $ENV{MMSOURCE_DEV})

include(makefiles/cmake/shared.cmake)
# s2sdk, for s2sdk_protobufs(): the game protobufs from its protos
add_subdirectory(${SOURCESDK_DIR} ${CMAKE_BINARY_DIR}/s2sdk)

add_subdirectory(vendor/dynlibutils)

if (LINUX)
    include(${CMAKE_SOURCE_DIR}/makefiles/cmake/linux.cmake)
elseif (WIN32)
    include(${CMAKE_SOURCE_DIR}/makefiles/cmake/windows.cmake)
endif()

# The metamod .vdf from makefiles/metamod/ (metamod.ambuild on the AMBuild side).
include(${CMAKE_SOURCE_DIR}/makefiles/cmake/metamod.cmake)

# The plugin binary (makefiles/ambuild/AMBuilder on the AMBuild side).
include(${CMAKE_SOURCE_DIR}/makefiles/cmake/CMakeBuilder.cmake)

# Install layout (makefiles/ambuild/package.ambuild on the AMBuild side).
include(${CMAKE_SOURCE_DIR}/makefiles/cmake/package.cmake)
