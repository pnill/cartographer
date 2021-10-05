#pragma once

#include "Blam\Common\Common.h"
#pragma pack(push, 1)
struct s_game_engine_globals
{
	BYTE gap0[10];
	__int16 Unk1;
	BYTE gap1[3144];
	DWORD Unk2;
	BYTE gap2[132];
	static s_game_engine_globals* get();
};
#pragma pack(pop)
CHECK_STRUCT_SIZE(s_game_engine_globals, 0xCDC);