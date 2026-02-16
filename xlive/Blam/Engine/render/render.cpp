#include "stdafx.h"
#include "render.h"

#include "render_contrails.h"
#include "render_debug.h"
#include "render_first_person.h"
#include "render_lights.h"
#include "render_primitive.h"
#include "render_sky.h"
#include "render_visibility.h"
#include "render_visible_geometry.h"
#include "render_water.h"
#include "render_weather.h"

#include "bink/wmv_playback.h"
#include "cache/pc_geometry_cache.h"
#include "effects/beam.h"
#include "effects/player_effects.h"
#include "game/game_engine.h"
#include "game/players.h"
#include "interface/hud.h"
#include "interface/terminal.h"
#include "interface/user_interface.h"
#include "math/color_math.h"
#include "main/main_time.h"
#include "objects/lights.h"
#include "rasterizer/dx9/rasterizer_dx9.h"
#include "rasterizer/dx9/rasterizer_dx9_decals.h"
#include "rasterizer/dx9/rasterizer_dx9_errors.h"
#include "rasterizer/dx9/rasterizer_dx9_main.h"
#include "rasterizer/dx9/rasterizer_dx9_screen_effect.h"
#include "rasterizer/dx9/rasterizer_dx9_shader_submit_new.h"
#include "rasterizer/dx9/rasterizer_dx9_water.h"
#include "rasterizer/rasterizer_fog.h"
#include "rasterizer/rasterizer_globals.h"
#include "rasterizer/rasterizer_lens_flares.h"
#include "rasterizer/rasterizer_occlusion.h"
#include "rasterizer/rasterizer_settings.h"
#include "rasterizer/rasterizer_transparent_geometry.h"
#include "scenario/scenario_fog.h"
#include "structures/structures.h"
#include "widgets/liquid.h"

/* globals */

bool g_render_layer_view_4 = false;
bool g_render_layer_view_5 = false;

bool g_submit_occlusion_tests = true;

bool render_decals_enabled = true;
bool render_lens_flares_enabled = true;
bool render_patchy_fog_enabled = true;
bool render_water_enabled = true;

e_controller_index g_render_current_controller_index = _controller_index_0;
uint32 g_render_current_user_index = 0;

window_bound g_user_window_bounds[k_number_of_controllers]{};

/* prototypes */

int32* get_global_window_out_cluster_index(int32 index);
int32* get_global_window_out_leaf_index(int32 index);
uint32* global_scene_rendered_count_get(void);

void render_view(
	real_rectangle2d* frustum_bounds,
	render_camera* rasterizer_camera,
	int32 window_bound_index,
	render_camera* render_camera,
	bool is_texture_camera,
	int32 cluster_index,
	int32 leaf_index,
	bool bsp_test_failed,
	int16 render_type,
	int32 user_index,
	int32 controller_index,
	bool draw_sky,
	int32 sky_index,
	s_scenario_fog_result* fog,
	int8 zero_1,
	int16 neg_one,
	s_bloom_window_data* window_bound_unk_data,
	int8 zero_2,
	s_screen_flash* screen_flash);

void __cdecl render_scene_bitflags_set(void);
void __cdecl render_camera_scene(
	int32 render_layer_debug_view,
	bool render_transparent_geo,
	bool lens_flare_occlusion_test,
	bool render_layer_selfibloomination,
	int32 hologram_flag,
	int32 effect_flag);

/* public code */

void render_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x19224A), render_window);
	PatchCall(Memory::GetAddress(0x19DA7C), render_window);
#ifdef TERMINAL_ENABLED
	PatchCall(Memory::GetAddress(0x190E3B), terminal_draw);
	PatchCall(Memory::GetAddress(0x190E45), main_time_frame_rate_display);
#endif
	return;
}

s_render* render_get(void)
{
	return Memory::GetAddress<s_render*>(0x4E66C8);
}

window_bound* get_user_window_bounds(int32 user_index)
{
	return &g_user_window_bounds[user_index];
}

int32 get_global_render_window_count()
{
	return *Memory::GetAddress<int32*>(0x4E6974);
}

e_controller_index global_render_current_controller_index()
{
	return g_render_current_controller_index;
}

uint32 global_render_current_user_index()
{
	return g_render_current_user_index;
}

int32* get_global_window_bound_index(void)
{
	return Memory::GetAddress<int32*>(0x4E6978, 0x50EC48);
}

s_frame* global_window_parameters_get(void)
{
	return Memory::GetAddress<s_frame*>(0xA3DF70);
}

s_frame_parameters* global_frame_parameters_get(void)
{
	return Memory::GetAddress<s_frame_parameters*>(0xA3E208);
}

int32* global_rasterizer_pixel_shader_index_get(void)
{
	return Memory::GetAddress<int32*>(0xA49494);
}

real64 get_current_render_time(void)
{
	return *Memory::GetAddress<real64*>(0x4E6968);
}

int32 get_player_window_count(void)
{
	return *Memory::GetAddress<int32*>(0x4E6974);
}

real32* hs_texture_camera_scale_get(void)
{
	return Memory::GetAddress<real32*>(0x41F850);
}

bool* hs_texture_camera_view_get(void)
{
	return Memory::GetAddress<bool*>(0x4F435C);
}

int32* global_user_render_index_get(void)
{
	return Memory::GetAddress<int32*>(0x4E6800);
}

uint32* global_effect_flag_get(void)
{
	return Memory::GetAddress<uint32*>(0xA3DA34);
}

uint32* global_frame_index_get(void)
{
	return Memory::GetAddress<uint32*>(0x4E695C);
}

bool __cdecl render_structure_find_camera(real_point3d* point, int32* out_cluster_index, int32* out_leaf_index)
{
	return INVOKE(0x191032, 0x0, render_structure_find_camera, point, out_cluster_index, out_leaf_index);
}

e_screen_split_type get_screen_split_type(int32 render_user_index)
{
	switch(get_global_render_window_count())
	{
		case 2:
			return _screen_split_type_half;
		case 3:
			if (render_user_index == 0)
				return _screen_split_type_half; 
			return _screen_split_type_quarter;
		case 4:
			return _screen_split_type_quarter;
		default:
			return _screen_split_type_full;
	}
}

bool frame_parameters_type_is_above_or_equal_to_7(void)
{
	return global_frame_parameters_get()->frame_type - 5 <= 2;
}

void render_scene_geometry(e_collection_type collection_type, e_render_layer render_layer)
{
	ASSERT(VALID_INDEX(collection_type, k_number_collection_types));
	ASSERT(VALID_INDEX(render_layer, k_number_of_render_layers));

	if (render_layer != _render_layer_debug_view && render_layer_begin(render_layer))
	{
		render_layer_draw();
		render_layer_end();
	}
	return;
}

void render_frame_present(bitmap_data* bitmap)
{
	rasterizer_dx9_present(bitmap, false);
	return;
}

void __cdecl render_frame(
	uint32 frame_render_type,
	int32 window_count,
	int32 player_count,
	int32 display_split_type,
	window_bound* window)
{
	INVOKE(0x192140, 0x0, render_frame, frame_render_type, window_count, player_count, display_split_type, window);
	return;
}

void __cdecl render_window(window_bound* window, bool is_texture_camera)
{
	s_screen_flash screen_flash;
	screen_flash.color = *global_real_argb_black;
	*get_global_window_bound_index() = window->window_bound_index;
	int32 cluster_index = *get_global_window_out_cluster_index(window->window_bound_index);
	int32 leaf_index = *get_global_window_out_leaf_index(window->window_bound_index);
	screen_flash.intensity = 0.0f;
	screen_flash.type = _screen_flash_type_none;

	const bool found_camera = render_structure_find_camera(&window->render_camera.point, &cluster_index, &leaf_index);
	if (found_camera)
	{
		*get_global_window_out_cluster_index(window->window_bound_index) = cluster_index;
		*get_global_window_out_leaf_index(window->window_bound_index) = leaf_index;
	}

	int32 visible_sky_index;
	real_rgb_color clear_color = *global_real_rgb_white;
	bool clear_color_active = false;
	bool draw_sky = structure_get_sky(cluster_index, &visible_sky_index, &clear_color, &clear_color_active);

	s_scenario_fog_result fog;
	render_scenario_fog(cluster_index, &window->render_camera, &window->render_camera.forward, draw_sky, *get_render_fog_enabled(), &fog);
	if (clear_color_active)
	{
		fog.clear_color = clear_color;
	}

	if (fog.view_max_distance_changed && window->render_camera.z_far > fog.view_max_distance && fog.view_max_distance > window->render_camera.z_near)
	{
		window->render_camera.z_far = fog.view_max_distance;
	}

	real_rectangle2d frustum_bounds;
	render_camera_build_viewport_frustum_bounds(&window->render_camera, &frustum_bounds);

	e_controller_index controller_index = k_no_controller;
	if (window->user_index != NONE)
	{
		controller_index = ((player_datum*)datum_get(player_data_get(), player_index_from_user_index(window->user_index)))->controller_index;
		render_screen_flash(window->user_index, &screen_flash);
	}

	ASSERT(!memcmp(&window->render_camera.viewport_bounds, &window->rasterizer_camera.viewport_bounds, sizeof(rectangle2d)));
	ASSERT(!memcmp(&window->render_camera.window_bounds, &window->rasterizer_camera.window_bounds, sizeof(rectangle2d)));

	rasterizer_globals_get()->rasterizer_draw_on_main_back_buffer = false;
	if (window->render_camera.vertical_field_of_view > k_real_epsilon)
	{
		g_render_current_controller_index = controller_index;
		g_render_current_user_index = window->user_index;

		// if user_index is NONE, likely we're in a cutscene
		if (window->user_index != NONE)
		{
			csmemcpy(&g_user_window_bounds[window->user_index], window, sizeof(window_bound));
		}
		else
		{
			csmemset(&g_user_window_bounds[0], 0, sizeof(g_user_window_bounds));
			csmemcpy(&g_user_window_bounds[0], window, sizeof(window_bound));
		}

		render_view(
			&frustum_bounds,
			&window->rasterizer_camera,
			window->window_bound_index,
			&window->render_camera,
			is_texture_camera,
			cluster_index,
			leaf_index,
			!found_camera,
			0,
			window->user_index,
			controller_index,
			draw_sky,
			visible_sky_index,
			&fog,
			0,
			-1,
			&window->bloom_data,
			0,
			&screen_flash);
	}
	else
	{
		error(_error_delayed, "Tried to render a view with a field of view of %f", window->render_camera.vertical_field_of_view);
	}

	return;
}

void __cdecl render_scene(
	int32 render_layer_debug_view,
	bool render_transparent_geo,
	bool lens_flare_occlusion_test,
	bool render_layer_selfibloomination,
	int32 hologram_flag,
	int32 effect_flag,
	real32 depth_range)
{
	const s_render* render = render_get();

	rasterizer_scene_begin_parameters parameters;
	uint32* g_scene_rendered_count = global_scene_rendered_count_get();
	parameters.scene_rendered_count = ++*g_scene_rendered_count;
	parameters.effect_flags = effect_flag;
	parameters.depth_range = depth_range;

	
	rasterizer_dx9_render_scene_start(&parameters);

	const s_frame* global_window_parameters = global_window_parameters_get();
	create_visible_render_primitives(hologram_flag);

	const real32 dr = global_window_parameters->camera.z_far - global_window_parameters->camera.z_near;
	const real_vector4d constants { 0.f, 1.f, dr, 1.f };
	IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();
	if (rasterizer_get_main_pixel_shader_cache()->test_cache(16, &constants, 1))
	{
		global_d3d_device->SetPixelShaderConstantF(16, (real32*)&constants, 1);
	}

	s_rasterizer_globals* rasterizer_globals = rasterizer_globals_get();
	if (render_layer_debug_view != 5)
	{
		if (render_layer_debug_view == 1 || render_layer_debug_view == 2)
		{
			if (rasterizer_globals->d3d9_sm3_supported)
			{
				*global_rasterizer_pixel_shader_index_get() = 2;
				g_dx9_dont_draw_to_depth_target_if_mrt_is_used = false;
			}

			/*
			* TODO:
			* DO NOT ENABLE THIS (for now)
			* Causes graphical issues on the start of outskirts with black geo being rendered
			rasterizer_dx9_perf_event_begin("texaccum", NULL);
			render_scene_geometry(_collection_type_0, _render_layer_texture_accumulate);
			rasterizer_dx9_perf_event_end("texaccum");
			*/

			rasterizer_dx9_perf_event_begin("lightmap_indirect", NULL);
			render_scene_geometry(_collection_type_0, _render_layer_lightmap_indirect);
			rasterizer_dx9_perf_event_end("lightmap_indirect");

			rasterizer_dx9_perf_event_begin("sh_prt", NULL);
			render_scene_geometry(_collection_type_0, _render_layer_spherical_harmonics_prt);
			rasterizer_dx9_perf_event_end("sh_prt");

			g_dx9_dont_draw_to_depth_target_if_mrt_is_used = true;

			rasterizer_dx9_perf_event_begin("environment_map", NULL);
			render_scene_geometry(_collection_type_0, _render_layer_enviroment_map);
			rasterizer_dx9_perf_event_end("environment_map");

			rasterizer_dx9_perf_event_begin("decal", NULL);
			render_scene_geometry(_collection_type_0, _render_layer_decal);
			rasterizer_dx9_perf_event_end("decal");


			rasterizer_dx9_perf_event_begin("decals_alpha_blend_prelight", NULL);
			if (render_decals_enabled)
			{
				rasterizer_dx9_decals_begin(_decal_layer_lit_alphablend_prelight);
				render_decal_visibility(rasterizer_dx9_decals_draw, 0);
				rasterizer_dx9_decals_end();
			}
			rasterizer_dx9_perf_event_end("decals_alpha_blend_prelight");

			rasterizer_dx9_perf_event_begin("render_cinematic_lightmap_shadows", NULL);
			render_cinematic_lightmap_shadows(effect_flag);
			rasterizer_dx9_perf_event_end("render_cinematic_lightmap_shadows");

			rasterizer_dx9_perf_event_begin("selfillumination", NULL);
			render_scene_geometry(_collection_type_0, _render_layer_selfillumination);
			rasterizer_dx9_perf_event_end("selfillumination");

			rasterizer_dx9_perf_event_begin("render_lights_new", NULL);
			render_lights_new();
			rasterizer_dx9_perf_event_end("render_lights_new");

			rasterizer_dx9_perf_event_begin("decals_alpha_blend", NULL);
			if (render_decals_enabled)
			{
				rasterizer_dx9_decals_begin(_decal_layer_lit_alphablend);
				render_decal_visibility(rasterizer_dx9_decals_draw, 0);
				rasterizer_dx9_decals_end();
			}
			rasterizer_dx9_perf_event_end("decals_alpha_blend");

			rasterizer_dx9_perf_event_begin("overlay", NULL);
			render_scene_geometry(_collection_type_0, _render_layer_overlay);
			rasterizer_dx9_perf_event_end("overlay");

			rasterizer_dx9_perf_event_begin("decals", NULL);
			if (render_decals_enabled)
			{
				rasterizer_dx9_decals_begin(_decal_layer_double_multiply);
				render_decal_visibility(rasterizer_dx9_decals_draw, 0);
				rasterizer_dx9_decals_end();

				rasterizer_dx9_decals_begin(_decal_layer_multiply);
				render_decal_visibility(rasterizer_dx9_decals_draw, 0);
				rasterizer_dx9_decals_end();

				rasterizer_dx9_decals_begin(_decal_layer_max);
				render_decal_visibility(rasterizer_dx9_decals_draw, 0);
				rasterizer_dx9_decals_end();

				rasterizer_dx9_decals_begin(_decal_layer_add);
				render_decal_visibility(rasterizer_dx9_decals_draw, 0);
				rasterizer_dx9_decals_end();
				
				rasterizer_dx9_decals_begin(_decal_layer_error);
				render_decal_visibility(rasterizer_dx9_decals_draw, 0);
				rasterizer_dx9_decals_end();
			}
			rasterizer_dx9_perf_event_end("decals");
		}
		// RENDER LAYER 0, 3 - 5
		else
		{
			/*
			* TODO:
			* DO NOT ENABLE THIS (for now)
			* Causes graphical issues on the start of outskirts with black geo being rendered
			rasterizer_dx9_perf_event_begin("texaccum", NULL);
			render_scene_geometry(_collection_type_0, _render_layer_texture_accumulate);
			rasterizer_dx9_perf_event_end("texaccum");
			*/

			rasterizer_dx9_perf_event_begin("lightmap_indirect", NULL);
			render_scene_geometry(_collection_type_0, _render_layer_lightmap_indirect);
			rasterizer_dx9_perf_event_end("lightmap_indirect");

			rasterizer_dx9_perf_event_begin("sh_prt", NULL);
			render_scene_geometry(_collection_type_0, _render_layer_spherical_harmonics_prt);
			rasterizer_dx9_perf_event_end("sh_prt");

			rasterizer_dx9_perf_event_begin("overlay", NULL);
			render_scene_geometry(_collection_type_0, _render_layer_overlay);
			rasterizer_dx9_perf_event_end("overlay");
		}

		// RENDER LAYER 2
		if (render_layer_debug_view == 2)
		{
render_layer_2:
			render_scene_geometry(_collection_type_0, _render_layer_transparent);
			
			if (render_layer_debug_view != 2)
			{
				rasterizer_dx9_perf_event_begin("game_engine_render", NULL);
				game_engine_render();
				rasterizer_dx9_perf_event_end("game_engine_render");

				rasterizer_dx9_perf_event_begin("contrails", NULL);
				contrails_render();
				rasterizer_dx9_perf_event_end("contrails");
			}

			if (render_transparent_geo)
			{
				rasterizer_transparent_geometry_draw();
			}

			if (g_submit_occlusion_tests && effect_flag != 2)
			{
				submit_occlusion_tests(lens_flare_occlusion_test);
			}

			if (render_layer_debug_view != 2 &&
				*get_render_fog_enabled() &&
				render_patchy_fog_enabled &&
				effect_flag != 2 &&
				!render->fog.field_96)
			{
				rasterizer_dx9_perf_event_begin("patchy_fog", NULL);
				render_patchy_fog(render_section_visibility_get_model_group_count() <= 0, true);
				rasterizer_dx9_perf_event_end("patchy_fog");
			}

			if (render_layer_selfibloomination)
			{
				if (effect_flag == 2)
				{
render_postprocess:
					rasterizer_dx9_perf_event_begin("lens_flares", NULL);
					if (lens_flare_occlusion_test)
					{
						lens_flares_draw(_rasterizer_target_render_primary);
					}
					rasterizer_dx9_perf_event_end("lens_flares");

					if (render_layer_debug_view == 2)
					{
						rasterizer_suns_glow_occlude(_rasterizer_target_render_primary);
					}
					else
					{
						c_render_primitive_list* list_type = render_primitive_get_by_primitive_list_type(0);
						list_type->m_primitive_count = 0;
						list_type->m_field_C = 0;
						list_type->m_render_layer_flags = 0;
						rasterizer_transparent_geometry_reset_counts();
						render_camera_scene(
							render_layer_debug_view,
							render_transparent_geo,
							lens_flare_occlusion_test,
							render_layer_selfibloomination,
							hologram_flag,
							effect_flag);
					}

					rasterizer_dx9_set_render_state(D3DRS_FILLMODE, D3DBLEND_SRCCOLOR);
					if (effect_flag != 2 && render_layer_debug_view != 2)
					{
						rasterizer_dx9_postprocess_scene(
							render_layer_debug_view,
							lens_flare_occlusion_test,
							render_layer_selfibloomination);
					}
					goto render_scene_end;
				}

				render_scene_geometry(_collection_type_0, _render_layer_selfibloomination);
			}

			if (effect_flag != 2)
			{
				rasterizer_occlusion_submit();
			}
			goto render_postprocess;
		}

		if (effect_flag != 2)
		{
			if (global_window_parameters->fog_result.draw_sky)
			{
				rasterizer_dx9_perf_event_begin("render_sky", NULL);
				rasterizer_dx9_set_stencil_mode(2);
				render_sky();
				rasterizer_dx9_set_stencil_mode(0);
				rasterizer_dx9_perf_event_end("render_sky");
			}

			if (get_render_fog_enabled())
			{
				// If we're not using shader model 3 draw the depth on the backbuffer in a seperate pass
				// In shader model 3 these are already drawn during the lightmap_indirect stage
				if (!rasterizer_globals->d3d9_sm3_supported)
				{
					rasterizer_dx9_perf_event_begin("render depth to backbuffer", NULL);

					const real32 new_depth_range = global_window_parameters->camera.z_far - global_window_parameters->camera.z_near;
					const real_vector4d ps_constants{ 0.f, 1.f, new_depth_range, 1.f };
					rasterizer_dx9_set_target(_rasterizer_target_backbuffer, 0, true);
					
					rasterizer_dx9_log(
						global_d3d_device->Clear(
							0,
							NULL,
							D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
							global_white_pixel32,
							1.f,
							0
						)
					);

					if (rasterizer_get_main_pixel_shader_cache()->test_cache(16, &ps_constants, 1))
					{
						global_d3d_device->SetPixelShaderConstantF(16, (real32*)&ps_constants, 1);
					}

					*global_rasterizer_pixel_shader_index_get() = 1;

					render_scene_geometry(_collection_type_0, _render_layer_lightmap_indirect);
					render_scene_geometry(_collection_type_0, _render_layer_spherical_harmonics_prt);

					rasterizer_dx9_perf_event_end("render depth to backbuffer");
				}

				if (rasterizer_globals->d3d9_sm3_supported)
				{
					g_dx9_dont_draw_to_depth_target_if_mrt_is_used = true;
				}
				else
				{
					*global_rasterizer_pixel_shader_index_get() = 0;
				}

				render_atmospheric_fog();
				render_scene_geometry(_collection_type_0, _render_layer_fog);
			}
		}

		if (render_water_enabled)
		{
			g_water_refraction_surface_updated = false;

			const bool water_enabled = !render->fog.camera_immersion_flags.test(_camera_immersion_disable_water_bit);
			bool clear_target = false;

			if (water_enabled)
			{
				const c_render_primitive_list* primitive_list = render_primitive_get_by_primitive_list_type(0);
				clear_target = primitive_list->empty(_render_layer_water_alpha_masks);
				render_scene_geometry(_collection_type_0, _render_layer_water_alpha_masks);
			}

			render_water(water_enabled, clear_target);
		}

		if (get_render_fog_enabled() && render_patchy_fog_enabled)
		{
			if (effect_flag == 2)
			{
render_widgets:
				rasterizer_dx9_perf_event_begin("liquids", NULL);
				liquid_render();
				rasterizer_dx9_perf_event_end("liquids");
				rasterizer_dx9_perf_event_begin("weather", NULL);
				weather_render();
				rasterizer_dx9_perf_event_end("weather");
				goto render_layer_2;
			}

			rasterizer_dx9_perf_event_begin("patchy_fog", NULL);
			if (render->fog.field_96)
			{
				render_patchy_fog(1, 1);
			}
			else if (render_section_visibility_get_model_group_count() > 0)
			{
				render_patchy_fog(1, 0);
			}
			rasterizer_dx9_perf_event_end("patchy_fog");
		}

		if (effect_flag != 2)
		{
			if (lens_flare_occlusion_test && render_lens_flares_enabled)
			{
				render_light_lens_flares();
				render_sky_lens_flares();
			}

			if (!g_submit_occlusion_tests)
			{
				submit_occlusion_tests(lens_flare_occlusion_test);
			}
		}
		goto render_widgets;
	}

render_scene_end:
	rasterizer_dx9_render_scene_end();
	return;
}

void __cdecl render_nonplayer_frame(window_bound* window_bounds)
{
	INVOKE(0x191FF9, 0x0, render_nonplayer_frame, window_bounds);
	return;
}

/* private code */

int32* get_global_window_out_cluster_index(int32 index)
{
	return &Memory::GetAddress<int32*>(0x4E697C, 0x50EC4C)[index];
}

int32* get_global_window_out_leaf_index(int32 index)
{
	return &Memory::GetAddress<int32*>(0x4E698C, 0x50EC5C)[index];
}

uint32* global_view_frame_num_get(void)
{
	return Memory::GetAddress<uint32*>(0x4E6960);
}

uint32* global_scene_rendered_count_get(void)
{
	return Memory::GetAddress<uint32*>(0x4E6964);
}

void render_view(
	real_rectangle2d* frustum_bounds,
	render_camera* rasterizer_camera,
	int32 window_bound_index,
	render_camera* render_camera,
	bool is_texture_camera,
	int32 cluster_index,
	int32 leaf_index,
	bool bsp_test_failed,
	int16 render_type,
	int32 user_index,
	int32 controller_index,
	bool sky_active,
	int32 sky_index,
	s_scenario_fog_result* fog,
	int8 zero_1,
	int16 neg_one,
	s_bloom_window_data* bloom_data,
	int8 zero_2,
	s_screen_flash* screen_flash) 
{
	ASSERT(render_camera);
	struct render_camera* camera = (rasterizer_camera ? rasterizer_camera : render_camera);

	++*global_view_frame_num_get();

	ASSERT(fog);
	
	s_render* render = render_get();
	render->player_window_index = window_bound_index;
	render->cluster_index = cluster_index;
	render->leaf_index = leaf_index;
	render->bsp_test_failed = bsp_test_failed;
	render->user_render_index = user_index;
	render->controller_render_index = controller_index;
	render->visible_sky_model = sky_active;
	render->visible_sky_index = sky_index;
	render->fog = *fog;
	render->unk_0 = false;
	render->camera = *render_camera;

	render_camera_build_projection(&render->camera, frustum_bounds, &render->projection);
	render_first_person();
	render_sky_model();
	render_lights();

	s_frame frame = {};
	frame.camera = *camera;
	render_camera_build_projection(&frame.camera, frustum_bounds, &frame.projection);

	frame.render_type = render_type;
	frame.field_6 = zero_1;
	frame.window_bound_index = (int16)window_bound_index;
	frame.is_texture_camera = is_texture_camera;
	frame.field_4 = neg_one;
	frame.render_fog = true;
	frame.color = fog->clear_color;
	frame.alpha = 2;
	frame.fog_result = *fog;
	frame.screen_flash = *screen_flash;
	frame.bloom_data = bloom_data;
	frame.field_294_zero = zero_2;

	bool result = rasterizer_window_begin(&frame);
	rasterizer_shader_level_of_detail_bias_update();
	if (result)
	{
		if (wmv_playback_in_progress())
		{
			wmv_render();
		}
		else
		{
			pc_geometry_cache_block_count_clear();
			render_beam();
			render_light_clear_data();
			render_view_visibility_compute_to_usercall(user_index);
			rasterizer_render_scene(is_texture_camera);

			rasterizer_dx9_perf_event_begin("interface", NULL);
			rasterizer_dx9_set_stencil_mode(0);
			interface_draw_screen();
			rasterizer_dx9_render_screen_flash();
			render_menu_user_interface(controller_index, (e_user_interface_render_window)NONE, &camera->viewport_bounds);
			rasterizer_dx9_perf_event_end("interface");

#ifndef NDEBUG
			rasterizer_dx9_perf_event_begin("debug", NULL);
			render_debug();
			rasterizer_dx9_perf_event_end("debug");
#endif

			rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DBLEND_INVSRCCOLOR);
		}
	}

	rasterizer_window_end();
	return;
}

void __cdecl render_scene_bitflags_set(void)
{
	INVOKE(0x2664AF, 0x0, render_scene_bitflags_set);
	return;
}

bool render_scene_is_splitscreen(void)
{
	const s_frame* global_window_parameters = global_window_parameters_get();

	// The following checks determine if the current camera viewport bounds are covering the whole screen
	// If the viewport bounds are not the entire screen we assume that there's more than 1 viewport being drawn
	// We also check if the player count is above 1 for good measure
	return
		global_window_parameters->camera.viewport_bounds.left != 0 ||
		global_window_parameters->camera.viewport_bounds.top != 0 ||
		global_window_parameters->camera.viewport_bounds.right != (int16)rasterizer_get_width() ||
		global_window_parameters->camera.viewport_bounds.bottom != (int16)rasterizer_get_height() ||
		get_player_window_count() > 1;
}

void rasterizer_render_scene(bool is_texture_camera)
{
	const s_frame* global_window_parameters = global_window_parameters_get();
	const s_frame_parameters* g_frame_parameters = global_frame_parameters_get();

	bool lens_flare_occlusion_test;
	bool render_layer_selfibloomination;
	bool render_transparent_geo = true;

	int32 render_layer_debug_view = 1;
	real32 depth_range = 0.f;

	render_scene_bitflags_set();

	if (g_render_layer_view_4)
	{
		render_layer_debug_view = 4;
		render_transparent_geo = false;
		lens_flare_occlusion_test = false;
		render_layer_selfibloomination = false;
	}
	else if (g_render_layer_view_5)
	{
		render_layer_debug_view = 5;
		render_transparent_geo = false;
		lens_flare_occlusion_test = false;
		render_layer_selfibloomination = false;
	}
	else if (global_window_parameters->render_type == 2)
	{
		lens_flare_occlusion_test = false;
		render_layer_selfibloomination = false;
	}
	else if (!is_texture_camera)
	{
		lens_flare_occlusion_test = g_frame_parameters->frame_type != 6;
		render_layer_selfibloomination = (g_frame_parameters->frame_type != 7 ? !render_scene_is_splitscreen() : false);
	}
	else
	{
		lens_flare_occlusion_test = false;
		render_layer_selfibloomination = false;
	}
	
	render_scene(render_layer_debug_view, render_transparent_geo, lens_flare_occlusion_test, render_layer_selfibloomination, 0, 0, depth_range);
	return;
}

void __cdecl render_camera_scene(
	int32 render_layer_debug_view,
	bool render_transparent_geo,
	bool lens_flare_occlusion_test,
	bool render_layer_selfibloomination,
	int32 hologram_flag,
	int32 effect_flag)
{
	INVOKE(0x1912B3, 0x0, render_camera_scene,
		render_layer_debug_view,
		render_transparent_geo,
		lens_flare_occlusion_test,
		render_layer_selfibloomination,
		hologram_flag,
		effect_flag);
	return;
}
