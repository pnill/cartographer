#pragma once
#include "rasterizer_globals.h"

/* constants */

#define k_max_display_option_count 64

#define k_max_default_display_refresh_rate_count 8
#define k_new_max_display_refresh_rate_count 24

/* enums */

enum e_display_aspect_ratio : int32
{
	_aspect_ratio_4x3 = 0,
	_aspect_ratio_16x9 = 1,
	_aspect_ratio_16x10 = 2
};

enum 
{
	_rasterizer_settings_display_mode_last_user_selectable = _rasterizer_window_mode_borderless,
};

/* structures */

struct s_aspect_ratio
{
	uint8 x;
	uint8 y;
};

struct s_display_option
{
	uint32 width;
	uint32 height;
	e_display_aspect_ratio aspect_ratio;
};

struct s_video_mode
{
	uint32 width;
	uint32 height;
	uint32 refresh_rate_count;
	int32 refresh_rate[k_max_default_display_refresh_rate_count];
};
ASSERT_STRUCT_SIZE(s_video_mode, 44);

struct s_rasterizer_settings
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
ASSERT_STRUCT_SIZE(s_rasterizer_settings, 48);

/* prototypes */

void rasterizer_settings_apply_hooks(void);

bool* get_render_fog_enabled(void);

s_rasterizer_settings* rasterizer_settings_get(void);

int32* rasterizer_low_level_texture_detail_get(void);

void __cdecl rasterizer_settings_set_antialiasing(uint32* out_quality);

void __cdecl rasterizer_settings_update_window_position(void);

void rasterizer_settings_set_default_settings(void);

void __cdecl rasterizer_settings_create_registry_keys(bool is_game);

bool __cdecl rasterizer_settings_has_available_video_modes();
void __cdecl rasterizer_settings_set_display_mode(const e_rasterizer_window_mode* display_mode);
void __cdecl rasterizer_settings_store_current_settings_for_revert();
void __cdecl rasterizer_settings_set_display_option(int32* display_option_index);
void __cdecl rasterizer_settings_process_display_changes(bool force_update_params, bool display_blackness);


int32 rasterizer_settings_get_refresh_rate(void);

void __cdecl rasterizer_settings_apply_settings(int32 setting);

void __cdecl rasterizer_settings_write_to_registry();

DWORD __cdecl rasterizer_settings_get_window_flags(e_rasterizer_window_mode window_mode, DWORD* style);

string_id __cdecl rasterizer_settings_get_display_mode_string(e_rasterizer_window_mode display_mode);
void __cdecl rasterizer_settings_get_display_option_resolution_string(int32 display_option_index, wchar_t* out_text, int32 out_text_max_length);
string_id __cdecl rasterizer_settings_get_brightness_level_string(int32 brightness_level);
string_id __cdecl rasterizer_settings_get_gamma_setting_string(int32 gamma);
string_id __cdecl rasterizer_settings_get_anti_aliasing_string(int32 anti_aliasing);
string_id __cdecl rasterizer_settings_get_lod_setting_string(int32 level_of_detail);
string_id __cdecl rasterizer_settings_get_safe_area_string(int32 safe_area);