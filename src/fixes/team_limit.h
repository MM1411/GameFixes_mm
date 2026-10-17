/**
 * vim: set ts=4 sw=4 tw=99 noet:
 * =============================================================================
 * GameFixes_mm
 * Copyright (C) 2026 Michal "Slynx (˙·٠● S l y n x ●٠·˙)" Přikryl.
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
 *   - Michal "Slynx (˙·٠● S l y n x ●٠·˙)" Přikryl
 *
 * Project: GameFixes_mm
 */

// A join is refused as "team full" once a team has as many players as the
// smaller of CCSGameRules' m_iMaxNum<team> and m_iSpawnPointCount_<team>, the
// number of enabled spawn points for it. The game sets the first at level init
// and recounts the second whenever a spawn point is created, enabled or
// disabled -- maps do that mid-round -- so both are raised to maxplayers every
// frame, when they are below it. Players beyond the map's spawn points share
// them. m_iNumSpawnable<team> are live player counts the game balances teams
// with, and are left alone.
#pragma once

#include "fix.h"
#include "sdk/schema/CCSGameRules.h"

class CTeamLimitFix final : public CFix
{
public:
    const char* GetName() const override { return "team_limit"; }

    bool Load(const FixModules& modules, char* error, size_t maxlen) override;
    void Unload() override;

    void OnStartupServer(const GameSessionConfiguration_t& config, const char* pszMapName) override;
    void OnGameFrame(bool simulating) override;

private:
    CCSGameRules* GetGameRules();

    // cs_gamerules' rules object, looked up once per map.
    CCSGameRules* m_pGameRules = nullptr;
};
