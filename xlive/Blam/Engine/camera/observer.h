#pragma once
#include "math/matrix_math.h"
#include "objects/objects.h"

#define k_observer_command_values 6

enum e_observer_command_flags : uint8
{
	_observer_command_bit_0 = 0,
	_observer_command_bit_1 = 1,
	_observer_command_bit_2 = 2,
	_observer_command_bit_3 = 3
};

struct s_observer_result
{
	real_point3d position;
	s_location location;
	real_vector3d field_14;
	real_vector3d forward;
	real_vector3d up;
	real32 horizontal_field_of_view;
	real32 field_3C;
	real_vector3d field_40;
	real32 vertical_field_of_view;
	real32 scale;
};
ASSERT_STRUCT_SIZE(s_observer_result, 84);

union camera_displacement
{
	struct
	{
		real_vector3d main;
		real_vector3d focus;
		real_vector3d vector_3;
		real32 field_4;
		real_vector3d rotational;
	};
	real32 n[13];
};
ASSERT_STRUCT_SIZE(camera_displacement, 52);

struct s_observer_command_displacement
{
	real_point3d position;
	real_vector3d orientation;
};
ASSERT_STRUCT_SIZE(s_observer_command_displacement, 24);

struct s_observer_command
{
	uint8 flags;
	int8 pad[3];
	s_observer_command_displacement position;
	real32 field_1C;
	real32 field_20;
	real32 focus_distance;
	real32 field_of_view;
	real_vector3d forward;
	real_vector3d up;
	real_vector3d velocity;
	real_matrix4x3 focus_space;
	int32 field_84;
	real32 timer;
	e_observer_command_flags observer_command_flags[k_observer_command_values];
	int16 field_92;
	real32 observer_command_values[k_observer_command_values];
};
ASSERT_STRUCT_SIZE(s_observer_command, 172);

struct s_observer
{
	int32 header_signature;
	s_observer_command* command;
	s_observer_command pending_command;
	bool updated_for_frame;
	bool field_B5;
	bool field_B6;
	bool field_B7;
	s_observer_result result;
	real_point3d position;
	real_vector3d focus_offset;
	int8 field_124[8];
	real32 focus_distance;
	real32 horizontal_field_of_view;
	real_vector3d forward;
	real_vector3d up;
	real_matrix4x3 focus_space;
	camera_displacement velocities;
	camera_displacement accelerations;
	int8 field_1E8[312];
	camera_displacement displacements;
	int32 trailer_signature;
};
ASSERT_STRUCT_SIZE(s_observer, 856);

s_observer* observer_get_from_user(int32 user_index);

void observer_apply_patches(void);
