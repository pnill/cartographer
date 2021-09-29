#include "RenderHooks.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/Config/Config.h"
#include "H2MOD/GUI/GUI.h"

bool ras_layer_overrides[25];
bool geo_render_overrides[24];
namespace RenderHooks
{
	int layer_calls[]
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
	int render_calls[]
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

	byte* reset_screen;

	typedef bool(__cdecl p_initialize_rasterizer_layer)(e_layer_type type, unsigned int width, unsigned int height, bool fmt, int a5);
	p_initialize_rasterizer_layer* c_initialize_rasterizer_layer;

	typedef void(__cdecl p_render_geometry)(e_render_geometry_type type);
	p_render_geometry* c_render_geometry;

	int oWidth = 0;
	int oHeight = 0;

	int getWidth(e_layer_type e, unsigned int width)
	{
		int window_width = GUI::GetDisplayMode().Width;

		if (e == e_layer_type::lightmap_shadows_1 || e == e_layer_type::lightmap_shadows_2)
			switch(H2Config_Override_Shadows)
			{
				case tex_low: 
					return 512;
				case tex_high: 
					return 2048;
				case tex_ultra: 
					return 4096;
			}
		if (e == e_layer_type::dynamic_shadows) {
			switch (H2Config_Override_Shadows)
			{
				case tex_low:
					return 512;
				case tex_high:
					return 2048;
				case tex_ultra:
					return 4096;
			}
		}
		if(e == e_layer_type::water)
		{
			switch(H2Config_Override_Water)
			{
				case tex_low:
					return 512;
				case tex_high:
					return 2048;
				case tex_ultra:
					return 4096;
			}
		}
		if (e == e_layer_type::bloom)
		{
			switch (H2Config_Override_ScreenFX)
			{
			case tex_low:
				return window_width / 8;
			case tex_high:
				return window_width;
			case tex_ultra:
				return window_width * 2;
			}
		}
		if (e == e_layer_type::unk_15)
		{
			switch (H2Config_Override_ScreenFX)
			{
			case tex_low:
				return window_width / 4;
			case tex_high:
				return window_width;
			case tex_ultra:
				return window_width * 2;
			}
		}
		if (e == e_layer_type::tex_camera)
		{
			switch (H2Config_Override_ScreenFX)
			{
			case tex_low:
				return window_width / 4;
			case tex_high:
				return window_width;
			case tex_ultra:
				return window_width * 2;
			}
		}
		return width;
	}
	int getHeight(e_layer_type e, unsigned int height)
	{
		int window_height = GUI::GetDisplayMode().Height;

		if (e == e_layer_type::lightmap_shadows_1 || e == e_layer_type::lightmap_shadows_2)
			switch (H2Config_Override_Shadows)
			{
				case tex_low:
					return 512;
				case tex_high:
					return 2048;
				case tex_ultra:
					return 4096;
			}
		if (e == e_layer_type::dynamic_shadows) {
			switch (H2Config_Override_Shadows)
			{
				case tex_low:
					return 512;
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
		if (e == e_layer_type::bloom)
		{
			switch (H2Config_Override_ScreenFX)
			{
			case tex_low:
				return window_height / 8;
			case tex_high:
				return window_height;
			case tex_ultra:
				return window_height * 2;
			}
		}
		if (e == e_layer_type::unk_15)
		{
			switch (H2Config_Override_ScreenFX)
			{
			case tex_low:
				return window_height / 4;
			case tex_high:
				return window_height;
			case tex_ultra:
				return window_height * 2;
			}
		}
		if (e == e_layer_type::tex_camera)
		{
			switch (H2Config_Override_ScreenFX)
			{
			case tex_low:
				return window_height / 4;
			case tex_high:
				return window_height;
			case tex_ultra:
				return window_height * 2;
			}
		}
		return height;
	}
	bool __cdecl h_initialize_rasterizer_layer(e_layer_type type, unsigned int width, unsigned int height, bool fmt, int a5)
	{
		oWidth = getWidth(type, width);
		oHeight = getHeight(type, height);
		if (ras_layer_overrides[(int)type - 1])
		{
			oWidth = 128;
			oHeight = 128;
		}
		LOG_TRACE_GAME(L"[Render Hooks] init_rasterizer_layer: {} {} {} {} {}", type, oWidth, oHeight, fmt, a5);
		return c_initialize_rasterizer_layer(type, oWidth, oHeight, fmt, a5);
	}

	void __cdecl h_render_geometry(e_render_geometry_type type)
	{
		if (!geo_render_overrides[(int)type - 1]) {
			return c_render_geometry(type);
		}
	}

	void ResetDevice()
	{
		*reset_screen = 1;
	}

	void ApplyHooks()
	{
		//c_initialize_rasterizer_layer = (p_initialize_rasterizer_layer)DetourFunc(Memory::GetAddress<BYTE*>(0x28024C), (BYTE*)h_initialize_rasterizer_layer, 7);
		//c_render_geometry = (p_render_geometry)DetourFunc(Memory::GetAddress<BYTE*>(0x1A155C), (BYTE*)h_render_geometry, 13);
		c_initialize_rasterizer_layer = Memory::GetAddress<p_initialize_rasterizer_layer*>(0x28024C);
		c_render_geometry = Memory::GetAddress<p_render_geometry*>(0x1A155C);
		for(auto &call : layer_calls)
		{
			PatchCall(Memory::GetAddress(call), h_initialize_rasterizer_layer);
		}
		for(auto &call : render_calls)
		{
			PatchCall(Memory::GetAddress(call), h_render_geometry);
		}
	}

	void Initialize()
	{
		reset_screen = Memory::GetAddress<byte*>(0xA3E4D4);
		ApplyHooks();
	}
}
