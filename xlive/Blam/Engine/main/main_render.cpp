#include "stdafx.h"
#include "main_render.h"

#include "render/render_cartographer_ingame_ui.h"

/*  prototypes */

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
	render_cartographer_ingame_ui();

	return;
}
