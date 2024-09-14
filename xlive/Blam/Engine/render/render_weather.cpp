#include "stdafx.h"
#include "render_weather.h"

#include "rasterizer/dx9/shaders/compiled/weather_particle_generic.h"
#include "rasterizer/dx9/shaders/compiled/weather_particle_rain.h"
#include "rasterizer/dx9/shaders/compiled/weather_plate.h"
#include "rasterizer/dx9/rasterizer_dx9_main.h"
#include "rasterizer/dx9/rasterizer_dx9_weather.h"
#include "rasterizer/rasterizer_globals.h"

/* constants */

const DWORD* k_weather_shaders_bytecode_ps_2_0[k_dx9_weather_shader_type_count]
{
	(DWORD*)k_weather_particle_generic_ps_2_0,
	(DWORD*)k_weather_particle_rain_ps_2_0,
	(DWORD*)k_weather_plate_ps_2_0
};

const DWORD* k_weather_shaders_bytecode_ps_3_0[k_dx9_weather_shader_type_count]
{
	(DWORD*)k_weather_particle_generic_ps_3_0,
	(DWORD*)k_weather_particle_rain_ps_3_0,
	(DWORD*)k_weather_plate_ps_3_0
};

/* globals */

IDirect3DPixelShader9* g_d3d9_weather_shaders[k_dx9_weather_shader_type_count]{};

/* public code */

void render_weather_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x263568), c_particle_system_lite::initialize);
	PatchCall(Memory::GetAddress(0x263821), c_particle_system_lite::dispose);

	PatchCall(Memory::GetAddress(0x199750), c_particle_system_lite::draw);
	return;
}

real32* weather_dt_global_get(void)
{
	return Memory::GetAddress<real32*>(0x4EC3DC);
}


void __cdecl c_particle_system_lite::initialize(void)
{
	IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();
	if (global_d3d_device)
	{
		const DWORD** bytecode = rasterizer_globals_get()->d3d9_sm3_supported ? k_weather_shaders_bytecode_ps_3_0 : k_weather_shaders_bytecode_ps_2_0;
		for (uint8 i = 0; i < NUMBEROF(g_d3d9_weather_shaders); ++i)
		{
			HRESULT hr = global_d3d_device->CreatePixelShader(bytecode[i], &g_d3d9_weather_shaders[i]);
			if (!SUCCEEDED(hr))
			{
				error(2, "render:weather: c_particle_system_lite::initialize: CreatePixelShader failed with hr == 0x%08x", hr);
				g_d3d9_weather_shaders[i] = NULL;
			}
		}
	}
	return;
}

void __cdecl c_particle_system_lite::dispose(void)
{
	IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();

	for (uint8 i = 0; i < NUMBEROF(g_d3d9_weather_shaders); ++i)
	{
		g_d3d9_weather_shaders[i]->Release();
	}
	return;
}

bool __fastcall c_particle_system_lite::draw(c_particle_system_lite* _this)
{
	return rasterizer_dx9_draw_weather_particles(_this);
}

void __cdecl weather_render(void)
{
	INVOKE(0x199BA9, 0x0, weather_render);
	return;
}
