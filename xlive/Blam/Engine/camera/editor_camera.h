#pragma once
#include "observer.h"

#include "math/real_math.h"

/* enums */

enum e_editor_camera_mode : int16
{
	_editor_camera_mode_flying = 0,
	_editor_camera_mode_orbiting = 1
};

/* structures */

struct s_editor_camera
{
	union
	{
		real_point3d position;
		struct
		{
			int8 gap_0[4];
			real32 focus_distance;
			datum slave_object;
		};
	};
	real_euler_angles2d facing;
	real32 roll;
	int8 gap_18[4];
};

struct s_editor_camera_globals
{
	s_observer_result saved_observer_result;
	bool scripted;
	uint8 teleport_frame_count;
	bool roll_enabled;
	bool state_valid;
	real_point3d saved_position;
	real_euler_angles2d saved_facing;
	s_editor_camera* user_camera;
	real_point3d relative_focus_point;
	int32 field_7C;
	e_editor_camera_mode mode;
	int16 pad;
};
ASSERT_STRUCT_SIZE(s_editor_camera_globals, 132);

/* prototypes */

void editor_camera_apply_patches();

s_editor_camera_globals* editor_camera_globals_get(void);

void editor_camera_set_position(const real_point3d* point, const real_euler_angles2d* angles);

void editor_camera_flying_reset(void);
