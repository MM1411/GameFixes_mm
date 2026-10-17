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

#ifndef _INCLUDE_GAMEFIXES_IFACE_H_
#define _INCLUDE_GAMEFIXES_IFACE_H_
#ifdef _WIN32
#pragma once
#endif

#include <cstdint>

#define GAMEFIXES_INTERFACE_VERSION "IGameFixes001"

// What other plugins can ask of GameFixes, through metamod:
//
//   IGameFixes* pGameFixes = static_cast<IGameFixes*>(g_SMAPI->MetaFactory(GAMEFIXES_INTERFACE_VERSION, nullptr, nullptr));
//
// A call to a fix that is off (or failed to load) does nothing and returns false.
class IGameFixes
{
public:
	// steam_ban: the engine's GC ban / competitive cooldown kick (game bans,
	// VAC Live, cooldowns) leaves this account alone, like the whitelist in
	// game_fixes.ini, until DisallowAccount or a plugin reload. An account the
	// engine wants kicked is held for "hold_seconds" first, so a plugin that
	// learns who the player is a moment after they connect (an admin list over
	// HTTP) can still allow them in time.
	virtual bool SteamBanAllowAccount(uint64_t steamId64) = 0;
	virtual bool SteamBanDisallowAccount(uint64_t steamId64) = 0;
};

#endif // _INCLUDE_GAMEFIXES_IFACE_H_
