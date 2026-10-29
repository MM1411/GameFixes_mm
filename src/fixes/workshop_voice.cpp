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

#include "workshop_voice.h"

#include "utils.hpp"
#include "vprof.hpp"

#include <dynlibutils/module.hpp>
#include <netmessages.h>
#include <networksystem/inetworkserializer.h>

#include <cstdio>
#include <cstring>
#include <random>

using namespace DynLibUtils;

SH_DECL_HOOK2(CServerSideClientBase, SendNetMessage, SH_NOATTRIB, 0, bool, const CNetMessage*, NetChannelBufType_t);

bool CWorkshopVoiceFix::Load(const FixModules& modules, char* error, size_t maxlen)
{
    // An engine class with no interface to fetch and no instance at load time; the vtable is resolved by name and the hook covers every client sharing it.
    CMemory pVTable = modules.engine.GetVirtualTableByName("CServerSideClient");
    if (!pVTable)
    {
        std::snprintf(error, maxlen, "CServerSideClient vtable not found");
        return false;
    }

    m_VTable.m_pVTFs = pVTable.RCast<void**>();
    if (!KeepHook(SH_ADD_DVPHOOK(CServerSideClientBase, SendNetMessage, AsHookTarget<CServerSideClientBase>(m_VTable), SH_MEMBER(this, &CWorkshopVoiceFix::CServerSideClient_SendNetMessage), false)))
    {
        std::snprintf(error, maxlen, "failed to hook CServerSideClient::SendNetMessage");
        return false;
    }

    Log("hooked CServerSideClient::SendNetMessage on vtable %p", pVTable.GetPtr());
    return true;
}

void CWorkshopVoiceFix::Unload()
{
    RemoveHooks();
    m_VTable.m_pVTFs = nullptr;
}

void CWorkshopVoiceFix::OnStartupServer(const GameSessionConfiguration_t& config, const char* pszMapName)
{
    std::memset(m_PlayerSeeds, 0, sizeof(m_PlayerSeeds));
    m_bLoggedFirstRewrite = false;
}

bool CWorkshopVoiceFix::CServerSideClient_SendNetMessage(const CNetMessage* pData, NetChannelBufType_t bufType)
{
    GF_VPROF("GameFixes::workshop_voice::SendNetMessage");

    CServerSideClientBase* pThis = VHOOK_IFACEPTR(CServerSideClientBase);

    if (!pData)
        VHOOK_RETURN_VALUE(MRES_IGNORED, true);

    if (pData->GetMessageId() != SVC_Messages::svc_VoiceData)
        VHOOK_RETURN_VALUE(MRES_IGNORED, true);

    // The reference implementation's "playerid": the client being sent to, not the one talking.
    const int nRecipient = pThis->m_nClientSlot.Get();

    // Rewritten in place in Pre, so the engine serializes the modified message into this client's channel.
    CSVCMsg_VoiceData* pMsg = const_cast<CSVCMsg_VoiceData_t*>(pData->As<CSVCMsg_VoiceData_t>());

    // And its "msg.Entity": the speaker.
    const int nSpeaker = pMsg->entity();

    pMsg->set_xuid(SeedForRecipient(nRecipient) + static_cast<uint64>(nSpeaker));

    if (!m_bLoggedFirstRewrite)
    {
        m_bLoggedFirstRewrite = true;
        Log("rewriting svc_VoiceData xuid, first packet was entity %d -> slot %d", nSpeaker, nRecipient);
    }

    VHOOK_RETURN_VALUE(MRES_IGNORED, true);
}

uint64 CWorkshopVoiceFix::SeedForRecipient(int nSlot)
{
    if (nSlot < 0 || nSlot >= ABSOLUTE_PLAYER_LIMIT)
        return 0;

    if (m_PlayerSeeds[nSlot] == 0)
    {
        if (m_nSeedCursor == 0)
        {
            std::random_device rd;
            std::mt19937_64 gen(rd());

            m_nSeedCursor = gen() | 1;
        }

        m_nSeedCursor += 66;
        m_PlayerSeeds[nSlot] = m_nSeedCursor;
    }

    return m_PlayerSeeds[nSlot];
}
