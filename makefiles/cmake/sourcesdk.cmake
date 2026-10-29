# Wend4r/sourcesdk, built as part of this project: it brings its own static
# tier1 / mathlib / entity2 / kv3lib / interfaces / protobuf libraries and the
# imported tier0 and steam_api, all behind the sourcesdk::sourcesdk target.

# The protos the plugin uses on top of the ones the SDK itself needs
# (netmessages, network_connection, networkbasetypes,
# networksystem_protomessages, gameevents, usermessages, source2_steam_stats,
# valveextensions): none, so the SDK's minimal list stands.
set(SOURCESDK_GAME_TARGET "cs2" CACHE STRING "" FORCE)

# Valve's allocator (tier0's g_pMemAlloc), as the game's own modules use it.
set(SOURCESDK_MALLOC_OVERRIDE ON CACHE BOOL "" FORCE)

# CreateInterface is the SDK's (tier0_perproject/interface.cpp): PLUGIN_EXPOSE
# registers the plugin with EXPOSE_SINGLE_INTERFACE_GLOBALVAR, which it serves.
set(SOURCESDK_CREATE_INTEFACE_OVERRIDE OFF CACHE BOOL "" FORCE)

# The SDK's version script exports CreateInterface and its own exports only.
set(SOURCESDK_CONFIGURE_EXPORT_MAP ON CACHE BOOL "" FORCE)

# The C++/STL symbols tier0 and steam_api re-export would otherwise bind to
# ours under the static libstdc++.
set(SOURCESDK_LINK_STRIP_CPP_EXPORTS ON CACHE BOOL "" FORCE)

add_subdirectory(${SOURCESDK_DIR} sourcesdk)
