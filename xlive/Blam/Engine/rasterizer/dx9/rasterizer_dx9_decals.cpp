#include "stdafx.h"
#include "rasterizer_dx9_decals.h"

#include "rasterizer_dx9_main.h"

/* public code */

void __cdecl rasterizer_dx9_decals_begin(e_decal_layer layer)
{
	INVOKE(0x275865, 0x0, rasterizer_dx9_decals_begin, layer);
	return;
}

void __cdecl rasterizer_dx9_decals_end(void)
{
	rasterizer_dx9_set_render_state(D3DRS_ZENABLE, D3DBLEND_ZERO);
	rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DBLEND_INVSRCCOLOR);
	rasterizer_dx9_set_render_state(D3DRS_DEPTHBIAS, 0);
	rasterizer_dx9_set_render_state(D3DRS_SLOPESCALEDEPTHBIAS, 0);
	return;
}

void __cdecl rasterizer_dx9_decals_draw(e_decal_layer layer, int16 a2)
{
	INVOKE(0x275BD8, 0x0, rasterizer_dx9_decals_draw, layer, a2);
	return;
}
