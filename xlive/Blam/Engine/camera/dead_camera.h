#pragma once
#include "observer.h"

/* structures */

struct s_dead_camera
{
	real_point3d position;
	real_point3d focus_position;
	real_euler_angles2d facing;
	real32 focus_distance;
	real32 field_of_view;
	real32 transition_timer;
	datum player_index;
	datum view_player_index;
	datum target_object_index;
	real32 switch_timer;
	uint8 death_transition_ticks;
	int8 user_index;
};
ASSERT_STRUCT_SIZE(s_dead_camera, 0x40);

/* prototypes */

void dead_camera_apply_patches();

void __cdecl dead_camera_new(s_dead_camera* camera, int32 user_index, datum target_object_index);

void __cdecl dead_camera_update(s_dead_camera* camera, struct s_director_update* director_update, s_observer_command* observer_command);