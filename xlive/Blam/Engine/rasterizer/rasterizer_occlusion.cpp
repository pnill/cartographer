#include "stdafx.h"
#include "rasterizer_occlusion.h"

#include "render/render.h"
#include "rasterizer/dx9/rasterizer_dx9.h"
#include "rasterizer/dx9/rasterizer_dx9_lens_flares.h"

/* public code */

void rasterizer_occlusion_initialize(void)
{
	INVOKE(0x27E1E4, 0x0, rasterizer_occlusion_initialize);
	return;
}

void submit_occlusion_tests(bool lens_flare_occlusion_test)
{
	rasterizer_dx9_perf_event_begin("occlusion tests", NULL);
	if (lens_flare_occlusion_test && render_lens_flares_enabled)
	{
		lens_flares_submit_occlusions();
	}
	rasterizer_dx9_perf_event_end("occlusion tests");
	return;
}

void __cdecl rasterizer_occlusion_submit(void)
{
	INVOKE(0x27E29A, 0x0, rasterizer_occlusion_submit);
	return;
}
