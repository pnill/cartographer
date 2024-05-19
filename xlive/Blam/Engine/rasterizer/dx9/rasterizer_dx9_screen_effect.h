#pragma once

void rasterizer_dx9_screen_effect_apply_patches(void);

void rasterizer_dx9_render_screen_effects(int32 render_layer_debug_view, bool lens_flare_occlusion_test, bool render_layer_selfibloomination);

void __cdecl rasterizer_dx9_render_screen_flash(void);
