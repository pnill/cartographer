#pragma once
#include "objects/object_definition.h"

/* constants */

enum
{
    DEVICE_DEFINITION_TAG = 'devi',
    DEVICE_DEFINITION_VERSION = 1,  // TODO: verify
};

/* macros */

#define device_definition_get(index)	((struct device_definition*)tag_get(DEVICE_DEFINITION_TAG, (index)))

/* enums */

enum e_device_definition_flags : int32
{
	_device_definition_position_loops = FLAG(0),
	_device_definition_unused = FLAG(1),
	_device_definition_allow_interpolation = FLAG(2)
};

enum e_device_lightmap_flags : int16
{
	_lightmap_dont_use_in_lightmap = FLAG(0),
	_lightmap_dont_use_in_lightprobe = FLAG(1),
};

/* structures */

struct _device_definition
{
	// Explaination("$$$ DEVICE $$$", "")

	e_device_definition_flags flags;
	real32 power_transition_time_seconds;
	real32 power_acceleration_time_seconds;
	real32 position_transition_time_seconds;
	real32 position_acceleration_time_seconds;
	real32 depowered_position_transition_time_seconds;
	real32 depowered_position_acceleration_time_seconds;

	e_device_lightmap_flags lightmap_flags;
	int16 pad;

	// Sounds
	tag_reference open;     // up
	tag_reference close;    // down
	tag_reference opened;
	tag_reference closed;
	tag_reference depowered;
	tag_reference repowered;

	real32 delay_time_seconds;
	tag_reference delay_effect;
	real32 automatic_activation_radius; // World units
};

struct device_definition
{
	_object_definition object;
	_device_definition device;
};
ASSERT_STRUCT_SIZE(device_definition, 284);

