/**
 * vim: set ts=4 sw=4 tw=99 noet:
 * =============================================================================
 * GameFixes_mm
 * Copyright (C) 2026 Michal "Slynx (˙·٠● S l y n x ●٠·˙)" Přikryl.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *   - Michal "Slynx (˙·٠● S l y n x ●٠·˙)" Přikryl
 *
 * Project: GameFixes_mm
 */

#pragma once

#include "utils.hpp"

#include <dynlibutils/virtual.hpp>
#include <eiface.h>
#include <igameevents.h>
#include <iserver.h>
#include <ISmmPlugin.h>

// After ISmmPlugin.h: the SH_* hook macros go through this plugin's own
// SourceHook (vendor/sourcehook, with inline hooks), not metamod's shared one.
#include <sourcehook/sourcehook_metamod_override.h>

// Handlers of vtable hooks (SH_ADD_HOOK / SH_ADD_DVPHOOK) answer through these,
// never through RETURN_META / SH_IFACEPTR. Those pick the inline-hook frame
// whenever SourceHook's inline frame stack is not empty, and it stays set while
// an inline-hooked function's original runs: a hooked virtual called from in
// there (CheckSteamBan kicking a client into Disconnect, AcceptInput running a
// server command) would read that frame's this and result instead of its own.
#define VHOOK_IFACEPTR(type) reinterpret_cast<type*>(SH_GLOB_SHPTR->GetIfacePtr())
#define VHOOK_RETURN(result) do { SH_GLOB_SHPTR->SetRes(result); return; } while (0)
#define VHOOK_RETURN_VALUE(result, value) do { SH_GLOB_SHPTR->SetRes(result); return (value); } while (0)

#include "IGameFixes.h"

#include <memory>
#include <vector>

class CFix;
struct FixModules;

class Plugin final : public ISmmPlugin, public IMetamodListener, public IGameFixes
{
public:
    void* OnMetamodQuery(const char* iface, int* ret) override;

public: // IGameFixes
    bool SteamBanAllowAccount(uint64_t steamId64) override;
    bool SteamBanDisallowAccount(uint64_t steamId64) override;

    bool Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late) override;
    bool Unload(char* error, size_t maxlen) override;

    const char* GetAuthor() override;
    const char* GetName() override;
    const char* GetDescription() override;
    const char* GetURL() override;
    const char* GetLicense() override;
    const char* GetVersion() override;
    const char* GetDate() override;
    const char* GetLogTag() override;

public: // Hooks
    void CSource2Server_GameFrame(bool simulating, bool bFirstTick, bool bLastTick);
    void INetworkServerService_StartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession* pSession, const char* pszMapName);
    int CGameEventManager_LoadEventsFromFile(const char* pszFilename, bool bSearchAll);

    // SourceHook ids, 0 while not hooked.
    int m_iGameFrameHookID = 0;
    int m_iStartupServerHookID = 0;
    int m_iLoadEventsFromFileHookID = 0;

private:
    std::vector<std::unique_ptr<CFix>> CreateFixes();
    // A loaded fix by its name, or null
    CFix* FindFix(const char* pszName) const;
    bool LoadFixes(const FixModules& modules, char* error, size_t maxlen);

    std::vector<std::unique_ptr<CFix>> m_fixes;

    // IGameEventManager2 has no interface to fetch: the CGameEventManager vtable (from libserver) is hooked and the first LoadEventsFromFile hands the instance over -- see CGameEventManager_LoadEventsFromFile.
    DynLibUtils::VirtualTable m_GameEventManagerVTable;
    IGameEventManager2* m_pGameEventManager = nullptr;
};

extern Plugin g_Plugin;

PLUGIN_GLOBALVARS();
