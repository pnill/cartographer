#pragma once
#include "math/color_math.h"

/* constants */

#define k_screen_effect_script_value_count 4
#define k_cinematic_layer_count 32

/* structures */

struct s_cinematic_dof_parameters
{
	real32 seperation_distance;
	real_bounds near_blur;
	real32 near_blur_start_time;
	real32 near_blur_end_time;
	real_bounds far_blur;
	real32 far_blur_start_time;
	real32 far_blur_end_time;
};

struct s_cinematic_crossfade_parameters
{
	real32 exponent;
	real32 transition_start_time;
	real32 transition_end_time;
};

struct s_cinematic_environment_map_attenuation_parameters
{
	bool initialized;
	int8 pad[3];
	real_bounds attenuation;
	real32 attenuation_start_time;
	real32 attenuation_end_time;
};


struct s_cinematic_environment_map_tint_parameters
{
	bool initialized;
	int8 pad[3];
	real32 perpendicular_brightness;
	real_rgb_color perpendicular_color;
	real32 parallel_brightness;
	real_rgb_color parallel_color;
};

struct s_cinematic_layer_position
{
	real32 current_position;
	real32 new_position;
};

struct s_cinematic_layer_time
{
	real32 start_time;
	real32 end_time;
};

struct s_rasterizer_cinematics_globals
{
	int16 field_0;
	int16 field_2;
	real32 field_4;
	int8 field_8[16];
	real_rgb_color color_18;
	int8 field_24[24];
	bool field_3C;
	int8 field_3D[3];
	real32 field_40;
	real_bounds field_44;
	int8 field_4C[84];
	bool field_A0;
	int8 field_A1[3];
	real_bounds field_AC;
	bool cinematic_screen_effect_initialized;
	bool cinematic_screen_effect_enabled;
	int16 pad_AE;
	real32 field_B0;
	real32 field_B4;
	real32 field_B8;
	real32 field_BC;
	int8 field_C0[16];
	real32 field_D0;
	real32 field_D4;
	s_cinematic_dof_parameters dof;
	int8 field_D8[120];
	s_cinematic_crossfade_parameters crossfade;
	real32 screen_effect_script_values[k_screen_effect_script_value_count];
	real32 near_clip_distance;
	real32 far_clip_distance;
	s_cinematic_environment_map_attenuation_parameters attenuation;
	bool env_map_is_forced;
	datum environment_map_bitmap_index;
	s_cinematic_environment_map_tint_parameters environment_map_tint;
	int8 pad3[32];
	s_cinematic_layer_position cinematic_layers_position[k_cinematic_layer_count];
	s_cinematic_layer_time cinematic_layer_time[k_cinematic_layer_count];
	real32 gamma_ramp;
};
ASSERT_STRUCT_SIZE(s_rasterizer_cinematics_globals, 1020)

/* public code */

s_rasterizer_cinematics_globals* __cdecl rasterizer_cinematic_screen_effect_update(s_rasterizer_cinematics_globals* globals);
