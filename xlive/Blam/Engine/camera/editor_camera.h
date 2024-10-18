#pragma once

/* enums */

enum e_editor_camera_mode : uint8
{
	_editor_camera_mode_flying = 0,
	_editor_camera_mode_orbiting = 1
};


struct s_editor_camera
{
	int8 gap_1[4];
	real32 focus_distance;
	datum slave_object;
	real_euler_angles2d facing;
	int8 gap_18[8];
};

void editor_camera_apply_patches();