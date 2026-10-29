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

#include "server_list_players.h"

#include "scheduler.h"
#include "sdk/schema/classes/CBasePlayerController.h"
#include "utils.hpp"
#include "vprof.hpp"

#include <eiface.h>
#include <entity2/entitysystem.h>
#include <interfaces/interfaces.h>
#include <tier0/keyvalues.h>

SH_DECL_HOOK0_void(ISource2Server, GameServerSteamAPIActivated, SH_NOATTRIB, 0);
SH_DECL_HOOK0_void(ISource2Server, GameServerSteamAPIDeactivated, SH_NOATTRIB, 0);

void CServerListPlayersFix::ReadConfig(KeyValues* pConfig)
{
    m_flUpdateInterval = pConfig->GetFloat("update_interval", 5.0f);

    if (m_flUpdateInterval < 1.0f)
        m_flUpdateInterval = 1.0f;
}

bool CServerListPlayersFix::Load(const FixModules& modules, char* error, size_t maxlen)
{
    if (!KeepHook(SH_ADD_HOOK(ISource2Server, GameServerSteamAPIActivated, g_pSource2Server, SH_MEMBER(this, &CServerListPlayersFix::CSource2Server_GameServerSteamAPIActivated), true)) ||
        !KeepHook(SH_ADD_HOOK(ISource2Server, GameServerSteamAPIDeactivated, g_pSource2Server, SH_MEMBER(this, &CServerListPlayersFix::CSource2Server_GameServerSteamAPIDeactivated), true)))
    {
        std::snprintf(error, maxlen, "failed to hook ISource2Server::GameServerSteamAPIActivated / Deactivated");
        return false;
    }

    // Loaded after the API already came up (late load): there won't be another activation, so try it right away.
    m_bSteamAPIReady = SteamGameServer() != nullptr;

    m_pUpdateTimer = scheduler::AddTimer(m_flUpdateInterval, [this]() { UpdatePlayers(); }, TIMER_FLAG_REPEAT);

    Log("pushing player data to Steam every %.0f s (Steam API %s)", m_flUpdateInterval, m_bSteamAPIReady ? "ready" : "not up yet");
    return true;
}

void CServerListPlayersFix::Unload()
{
    RemoveHooks();

    scheduler::KillTimer(m_pUpdateTimer);
    m_pUpdateTimer = nullptr;

    m_bSteamAPIReady = false;
}

void CServerListPlayersFix::CSource2Server_GameServerSteamAPIActivated()
{
    GF_VPROF("GameFixes::server_list_players::SteamAPIActivated");

    ISource2Server* pThis = VHOOK_IFACEPTR(ISource2Server);

    m_bSteamAPIReady = SteamGameServer() != nullptr;

    VHOOK_RETURN(MRES_IGNORED);
}

void CServerListPlayersFix::CSource2Server_GameServerSteamAPIDeactivated()
{
    GF_VPROF("GameFixes::server_list_players::SteamAPIDeactivated");

    ISource2Server* pThis = VHOOK_IFACEPTR(ISource2Server);

    m_bSteamAPIReady = false;

    VHOOK_RETURN(MRES_IGNORED);
}

void CServerListPlayersFix::UpdatePlayers()
{
    GF_VPROF("GameFixes::server_list_players::UpdatePlayers");

    if (!m_bSteamAPIReady)
        return;

    ISteamGameServer* pGameServer = SteamGameServer();
    CGameEntitySystem* pEntitySystem = GameEntitySystem();
    CGlobalVars* pGlobals = g_pEngineServer->GetServerGlobals();
    if (!pGameServer || !pEntitySystem || !pGlobals)
        return;

    for (int i = 0; i < pGlobals->maxClients; i++)
    {
        const CSteamID* pSteamId = g_pEngineServer->GetClientSteamID(CPlayerSlot(i));
        if (!pSteamId)
            continue;

        auto* pController = static_cast<CBasePlayerController*>(pEntitySystem->GetEntityInstance(CEntityIndex(i + 1)));
        if (!pController)
            continue;

        pGameServer->BUpdateUserData(*pSteamId, pController->GetPlayerName(), g_pSource2GameClients->GetPlayerScore(CPlayerSlot(i)));
    }
}
