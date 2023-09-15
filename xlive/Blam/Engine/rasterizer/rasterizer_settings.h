#pragma once

#define k_max_display_option_count 64

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
	DWORD width;
	DWORD height;
	e_display_aspect_ratio aspect_ratio;
};

struct s_video_settings
{
	DWORD display_mode;
	DWORD aspect_ratio;
	DWORD screen_width;
	DWORD screen_height;
	DWORD refresh_rate;
	DWORD display_option_index;
	DWORD brightness;
	DWORD gamma;
	DWORD anti_aliasing;
	DWORD hud_size;
	DWORD safe_area;
	DWORD level_of_detail;
};
static_assert(sizeof(s_video_settings) == 48, "s_video_settings total bytes wrong");

void rasterizer_settings_apply_hooks();
