#pragma once
#include "game/game_globals.h"

/* constants */

#define k_max_texture_stages 4

/* enums */

enum e_rasterizer_window_mode : uint32
{
	_rasterizer_window_mode_real_fullscreen = 0,
	_rasterizer_window_mode_windowed = 1,
	_rasterizer_window_mode_funky_fullscreen = 2
};

enum e_display_type : uint8
{
	_display_type_widescreen = 0,
	_display_type_4_by_3 = 1
};

/* structures */

struct s_rasterizer_globals_display_parameters
{
	uint64 frame_presented_count;
	int64 pad_2;
	int64 field_8;
	int64 pad_3;
	int16 refresh_rate;
	int16 pad;
	D3DFORMAT backbuffer_format;
	D3DFORMAT depthstencil_format;
	int16 font_width;
	int16 font_height;
	e_display_type display_type;
	int8 unused_window_mode;
	int8 pad_1[2];
	e_rasterizer_window_mode window_mode;
};
ASSERT_STRUCT_SIZE(s_rasterizer_globals_display_parameters, 56);

struct s_rasterizer_parameters
{
	int16 field_0;
	int32 field_4;
	int32 field_8;
};
ASSERT_STRUCT_SIZE(s_rasterizer_parameters, 12);

struct s_rasterizer_globals_clipping_parameters
{
	s_rasterizer_parameters parameters;
	real32 z_near;
	real32 z_far;
	real32 depth_near;
	real32 depth_far;
};
ASSERT_STRUCT_SIZE(s_rasterizer_globals_clipping_parameters, 28);

struct s_rasterizer_globals_bitmaps
{
	datum bitmap_2d_unused;
	datum bitmap_3d_unused;
	datum bitmap_cubemap_unused;
	bool field_90;
	bool field_91;
	int8 pad_2[2];
	uint32 textures_staged_width[k_max_texture_stages];
	uint32 textures_staged_height[k_max_texture_stages];
};
ASSERT_STRUCT_SIZE(s_rasterizer_globals_bitmaps, 48);

struct s_rasterizer_fullscreen_parameters
{
	real32 gamma;
	real32 brightness;
	real32 field_8;	// unused
	real32 field_C;	// unused
};
ASSERT_STRUCT_SIZE(s_rasterizer_fullscreen_parameters, 16);

struct s_rasterizer_globals
{
	bool rasterizer_initialized;
	bool use_d3d9_ex;
	int8 pad[2];
	decltype(Direct3DCreate9Ex)* d3d9_create_ex_proc;
	uint32 resolution_x;
	uint32 resolution_y;
	rectangle2d screen_bounds;
	rectangle2d frame_bounds;
	int16 field_20;
	int16 field_22;
	real32 ui_scale;
	int16 unk;
	int8 pad_1[6];
	s_rasterizer_globals_display_parameters display_parameters;
	s_rasterizer_globals_clipping_parameters clipping_parameters;
	s_rasterizer_globals_bitmaps bitmaps;
	real_vector3d forward;	// unused
	s_rasterizer_fullscreen_parameters fullscreen_parameters;
	real32 last_frame_time;
	bool reset_screen;
	bool rasterizer_draw_on_main_back_buffer;
	int16 pad_3;
	real32 sun_width_scale;
	int32 rasterizer_blur_type;
	bool field_E0;
	bool d3d9_sm3_supported;
	int16 pad_4;
	bitmap_data* bitmap_data_array[1024];
	uint32 next_bitmap_index;
	uint32 bitmap_data_count;
	bool field_10EC;
	bool field_10ED;
	int8 pad_5[2];
};
ASSERT_STRUCT_SIZE(s_rasterizer_globals, 4336);

/* prototypes */

s_rasterizer_globals* rasterizer_globals_get(void);

s_game_globals_rasterizer_data* rasterizer_globals_get_data(void);

uint32 rasterizer_get_width(void);

uint32 rasterizer_get_height(void);

void rasterizer_get_screen_bounds(rectangle2d* screen_bounds);

void rasterizer_get_frame_bounds(rectangle2d* frame_bounds);
