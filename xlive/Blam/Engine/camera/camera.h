#pragma once

struct s_camera
{
	real_point3d point;
	real_vector3d forward;
	real_vector3d up;
	bool bool_0;
	int8 pad[3];
	real32 vertical_field_of_view;
	real32 scale;
	rectangle2d viewport_bounds;
	rectangle2d window_bounds;
	real32 z_near;
	real32 z_far;
	real_point3d point1;
	int8 gap_4C[4];
	bool tiled;
	int8 pad1[3];
	real32 unk_floats[3];
	bool bool_2;
	int8 pad2[3];
	real32 frustum_multiplier_x;
	real32 frustum_multiplier_y;
};
ASSERT_STRUCT_SIZE(s_camera, 0x74);

// Get global camera data
s_camera* get_global_camera(void);

s_camera* get_effect_camera();

void camera_apply_patches(void);