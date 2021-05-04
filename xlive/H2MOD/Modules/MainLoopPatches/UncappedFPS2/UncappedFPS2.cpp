#include "UncappedFPS2.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/EventHandler/EventHandler.hpp"
#include "H2MOD/Modules/Config/Config.h"

BYTE toggleZoom[] = { 0x66, 0x89, 0x45, 0x3E };

static float gameEffectsUnknownValue = 30.f;

void UncappedFPS2::OnGameStateChange(game_life_cycle state)
{
	switch(state)
	{
		case life_cycle_pre_game:
			gameEffectsUnknownValue = 30.f;
			WriteBytes(Memory::GetAddress(0x9355C), toggleZoom, 4);
			break;
		case life_cycle_in_game:
			time_globals* timeGlobals = time_globals::get();
			gameEffectsUnknownValue = timeGlobals->ticks_per_second;
			WriteValue<BYTE>(Memory::GetAddress(0x7C389), (BYTE)timeGlobals->ticks_per_second);
			WritePointer(Memory::GetAddress(0x104950), &gameEffectsUnknownValue);
			NopFill(Memory::GetAddress(0x9355C), 4);
			break;
	}
}

void UncappedFPS2::Init()
{
	//EventHandler::register_callback(UncappedFPS2::OnGameStateChange, EventType::gamestate_change, after, true);
	/*EventHandler::registerGameStateCallback({
			"UncappedPregame",
			life_cycle_pre_game,
			[]
			{
				gameEffectsUnknownValue = 30.f;
				WriteBytes(Memory::GetAddress(0x9355C), toggleZoom, 4);
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
				WriteValue<BYTE>(Memory::GetAddress(0x7C389), (BYTE)timeGlobals->ticks_per_second);
				WritePointer(Memory::GetAddress(0x104950), &gameEffectsUnknownValue);
				NopFill(Memory::GetAddress(0x9355C), 4);				
			},
			false
		}, false);*/
}
