#pragma once
#include "Blam\Engine\Players\PlayerActions.h"
#include "Blam\Engine\Networking\PlayerMotion.h"
#include "Blam\Engine\Players\PlayerControls.h"

namespace PlayerControl
{
	void Init();
	void ApplyHooks();
	s_player_actions GetPlayerActions(int player_index);
	s_player_control* GetControls(int local_player_index);
	s_player_motion* GetPlayerMotion(int player_index);
	void DisableLocalCamera(bool);
}
