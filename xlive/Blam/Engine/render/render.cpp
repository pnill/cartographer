#include "stdafx.h"
#include "render.h"

#include "render_first_person.h"
#include "render_lights.h"
#include "render_sky.h"
#include "render_visibility_collection.h"

#include "bink/wmv_playback.h"
#include "cache/pc_geometry_cache.h"
#include "effects/beam.h"
#include "effects/player_effects.h"
#include "game/players.h"
#include "interface/hud.h"
#include "interface/user_interface.h"
#include "main/main_screenshot.h"
#include "math/color_math.h"
#include "rasterizer/dx9/rasterizer_dx9_main.h"
#include "rasterizer/dx9/rasterizer_dx9_screen_effect.h"
#include "rasterizer/dx9/rasterizer_dx9_shader_submit_new.h"
#include "rasterizer/rasterizer_settings.h"
#include "scenario/scenario_fog.h"
#include "structures/structures.h"


void render_apply_patches(void)
{
    PatchCall(Memory::GetAddress(0x19224A), render_window);
    PatchCall(Memory::GetAddress(0x19DA7C), render_window);
    return;
}

real64 get_current_render_time(void)
{
    return *Memory::GetAddress<real64*>(0x4E6968);
}

int32* get_global_window_bound_index(void)
{
    return Memory::GetAddress<int32*>(0x4E6978, 0x50EC48);
}

int32* get_global_window_out_cluster_index(int32 index)
{
    return &Memory::GetAddress<int32*>(0x4E697C, 0x50EC4C)[index];
}

int32* get_global_window_out_leaf_index(int32 index)
{
    return &Memory::GetAddress<int32*>(0x4E698C, 0x50EC5C)[index];
}

uint32* global_frame_num_get(void)
{
    return Memory::GetAddress<uint32*>(0x4E6960);
}

int32* curent_window_bound_index_get(void)
{
    return Memory::GetAddress<int32*>(0x4E67FC);
}

int32* global_cluster_index_get(void)
{
    return Memory::GetAddress<int32*>(0x4E680C);
}

int32* global_user_render_index_get(void)
{
    return Memory::GetAddress<int32*>(0x4E6800);
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

s_scenario_fog_result* global_fog_result_get(void)
{
    return Memory::GetAddress<s_scenario_fog_result*>(0x4E6818);
}

bool* global_byte_4E6938_get(void)
{
    return Memory::GetAddress<bool*>(0x4E6938);
}

bool __cdecl structure_get_cluster_and_leaf_from_render_point(real_point3d* point, int32* out_cluster_index, int32* out_leaf_index)
{   
    return INVOKE(0x191032, 0x0, structure_get_cluster_and_leaf_from_render_point, point, out_cluster_index, out_leaf_index);
}

void __cdecl render_scene_wrapper(bool is_texture_camera) 
{
    INVOKE(0x25F015, 0x0, render_scene_wrapper, is_texture_camera);
    return;
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
    bool sky_active,
    int32 sky_id,
    s_scenario_fog_result* fog,
    int8 zero_1,
    int16 neg_one,
    void* window_bound_unk_data,
    int8 zero_2,
    s_screen_flash* screen_flash) 
{
    ASSERT(render_camera);
    s_camera* camera = (rasterizer_camera ? rasterizer_camera : render_camera);
    ++*global_frame_num_get();

    ASSERT(fog);
    *curent_window_bound_index_get() = window_bound_index;
    *global_cluster_index_get() = cluster_index;
    *global_leaf_index_get() = leaf_index;
    *global_bsp_test_failed_get() = bsp_test_failed;
    *global_user_render_index_get() = user_index;
    *global_sky_active_get() = sky_active;
    *global_sky_index_get() = sky_id;

    *global_fog_result_get() = *fog;
    *global_byte_4E6938_get() = 0;
    
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
    frame.window_bound_index = window_bound_index;
    frame.is_texture_camera = is_texture_camera;
    frame.field_4 = neg_one;
    frame.color = fog->color;
    frame.alpha = 2;
    frame.fog_result = *fog;
    frame.screen_flash = *screen_flash;
    frame.field_290 = window_bound_unk_data;
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
            render_scene_visibility_to_usercall(user_index);
            render_scene_wrapper(is_texture_camera);

            IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();
            IDirect3DSurface9* surface = NULL;
            IDirect3DSurface9* global_d3d_surface_screenshot = global_d3d_surface_screenshot_get();
            if (screenshot_in_progress())
            {
                surface = global_d3d_surface_screenshot;
                global_d3d_surface_screenshot->AddRef();
            }
            else
            {
                global_d3d_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &surface);
            }

            rasterizer_dx9_set_render_target(surface, NONE, 1);
            if (surface)
            {
                surface->Release();
                surface = NULL;
            }

            rasterizer_dx9_set_stencil_mode(0);
            draw_hud();
            rasterizer_dx9_render_screen_effect();

            if (screenshot_in_progress())
            {
                surface = global_d3d_surface_screenshot;
                global_d3d_surface_screenshot->AddRef();
            }
            else
            {
                global_d3d_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &surface);
            }

            rasterizer_dx9_set_render_target(surface, NONE, 1);
            if (surface)
            {
                surface->Release();
                surface = NULL;
            }
            render_menu_user_interface_to_usercall(0, controller_index, NONE, &camera->viewport_bounds);
            rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DBLEND_INVSRCCOLOR);
        }
    }

    rasterizer_update_cameras();
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
    screen_flash.field_0 = 0;
    bool bsp_test_success = structure_get_cluster_and_leaf_from_render_point(&window->render_camera.point, &cluster_index, &leaf_index);

    if (bsp_test_success)
    {
        *get_global_window_out_cluster_index(window->window_bound_index) = cluster_index;
        *get_global_window_out_leaf_index(window->window_bound_index) = leaf_index;
    }

    int32 visible_sky_index;
    real_rgb_color clear_color = *global_real_rgb_white;
    bool clear_color_active = false;
    bool sky_bool = structure_get_sky(cluster_index, &visible_sky_index, &clear_color, &clear_color_active);

    s_scenario_fog_result fog;
    render_scenario_fog(cluster_index, &window->render_camera, &window->render_camera.forward, sky_bool, *get_render_fog_enabled(), &fog);
    if (clear_color_active)
    {
        fog.color = clear_color;
    }

    if (fog.unk_bool_18 && window->render_camera.z_far > fog.z_far && fog.z_far > window->render_camera.z_near)
    {
        window->render_camera.z_far = fog.z_far;
    }

    real_rectangle2d frustum_bounds;
    render_camera_build_viewport_frustum_bounds(&window->render_camera, &frustum_bounds);
    
    int32 controller_index = NONE;
    if (window->user_index != NONE)
    {
        controller_index = s_player::get(player_index_from_user_index(window->user_index))->controller_index;
        render_screen_flash(window->user_index, &screen_flash);
    }

    ASSERT(!memcmp(&window->render_camera.viewport_bounds, &window->rasterizer_camera.viewport_bounds, sizeof(rectangle2d)));
    ASSERT(!memcmp(&window->render_camera.window_bounds, &window->rasterizer_camera.window_bounds, sizeof(rectangle2d)));

    *rasterizer_clear_screen_global_get() = false;
    if (window->render_camera.vertical_field_of_view > k_real_math_epsilon)
    {
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
            sky_bool,
            visible_sky_index,
            &fog,
            0,
            -1,
            window->gap_f4,
            0,
            &screen_flash);
    }
    else
    {
        error(2, "Tried to render a view with a field of view of %f", window->render_camera.vertical_field_of_view);
    }

    return;
}
