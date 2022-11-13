#pragma once
#include "Blam/Common/Common.h"

namespace game
{
#pragma pack(push, 1)
	struct s_game_engine_globals
	{
		BYTE gap0[10];
		__int16 Unk1;
		BYTE gap1[216];
		float unk_local_player_hud_field[4];
		BYTE gap2[2912];
		DWORD game_engine_index;
		BYTE gap3[132];
		static s_game_engine_globals* get();
	};
	CHECK_STRUCT_SIZE(s_game_engine_globals, 0xCDC);
#pragma pack(pop)

	int get_game_mode_engine();
}
