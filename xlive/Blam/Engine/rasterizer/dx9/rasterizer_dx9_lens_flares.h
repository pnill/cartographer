#pragma once

/* public code */

void rasterizer_dx9_lens_flares_apply_patches(void);

IDirect3DTexture9* rasterizer_dx9_sun_texture_get(void);

IDirect3DSurface9* rasterizer_dx9_sun_alpha_surface_get(void);

uint32* rasterizer_dx9_sun_glow_target_width_get(void);
