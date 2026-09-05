#pragma once
#include "dead_camera.h"
#include "editor_camera.h"
#include "following_camera.h"
#include "observer.h"

/* enums */

enum e_director_mode : int32
{
	_director_mode_game = 0,
	_director_mode_observer = 1,
	_director_mode_editor = 2,
	_director_mode_unused = 3,
	_director_mode_first_person = 4,

	k_director_game_modes_count 
};

/* structures */

struct s_director_update
{
	int32 user_index;
	bool has_input;
	bool unk_2;
	bool unk_3;
	bool unk_4;
	real32 delta;
	real_euler_angles2d facing;
	real32 roll;
	real32 movement_forward;
	real32 movement_side;
	real32 movement_up;
	real32 turn_scale;
	bool buttons[4];
};

struct s_director_variable
{
	real32 value;
	real32 field_4;
	real32 field_8;
};
ASSERT_STRUCT_SIZE(s_director_variable, 12);

struct s_camera_director
{
	bool initialized;
	int8 pad_1[3];
	real32 transition_time;
	uint32 perspective_toggle_msec;
	void* update_function;
	union
	{
		s_dead_camera dead_camera;
		s_editor_camera editor_camera;
		s_following_camera following_camera;
		int8 camera_storage[0x40];
	};
	int32 field_50;
	int8 field_54;
	bool inhibit_facing;
	bool inhibit_input;
	int8 pad_57;
	int16 desired_perspective;
	int16 pad_5A;
	s_observer_command command;
	bool gamepad_camera_control;
	real32 look_scale;
	s_director_variable variables[4];
};
ASSERT_STRUCT_SIZE(s_camera_director, 320);
ASSERT_STRUCT_OFFSET(s_camera_director, update_function, 12);
ASSERT_STRUCT_OFFSET(s_camera_director, dead_camera, 16);
ASSERT_STRUCT_OFFSET(s_camera_director, inhibit_facing, 85);
ASSERT_STRUCT_OFFSET(s_camera_director, inhibit_input, 86);
ASSERT_STRUCT_OFFSET(s_camera_director, desired_perspective, 88);
ASSERT_STRUCT_OFFSET(s_camera_director, command, 92);
ASSERT_STRUCT_OFFSET(s_camera_director, gamepad_camera_control, 264);
ASSERT_STRUCT_OFFSET(s_camera_director, look_scale, 268);
ASSERT_STRUCT_OFFSET(s_camera_director, variables, 272);

/* prototypes */

s_camera_director* director_get(int32 user_index);

e_director_mode director_get_mode(void);

void director_set_mode(e_director_mode mode);

void __cdecl director_debug_camera(bool enabled);

void __cdecl director_update(real32 dt);

void __cdecl director_game_tick(void);

int32 __cdecl director_get_perspective(int32 user_index);

void director_inhibit_facing(int32 user_index);

void director_inhibit_input(int32 user_index);

bool director_inhibited_facing(int32 user_index);

bool director_inhibited_input(int32 user_index);
