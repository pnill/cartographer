#pragma once

namespace RenderHooks
{
	enum e_layer_type : int
	{
		unk_1 = 1,
		unk_2 = 2,
		unk_3 = 3,
		//Fog shit /Glowing stuff?
		unk_4 = 4,
		unk_5 = 5,
		unk_6 = 6,
		dynamic_shadows = 7,
		lightmap_shadows_1 = 8,
		lightmap_shadows_2 = 9,
		unk_10 = 10,
		unk_11 = 11,
		unk_12 = 12,
		unk_13 = 13,
		unk_14 = 14,
		//Active camo
		unk_15 = 15,
		unk_16 = 16,
		unk_17 = 17,
		unk_18 = 18,
		water = 19,
		//Screen distortion effects
		unk_20 = 20,
		//Out of focus blur
		unk_21 = 21,
		//Main Menu Transition Blur
		unk_22 = 22,
		// main menu background blur
		unk_23 = 23,
		unk_24 = 24,
		unk_25 = 25,

		end = unk_25
	};
	enum e_render_geometry_type
	{
		texture_accumulate = 0x1,
		overlay = 0x2,
		lightmap_indirect = 0x3,
		lightmap_specular = 0x4,
		spherical_harmonics_PRT = 0x5,
		water_alpha_masks = 0x6,
		selfillumination = 0x7,
		env_map = 0x8,
		stencil_shadows = 0x9,
		shadow_buffer_generate = 0xA,
		shadow_buffer_apply = 0xB,
		lightaccum_diffuse = 0xC,
		lightaccum_specular = 0xD,
		lightaccum_albedo = 0xE,
		transparent = 0xF,
		fog = 0x10,
		selfibloomination = 0x11,
		active_camo = 0x12,
		active_camo_stencil_modulate = 0x13,
		decal = 0x14,
		hologram = 0x15,
		error_report = 0x16,
		debug_view = 0x17,
		hud = 0x18,
	};

	void ResetDevice();
	void ApplyHooks();
	void Initialize();
}

extern bool ras_layer_overrides[RenderHooks::end];
extern bool geo_render_overrides[24];