#pragma once

struct s_patchy_fog_window_data
{
	int8 field_0[136];
};

struct s_patchy_fog_layer
{
	int32 index;
	real32 animation_time;
	int8 field_C[12];
	real32 field_14;
	int8 field_18[32];
};

struct s_patchy_fog_globals
{
	int32 player_window_index;
	s_patchy_fog_window_data data[4];
	int32 pad[20];
	s_patchy_fog_layer layers[5];
	int32 field_38C;
	real32 patchy_fog_apply_scale;
	real32 field_394[2];
	bool separate_layer_depths;
	int8 pad_1[3];
	bool render_patchy_fog_parameters[2];
	bool display_patchy_fog_frame_parameters;
	int8 pad_2;
};

/* prototypes */

s_patchy_fog_globals* patchy_fog_globals_get(void);
