#pragma once

/* enums */

enum e_render_layer : uint32
{
	_render_layer_none = 0,
	_render_layer_texture_accumulate = 1,
	_render_layer_overlay = 2,
	_render_layer_lightmap_indirect = 3,
	_render_layer_lightmap_specular = 4,
	_render_layer_spherical_harmonics_prt = 5,
	_render_layer_water_alpha_masks = 6,
	_render_layer_selfillumination = 7,
	_render_layer_enviroment_map = 8,
	_render_layer_stencil_shadows = 9,
	_render_layer_shadow_buffer_generate = 10,
	_render_layer_shadow_buffer_apply = 11,
	_render_layer_lightaccum_diffuse = 12,
	_render_layer_lightaccum_specular = 13,
	_render_layer_lightaccum_albedo = 14,
	_render_layer_transparent = 15,
	_render_layer_fog = 16,
	_render_layer_selfibloomination = 17,
	_render_layer_active_camo = 18,
	_render_layer_active_camo_stencil_modulate = 19,
	_render_layer_decal = 20,
	_render_layer_hologram = 21,
	_render_layer_error_report = 22,
	_render_layer_debug_view = 23,
	_render_layer_hud = 24,
	k_number_of_render_layers
};

/* structures */

struct s_render_layer_globals
{
	bool process_bloom_layer;
	bool transparent_submit_in_progress;
	bool first_person_render_in_progress;
	bool render_light_in_progress;
};


/* public code */

s_render_layer_globals* render_layer_globals_get(void);

bool __cdecl prepare_render_layer(e_render_layer layer);

void __cdecl draw_render_layer(void);

void __cdecl reset_after_render_layer_draw(void);
