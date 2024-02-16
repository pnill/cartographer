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

int32 __cdecl director_get_perspective(int32 user_index);
