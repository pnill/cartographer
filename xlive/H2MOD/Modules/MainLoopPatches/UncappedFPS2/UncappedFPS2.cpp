#include "UncappedFPS2.h"
#include "Util/Hooks/Hook.h"
#include "Blam/Cache/TagGroups/globals_definition.hpp"
#include "H2MOD/Modules/EventHandler/EventHandler.h"
#include "H2MOD/Modules/Config/Config.h"
#include "Blam/Engine/Game/GameTimeGlobals.h"

BYTE toggleZoom[] = { 0x66, 0x89, 0x45, 0x3E };

static float gameEffectsUnknownValue = 30.f;

void UncappedFPS2::Init()
{
	EventHandler::registerGameStateCallback({
			"UncappedPregame",
			life_cycle_pre_game,
			[]
			{
				gameEffectsUnknownValue = 30.f;
				WriteBytes(h2mod->GetAddress(0x9355C), toggleZoom, 4);
			},
			false
		}, false);
	EventHandler::registerGameStateCallback({
			"UncappedInGame",
			life_cycle_in_game,
			[]
			{
				time_globals* timeGlobals = time_globals::get();
				gameEffectsUnknownValue = timeGlobals->ticks_per_second;
				WriteValue<BYTE>(h2mod->GetAddress(0x7C389), (BYTE)timeGlobals->ticks_per_second);
				WritePointer(h2mod->GetAddress(0x104950), &gameEffectsUnknownValue);
				NopFill(h2mod->GetAddress(0x9355C), 4);				
			},
			false
		}, false);
}
