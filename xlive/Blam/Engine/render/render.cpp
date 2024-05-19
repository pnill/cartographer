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
#include "rasterizer/dx9/rasterizer_dx9.h"
#include "rasterizer/dx9/rasterizer_dx9_main.h"
#include "rasterizer/dx9/rasterizer_dx9_screen_effect.h"
#include "rasterizer/dx9/rasterizer_dx9_shader_submit_new.h"
#include "rasterizer/rasterizer_settings.h"
#include "scenario/scenario_fog.h"
#include "structures/structures.h"

/* prototypes */

int32* get_global_window_bound_index(void);
int32* get_global_window_out_cluster_index(int32 index);
int32* get_global_window_out_leaf_index(int32 index);
uint32* global_frame_num_get(void);
int32* curent_window_bound_index_get(void);
int32* global_cluster_index_get(void);
int32* global_user_render_index_get(void);
int32* global_leaf_index_get(void);
bool* global_bsp_test_failed_get(void);
bool* global_sky_active_get(void);
int32* global_sky_index_get(void);
s_scenario_fog_result* global_fog_result_get(void);
bool* global_byte_4E6938_get(void);
void __cdecl rasterizer_render_scene(bool is_texture_camera);
bool rasterizer_dx9_get_vertex_declaration_format(D3DVERTEXELEMENT9* vertex_elements, UINT* vertex_element_count);

bool __cdecl DrawPrimitiveUP_hook_get_vertex_decl(
    D3DPRIMITIVETYPE PrimitiveType,
    UINT PrimitiveCount,
    const void* pVertexStreamZeroData,
    UINT VertexStreamZeroStride);

void rasterizer_get_screen_frame_bounds(rectangle2d* rect);

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

/* public code */

void render_apply_patches(void)
{
    PatchCall(Memory::GetAddress(0x19224A), render_window);
    PatchCall(Memory::GetAddress(0x19DA7C), render_window);

    // fix the missing motion sensor blip
    PatchCall(Memory::GetAddress(0x284BF3), rasterizer_set_render_target_internal_hook_set_main_render_surface);

    //PatchCall(Memory::GetAddress(0x2220CA), DrawPrimitiveUP_hook_get_vertex_decl);
    //PatchCall(Memory::GetAddress(0x27D746), DrawPrimitiveUP_hook_get_vertex_decl);

    // ### FIXME re-enable text/user interface text
    // *Memory::GetAddress<bool*>(0x46818E) = false;

    return;
}

s_frame* global_window_parameters_get(void)
{
    return Memory::GetAddress<s_frame*>(0xA3DF70);
}

s_frame_parameters* global_frame_parameters_get(void)
{
    return Memory::GetAddress<s_frame_parameters*>(0xA3E208);
}

real64 get_current_render_time(void)
{
    return *Memory::GetAddress<real64*>(0x4E6968);
}

bool* hs_texture_camera_view_get(void)
{
    return Memory::GetAddress<bool*>(0x4F435C);
}

s_scenario_fog_result* global_fog_result_get(void)
{
    return Memory::GetAddress<s_scenario_fog_result*>(0x4E6818);
}


uint32* global_effect_flag_get(void)
{
    return Memory::GetAddress<uint32*>(0xA3DA34);
}

bool __cdecl structure_get_cluster_and_leaf_from_render_point(real_point3d* point, int32* out_cluster_index, int32* out_leaf_index)
{
    return INVOKE(0x191032, 0x0, structure_get_cluster_and_leaf_from_render_point, point, out_cluster_index, out_leaf_index);
}

bool frame_parameters_type_is_above_or_equal_to_7(void)
{
    return global_frame_parameters_get()->frame_type - 5 <= 2;
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

    *rasterizer_target_back_buffer() = false;
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

/* private code */

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

bool* global_byte_4E6938_get(void)
{
    return Memory::GetAddress<bool*>(0x4E6938);
}

void __cdecl rasterizer_render_scene(bool is_texture_camera) 
{
    INVOKE(0x25F015, 0x0, rasterizer_render_scene, is_texture_camera);
    return;
}

//struct s_vs_shader_decl_draw_screen
//{
//    D3DXVECTOR4 pos;
//    D3DXVECTOR3 texCoord;
//    D3DCOLOR color;
//};
//
//struct s_vs_shader_decl_36
//{
//    D3DXVECTOR4 position;
//    D3DXVECTOR2 texcoord;
//    D3DCOLOR color;
//};


bool rasterizer_dx9_get_vertex_declaration_format(D3DVERTEXELEMENT9* vertex_elements, UINT* vertex_element_count)
{
    IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();

    IDirect3DVertexDeclaration9* vertex_dcl;
    global_d3d_device->GetVertexDeclaration(&vertex_dcl);
    bool res = !FAILED(vertex_dcl->GetDeclaration(vertex_elements, vertex_element_count));

    vertex_dcl->Release();

    return res;
}

bool __cdecl DrawPrimitiveUP_hook_get_vertex_decl(
    D3DPRIMITIVETYPE PrimitiveType,
    UINT PrimitiveCount,
    const void* pVertexStreamZeroData,
    UINT VertexStreamZeroStride)
{
    IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();

    D3DVERTEXELEMENT9 vertex_elements[MAXD3DDECLLENGTH];
    UINT vertex_element_couunt;

    if (rasterizer_dx9_get_vertex_declaration_format(vertex_elements, &vertex_element_couunt))
    {
    }

    return !FAILED(global_d3d_device->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride));
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
	vc[0].w = -(1.0f / (real32)width + 1.0f) - ((real32)global_camera->viewport_bounds.left * 2.0f / width); // offset from x

	vc[1].i = 0.0f;
	vc[1].j = -(2.0f / (real32)height); // y
	vc[1].k = 0.0f;
	vc[1].w = (1.0f / (real32)height + 1.0f) + ((real32)global_camera->viewport_bounds.top * 2.0f / height); // offset from y

	vc[2].i = 0.0f;
	vc[2].j = 0.0f;
	vc[2].k = 0.0f; // z
	vc[2].w = 0.5f; // acts as an offset, facing (<=1.0f is towards the viewport, above 1.0f facing from the viewport)

	vc[3].i = 0.0f;
	vc[3].j = 0.0f;
	vc[3].k = 0.0f;
	vc[3].w = 1.0f; // w scaling component

	// the c181 register seems unused?
	vc[4].i = 0.0f;
	vc[4].j = 0.0f;
	vc[4].k = 0.0f;
	vc[4].w = 0.0f;

	// avoid unnecessary API calls by testing the user mode memory cache
	if (rasterizer_get_main_vertex_shader_cache()->test_cache(177, vc, NUMBEROF(vc)))
	{
		global_d3d_device->SetVertexShaderConstantF(177, (real32*)vc, NUMBEROF(vc));
	}
    return;
}

void rasterizer_get_screen_frame_bounds(rectangle2d* rect)
{
    *rect = *Memory::GetAddress<rectangle2d*>(0xA3E418);
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

    ++*global_frame_num_get();

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
    frame.window_bound_index = window_bound_index;
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
            rasterizer_render_scene(is_texture_camera);

            rasterizer_dx9_perf_event_begin("interface", 0);
            rasterizer_dx9_set_stencil_mode(0);
            rasterizer_setup_2d_vertex_shader_user_interface_constants();
            draw_hud();
            rasterizer_dx9_render_screen_flash();
            render_menu_user_interface_to_usercall(0, controller_index, NONE, &camera->viewport_bounds);
            rasterizer_dx9_perf_event_end();

#ifndef NDEBUG
            rasterizer_dx9_perf_event_begin("debug", 0);
            rasterizer_dx9_perf_event_end();
#endif

            rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DBLEND_INVSRCCOLOR);
        }
    }

    rasterizer_update_cameras();
    return;
}
