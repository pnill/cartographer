#pragma once

/* structures */

struct real_decibel
{
    real32 db;
};

// max: 1
struct s_sound_playback_parameter
{
	real_bounds scale_bounds;
	real_bounds random_base_and_variance;
};
ASSERT_STRUCT_SIZE(s_sound_playback_parameter, 16);

