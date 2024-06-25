#include "stdafx.h"
#include "render_layers.h"

/* public code */

s_render_layer_globals* render_layer_globals_get(void)
{
	return Memory::GetAddress<s_render_layer_globals*>(0xA3E3E4);
}

bool __cdecl prepare_render_layer(e_render_layer layer)
{
	return INVOKE(0x1A155C, 0x0, prepare_render_layer, layer);
}

void __cdecl draw_render_layer(void)
{
	return INVOKE(0x1A176F, 0x0, draw_render_layer);
}

void __cdecl reset_after_render_layer_draw(void)
{
	return INVOKE(0x1A17D9, 0x0, reset_after_render_layer_draw);
}
