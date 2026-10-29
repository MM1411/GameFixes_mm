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

#include "demo_record.h"

#include <iloopmode.h>
#include "utils.hpp"
#include "vprof.hpp"

#include <dynlibutils/module.hpp>

#include <cstdio>

using namespace DynLibUtils;

SH_DECL_HOOK2_void(CServerSideClientBase, Disconnect, SH_NOATTRIB, 0, ENetworkDisconnectionReason, const char*);

bool CDemoRecordFix::Load(const FixModules& modules, char* error, size_t maxlen)
{
    m_tvEnable = std::make_unique<CConVarRef<bool>>("tv_enable");

    CMemory pVTable = modules.engine.GetVirtualTableByName("CServerSideClient");
    if (!pVTable)
    {
        std::snprintf(error, maxlen, "CServerSideClient vtable not found");
        return false;
    }

    m_VTable.m_pVTFs = pVTable.RCast<void**>();
    if (!KeepHook(SH_ADD_DVPHOOK(CServerSideClientBase, Disconnect, AsHookTarget<CServerSideClientBase>(m_VTable), SH_MEMBER(this, &CDemoRecordFix::CServerSideClient_Disconnect), false)))
    {
        std::snprintf(error, maxlen, "failed to hook CServerSideClient::Disconnect");
        return false;
    }

    Log("hooked CServerSideClient::Disconnect on vtable %p, tv_enable is %s", pVTable.GetPtr(), IsTvEnabled() ? "on" : "off");
    return true;
}

void CDemoRecordFix::Unload()
{
    RemoveHooks();
    m_VTable.m_pVTFs = nullptr;
    m_tvEnable.reset();
}

bool CDemoRecordFix::IsTvEnabled() const
{
    return m_tvEnable && m_tvEnable->IsValidRef() && m_tvEnable->Get();
}

void CDemoRecordFix::OnStartupServer(const GameSessionConfiguration_t& config, const char* pszMapName)
{
    if (!IsTvEnabled())
        return;

    // The engine reads max_clients after this Pre hook, so the GOTV slot comes on top of the configured player count instead of eating one.
    Log("GOTV is enabled, expanding the session's maxplayers (%u -> %u)", config.max_clients(), config.max_clients() + 1);
    const_cast<GameSessionConfiguration_t&>(config).set_max_clients(config.max_clients() + 1);
}

void CDemoRecordFix::CServerSideClient_Disconnect(ENetworkDisconnectionReason reason, const char* pszInternalReason)
{
    GF_VPROF("GameFixes::demo_record::Disconnect");

    CServerSideClientBase* pThis = VHOOK_IFACEPTR(CServerSideClientBase);

    if (pThis->m_bIsHLTV && IsTvEnabled())
    {
        Log("GOTV is enabled, blocking the GOTV client's disconnect");
        VHOOK_RETURN(MRES_SUPERCEDE);
    }

    VHOOK_RETURN(MRES_IGNORED);
}
