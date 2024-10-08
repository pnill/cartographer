#include "stdafx.h"
#include "render.h"

#include "render_contrails.h"
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
#include "game/players.h"
#include "interface/hud.h"
#include "interface/user_interface.h"
#include "math/color_math.h"
#include "objects/lights.h"
#include "rasterizer/dx9/rasterizer_dx9.h"
#include "rasterizer/dx9/rasterizer_dx9_decals.h"
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

/* type definitions */

typedef bool(__cdecl* t_render_ingame_user_interface_hud_element)(
    real32 left,
    real32 top,
    int16 x,
    int16 y,
    real32 scale,
    real32 rotation_rad,
    datum bitmap_tag_index,
    datum bitmap,
    real_rectangle2d* bounds,
    datum shader_tag_index);

typedef bool(__cdecl* t_render_ingame_user_interface_hud_indicators_element_hook)(
    int32* a1,
    datum tag_index,
    datum bitmap_index,
    int32* a4,
    datum shader_index);

/* globals */

bool g_render_layer_view_4 = false;
bool g_render_layer_view_5 = false;

bool g_submit_occlusion_tests = true;

bool render_decals_enabled = true;
bool render_lens_flares_enabled = true;
bool render_patchy_fog_enabled = true;
bool render_water_enabled = true;

t_render_ingame_user_interface_hud_element p_draw_ingame_user_interface_hud_element;
t_render_ingame_user_interface_hud_indicators_element_hook p_render_ingame_user_interface_hud_indicators_element;

e_controller_index g_render_current_controller_index = _controller_index_0;
uint32 g_render_current_user_index = 0;

/* prototypes */

int32* get_global_window_out_cluster_index(int32 index);
int32* get_global_window_out_leaf_index(int32 index);
uint32* global_frame_num_get(void);
uint32* global_scene_rendered_count_get(void);
int32* curent_window_bound_index_get(void);
int32* global_cluster_index_get(void);
int32* global_leaf_index_get(void);
bool* global_bsp_test_failed_get(void);
bool* global_sky_active_get(void);
int32* global_sky_index_get(void);
s_scenario_fog_result* global_fog_result_get(void);
bool* global_byte_4E6938_get(void);
void __cdecl rasterizer_render_scene(bool is_texture_camera);

bool __cdecl render_ingame_user_interface_hud_indicators_element_hook(
    int32* a1,
    datum tag_index,
    datum bitmap_index,
    int32* a4,
    datum shader_index);

void render_view(
    real_rectangle2d* frustum_bounds,
    s_camera* rasterizer_camera,
    int32 window_bound_index,
    s_camera* render_camera,
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
void render_scene_wrapper(bool is_texture_camera);
void __cdecl render_camera(
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

    DETOUR_ATTACH(p_draw_ingame_user_interface_hud_element, Memory::GetAddress<t_render_ingame_user_interface_hud_element>(0x221E3B), render_ingame_user_interface_hud_element_hook);
    DETOUR_ATTACH(p_render_ingame_user_interface_hud_indicators_element, Memory::GetAddress<t_render_ingame_user_interface_hud_indicators_element_hook>(0x221C77), render_ingame_user_interface_hud_indicators_element_hook);
    return;
}

int32 get_global_render_window_count()
{
    return *Memory::GetAddress<int32*>(0x4E6974);
}

bool get_global_render_split_horizontally()
{
    return (*Memory::GetAddress<int32*>(0x4E6970) == 1);
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

s_scenario_fog_result* global_fog_result_get(void)
{
    return Memory::GetAddress<s_scenario_fog_result*>(0x4E6818);
}

int32* global_user_render_index_get(void)
{
    return Memory::GetAddress<int32*>(0x4E6800);
}

uint32* global_effect_flag_get(void)
{
    return Memory::GetAddress<uint32*>(0xA3DA34);
}

uint32* global_frame_num_get(void)
{
    return Memory::GetAddress<uint32*>(0x4E695C);
}

int32* load_low_detail_textures_get(void)
{
    return Memory::GetAddress<int32*>(0x4E6A60);
}

bool __cdecl structure_get_cluster_and_leaf_from_render_point(real_point3d* point, int32* out_cluster_index, int32* out_leaf_index)
{
    return INVOKE(0x191032, 0x0, structure_get_cluster_and_leaf_from_render_point, point, out_cluster_index, out_leaf_index);
}

e_screen_split_type get_screen_split_type(uint32 render_user_index)
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

    if (render_layer != _render_layer_debug_view && prepare_render_layer(render_layer))
    {
        draw_render_layer();
        reset_after_render_layer_draw();
    }
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
    bool bsp_test_success = structure_get_cluster_and_leaf_from_render_point(&window->render_camera.point, &cluster_index, &leaf_index);

    if (bsp_test_success)
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
        controller_index = s_player::get(player_index_from_user_index(window->user_index))->controller_index;
        render_screen_flash(window->user_index, &screen_flash);
    }

    ASSERT(!memcmp(&window->render_camera.viewport_bounds, &window->rasterizer_camera.viewport_bounds, sizeof(rectangle2d)));
    ASSERT(!memcmp(&window->render_camera.window_bounds, &window->rasterizer_camera.window_bounds, sizeof(rectangle2d)));

    rasterizer_globals_get()->rasterizer_draw_on_main_back_buffer = false;
    if (window->render_camera.vertical_field_of_view > k_real_math_epsilon)
    {
        g_render_current_controller_index = controller_index;
        g_render_current_user_index = window->user_index;

        render_view(
            &frustum_bounds,
            &window->rasterizer_camera,
            window->window_bound_index,
            &window->render_camera,
            is_texture_camera,
            cluster_index,
            leaf_index,
            !bsp_test_success,
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
        error(2, "Tried to render a view with a field of view of %f", window->render_camera.vertical_field_of_view);
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
    s_render_scene_parameters parameters;
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
            DRAW_RENDER_LAYER(_render_layer_texture_accumulate);
            rasterizer_dx9_perf_event_end("texaccum");
            */

            rasterizer_dx9_perf_event_begin("lightmap_indirect", NULL);
            DRAW_RENDER_LAYER(_render_layer_lightmap_indirect);
            rasterizer_dx9_perf_event_end("lightmap_indirect");

            rasterizer_dx9_perf_event_begin("sh_prt", NULL);
            DRAW_RENDER_LAYER(_render_layer_spherical_harmonics_prt);
            rasterizer_dx9_perf_event_end("sh_prt");

            g_dx9_dont_draw_to_depth_target_if_mrt_is_used = true;

            rasterizer_dx9_perf_event_begin("environment_map", NULL);
            DRAW_RENDER_LAYER(_render_layer_enviroment_map);
            rasterizer_dx9_perf_event_end("environment_map");

            rasterizer_dx9_perf_event_begin("decal", NULL);
            DRAW_RENDER_LAYER(_render_layer_decal);
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
            DRAW_RENDER_LAYER(_render_layer_selfillumination);
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
            DRAW_RENDER_LAYER(_render_layer_overlay);
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
            DRAW_RENDER_LAYER(_render_layer_texture_accumulate);
            rasterizer_dx9_perf_event_end("texaccum");
            */

            rasterizer_dx9_perf_event_begin("lightmap_indirect", NULL);
            DRAW_RENDER_LAYER(_render_layer_lightmap_indirect);
            rasterizer_dx9_perf_event_end("lightmap_indirect");

            rasterizer_dx9_perf_event_begin("sh_prt", NULL);
            DRAW_RENDER_LAYER(_render_layer_spherical_harmonics_prt);
            rasterizer_dx9_perf_event_end("sh_prt");

            rasterizer_dx9_perf_event_begin("overlay", NULL);
            DRAW_RENDER_LAYER(_render_layer_overlay);
            rasterizer_dx9_perf_event_end("overlay");
        }

        // RENDER LAYER 2
        if (render_layer_debug_view == 2)
        {
render_layer_2:
            DRAW_RENDER_LAYER(_render_layer_transparent);
            
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
                transparent_geometry_draw();
            }

            if (g_submit_occlusion_tests && effect_flag != 2)
            {
                submit_occlusion_tests(lens_flare_occlusion_test);
            }

            const s_scenario_fog_result* g_fog_result = global_fog_result_get();
            if (render_layer_debug_view != 2 &&
                *get_render_fog_enabled() &&
                render_patchy_fog_enabled &&
                effect_flag != 2 &&
                !g_fog_result->field_96)
            {
                rasterizer_dx9_perf_event_begin("patchy_fog", NULL);
                render_patchy_fog(*visible_geometry_group_count_get() <= 0, true);
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
                        list_type->m_field_4 = 0;
                        list_type->m_field_C = 0;
                        list_type->m_render_layer_flags.clear();
                        rasterizer_transparent_geometry_reset_counts();
                        render_camera(
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

                DRAW_RENDER_LAYER(_render_layer_selfibloomination);
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
                const s_rasterizer_dx9_main_globals* dx9_globals = rasterizer_dx9_main_globals_get();
                const s_rasterizer_globals* rasterizer_globals = rasterizer_globals_get();

                // If we're not using shader model 3 draw the depth on the backbuffer in a seperate pass
                // In shader model 3 these are already drawn during the lightmap_indirect stage
                if (!rasterizer_globals->d3d9_sm3_supported)
                {
                    rasterizer_dx9_perf_event_begin("render depth to backbuffer", NULL);

                    const real32 depth_range = global_window_parameters->camera.z_far - global_window_parameters->camera.z_near;
                    const real_vector4d constants{ 0.f, 1.f, depth_range, 1.f };
                    rasterizer_dx9_set_target(_rasterizer_target_backbuffer, 0, true);
                    global_d3d_device->Clear(
                        0,
                        NULL,
                        D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                        global_white_pixel32.color,
                        1.f,
                        0);

                    if (rasterizer_get_main_pixel_shader_cache()->test_cache(16, &constants, 1))
                    {
                        global_d3d_device->SetPixelShaderConstantF(16, (real32*)&constants, 1);
                    }

                    *global_rasterizer_pixel_shader_index_get() = 1;

                    DRAW_RENDER_LAYER(_render_layer_lightmap_indirect);
                    DRAW_RENDER_LAYER(_render_layer_spherical_harmonics_prt);

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
                DRAW_RENDER_LAYER(_render_layer_fog);
            }
        }

        if (render_water_enabled)
        {
            g_water_refraction_surface_updated = false;

            const s_scenario_fog_result* g_fog_result = global_fog_result_get();

            const bool water_enabled = !g_fog_result->camera_immersion_flags.test(_camera_immersion_disable_water_bit);
            bool clear_target = false;

            if (water_enabled)
            {
                const c_render_primitive_list* list_type = render_primitive_get_by_primitive_list_type(0);
                clear_target = list_type->is_layer_different(_render_layer_water_alpha_masks);
                DRAW_RENDER_LAYER(_render_layer_water_alpha_masks);
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

            const s_scenario_fog_result* g_fog_result = global_fog_result_get();
            rasterizer_dx9_perf_event_begin("patchy_fog", NULL);
            if (g_fog_result->field_96)
            {
                render_patchy_fog(1, 1);
            }
            else if (*visible_geometry_group_count_get() > 0)
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

int32* curent_window_bound_index_get(void)
{
    return Memory::GetAddress<int32*>(0x4E67FC);
}

int32* global_cluster_index_get(void)
{
    return Memory::GetAddress<int32*>(0x4E680C);
}

int32* global_leaf_index_get(void)
{
    return Memory::GetAddress<int32*>(0x4E6808);
}

bool* global_bsp_test_failed_get(void)
{
    return Memory::GetAddress<bool*>(0x4E6810);
}

bool* global_sky_active_get(void)
{
    return Memory::GetAddress<bool*>(0x4E6811);
}

int32* global_sky_index_get(void)
{
    return Memory::GetAddress<int32*>(0x4E6814);
}

bool* global_byte_4E6938_get(void)
{
    return Memory::GetAddress<bool*>(0x4E6938);
}

void __cdecl rasterizer_render_scene(bool is_texture_camera) 
{
    INVOKE(0x25F015, 0x0, rasterizer_render_scene, is_texture_camera);
    return;
}

void rasterizer_setup_2d_vertex_shader_user_interface_constants()
{
	IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();

	real_vector4d vc[5];
	int16 width, height;

	s_camera* global_camera = get_global_camera();

	rectangle2d screen_bounds = global_camera->viewport_bounds;
	width = rectangle2d_width(&screen_bounds);
	height = rectangle2d_height(&screen_bounds);

	// vertex shaders use normalized device coordinates system (NDC)
	vc[0].i = 2.0f / (real32)width; // x
	vc[0].j = 0.0f;
	vc[0].k = 0.0f;
	vc[0].l = -(1.0f / (real32)width + 1.0f) - ((real32)screen_bounds.left * 2.0f / width); // offset from x

	vc[1].i = 0.0f;
	vc[1].j = -(2.0f / (real32)height); // y
	vc[1].k = 0.0f;
	vc[1].l = (1.0f / (real32)height + 1.0f) + ((real32)screen_bounds.top * 2.0f / height); // offset from y

	vc[2].i = 0.0f;
	vc[2].j = 0.0f;
	vc[2].k = 0.0f; // z
	vc[2].l = 0.5f; // acts as an offset, facing (<=1.0f is towards the viewport, above 1.0f facing from the viewport)

	vc[3].i = 0.0f;
	vc[3].j = 0.0f;
	vc[3].k = 0.0f;
	vc[3].l = 1.0f; // w scaling component

	// the c181 register seems unused?
	vc[4].i = 0.0f;
	vc[4].j = 0.0f;
	vc[4].k = 0.0f;
	vc[4].l = 0.0f;

	// avoid unnecessary API calls by testing the user mode memory cache
	if (rasterizer_get_main_vertex_shader_cache()->test_cache(177, vc, NUMBEROF(vc)))
	{
		global_d3d_device->SetVertexShaderConstantF(177, (const real32*)vc, NUMBEROF(vc));
	}
    return;
}

// hud fixes 

bool __cdecl render_ingame_user_interface_hud_element_hook(
    real32 left,
    real32 top,
    int16 x,
    int16 y,
    real32 scale,
    real32 rotation_rad,
    datum bitmap_tag_index,
    datum bitmap,
    real_rectangle2d* bounds,
    datum shader_tag_index)
{
    rasterizer_setup_2d_vertex_shader_user_interface_constants();
    return p_draw_ingame_user_interface_hud_element(left, top, x, y, scale, rotation_rad, bitmap_tag_index, bitmap, bounds, shader_tag_index);
}

bool __cdecl render_ingame_user_interface_hud_indicators_element_hook(int32* a1, datum tag_index, datum bitmap_index, int32* a4, datum shader_index)
{
    rasterizer_setup_2d_vertex_shader_user_interface_constants();
    return p_render_ingame_user_interface_hud_indicators_element(a1, tag_index, bitmap_index, a4, shader_index);
}

void render_view(
    real_rectangle2d* frustum_bounds,
    s_camera* rasterizer_camera,
    int32 window_bound_index,
    s_camera* render_camera,
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
    s_bloom_window_data* bloom_data,
    int8 zero_2,
    s_screen_flash* screen_flash) 
{
    ASSERT(render_camera);
    s_camera* camera = (rasterizer_camera ? rasterizer_camera : render_camera);

    ++*global_view_frame_num_get();

    ASSERT(fog);
    
    *curent_window_bound_index_get() = window_bound_index;
    *global_cluster_index_get() = cluster_index;
    *global_leaf_index_get() = leaf_index;
    *global_bsp_test_failed_get() = bsp_test_failed;
    *global_user_render_index_get() = user_index;
    *global_sky_active_get() = draw_sky;
    *global_sky_index_get() = sky_index;
    
    *global_fog_result_get() = *fog;
    *global_byte_4E6938_get() = false;

    s_camera* global_camera = get_global_camera();
    *global_camera = *render_camera;

    render_camera_build_projection(global_camera, frustum_bounds, global_projection_get());
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
        if (media_foundation_player_running())
        {
            media_foundation_player_frame();
        }
        else
        {
            pc_geometry_cache_block_count_clear();
            render_beam();
            render_light_clear_data();
            render_view_visibility_compute_to_usercall(user_index);
            render_scene_wrapper(is_texture_camera);

            rasterizer_dx9_perf_event_begin("interface", NULL);
            rasterizer_dx9_set_stencil_mode(0);
            hud_draw_screen();
            rasterizer_dx9_render_screen_flash();
            render_menu_user_interface_to_usercall(0, controller_index, NONE, &camera->viewport_bounds);
            rasterizer_dx9_perf_event_end("interface");

#ifndef NDEBUG
            rasterizer_dx9_perf_event_begin("debug", NULL);
            rasterizer_dx9_perf_event_end("debug");
#endif

            rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DBLEND_INVSRCCOLOR);
        }
    }

    rasterizer_update_cameras();
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
        global_window_parameters->camera.viewport_bounds.right != rasterizer_get_width() ||
        global_window_parameters->camera.viewport_bounds.bottom != rasterizer_get_height() ||
        get_player_window_count() > 1;
}

void render_scene_wrapper(bool is_texture_camera)
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

void __cdecl render_camera(
    int32 render_layer_debug_view,
    bool render_transparent_geo,
    bool lens_flare_occlusion_test,
    bool render_layer_selfibloomination,
    int32 hologram_flag,
    int32 effect_flag)
{
    INVOKE(0x1912B3, 0x0, render_camera,
        render_layer_debug_view,
        render_transparent_geo,
        lens_flare_occlusion_test,
        render_layer_selfibloomination,
        hologram_flag,
        effect_flag);
    return;
}