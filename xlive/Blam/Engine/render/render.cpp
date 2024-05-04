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
#include "rasterizer/rasterizer_vertex_cache.h"


/* prototypes */
bool __cdecl DrawPrimitiveUP_hook_get_vertex_decl(
    D3DPRIMITIVETYPE PrimitiveType,
    UINT PrimitiveCount,
    const void* pVertexStreamZeroData,
    UINT VertexStreamZeroStride);

void rasterizer_get_screen_frame_bounds(rectangle2d* rect);

void __cdecl rasterizer_fullscreen_apply_gamma(e_rasterizer_target target_surface);

// ### TOOD rename
void __cdecl sub_67D4EF(
    float* a1,
    unsigned __int8(__cdecl* a2)(int),
    bool(__cdecl* a3)(
        e_vertex_shader_input_type input_type,
        real32* bounds,
        real32* location,
        void* output,
        void* ctx),
    int(__cdecl* a4)(int),
    int a5,
    int16 a6,
    bool a7);

void render_apply_patches(void)
{
    PatchCall(Memory::GetAddress(0x19224A), render_window);
    PatchCall(Memory::GetAddress(0x19DA7C), render_window);

    // fix the missing motion sensor blip
    PatchCall(Memory::GetAddress(0x284BF3), rasterizer_set_render_target_internal_hook_set_main_render_surface);
    PatchCall(Memory::GetAddress(0x27DA9D), rasterizer_set_render_target_internal_hook_set_viewport);

    //PatchCall(Memory::GetAddress(0x2220CA), DrawPrimitiveUP_hook_get_vertex_decl);
    //PatchCall(Memory::GetAddress(0x27D746), DrawPrimitiveUP_hook_get_vertex_decl);

    PatchCall(Memory::GetAddress(0x27DAF7), sub_67D4EF);

    PatchCall(Memory::GetAddress(0x262684), rasterizer_fullscreen_apply_gamma);

    // ### FIXME re-enable text/user interface text
    // *Memory::GetAddress<bool*>(0x46818E) = false;

    return;
}

void __cdecl rasterizer_get_texture_surface_size(e_rasterizer_target target, int32* out_width, int32* out_height)
{
    return INVOKE(0x280413, 0x0, rasterizer_get_texture_surface_size, target, out_width, out_height);
}

void __cdecl rasterizer_fullscreen_apply_gamma(e_rasterizer_target target_surface)
{
	INVOKE(0x27D8E5, 0x0, rasterizer_fullscreen_apply_gamma, target_surface);
}

bool __cdecl rasterizer_fullscreen_effects_build_vertex_buffer_cb(
    e_vertex_shader_input_type input_type, 
    real32* bounds, 
    real32* location, 
    void* output, 
    void* ctx
)
{
    real32 bounds_width = bounds[1] - bounds[0];
    real32 bounds_height = bounds[3] - bounds[2];

    if (input_type == _vertex_shader_input_type_position)
    {
        real32* position_output = (real32*)output;

        // position normalized device coordinates
        // make sure to set the viewport, otherwise it'll represent the entire surface
        // which is not good during split-screen
        position_output[0] = location[0] * 2.0f - 1.0f; // x
        position_output[1] = -(location[1] * 2.0f - 1.0f); // y
        position_output[2] = 1.0f; // z
        position_output[3] = 1.0f; // scale

        return true;
    }
    else if (input_type == _vertex_shader_input_type_texcoord)
    {
        real32* texcoord_output = (real32*)output;

        // texcoords normalized device coordinates (origin in 0,0 bottom left, max 1,1 top right)
        // set the tex coords based on the texture set to be drawn on the screen
        int32 main_resolved_surface_size_x, main_resolved_surface_size_y;
        // get the size of the surface
        rasterizer_get_texture_surface_size(_rasterizer_target_37, &main_resolved_surface_size_x, &main_resolved_surface_size_y);

        // explanation: locations are per vertex, this draw call will take 4 vertices
        // this function will get called 4 times, for each single vertex
        // the location of the first vertex is at 0, 0. and according to texcoords normalized device coordinates, the origin is in bottom left corner of the texture surface
        // then it goes counter-clockwise until it forms a rectangle, so 0,0 -> 1,0 (right down) -> 1,1 (right top) -> 0,1 (left top) -> (end) 0,0 (left bottom)
        // if the location is 1, for each component we scale it with the bounds width/lenght and offset that with the verticies located on the left
        // if it is 0, then the vertex is either at the left of the screen, or at the bottom, depending on which component is 0
        texcoord_output[0] = (0.5f / bounds_width) + (location[0] * bounds_width) / main_resolved_surface_size_x + bounds[0] / main_resolved_surface_size_x;
        texcoord_output[1] = (0.5f / bounds_height) + (location[1] * bounds_height) / main_resolved_surface_size_y + bounds[2] / main_resolved_surface_size_y;

		texcoord_output[2] = 0.0f;
		texcoord_output[3] = 1.0f;

        return true;
    }
    else if (input_type == _vertex_shader_input_type_color)
    {
        D3DCOLOR* color = (D3DCOLOR*)output;
        *color = global_white_pixel32.color;
        return true;
    }
    else
    {
        return false;
    }
}

void __cdecl sub_67D4EF(
    float* a1,
    unsigned __int8(__cdecl* a2)(int),
    bool(__cdecl* a3)(
        e_vertex_shader_input_type input_type,
        real32* bounds,
        real32* location,
        void* output,
        void* ctx),
    int(__cdecl* a4)(int),
    int a5,
    int16 a6,
    bool a7)
{
    return INVOKE(0x27D4EF, 0x0, sub_67D4EF, a1, a2, rasterizer_fullscreen_effects_build_vertex_buffer_cb, a4, a5, a6, a7);
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

	real32 vc[20];
	int16 width, height;

	s_camera* global_camera = get_global_camera();

	rectangle2d screen_bounds = global_camera->viewport_bounds;
	width = rectangle2d_width(&screen_bounds);
	height = rectangle2d_height(&screen_bounds);

	// vertex shaders use normalized device coordinates system (NDC)
	vc[0 + 0] = 2.0f / (real32)width; // x
	vc[0 + 1] = 0.0f;
	vc[0 + 2] = 0.0f;
	vc[0 + 3] = -(1.0f / (real32)width + 1.0f) - ((real32)global_camera->viewport_bounds.left * 2.0f / width); // offset from x

	vc[4 + 0] = 0.0f;
	vc[4 + 1] = -(2.0f / (real32)height); // y
	vc[4 + 2] = 0.0f;
	vc[4 + 3] = (1.0f / (real32)height + 1.0f) + ((real32)global_camera->viewport_bounds.top * 2.0f / height); // offset from y

	vc[8 + 0] = 0.0f;
	vc[8 + 1] = 0.0f;
	vc[8 + 2] = 0.0f; // z
	vc[8 + 3] = 0.5f; // acts as an offset, facing (<=1.0f is towards the viewport, above 1.0f facing from the viewport)

	vc[12 + 0] = 0.0f;
	vc[12 + 1] = 0.0f;
	vc[12 + 2] = 0.0f;
	vc[12 + 3] = 1.0f; // w scaling component (1.0f * (width * height / (offset_x * offset_y)))

	// the c181 register seems unused?
	vc[16 + 0] = 0.0f;
	vc[16 + 1] = 0.0f;
	vc[16 + 2] = 0.0f;
	vc[16 + 3] = 0.0f;

	// avoid unnecessary API calls by testing the user mode memory cache
	if (rasterizer_get_main_vertex_shader_cache()->test_cache(177, vc, 5))
	{
		global_d3d_device->SetVertexShaderConstantF(177, (float*)vc, 5);
	}
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
            render_view_visibility_compute_to_usercall(user_index);
            rasterizer_render_scene(is_texture_camera);

            rasterizer_dx9_set_stencil_mode(0);
            rasterizer_setup_2d_vertex_shader_user_interface_constants();
            draw_hud();
            rasterizer_dx9_render_screen_effect();

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
            window->gap_f4,
            0,
            &screen_flash);
    }
    else
    {
        error(2, "Tried to render a view with a field of view of %f", window->render_camera.vertical_field_of_view);
    }
    *rasterizer_target_back_buffer() = true;

    return;
}
