# GameFixes_mm

**One Metamod:Source plugin for the CS2 server fixes that used to be five separate ones (BeamCrashFix, DemoRecordFix, HammerIdFix, SlowAnimationFix, WorkshopVoiceFix), the GC-ban whitelist that used to live in FUNPLAY Core, and a few more (team limit, input activator crash, sv_cheats noclip, server list players).** Every fix is toggled in `game_fixes.ini`.

---

## Fixes

| Block | What it fixes |
|---|---|
| `abstract_entity` | The entity system allocates any registered classname but only constructs a class its schema allows; for any other (`weapon_cs_base` in old map ports and their `game_player_equip`) it calls into the zeroed object and crashes on a null vtable. `CEntitySystem::CreateEntityFromKeyValues` refuses such classnames like an unknown one. |
| `beam_crash` | `CBeam::SetBeamOrigin` / `SetBeamEndPos` loop forever on a beam without a parent, crashing the server. The unparented path is reimplemented. |
| `demo_record` | With `tv_enable 1`, the GOTV client is never disconnected and gets a slot on top of the session's maxplayers, so demo recording survives match restarts on the same map. |
| `hammer_id` | `SetSchemaHammerUniqueId` skips `m_sUniqueHammerID` behind a `jnz`; patched to `jmp` so every entity gets its id. |
| `slow_animation` | `curtime` is a 32-bit float and loses precision after a day or two on one map (sluggish animations, movement, lag compensation). While the server is empty at a check, the map is reloaded; the remaining `mp_timelimit` is carried over. `reload_interval` sets the check period in seconds. |
| `workshop_voice` | Clients key voice playback off the `xuid` in `svc_VoiceData`; on workshop maps it doesn't identify the speaker uniquely, streams collide and players stop hearing each other. Every speaker gets a distinct xuid per listener, rewritten in place in `CServerSideClient::SendNetMessage` (once per recipient). Seeds reset on every map change. |
| `steam_ban` | The engine's per-frame GC ban / competitive cooldown kick pass (`GameSystem_Think_CheckSteamBan`, hooked by signature). Accounts in `whitelist` (SteamID64s) are stripped out before it runs, so are competitive cooldowns while `sv_kick_players_with_cooldown` is below 2, and with `clear_after_pass` whatever it leaves behind is cleared so a stale entry can't hit the next player to join. Other plugins allow accounts at runtime through `IGameFixes` (below); an account nobody allowed yet is held for `hold_seconds` before the pass may kick it. |
| `team_limit` | `CCSGameRules`' max T/CT counts and enabled spawn point counts are kept at maxplayers every frame, so a join is never refused because a team is "full", also on maps with few spawns. |
| `input_activator_crash` | `CBaseFilter::InputTestActivator` dereferences the input's activator unchecked; a `TestActivator` fired with none crashes the server. Those calls are dropped. |
| `sv_cheats` | Turning `sv_cheats` off leaves anyone in noclip flying. Every such pawn goes back to `MOVETYPE_WALK` the moment the cvar flips to 0. |
| `rampbug` | Surf ramps: a movement trace that stops short of a ramp or lands on its edge feeds `TryPlayerMove` a plane the player never hit, and they are flung off. The traces are redone from a small offset along the last valid plane and, with `tryplayermove_override` (off by default, the game has its own mitigation now), a bad result is put back (CS2KZ's fix, via CS2Fixes-RampbugFix). What a player was on is forgotten on a teleport. Lessens rampbugs, does not end them. |
| `water_jump` | Touching the bottom of a body of water, the `CategorizePosition` after `WaterMove` counts the player as standing on it and a jump out of the water no longer takes. That one call is skipped. |
| `voice_flood` | Malformed (`clc_VoiceData` with no audio, a sender other than the client, an unknown format, packets with no data) or oversized voice messages -- over 16 KiB or 64 packets -- are dropped in `CServerSideClient::ProcessVoiceData` before the engine handles them, and every client is held to 192 messages, 256 KiB and 1024 packets per second. Nobody is kicked. |
| `console_spam` | A client that never finishes signing on keeps its net channel and can still send `say`; with its player entity gone, `Host_Say` gets a null player and posts the message as `Console`, past every chat filter. `say` / `say_team` (compared case-insensitively, the engine also dispatches `SAY`) from a client not fully in game are dropped in `ICvar::DispatchConCommand`, as CS2Fixes `e5a3557` does. The server console is untouched. |
| `server_list_players` | Pushes every connected player's SteamID, name and score to the Steam game server API every `update_interval` seconds, so the server browser lists the players. Port of [Source2ZE/ServerListPlayersFix](https://github.com/Source2ZE/ServerListPlayersFix). |

A fix that fails to find what it needs (a signature, a vtable) is logged and left off; the rest of the plugin still loads.

## Configuration

`addons/configs/game_fixes/game_fixes.ini` -- one block per fix, `"enable" "1"` turns it on, a missing block counts as off. Extra options live in the same block:

```
"slow_animation"
{
	"enable"			"1"
	"reload_interval"	"1800"
}

"steam_ban"
{
	"enable"	"1"
	"clear_after_pass"	"1"
	"hold_seconds"	"15"
	"whitelist"
	{
		"Slynx"	"76561198000000000"
	}
}
```

## Interface for other plugins

`public/IGameFixes.h` (`IGameFixes001`), through metamod:

```cpp
IGameFixes* pGameFixes = static_cast<IGameFixes*>(g_SMAPI->MetaFactory(GAMEFIXES_INTERFACE_VERSION, nullptr, nullptr));
pGameFixes->SteamBanAllowAccount(steamId64);    // steam_ban leaves the account alone
pGameFixes->SteamBanDisallowAccount(steamId64);
```

A call to a fix that is off does nothing and returns false.

## Layout

```
public/
  IGameFixes.h      what other plugins can call
src/
  plugin.*          metamod glue: interfaces, ini, the shared GameFrame/StartupServer hooks
  scheduler.*       timers + next-frame queue, ticked from GameFrame
  utils.hpp         WIN_LINUX and WriteCode, the one code-byte patch (on DynLibUtils)
  vprof.hpp         GF_VPROF, the profiler scope every hook handler and timer callback opens
  fixes/fix.h       the CFix interface every fix implements
  fixes/<name>.*    one fix per file pair
  sdk/              schema field access and the few SDK classes the fixes touch
```

Hooks go through the plugin's own private SourceHook (`vendor/sourcehook`, SlynxCZ/SourceHook): `SH_DECL_HOOK` / `SH_ADD_HOOK` / `SH_ADD_DVPHOOK` for virtual functions, `SH_DECL_INLINEHOOK` / `SH_ADD_INLINEHOOK` (safetyhook underneath) for the functions found by signature. It is opted into with `sourcehook/sourcehook_metamod_override.h` and `SH_METAMOD_OVERRIDE_SAVEVARS()`, so nothing depends on the SourceHook or KHook the server's metamod ships. Signature scanning and vtable lookup use `vendor/dynlibutils`.

Every handler shows up in the engine's profiler as `GameFixes::<fix>::<function>`, under the `GameFixes` budget group: `vprof_on`, let it run, `vprof_generate_report`, `vprof_off` (the commands are development-only, so something has to unlock them first).

## Building

Built with CMake against:

* **Metamod:Source build 1411** (`external/metamod-source`, alliedmodders/metamod-source `7ec0f16`, the last build before KHook replaced SourceHook; plugin API 17). Only its `core/` headers are used.
* **Wend4r/sourcesdk** (`external/sourcesdk`, `main`), built as part of the project through `add_subdirectory`: tier1, mathlib, entity2, kv3lib, protobuf and the game protobufs come from it, tier0 / steam_api from its `lib/`.

```
git clone --recursive https://github.com/MM1411/GameFixes_mm
cmake -S GameFixes_mm -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Other checkouts of the dependencies can be used with `-DSOURCESDK_DIR=...` / `-DMETAMOD_DIR=...` (or the `SOURCESDK_DIR` / `HL2SDKCS2` and `METAMOD_DIR` / `MMSOURCE_DEV` environment variables). Windows needs MSVC (a Developer Command Prompt), Linux clang; CI builds Linux in `docker/` (Steam Runtime sniper, clang-18).

`build/package/` is the complete install tree, in the layout Pisex's plugins use; copy its `addons/` into `game/csgo/`:

```
addons/plugins/game_fixes.so | game_fixes.dll
addons/configs/game_fixes/game_fixes.ini
```

There is no metamod `.vdf`: Pisex's `utils` loads every binary under `addons/plugins/` (recursively).

## Requirements

* Metamod:Source build 1411 (CS2), the one before KHook
* Pisex's `utils` (cs2-menus), whose autoloader loads `addons/plugins/`

## Author

Slynx (˙·٠● S l y n x ●٠·˙)
[https://slynxdev.cz](https://slynxdev.cz)
