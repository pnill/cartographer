#pragma once

struct s_following_camera
{
	bool valid;
	int8 unk_1;
	bool unit_control_flags_test;
	int8 unk_2;
	bool tight_camera_track;
	int8 pad_2[3];
	datum slave_object;
	int16 parent_seat_index;
	int16 padding;
	real_euler_angles2d facing;
	real32 turn_scale_maybe;
	int8 gap_18[4];
};
ASSERT_STRUCT_SIZE(s_following_camera, 32);
