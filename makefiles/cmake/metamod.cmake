# Metamod:Source build 1411 (alliedmodders/metamod-source 7ec0f16, the last one
# before KHook replaced SourceHook): only core/'s plugin API headers.
#
# NOT core/sourcehook: the plugin brings its own private SourceHook
# (vendor/sourcehook), whose include/sourcehook.h shim is what ISmmPlugin.h's
# bare #include "sourcehook.h" resolves to.

if (NOT EXISTS "${METAMOD_DIR}/core/ISmmPlugin.h")
    message(FATAL_ERROR "METAMOD_DIR has no core/ISmmPlugin.h: \"${METAMOD_DIR}\"")
endif ()

if (EXISTS "${METAMOD_DIR}/third_party/khook")
    message(WARNING "METAMOD_DIR looks like a Metamod:Source with KHook (newer than build 1411); "
                    "a plugin built against it will not load on 1411.")
endif ()

set(METAMOD_COMPILE_DEFINITIONS
    META_IS_SOURCE2
)

set(METAMOD_INCLUDE_DIRS
    ${METAMOD_DIR}/core
)
