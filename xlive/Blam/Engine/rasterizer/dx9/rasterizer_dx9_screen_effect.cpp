#include "stdafx.h"
#include "rasterizer_dx9_screen_effect.h"

#include "rasterizer_dx9.h"
#include "rasterizer_dx9_main.h"
#include "rasterizer_dx9_dof.h"
#include "rasterizer_dx9_fullscreen_passes.h"

#include "cutscene/cinematics.h"
#include "render/render.h"
#include "rasterizer/rasterizer_cinematics.h"
#include "rasterizer/rasterizer_lens_flares.h"
#include "rasterizer/rasterizer_screen_effects.h"
#include "scenario/scenario.h"

/* constants */

#define k_bloom_blur_amount 3.992f
#define k_bloom_box_factor 0.25f
#define k_bloom_max_factor 0.f


/* globals */

bool g_disable_bloom = false;
bool g_render_scenario_screen_effects = true;

uint16 g_bloom_info_log_timer = 0;

//real32 x_rasterizer_screen_effect_inputs[4];	// Unused for now, only used for size

/* prototypes */
void __cdecl rasterizer_dx9_render_crossfade(real32 lower, real32 upper);

/* public code */

void rasterizer_dx9_screen_effect_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x191BB1), rasterizer_dx9_render_screen_effects);
	return;
}

void rasterizer_dx9_render_screen_effects(int32 render_layer_debug_view, bool lens_flare_occlusion_test, bool render_layer_selfibloomination)
{
	rasterizer_dx9_perf_event_begin("screen effects", NULL);
	rasterizer_dx9_reset_depth_buffer();

	IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();
	const s_frame* global_window_parameters = global_window_parameters_get();

	int16 width = rectangle2d_width(&global_window_parameters->camera.viewport_bounds);
	int16 height = rectangle2d_height(&global_window_parameters->camera.viewport_bounds);
	int16 left = global_window_parameters->camera.viewport_bounds.left;
	int16 top = global_window_parameters->camera.viewport_bounds.top;

	if (render_layer_selfibloomination && !g_disable_bloom)
	{
		real32 brightness;
		real32 overbrightness;
		rasterizer_get_bloom_brightness(&brightness, &overbrightness);
		
		real32 bloom_threshold = 0.89999998f;
		if (global_window_parameters->bloom_data)
		{
			bloom_threshold *= (1.f - (1.f - global_window_parameters->bloom_data->field_10) * 1.f);
		}

		if (brightness > 0.0)
		{
			real32 brightness_calculation = (overbrightness + 1.0) * (brightness * 0.5);
			brightness = PIN(brightness_calculation, 0.f, 1.f);
			
			s_scenario_fog_result* g_fog_result = global_fog_result_get();

			if (g_fog_result->gamma_override > 0.f)
			{
				bloom_threshold += ((g_fog_result->field_E4 - bloom_threshold) * g_fog_result->gamma_override);
				brightness		+= ((g_fog_result->field_E8 - brightness) * g_fog_result->gamma_override);
			}

			// This logging doesn't seem to be setup properly in the base game?
			// ### TODO: fix this
			if (g_bloom_info_log_timer == 4096)
			{
				error(3, ">>> BLOOM BLUR AMOUNT= %f", k_bloom_blur_amount);
				error(3, ">>> BLOOM THRESHOLD=   %f", bloom_threshold);
				error(3, ">>> BLOOM BRIGHTNESS=  %f", brightness);
				error(3, ">>> BLOOM BOXFACTOR=   %f", k_bloom_box_factor);
				error(3, ">>> BLOOM MAXFACTOR=   %f", k_bloom_max_factor);
				g_bloom_info_log_timer = 0;
			}

			rasterizer_dx9_perf_event_begin("bloom", NULL);
			rasterizer_dx9_dof_render_bloom(k_bloom_blur_amount, bloom_threshold, brightness, k_bloom_box_factor, k_bloom_max_factor, true, global_frame_parameters_get()->frame_type == 6);
			rasterizer_dx9_perf_event_end();
		}
	}

	e_rasterizer_target target = _rasterizer_target_0;
	scenario* global_scenario = get_global_scenario();
	if (global_scenario)
	{
		if (!frame_parameters_type_is_above_or_equal_to_7()
			&& !global_window_parameters->is_texture_camera
			&& !*global_effect_flag_get()
			&& g_render_scenario_screen_effects)
		{
			if (cinematic_in_progress())
			{
				s_rasterizer_cinematics_globals* rasterizer_cinematic_globals = rasterizer_cinematic_screen_effect_update(NULL);
				if (rasterizer_cinematic_globals)
				{
					if (rasterizer_cinematic_globals->field_3C && rasterizer_cinematic_globals->field_40 > 0.f)
					{
						rasterizer_dx9_render_depth_of_field(
							rasterizer_cinematic_globals->field_44.lower,
							rasterizer_cinematic_globals->field_44.upper,
							rasterizer_cinematic_globals->field_40);
					}

					if (rasterizer_cinematic_globals->field_AC.lower > 0.f)
					{
						rasterizer_dx9_perf_event_begin("crossfade", NULL);
						rasterizer_dx9_render_crossfade(
							rasterizer_cinematic_globals->field_AC.lower,
							rasterizer_cinematic_globals->field_AC.upper);
						rasterizer_dx9_perf_event_end();
					}
				}
			}
		}

		if (global_scenario->screen_effect_references.count > 0)
		{
			real32 x = -0.5f / width;
			real32 y = -0.5f / height;

			real32 mesh_data[16];
			mesh_data[0] = x - 1.f;
			mesh_data[1] = y + 1.f;
			mesh_data[2] = 0.f;
			mesh_data[3] = 0.f;

			mesh_data[4] = x + 1.f;
			mesh_data[5] = y + 1.f;
			mesh_data[6] = 1.f;
			mesh_data[7] = 0.f;

			mesh_data[8] = x - 1.f;
			mesh_data[9] = y - 1.f;
			mesh_data[10] = 0.f;
			mesh_data[11] = 1.f;

			mesh_data[12] = x + 1.f;
			mesh_data[13] = y - 1.f;
			mesh_data[14] = 1.f;
			mesh_data[15] = 1.f;

			for (int32 i = 0; i < global_scenario->screen_effect_references.count; i++)
			{
				s_scenario_screen_effect_reference* screen_effect_reference = global_scenario->screen_effect_references[i];
				if (screen_effect_reference->screen_effect.index != NONE)
				{
					real32 interpolator_result[4] = { 0.f };
					if (screen_effect_reference->primary_scenario_interpolator_index != NONE)
					{
						interpolator_result[0] = scenario_interpolator_get_result(screen_effect_reference->primary_scenario_interpolator_index);
					}

					if (screen_effect_reference->secondary_scenario_interpolator_index != NONE)
					{
						interpolator_result[1] = scenario_interpolator_get_result(screen_effect_reference->secondary_scenario_interpolator_index);
					}

					if (interpolator_result[0] != 0.f || interpolator_result[1] != 0.f)
					{
						target = rasterizer_screen_effects_render(
							screen_effect_reference->screen_effect.index,
							interpolator_result,
							2,
							2,
							mesh_data);
					}
				}
			}
		}
	}
	

	if (lens_flare_occlusion_test)
	{
		target = rasterizer_suns_glow_draw(target);
	}

	rasterizer_dx9_perf_event_begin("gamma_brightness", NULL);
	rasterizer_dx9_apply_gamma_and_brightness(target);
	rasterizer_dx9_perf_event_end();

	global_d3d_device->StretchRect(global_d3d_surface_render_primary_get(), NULL, global_d3d_surface_render_resolved_get(), NULL, D3DTEXF_NONE);
	*rasterizer_target_back_buffer() = true;

	rasterizer_dx9_perf_event_end();
}

void __cdecl rasterizer_dx9_render_screen_flash(void)
{
	INVOKE(0x27C86D, 0x0, rasterizer_dx9_render_screen_flash);
	return;
}

/* private code */

void __cdecl rasterizer_dx9_render_crossfade(real32 lower, real32 upper)
{
	INVOKE(0x26CCC9, 0x0, rasterizer_dx9_render_crossfade, lower, upper);
	return;
}
