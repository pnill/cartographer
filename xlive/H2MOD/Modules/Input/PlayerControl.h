#pragma once
#include "Blam/Maths/real_math.h"
#include "Blam/Engine/Players/PlayerControls.h"

namespace PlayerControl
{
	void Init();
	void ApplyHooks();
	Blam::EngineDefinitions::Players::s_gamestate_player_controls* GetControls(int local_player_index);
}
