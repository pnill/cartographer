#include "stdafx.h"
#include "main_render.h"

#include "render/render_cartographer_ingame_ui.h"
#include "rasterizer/dx9/rasterizer_dx9.h"

/* prototypes */

void main_render_hook(void);

/* public code */

void main_render_apply_patches(void)
{
	// this is replacing a nullsub
	PatchCall(Memory::GetAddress(0x19228E), main_render_hook);
	return;
}

/* private code */

void main_render_hook(void)
{
	rasterizer_dx9_perf_event_begin("render cartographer text", NULL);
	render_cartographer_status_text();
	render_cartographer_achievements();
	render_cartographer_update();
	rasterizer_dx9_perf_event_end();
	return;
}

