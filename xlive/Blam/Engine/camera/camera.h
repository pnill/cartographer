#pragma once
#include "Blam/Engine/math/real_math.h"
#include "Blam/Engine/math/integer_math.h"

struct s_camera
{
	real_point3d point;
	real_vector3d forward;
	real_vector3d up;
	bool bool_0;
	__int8 pad[3];
	float vertical_field_of_view;
	float scale;
	rectangle2d viewport_bounds;
	rectangle2d window_bounds;
	float z_near;
	float z_far;
	real_point3d point1;
	__int8 gap_4C[4];
	bool bool_1;
	__int8 pad1[3];
	float unk_floats[3];
	bool bool_2;
	__int8 pad2[3];
	float frustum_multiplier_x;
	float frustum_multiplier_y;
};
CHECK_STRUCT_SIZE(s_camera, 0x74);

// Get global camera data
s_camera* get_global_camera();

s_camera* get_effect_camera();
