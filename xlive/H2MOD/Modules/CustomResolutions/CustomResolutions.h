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

struct s_display_resolution
{
	DWORD width;
	DWORD height;
	e_display_aspect_ratio aspect_ratio;
};

struct s_video_settings
{
	DWORD DisplayMode;
	DWORD AspectRatio;
	DWORD ScreenResX;
	DWORD ScreenResY;
	DWORD RefreshRate;
	DWORD ResArrayIndex; // this is the index of the current used hardcoded video resolution 
	DWORD Brightness;
	DWORD Gamma;
	DWORD AntiAliasing;
	DWORD HUDSize;
	DWORD SafeArea;
	DWORD LevelOfDetail;
};
static_assert(sizeof(s_video_settings) == 48, "s_video_settings total bytes wrong");

namespace CustomResolution
{
	void Initialize();
}