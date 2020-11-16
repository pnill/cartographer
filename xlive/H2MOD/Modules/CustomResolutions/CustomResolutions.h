#pragma once

#define TOTAL_RESOLUTIONS 16
#define DEFAULT_TOTAL_RESOLUTIONS 11

struct s_display_res
{
	int res_x;
	int res_y;
	int unk; // not sure what this is, maybe it defines what type of res it is, i.e widescreen, 4:3 etc.
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