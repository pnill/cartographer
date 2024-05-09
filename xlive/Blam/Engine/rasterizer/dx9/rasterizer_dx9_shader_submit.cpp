#include "stdafx.h"
#include "rasterizer_dx9_shader_submit.h"

/* public code */

void __cdecl rasterizer_dx9_set_sampler_state(uint32 sampler, D3DSAMPLERSTATETYPE state, uint32 value)
{
	INVOKE(0x26F943, 0x0, rasterizer_dx9_set_sampler_state, sampler, state, value);
	return;
}
