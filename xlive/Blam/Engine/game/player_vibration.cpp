#include "stdafx.h"
#include "player_vibration.h"

void rumble_player_set_scripted_scale(real32 scale)
{
	INVOKE(0x9004F, 0x8EC60, rumble_player_set_scripted_scale, scale);
	return;
}
