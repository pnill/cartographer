#pragma once
#include "bitmaps/bitmap_group.h"
#include "cseries/cseries_strings.h"

struct s_screenshot_globals
{
	bool taking_screenshot;
	bool unused;
	bool take_screenshot;
	bool web_map;
	bool save_as_tga;
	bool dump_camera_data;
	bool capture_cubemap;
	bool camera_name_set;
	int32 resolution_multiplier;
	int32 fieldC;
	static_string256 camera_name;
	bitmap_data* bloom_bitmap;
};
CHECK_STRUCT_SIZE(s_screenshot_globals, 276);

s_screenshot_globals* get_screenshot_globals();
