#pragma once
#include "tag_files/tag_reference.h"
#include "objects/object_definition.h"

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


struct _device_definition
{
    object_definition object;

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
ASSERT_STRUCT_SIZE(_device_definition, 284);
