#pragma once

/* prototypes */

void rasterizer_dx9_screen_effect_apply_patches(void);

void __cdecl rasterizer_dx9_create_screen_effect_pixel_shaders(void);

void rasterizer_dx9_set_screen_effect_pixel_shader(int32 local_pixel_shader);

void rasterizer_dx9_postprocess_scene(int32 render_layer_debug_view, bool lens_flare_occlusion_test, bool render_layer_selfibloomination);

void __cdecl rasterizer_dx9_render_screen_flash(void);
