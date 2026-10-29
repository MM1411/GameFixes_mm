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

// A client that never finishes signing on still has its net channel, and can
// still send "say". Its player entity is gone by then, so the say callback
// hands Host_Say a null player, which falls back to the name "Console" and
// userid 0: chat spam as "Console", past every chat filter that looks the
// controller up. say / say_team from a client that is not fully in game are
// dropped in ICvar::DispatchConCommand before the command runs (CS2Fixes
// e5a3557). The name is compared case-insensitively: the engine dispatches
// "SAY" to the same command.
#pragma once

#include "fix.h"
#include "plugin.h"

#include <tier0/icvar.h>

#include <cstdint>

class CConsoleSpamFix final : public CFix
{
public:
    const char* GetName() const override { return "console_spam"; }

    bool Load(const FixModules& modules, char* error, size_t maxlen) override;
    void Unload() override;

    void OnStartupServer(const GameSessionConfiguration_t& config, const char* pszMapName) override;

public: // Hooks
    void ICvar_DispatchConCommand(ConCommandRef cmd, const CCommandContext& ctx, const CCommand& args);

private:
    // The slot's client is connected and fully signed on, with a controller.
    static bool IsInGame(CPlayerSlot slot);

    uint64_t m_nBlocked = 0;
    bool m_bLoggedFirstBlock = false;
};
