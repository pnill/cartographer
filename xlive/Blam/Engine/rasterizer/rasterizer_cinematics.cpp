#include "stdafx.h"
#include "rasterizer_cinematics.h"

/* prototypes */

static s_rasterizer_cinematics_globals* rasterizer_cinematic_globals_get(void);

/* public code */

void __cdecl rasterizer_cinematics_initialize(
	void)
{
	INVOKE(0x2705F4, 0x0, rasterizer_cinematics_initialize);
	return;
}

real32 rasterizer_get_near_clip_distance(
	void)
{
	const s_rasterizer_cinematics_globals* rasterizer_cinematics_globals = rasterizer_cinematic_globals_get();

	real32 result = *Memory::GetAddress<real32*>(0x468150);	// global_z_near
	if (rasterizer_cinematics_globals && rasterizer_cinematics_globals->near_clip_distance>0.f)
	{
		result = rasterizer_cinematics_globals->near_clip_distance;
	}

	return result;
}

real32 rasterizer_get_far_clip_distance(
	void)
{
	const s_rasterizer_cinematics_globals* rasterizer_cinematics_globals = rasterizer_cinematic_globals_get();

	real32 result = *Memory::GetAddress<real32*>(0x468154);	// global_z_far
	if (rasterizer_cinematics_globals && rasterizer_cinematics_globals->far_clip_distance>0.f)
	{
		result = rasterizer_cinematics_globals->far_clip_distance;
	}
	
	return result;
}

real32 rasterizer_cinematics_get_gamma_ramp(
	void)
{
	return rasterizer_cinematic_globals_get()->gamma_ramp;
}

s_rasterizer_cinematics_globals* __cdecl rasterizer_cinematic_screen_effect_update(
	s_rasterizer_cinematics_globals* globals)
{
	return INVOKE(0x270713, 0x0, rasterizer_cinematic_screen_effect_update, globals);
}

/* private code */

static s_rasterizer_cinematics_globals* rasterizer_cinematic_globals_get(
	void)
{
	return *Memory::GetAddress<s_rasterizer_cinematics_globals**>(0xA4BDC8);
}
