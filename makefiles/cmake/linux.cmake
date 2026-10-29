# Linux settings. The SDK's own (_LINUX, POSIX, _GLIBCXX_USE_CXX11_ABI=0, -fPIC,
# the export map, tier0 / steam_api) come through sourcesdk::sourcesdk.

set(PLUGIN_COMPILE_DEFINITIONS
    ${PLUGIN_COMPILE_DEFINITIONS}
    _FILE_OFFSET_BITS=64
    _snprintf=snprintf
)

set(PLUGIN_COMPILE_OPTIONS
    -Wall -Wno-uninitialized -Wno-switch -Wno-unused
    -Wno-non-virtual-dtor -Wno-overloaded-virtual
    -Wno-invalid-offsetof -Wno-reorder
    -mfpmath=sse -msse -fno-strict-aliasing
    -fno-threadsafe-statics
    # The SDK builds with -fvisibility=default; only the export map decides
    # what leaves the .so, but nothing of ours needs to be in its symbol table.
    -fvisibility=hidden -fvisibility-inlines-hidden
)

# No --exclude-libs: CreateInterface itself comes from the SDK's static
# tier0_perproject; the SDK's version script already keeps everything else in.
set(PLUGIN_LINK_OPTIONS
    -static-libstdc++
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    list(APPEND PLUGIN_LINK_OPTIONS -static-libgcc)
else ()
    list(APPEND PLUGIN_LINK_OPTIONS -lgcc_eh)
endif ()
