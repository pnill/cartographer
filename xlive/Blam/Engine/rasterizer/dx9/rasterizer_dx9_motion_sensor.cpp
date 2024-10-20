#include "stdafx.h"
#include "rasterizer_dx9_motion_sensor.h"
#include "rasterizer/dx9/rasterizer_dx9_main.h"

#include "rasterizer/dx9/shaders/compiled/motion_sensor_sweep.h"

/* globals */
IDirect3DPixelShader9* motion_sensor_sweep_shader;

/* public code */

void __cdecl rasterizer_dx9_create_motion_sensor_shaders(void)
{
	INVOKE(0x28458C, 0x0, rasterizer_dx9_create_motion_sensor_shaders);

	// replace the sweep shader with the clamped alpha one
	const unsigned char* ps = rasterizer_globals_get()->d3d9_sm3_supported ? k_motion_sensor_sweep_ps_3_0 : k_motion_sensor_sweep_ps_2_0;
	rasterizer_dx9_device_get_interface()->CreatePixelShader((const DWORD*)ps, &motion_sensor_sweep_shader);

	WritePointer(Memory::GetAddress(0x2849EA) + 2, &motion_sensor_sweep_shader);
	return;
}
