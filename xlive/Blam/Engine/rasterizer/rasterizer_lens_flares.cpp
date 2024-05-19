#include "stdafx.h"
#include "rasterizer_lens_flares.h"

#include "rasterizer/dx9/rasterizer_dx9_targets.h"

/* public code */

void lens_flare_fix(void)
{
	//rasterizer_near_clip_distance <real>
	//Changed from game default of 0.06 to 0.0601
	real32* rasterizer_near_clip_distance = Memory::GetAddress<real32*>(0x468150, 0x0);
	*rasterizer_near_clip_distance = 0.0601f;
	return;
}

e_rasterizer_target __cdecl rasterizer_suns_glow_draw(e_rasterizer_target target)
{
	return INVOKE(0x26CF7F, 0x0, rasterizer_suns_glow_draw, target);
}