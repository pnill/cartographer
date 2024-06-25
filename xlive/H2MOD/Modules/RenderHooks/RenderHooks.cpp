#include "stdafx.h"

#include "RenderHooks.h"
#include "H2MOD/Modules/Shell/Config.h"

// TODO: remove all this code

bool ras_layer_overrides[RenderHooks::end];
bool geo_render_overrides[24];
namespace RenderHooks
{
	DWORD layer_calls[]
	{
		0x28053A,
		0x280557,
		0x280577,
		0x280597,
		0x2805B7,
		0x2805EF,
		0x280627,
		0x280659,
		0x280678,
		0x280699,
		0x2806BA,
		0x2806F2,
		0x28072A,
		0x28074C,
		0x28077A,
		0x280799,
		0x2807B7
	};
	DWORD render_calls[]
	{
		0x190B0F,
		0x1914D4,
		0x191668,
		0x191680,
		0x191698,
		0x1916CC,
		0x1916E4,
		0x191702,
		0x19171A,
		0x19175A,
		0x19179A,
		0x191888,
		0x191944,
		0x19195C,
		0x1919DA,
		0x191A79,
		0x191B1A,
		0x2265E2
	};

	typedef bool(__cdecl initialize_rasterizer_layer_t)(e_layer_type type, unsigned int width, unsigned int height, bool fmt, int a5);
	initialize_rasterizer_layer_t* p_initialize_rasterizer_layer;

	typedef void(__cdecl render_geometry_t)(e_render_geometry_type type);
	render_geometry_t* p_render_geometry;

	int getWidth(e_layer_type e, unsigned int width)
	{
		switch (e)
		{
		
		case RenderHooks::dynamic_shadows:
			switch (H2Config_Override_Shadows)
			{
			case tex_low:
				return 512;
			case tex_high:
				return 2048;
			case tex_ultra:
				return 4096;
			}
			break;

		case RenderHooks::lightmap_shadows_1:
		case RenderHooks::lightmap_shadows_2:
			switch (H2Config_Override_Shadows)
			{
			case tex_low:
				return 512;
			case tex_high:
				return 2048;
			case tex_ultra:
				return 4096;
			}
			break;

		case RenderHooks::water:
				switch (H2Config_Override_Water)
				{
				case tex_low:
					return 512;
				case tex_high:
					return 2048;
				case tex_ultra:
					return 4096;
				}
			break;

		default:
			break;
		}

		return width;
	}
	int getHeight(e_layer_type e, unsigned int height)
	{
		if (e == e_layer_type::lightmap_shadows_1 || e == e_layer_type::lightmap_shadows_2)
			switch (H2Config_Override_Shadows)
			{
				case tex_low:
					return 32;
				case tex_high:
					return 2048;
				case tex_ultra:
					return 4096;
			}
		if (e == e_layer_type::dynamic_shadows) {
			switch (H2Config_Override_Shadows)
			{
				case tex_low:
					return 32;
				case tex_high:
					return 2048;
				case tex_ultra:
					return 4096;
			}
		}
		if (e == e_layer_type::water)
		{
			switch (H2Config_Override_Water)
			{
				case tex_low:
					return 256;
				case tex_high:
					return 1024;
				case tex_ultra:
					return 2048;
			}
		}
		return height;
	}
	bool __cdecl initialize_rasterizer_layer_hook(e_layer_type type, unsigned int width, unsigned int height, bool fmt, int a5)
	{
		int texture_width = getWidth(type, width);
		int texture_height = getHeight(type, height);

		if (ras_layer_overrides[(int)type - 1])
		{
			texture_width = 128;
			texture_height = 128;
		}
		//LIMITED_LOG(15, LOG_TRACE_GAME, L"{} - : {} {} {} {} {}", __FUNCTIONW__, type, texture_width, texture_height, fmt, a5);
		return p_initialize_rasterizer_layer(type, texture_width, texture_height, fmt, a5);
	}

	void __cdecl render_geometry_hook(e_render_geometry_type type)
	{
		if (!geo_render_overrides[(int)type - 1]) {
			return p_render_geometry(type);
		}
	}

	void ApplyHooks()
	{
		//p_initialize_rasterizer_layer = (p_initialize_rasterizer_layer)DetourFunc(Memory::GetAddress<BYTE*>(0x28024C), (BYTE*)initialize_rasterizer_layer_hook, 7);
		//p_render_geometry = (render_geometry_t)DetourFunc(Memory::GetAddress<BYTE*>(0x1A155C), (BYTE*)render_geometry_hook, 13);
		p_initialize_rasterizer_layer = Memory::GetAddress<initialize_rasterizer_layer_t*>(0x28024C);
		p_render_geometry = Memory::GetAddress<render_geometry_t*>(0x1A155C);
		for(auto &call : layer_calls)
		{
			PatchCall(Memory::GetAddress(call), initialize_rasterizer_layer_hook);
		}
		for(auto &call : render_calls)
		{
			PatchCall(Memory::GetAddress(call), render_geometry_hook);
		}
	}

	void Initialize()
	{
		ApplyHooks();
	}
}
