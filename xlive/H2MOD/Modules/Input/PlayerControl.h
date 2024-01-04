#pragma once
#include "Blam/Engine/game/player_control.h"
#include "Blam/Engine/Networking/PlayerMotion.h"

namespace PlayerControl
{
	void Init();
	void ApplyHooks();
	player_action GetPlayerActions(int player_index);
	void DisableLocalCamera(bool);
}
