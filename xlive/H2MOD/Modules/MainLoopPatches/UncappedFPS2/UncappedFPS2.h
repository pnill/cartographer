#pragma once
#include "Blam/Engine/Networking/logic/life_cycle_manager.h"

namespace UncappedFPS2
{
	void OnMapLoad();
	void OnGameLifeCycleChange(e_game_life_cycle state);
	void Init();
}
