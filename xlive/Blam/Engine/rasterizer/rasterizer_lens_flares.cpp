#include "stdafx.h"
#include "rasterizer_lens_flares.h"

void lens_flare_fix()
{
	//rasterizer_near_clip_distance <real>
	//Changed from game default of 0.06 to 0.0601
	*Memory::GetAddress<float*>(0x468150, 0x0) = 0.0601f;
}