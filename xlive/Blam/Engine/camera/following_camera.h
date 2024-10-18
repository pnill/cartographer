#pragma once

struct s_following_camera
{
	bool valid;
	int8 unk_1;
	bool unit_control_flags_test;
	bool unk;
	bool desires_tight_camera_track;
	datum slave_object;
	int16 parent_seat_index;
	int16 padding;
	real_euler_angles2d facing;
	real32 turn_scale_maybe;
	int8 gap_18[4];
};

ASSERT_STRUCT_SIZE(s_following_camera, 32);


void following_camera_apply_patches();