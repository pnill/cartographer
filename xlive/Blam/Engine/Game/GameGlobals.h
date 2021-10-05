#pragma once
#include "GameOptions.h"
#include "GameTimeGlobals.h"

struct s_game_globals
{
	BYTE Unk1[8];
	s_game_options engine_settings;
	BYTE unk2[215];
	time_globals time_globals;
	static s_game_globals* get();
};
