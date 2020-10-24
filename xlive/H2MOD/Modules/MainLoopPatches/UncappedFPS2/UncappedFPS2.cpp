#include "UncappedFPS2.h"
#include "H2MOD/Modules/MainLoopPatches/UncappedFPS/UncappedFPS.h"
#include "Util/Hooks/Hook.h"
#include "Blam/Cache/TagGroups/globals_definition.hpp"
#include "H2MOD/Modules/EventHandler/EventHandler.h"
#include "H2MOD/Modules/Config/Config.h"

BYTE lobby[] = { 0x53, 0x30, 0xDB };
BYTE ingame[] = { 0xB0, 0x01, 0xC3 };
BYTE toggleZoom[] = { 0x66, 0x89, 0x45, 0x3E };
void UncappedFPS2::Init()
{
	/*EventHandler::registerGameStateCallback({
			"UncappedPregame",
			life_cycle_pre_game,
			[]
			{
				WriteBytes(h2mod->GetAddress(0x3A938), lobby, 3);
				WriteValue<unsigned long>(h2mod->GetAddress(0x104950), h2mod->GetAddress(0x3a03c4));
				WriteBytes(h2mod->GetAddress(0x9355C), toggleZoom, 4);

			},
			false
		}, false);
	EventHandler::registerGameStateCallback({
			"UncappedInGame",
			life_cycle_in_game,
			[]
			{
				if (H2Config_experimental_fps) {
					time_globals* timeGlobals = time_globals::get_game_time_globals();
					WriteValue<BYTE>(h2mod->GetAddress(0x7C389), (byte)timeGlobals->ticks_per_second);
					WriteValue<unsigned long>(h2mod->GetAddress(0x104950), h2mod->GetAddress(0x39E2F8));
					WriteValue<float>(h2mod->GetAddress(0x39E2F8), (float)timeGlobals->ticks_per_second);
					WriteBytes(h2mod->GetAddress(0x3A938), ingame, 3);
					NopFill(h2mod->GetAddress(0x9355C), 4);
				}
			},
			false
		}, false);*/
}
