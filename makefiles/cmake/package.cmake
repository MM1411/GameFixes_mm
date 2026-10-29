# The install layout, the one Pisex's plugins use: the binary straight in
# addons/plugins/ -- utils' autoloader loads every .so / .dll under it, no
# metamod .vdf -- and the config in addons/configs/game_fixes/.
#
# ${CMAKE_BINARY_DIR}/package is a complete, drop-in tree for game/csgo/.

set(PLUGIN_PACKAGE_DIR "${CMAKE_BINARY_DIR}/package")

set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_PACKAGE_DIR}/addons/plugins"
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_PACKAGE_DIR}/addons/plugins"
    # Multi-config generators append the config otherwise.
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PLUGIN_PACKAGE_DIR}/addons/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PLUGIN_PACKAGE_DIR}/addons/plugins"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PLUGIN_PACKAGE_DIR}/addons/plugins"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PLUGIN_PACKAGE_DIR}/addons/plugins"
    # The .lib / .exp MSVC writes next to a DLL stay out of the package.
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/pdb"
)

# The repository's addons/ is the install tree's own (addons/configs/game_fixes/...).
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/addons ${PLUGIN_PACKAGE_DIR}/addons
)

if (LINUX AND CMAKE_BUILD_TYPE STREQUAL "Release")
    find_program(STRIP_EXECUTABLE strip)
    if (STRIP_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${STRIP_EXECUTABLE} --strip-unneeded $<TARGET_FILE:${PROJECT_NAME}>
            COMMENT "Stripping symbols from $<TARGET_FILE:${PROJECT_NAME}>"
        )
    endif ()
endif ()
