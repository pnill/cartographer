#pragma once

/* constants */

enum
{
	CAMERA_TRACK_TAG = 'trak',
};

enum
{

	k_maximum_number_of_camera_track_control_points = 16
};

/* macros */

#define camera_track_definition_get(index)	((struct s_camera_track_definition*)tag_get(CAMERA_TRACK_TAG, (index)))

/* structures */

// max: k_maximum_number_of_camera_track_control_points
struct s_camera_track_control_point
{
	real_vector3d position;
	real_quaternion orientation;
};
ASSERT_STRUCT_SIZE(s_camera_track_control_point, 28);

struct s_camera_track_definition
{
	uint32 unused_flags;
	s_tag_block control_points;		// s_camera_track_control_point
};
ASSERT_STRUCT_SIZE(s_camera_track_definition, 12);
