#pragma once

#define k_max_display_option_count 64

#define k_max_default_display_refresh_rate_count 8
#define k_new_max_display_refresh_rate_count 24

enum e_display_aspect_ratio : int
{
	_aspect_ratio_4x3 = 0,
	_aspect_ratio_16x9 = 1,
	_aspect_ratio_16x10 = 2
};

struct s_aspect_ratio
{
	byte x;
	byte y;
};

struct s_display_option
{
	int32 width;
	int32 height;
	e_display_aspect_ratio aspect_ratio;
};

struct s_video_mode
{
	int32 width;
	int32 height;
	uint32 refresh_rate_count;
	int32 refresh_rate[8];
};
ASSERT_STRUCT_SIZE(s_video_mode, 12 + 8 * 4);

struct s_video_settings
{
	int32 display_mode;
	int32 aspect_ratio;
	int32 screen_width;
	int32 screen_height;
	int32 refresh_rate;
	int32 display_option_index;
	int32 brightness;
	int32 gamma;
	int32 anti_aliasing;
	int32 hud_size;
	int32 safe_area;
	int32 level_of_detail;
};
static_assert(sizeof(s_video_settings) == 48, "s_video_settings total bytes wrong");

bool* get_render_fog_enabled(void);

void rasterizer_settings_apply_hooks();
