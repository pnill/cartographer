#pragma once
#include "game/player_control.h"
#include "Networking/player_motion.h"

namespace PlayerControl
{
	player_action GetPlayerActions(int player_index);
}
