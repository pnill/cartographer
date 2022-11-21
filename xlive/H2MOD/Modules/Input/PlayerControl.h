#pragma once
#include "Blam/Engine/Game/game/player_control.h"
#include "Blam/Engine/Game/simulation/simulation_players.h"
#include "Blam/Engine/Networking/PlayerMotion.h"

namespace PlayerControl
{
	void Init();
	void ApplyHooks();
	s_player_actions GetPlayerActions(int player_index);
	s_player_control* GetControls(int local_player_index);
	s_player_motion* GetPlayerMotion(int player_index);
	void DisableLocalCamera(bool);
}
