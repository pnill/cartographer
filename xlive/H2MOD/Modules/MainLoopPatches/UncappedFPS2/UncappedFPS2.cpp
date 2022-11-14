#include "stdafx.h"

#include "UncappedFPS2.h"
#include "Blam/Engine/Game/game/game_time.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/EventHandler/EventHandler.hpp"
#include "Util/Hooks/Hook.h"

BYTE toggleZoom[] = { 0x66, 0x89, 0x45, 0x3E };

static float gameEffectsUnknownValue = 30.f;

void UncappedFPS2::OnGameLifeCycleChange(e_game_life_cycle state)
{
	switch (state)
	{
	case _life_cycle_pre_game:
		gameEffectsUnknownValue = 30.f;
		WriteBytes(Memory::GetAddress(0x9355C), toggleZoom, 4);
		break;
	case _life_cycle_in_game:
		time_globals* timeGlobals = time_globals::get();
		gameEffectsUnknownValue = timeGlobals->tickrate;
		WriteValue<BYTE>(Memory::GetAddress(0x7C389), (BYTE)timeGlobals->tickrate);
		WritePointer(Memory::GetAddress(0x104950), &gameEffectsUnknownValue);
		NopFill(Memory::GetAddress(0x9355C), 4);
		break;
	}
}

void UncappedFPS2::Init()
{
	EventHandler::register_callback(UncappedFPS2::OnGameLifeCycleChange, EventType::gamelifecycle_change, EventExecutionType::execute_after);
}
