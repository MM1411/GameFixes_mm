# Windows (MSVC) settings. The SDK's own (_WIN32, COMPILER_MSVC*, /permissive-,
# /utf-8, the static runtime, its libraries) come through sourcesdk::sourcesdk.

set(PLUGIN_COMPILE_OPTIONS
    /bigobj
    /EHsc
    /Zi
)

set(PLUGIN_LINK_OPTIONS
    /DEBUG
    $<$<CONFIG:Release>:/OPT:REF>
    $<$<CONFIG:Release>:/OPT:ICF>
)
