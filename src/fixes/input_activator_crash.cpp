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

#include "input_activator_crash.h"

#include "utils.hpp"
#include "vprof.hpp"

#include <dynlibutils/module.hpp>

#include <cstdio>
#include <cstring>

using namespace DynLibUtils;

SH_DECL_INLINEHOOK6(CEntityIdentity_AcceptInput, CEntityIdentity, bool, CUtlSymbolLarge*, CEntityInstance*, CEntityInstance*, variant_t*, void*, void*);

bool CInputActivatorCrashFix::Load(const FixModules& modules, char* error, size_t maxlen)
{
    // void CEntityIdentity::AcceptInput(CUtlSymbolLarge* pInputName, CEntityInstance* pActivator, CEntityInstance* pCaller, variant_t* pValue, void* a6, void* a7)
    CMemory pAcceptInput = modules.server.FindPattern(ParseStringPattern(WIN_LINUX("48 89 54 24 ? 48 89 4C 24 ? 55 53 56 57 41 55 41 56 41 57 48 8D 6C 24", "55 48 89 E5 41 57 41 56 4C 8D BD ? ? ? ? 4D 89 CE")));
    if (!pAcceptInput)
    {
        std::snprintf(error, maxlen, "CEntityIdentity::AcceptInput not found");
        return false;
    }

    if (!KeepHook(SH_ADD_INLINEHOOK(CEntityIdentity_AcceptInput, pAcceptInput.GetPtr(), SH_MEMBER(this, &CInputActivatorCrashFix::CEntityIdentity_AcceptInput), false)))
    {
        std::snprintf(error, maxlen, "failed to hook CEntityIdentity::AcceptInput");
        return false;
    }

    Log("hooked CEntityIdentity::AcceptInput (%p)", pAcceptInput.GetPtr());
    return true;
}

void CInputActivatorCrashFix::Unload()
{
    RemoveHooks();
}

bool CInputActivatorCrashFix::CEntityIdentity_AcceptInput(CUtlSymbolLarge* pInputName, CEntityInstance* pActivator, CEntityInstance* pCaller, variant_t* pValue, void* a6, void* a7)
{
    GF_VPROF("GameFixes::input_activator_crash::AcceptInput");

    CEntityIdentity* pThis = SH_IFACEPTR(CEntityIdentity);

    // If null activator (player disconnected & pawn removed), block the real TestActivator function from executing and crashing the server
    if (!V_strnicmp(pThis->GetClassname(), "filter_", 7) && !V_strcasecmp(pInputName->String(), "TestActivator") && !pActivator)
        RETURN_SH_VALUE(MRES_SUPERCEDE, true);

    RETURN_SH_VALUE(MRES_IGNORED, true);
}
