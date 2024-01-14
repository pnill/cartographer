#pragma once
#include "Blam/Engine/game/player_control.h"
#include "Blam/Engine/Networking/player_motion.h"

namespace PlayerControl
{
	player_action GetPlayerActions(int player_index);
}
