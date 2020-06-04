#pragma once
void SplitFixVideoInitialize();
void CreateCustomDepth();
void SplitFixDeviceReset();

struct s_video_settings
{
	DWORD DisplayMode;
	DWORD AspectRatio;
	DWORD ScreenResX;
	DWORD ScreenResY;
	DWORD RefreshRate;
	DWORD ResArrayIndex;
	DWORD Brightness;
	DWORD Gamma;
	DWORD AntiAliasing;
	DWORD HUDSize;
	DWORD SafeArea;
	DWORD LevelOfDetail;
};

enum split_type {
	horizontal_split = 1,
	vertical_split = 2
};


enum render_geometry_type
{
	texture_accumulate = 0x1,
	overlay = 0x2,
	lightmap_indirect = 0x3,
	lightmap_specular = 0x4,
	spherical_harmonics_PRT = 0x5,
	water_alpha_masks = 0x6,
	selfillumination = 0x7,
	env_map = 0x8,
	stencil_shadows = 0x9,
	shadow_buffer_generate = 0xA,
	shadow_buffer_apply = 0xB,
	lightaccum_diffuse = 0xC,
	lightaccum_specular = 0xD,
	lightaccum_albedo = 0xE,
	transparent = 0xF,
	fog = 0x10,
	selfibloomination = 0x11,
	active_camo = 0x12,
	active_camo_stencil_modulate = 0x13,
	decal = 0x14,
	hologram = 0x15,
	error_report = 0x16,
	debug_view = 0x17,
	hud = 0x18,
};
