#include "stdafx.h"
#include "director.h"

s_camera_director* get_director(int32 user_index)
{
	return &Memory::GetAddress<s_camera_director*>(0x4A84A4)[user_index];
}

int32 __cdecl director_get_perspective(int32 user_index)
{
	return INVOKE(0x59E2B, 0x47E58, director_get_perspective, user_index);
}

void director_set_unknown_flag(uint32 user_index)
{
	// sets flag in director[user_index].camera_data.gap_3E[8]
	// unsure what it does.. doesn't seem to actually do anything
	INVOKE(0x59DF7, 0, director_set_unknown_flag, user_index);
}
