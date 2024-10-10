#pragma once
#include "observer.h"

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

struct s_director_camera_data
{
	real_point3d position;
	int16 field_C;
	int32 field_10;
	int32 field_14;
	real_vector3d random_vector;
	real32 field_of_view;
	int32 flags_maybe;
	int32 dead_camera_local_player_index_1;
	int32 dead_camera_local_player_index_2;
	datum target_datum;
	int32 flags_38;
	uint8 incrementor;
	int8 dead_camera_local_user_index;
	int8 gap_3E[10];
};

struct s_camera_director
{
	bool initialized;
	int8 gap_0[3];
	real32 change_pause;
	int32 field_8;
	void* update_function;
	s_director_camera_data camera_data;
	uint16 user_index;
	int16 pad_5A;
	s_observer_command command;
	bool field_108;
	int8 pad_108[3];
	real32 field_10C;
	int8 gap_110[48];
};

s_camera_director* get_director(int32 user_index);

int32 __cdecl director_get_perspective(int32 user_index);

void __cdecl director_set_unknown_flag(uint32 user_index);