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

#include "console_spam.h"

#include "vprof.hpp"

#include <entity2/entitysystem.h>
#include <interfaces/interfaces.h>
#include <iserver.h>
#include <serversideclientbase.h>
#include <tier0/convar.h>
#include <tier0/tier0_strtools.h>

#include <cstdio>

SH_DECL_HOOK3_void(ICvar, DispatchConCommand, SH_NOATTRIB, 0, ConCommandRef, const CCommandContext&, const CCommand&);

bool CConsoleSpamFix::Load(const FixModules& modules, char* error, size_t maxlen)
{
    if (!KeepHook(SH_ADD_HOOK(ICvar, DispatchConCommand, g_pCVar, SH_MEMBER(this, &CConsoleSpamFix::ICvar_DispatchConCommand), false)))
    {
        std::snprintf(error, maxlen, "failed to hook ICvar::DispatchConCommand");
        return false;
    }

    Log("hooked ICvar::DispatchConCommand, say / say_team need a client fully in game");
    return true;
}

void CConsoleSpamFix::Unload()
{
    RemoveHooks();
}

void CConsoleSpamFix::OnStartupServer(const GameSessionConfiguration_t& config, const char* pszMapName)
{
    if (m_nBlocked)
        Log("%llu say message(s) from clients not in game blocked on the last map", static_cast<unsigned long long>(m_nBlocked));

    m_nBlocked = 0;
    m_bLoggedFirstBlock = false;
}

bool CConsoleSpamFix::IsInGame(CPlayerSlot slot)
{
    CNetworkGameServerBase* pServer = g_pNetworkServerService ? g_pNetworkServerService->GetIGameServer() : nullptr;
    if (!pServer)
        return false;

    CServerSideClientBase* pClient = pServer->GetClientBySlot(slot);
    if (!pClient || !pClient->IsInGame())
        return false;

    // What Host_Say gets handed: without it, the message goes out as "Console".
    CGameEntitySystem* pEntitySystem = GameEntitySystem();
    return pEntitySystem && pEntitySystem->GetEntityInstance(CEntityIndex(slot.Get() + 1)) != nullptr;
}

void CConsoleSpamFix::ICvar_DispatchConCommand(ConCommandRef cmd, const CCommandContext& ctx, const CCommand& args)
{
    GF_VPROF("GameFixes::console_spam::DispatchConCommand");

    // The server's own console (slot -1) may say what it likes.
    const CPlayerSlot slot = ctx.GetPlayerSlot();
    if (slot.Get() < 0 || args.ArgC() < 1)
        VHOOK_RETURN(MRES_IGNORED);

    const char* pszCommand = args.Arg(0);
    if (V_stricmp(pszCommand, "say") != 0 && V_stricmp(pszCommand, "say_team") != 0)
        VHOOK_RETURN(MRES_IGNORED);

    if (IsInGame(slot))
        VHOOK_RETURN(MRES_IGNORED);

    m_nBlocked++;
    if (!m_bLoggedFirstBlock)
    {
        m_bLoggedFirstBlock = true;
        Log("blocked %s from slot %d, not fully in game", pszCommand, slot.Get());
    }

    VHOOK_RETURN(MRES_SUPERCEDE);
}
