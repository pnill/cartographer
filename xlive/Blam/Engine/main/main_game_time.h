#pragma once

extern bool g_main_game_time_frame_limiter_enabled;

void main_game_time_apply_patches();

#pragma pack(push, 1)
struct s_main_time_globals
{
	// NOTE this is not the entire structure
	// this is somewhere in the middle of the base structure
	bool field_0;
	char pad_1[1];
	int game_time_passed;
	__int64 last_time_ms;
	bool should_reset;
	char pad_2[7];
	__int64 field_16[2];

	static s_main_time_globals* get()
	{
		return Memory::GetAddress<s_main_time_globals*>(0x479E92);
	}
};
#pragma pack(pop)
static_assert(sizeof(s_main_time_globals) == 38);