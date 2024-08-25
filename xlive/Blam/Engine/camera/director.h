#pragma once

enum e_director_mode : int32
{
	_director_mode_game = 0,
	_director_mode_observer = 1,
	_director_mode_editor = 2,
	_director_mode_unused = 3,
	_director_mode_unk = 4,

	k_director_game_modes_count 
};

struct s_director_update
{
	int32 user_index;
	int32 unk_4;
	real32 delta;
	int8 gap_C[32];
};

int32 __cdecl director_get_perspective(int32 user_index);
