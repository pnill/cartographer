#include "stdafx.h"
#include "player_vibration.h"

void __cdecl rumble_player_set_scripted_scale(real32 scale)
{
	INVOKE(0x9004F, 0x8EC60, rumble_player_set_scripted_scale, scale);
	return;
}

void __cdecl rumble_player_continuous(int32 user_index, real32 rumble_intensity_left, real32 rumble_intensity_right)
{
	INVOKE(0x90222, 0x8EE33, rumble_player_continuous, user_index, rumble_intensity_left, rumble_intensity_right);
	return;
}