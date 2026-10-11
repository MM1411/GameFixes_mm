# The install layout -- CMake twin of makefiles/ambuild/package.ambuild: the
# binary in addons/${PROJECT_NAME}/bin/<platform>/, and configs/ copied over the
# build root, carrying the metamod .vdf metamod.cmake wrote into it.
if (WIN32)
    set(PLUGIN_BIN_DIR "win64")
else ()
    set(PLUGIN_BIN_DIR "linuxsteamrt64")
endif ()

set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/addons/${PROJECT_NAME}/bin/${PLUGIN_BIN_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/addons/${PROJECT_NAME}/bin/${PLUGIN_BIN_DIR}"
)

# configs/ (the .ini and the generated metamod .vdf) lands next to the binary,
# so ${CMAKE_BINARY_DIR}/addons is a complete, drop-in install tree.
add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/configs ${CMAKE_BINARY_DIR}
)
