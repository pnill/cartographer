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
	bool unk_1;
	bool unk_2;
	bool unk_3;
	bool unk_4;
	real32 delta;
	real_euler_angles2d facing;
	int8 gap14[16];
	real32 turn_scale;
	int8 gap24[4];
};

int32 __cdecl director_get_perspective(int32 user_index);
