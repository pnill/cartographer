#pragma once

#include "Blam/Common/Common.h"
#pragma pack(push, 1)
struct s_game_engine_globals
{
	byte gap0[10];
	short fieldA;
	byte gapC[216];
	float unk_local_player_hud_field[4];
	byte gapF4[2912];
	DWORD game_engine_index;
	byte gapC58[132];
	static s_game_engine_globals* get();
};
#pragma pack(pop)
CHECK_STRUCT_SIZE(s_game_engine_globals, 0xCDC);