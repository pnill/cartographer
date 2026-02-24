#pragma once

/* defines */


/* globals */

extern bool g_rasterizer_dx9on12_enabled;
extern struct IDirect3DDevice9On12* g_d3d9on12_device;

/* prototypes */

int32 rasterizer_dx9_create_through_d3d9on12(struct IDirect3D9Ex** d3d, bool use_warp);
