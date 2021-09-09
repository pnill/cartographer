#pragma once
#include "Blam/Math/BlamMath.h"
#include "Blam/Engine/Players/PlayerControls.h"

using namespace Blam::EngineDefinitions::Players;
namespace PlayerControl
{
	void Init();
	void ApplyHooks();
	s_player_actions GetPlayerActions(int player_index);
	s_player_control* GetControls(int local_player_index);
	s_player_motion* GetPlayerMotion(int player_index);
}
