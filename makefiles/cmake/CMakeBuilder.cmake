# The plugin binary: its own sources, and what it links with.
# Included by CMakeLists.txt after shared.cmake and the platform file.

# Listed, not globbed: a new .cpp goes in here.
set(SOURCE_FILES
    src/plugin.cpp
    src/scheduler.cpp
    src/fixes/fix.cpp
    src/fixes/abstract_entity.cpp
    src/fixes/beam_crash.cpp
    src/fixes/console_spam.cpp
    src/fixes/demo_record.cpp
    src/fixes/hammer_id.cpp
    src/fixes/input_activator_crash.cpp
    src/fixes/rampbug.cpp
    src/fixes/water_jump.cpp
    src/fixes/server_list_players.cpp
    src/fixes/slow_animation.cpp
    src/fixes/steam_ban.cpp
    src/fixes/sv_cheats.cpp
    src/fixes/team_limit.cpp
    src/fixes/workshop_voice.cpp
    src/fixes/voice_flood.cpp
    src/sdk/schemasystem_helper.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    PREFIX ""
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    ${PLUGIN_COMPILE_DEFINITIONS}
    ${METAMOD_COMPILE_DEFINITIONS}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PLUGIN_INCLUDE_DIRS}
    ${METAMOD_INCLUDE_DIRS}
)

target_compile_options(${PROJECT_NAME} PRIVATE ${PLUGIN_COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME} PRIVATE ${PLUGIN_LINK_OPTIONS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    sourcehook
    dynlibutils
    sourcesdk::sourcesdk
)
