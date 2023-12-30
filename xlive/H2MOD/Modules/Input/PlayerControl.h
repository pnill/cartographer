#pragma once
#include "Blam/Engine/game/player_control.h"
#include "Blam/Engine/Networking/PlayerMotion.h"
#include "Blam/Engine/Simulation/SimulationWorld.h"

namespace PlayerControl
{
	void Init();
	void ApplyHooks();
	player_action GetPlayerActions(int player_index);
	s_player_motion* GetPlayerMotion(int player_index);
	void DisableLocalCamera(bool);
}
