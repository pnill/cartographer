#include "stdafx.h"
#include "rasterizer_dx9_lens_flares.h"

#include "rasterizer_dx9.h"
#include "rasterizer_dx9_dof.h"
#include "rasterizer_dx9_fullscreen_passes.h"
#include "rasterizer_dx9_main.h"
#include "rasterizer_dx9_shader_submit.h"
#include "rasterizer_dx9_submit.h"

#include "effects/lens_flares.h"
#include "math/color_math.h"
#include "rasterizer/dx9/shaders/compiled/sun_glow_convolve.h"
#include "rasterizer/rasterizer_globals.h"
#include "render/render.h"
#include "shell/shell_windows.h"

/* constants */
#define k_sun_quad_pass_count 16

/* globals */

IDirect3DPixelShader9* sun_glow_convolve_shader = NULL;

/* prototypes */

IDirect3DPixelShader9** lens_flare_pixel_shaders_get(void);
void __cdecl rasterizer_dx9_lens_flares_create_pixel_shaders(void);
bool rasterizer_dx9_sun_is_in_bounds(const real_rectangle2d* rect);
e_rasterizer_target __cdecl rasterizer_dx9_sun_glow_occlude(datum tag_index, real_point3d* point, e_rasterizer_target rasterizer_target);
void rasterizer_dx9_sun_glow_copy_source(const RECT* rect, e_rasterizer_target target);
e_rasterizer_target rasterizer_dx9_sun_glow_convolve(e_rasterizer_target primary_target, e_rasterizer_target secondary_target, int16 pass_count);

/* public code */

void rasterizer_dx9_lens_flares_apply_patches(void)
{
    PatchCall(Memory::GetAddress(0x2729B8), rasterizer_dx9_lens_flares_create_pixel_shaders);

    PatchCall(Memory::GetAddress(0x26CF58), rasterizer_dx9_sun_glow_occlude);
    return;
}

e_rasterizer_target rasterizer_dx9_sun_glow_draw(datum tag_index, real_point3d* point, e_rasterizer_target rasterizer_target)
{
    IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();
    ASSERT(global_d3d_device);

    const s_lens_flare_definition* definition = (s_lens_flare_definition*)tag_get_fast(tag_index);
    ASSERT(definition);

    const s_frame* global_window_parameters = global_window_parameters_get();

    real_vector3d direction;
    vector_from_points3d(point, &global_window_parameters->camera.point, &direction);
    normalize3d(&direction);
    real32 length = dot_product3d(&global_window_parameters->camera.forward, &direction);

    real32 product_magic = (length - 0.70710677f) / 0.29289323f;
    product_magic = PIN(product_magic, 0.f, 1.f);

    length = 1024.f / length;

    // ### TODO: fix global_window_parameters->camera.z_far being incorrectly set to 1.f from first person code
    //real32 length = global_window_parameters->camera.z_far / length;

    real_point3d position;
    point_from_line3d(&global_window_parameters->camera.point, &direction, length, &position);

    real_bounds bounds;
    real_vector4d sun_center;
    if (render_projection_point_to_screen(&position, definition->occlusion_radius, &sun_center, &bounds))
    {
        const int16 viewport_width = rectangle2d_width(&global_window_parameters->camera.viewport_bounds);
        const int16 viewport_height = rectangle2d_height(&global_window_parameters->camera.viewport_bounds);
        const int16 viewport_left = global_window_parameters->camera.viewport_bounds.left;
        const int16 viewport_top = global_window_parameters->camera.viewport_bounds.top;
        const int16 viewport_bottom = global_window_parameters->camera.viewport_bounds.bottom;

        // holds the position of the center of the sun
        real_rectangle2d sun_surface_quad;
        sun_surface_quad.x0 = sun_center.i - bounds.lower;    // Leftmost point on the screen
        sun_surface_quad.x1 = sun_center.i + bounds.lower;    // Rightmost point on the screen
        sun_surface_quad.y0 = sun_center.j - bounds.upper;    // Topmost point on the screen
        sun_surface_quad.y1 = sun_center.j + bounds.upper;    // Bottommost point on the screen

        bool in_bounds = rasterizer_dx9_sun_is_in_bounds(&sun_surface_quad);
        if (in_bounds)
        {
            // clear_alpha

            IDirect3DPixelShader9** lens_flare_pixel_shaders = lens_flare_pixel_shaders_get();

            rasterizer_dx9_set_vertex_shader_permutation(10);

            int16 viewport_middle_x = viewport_width / 2;
            int16 viewport_middle_y = viewport_height / 2;

            RECT rect;
            rect.left = viewport_left + (viewport_width * sun_surface_quad.x0 / 2.f + viewport_middle_x);
            rect.top = viewport_bottom - (viewport_height * sun_surface_quad.y1 / 2.f + viewport_middle_y);
            rect.right = viewport_left + (viewport_width * sun_surface_quad.x1 / 2.f + viewport_middle_x);
            rect.bottom = viewport_bottom - (viewport_height * sun_surface_quad.y0 / 2.f + viewport_middle_y);

            // Make sure we limit the rect to within the bounds of the viewport
            // Without this the StretchRect call to copy to the sun target will fail
            rect.left = PIN(rect.left, viewport_left, viewport_width + viewport_left);
            rect.right = PIN(rect.right, viewport_left, viewport_width + viewport_left);
            rect.top = PIN(rect.top, viewport_top, viewport_bottom + viewport_top);
            rect.bottom = PIN(rect.bottom, viewport_top, viewport_bottom + viewport_top);

            // works fairly the same as the motion sensor
            // it draws the mask on a surface that can have the image altered
            // in this case the main render target (backbuffer or the intermediary buffer)
            // then it copies the image from the surface including the mask in the sun's alpha calc surface
            // to produce the actual image with alpha 
            s_rasterizer_globals* rasterizer_globals = rasterizer_globals_get();

            // draw the alpha clear on the primary render surface
            rasterizer_globals->rasterizer_draw_on_main_back_buffer = false;
            rasterizer_dx9_set_target(*rasterizer_dx9_main_render_target_get(), 0, true);
            rasterizer_globals->rasterizer_draw_on_main_back_buffer = true;

            rasterizer_dx9_perf_event_begin("clear_alpha", NULL);

            rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DBLEND_ZERO);
            rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DBLEND_INVDESTALPHA);
            rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, (DWORD)0);
            rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, (DWORD)0);
            rasterizer_dx9_set_render_state(D3DRS_ZENABLE, (DWORD)0);
            rasterizer_dx9_set_render_state(D3DRS_DEPTHBIAS, (DWORD)0);
            global_d3d_device->SetPixelShader(lens_flare_pixel_shaders[1]);

            viewport_middle_x = 10.f / viewport_width;
            viewport_middle_y = 10.f / viewport_height;

            real_rectangle2d sun_alpha_rect;
            sun_alpha_rect.x0 = sun_surface_quad.x0 - viewport_middle_x;
            sun_alpha_rect.x1 = sun_surface_quad.x1 + viewport_middle_x;
            sun_alpha_rect.y0 = sun_surface_quad.y0 - viewport_middle_y;
            sun_alpha_rect.y1 = sun_surface_quad.y1 + viewport_middle_y;
            // real32 depth_range = 0.06f / 1024.f;

            rasterizer_dx9_draw_rect(&sun_alpha_rect, 1.f, { 0 });
            rasterizer_dx9_perf_event_end("clear_alpha");

            rasterizer_dx9_perf_event_begin("write_alpha", NULL);

            rasterizer_dx9_set_texture_direct(0, rasterizer_globals_get_data()->glow.index, 0, 0.f);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSU, 3);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSV, 3);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAGFILTER, 2);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MINFILTER, 2);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPFILTER, 2);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAXANISOTROPY, 1);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPMAPLODBIAS, 0);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAXMIPLEVEL, 0);
            rasterizer_dx9_set_render_state(D3DRS_ZENABLE, D3DBLEND_ZERO);
            rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DBLEND_INVSRCCOLOR);

            rasterizer_dx9_set_stencil_mode(8);
            global_d3d_device->SetPixelShader(lens_flare_pixel_shaders[2]);
            rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DBLEND_INVBLENDFACTOR);
            rasterizer_dx9_draw_rect(&sun_surface_quad, 1.f, global_yellow_pixel32);

            rasterizer_dx9_perf_event_end("write_alpha");

            rasterizer_dx9_set_stencil_mode(0);

            s_rasterizer_dx9_main_globals* dx9_globals = rasterizer_dx9_main_globals_get();

            // copy the surface drawn with the mask on it, also by specifying the size to be copied from the src surface
            // this definitely needs a helper function to pass just the rasterizer_target surfaces from the enum
            // and posibly the size to be copied with the rects
            rasterizer_dx9_perf_event_begin("copy_to_sun_glow_primary", NULL);
            global_d3d_device->StretchRect(dx9_globals->global_d3d_surface_render_primary, &rect, dx9_globals->global_d3d_surface_sun_glow_primary, NULL, D3DTEXF_LINEAR);

            // These are currently broken...
            // FIXME
            // Ideally we'd use this instead of the stretch rect but we do not have access to the texture of the render target
            //rasterizer_dx9_sun_glow_copy_source(&rect, _rasterizer_target_sun_glow_primary);
            //rasterizer_dx9_sun_glow_copy_source(&rect, _rasterizer_target_sun_glow_secondary);

            rasterizer_dx9_set_render_state(D3DRS_ZENABLE, FALSE);

            rasterizer_dx9_perf_event_end("copy_to_sun_glow_primary");

            rasterizer_dx9_perf_event_begin("sun_glow_convolve", NULL);
            e_rasterizer_target target = rasterizer_dx9_sun_glow_convolve(_rasterizer_target_sun_glow_primary, _rasterizer_target_sun_glow_secondary, 4);
            rasterizer_dx9_perf_event_end("sun_glow_convolve");

            // render the sun on the actual output surface
            // that is later copied to the back buffer (rasterizer_target should be the resolved or 37th surface)
            rasterizer_dx9_perf_event_begin("sun_glow_halo", NULL);

            rasterizer_dx9_set_target(rasterizer_target, 0, false);
            rasterizer_dx9_set_target_as_texture(0, target);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSU, 3u);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSV, 3u);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAGFILTER, 2u);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MINFILTER, 2u);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPFILTER, 2u);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAXANISOTROPY, 1u);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPMAPLODBIAS, 0);
            rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAXMIPLEVEL, 0);
            rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DBLEND_ZERO);
            rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DBLEND_DESTALPHA);
            rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, D3DBLEND_ZERO);
            rasterizer_dx9_set_render_state(D3DRS_SRCBLEND, D3DBLEND_ONE);
            rasterizer_dx9_set_render_state(D3DRS_DESTBLEND, D3DBLEND_ONE);
            rasterizer_dx9_set_render_state(D3DRS_BLENDOP, D3DBLEND_ZERO);
            rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, FALSE);
            rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DBLEND_INVDESTALPHA);
            rasterizer_dx9_set_render_state(D3DRS_DEPTHBIAS, FALSE);
            global_d3d_device->SetPixelShader(lens_flare_pixel_shaders[4]);

            real32 alpha = 1.f - global_window_parameters->fog_result.sky_fog_alpha;
            alpha = PIN(alpha, 0.f, 1.f);

            real32 brightness = alpha * product_magic;

            for (uint32 pass = 0; pass < k_sun_quad_pass_count; pass++)
            {
                real32 square_half_size = pass / (real32)k_sun_quad_pass_count;
                square_half_size *= 80.f;   // max square size, 16 passes
                square_half_size -= 4.f;    // minor offset
                square_half_size /= 640.f;  // baseline resolution

                real_rectangle2d sun_quad;
                // create the required quad from the sun surface quad
                sun_quad.x0 = sun_surface_quad.v[0] - square_half_size; // go left square_half_size
                sun_quad.x1 = sun_surface_quad.v[1] + square_half_size; // go right square_half_size
                sun_quad.y0 = sun_surface_quad.v[3] + square_half_size; // go up square_half_size
                sun_quad.y1 = sun_surface_quad.v[2] - square_half_size; // go down square_half_size

                real32 result_div = brightness / (real32)(pass + 1);

                real_argb_color real_color;
                real_color.red = result_div * 0.733333f;
                real_color.green = result_div * 0.733333f;
                real_color.blue = result_div * 0.5f;
                real_color.alpha = 1.f;

                // the mask prevents this from becoming a quad
                pixel32 color = real_argb_color_to_pixel32(&real_color);
                rasterizer_dx9_draw_rect(&sun_quad, 0.f, color);
            }
            rasterizer_dx9_perf_event_end("sun_glow_halo");
        }
    }

    return rasterizer_target;
}

/* private code */

IDirect3DPixelShader9** lens_flare_pixel_shaders_get(void)
{
    return Memory::GetAddress<IDirect3DPixelShader9**>(0xA4B008);
}

void __cdecl rasterizer_dx9_lens_flares_create_pixel_shaders(void)
{
    INVOKE(0x26CEC9, 0x0, rasterizer_dx9_lens_flares_create_pixel_shaders);

    rasterizer_dx9_device_get_interface()->CreatePixelShader((const DWORD*)sun_glow_convolve_pixel_shader_bytecode, &sun_glow_convolve_shader);
    return;
}

// Is the sun outside of our viewport
// TODO: change the bounds depending on viewport bounds on the screen
bool rasterizer_dx9_sun_is_in_bounds(const real_rectangle2d* sun_center)
{
    return sun_center->x0 < 1.f && sun_center->y0 < 1.f && sun_center->x1 > -1.f && sun_center->y1 > -1.f;
}

e_rasterizer_target __cdecl rasterizer_dx9_sun_glow_occlude(datum tag_index, real_point3d* point, e_rasterizer_target rasterizer_target)
{    
    const s_lens_flare_definition* definition = (s_lens_flare_definition*)tag_get_fast(tag_index);
    ASSERT(definition);

    s_frame* global_window_parameters = global_window_parameters_get();

    real_vector3d direction;
    vector_from_points3d(point, &global_window_parameters->camera.point, &direction);
    normalize3d(&direction);
    real32 length = dot_product3d(&global_window_parameters->camera.forward, &direction);
    
    length = global_window_parameters->camera.z_far / length;

    real_point3d sun_position;
    point_from_line3d(&global_window_parameters->camera.point, &direction, length, &sun_position);
    
    real_bounds bounds;
    real_vector4d center;
    if (render_projection_point_to_screen(&sun_position, definition->occlusion_radius, &center, &bounds))
    {        
        real_rectangle2d sun_occlusion_rect;
        sun_occlusion_rect.x0 = center.i - bounds.lower;
        sun_occlusion_rect.x1 = center.i + bounds.lower;
        sun_occlusion_rect.y0 = center.j - bounds.upper;
        sun_occlusion_rect.y1 = center.j + bounds.upper;

        bool in_bounds = rasterizer_dx9_sun_is_in_bounds(&sun_occlusion_rect);
        if (in_bounds)
        {
            //real32 depth_range = global_window_parameters->camera.z_near / global_window_parameters->camera.z_far;

            rasterizer_dx9_set_target(rasterizer_target, 0, true);
            rasterizer_dx9_set_vertex_shader_permutation(10);
            rasterizer_dx9_set_render_state(D3DRS_ZENABLE, D3DBLEND_ZERO);
            rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DBLEND_INVSRCCOLOR);
            rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, (DWORD)0);
            rasterizer_dx9_set_stencil_mode(7);
            rasterizer_dx9_draw_rect(&sun_occlusion_rect, 1.0f, global_yellow_pixel32);
        }
    }
    return rasterizer_target;
}

void rasterizer_dx9_sun_glow_copy_source(const RECT* rect, e_rasterizer_target target)
{
    if (target)
    {
        //IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();

        rasterizer_dx9_set_target(target, 0, false);
        // ### FIXME this requires the texture of render target primary
        // which cannot be easily obatined using DirectX 9
        // since render_primary resource is created using CreateRenderTarget, 
        // which internally is not a texture
        // but this code expects the surface that has the alpha mask drawn on the surface/texture
        // to pass it to the shader and compute the rest of the image
        // render_primary is used to draw that mask on it, but as aforementioned, it cannot be used as a texture...
        //rasterizer_dx9_set_target_as_texture_internal(0, _rasterizer_target_render_primary, );

        rasterizer_dx9_set_target_as_texture(0, _rasterizer_target_render_primary);

        D3DBLEND render_state = rasterizer_dx9_get_render_state(D3DRS_COLORWRITEENABLE);
        rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DBLEND_INVBLENDFACTOR);
        rasterizer_dx9_set_screen_effect_pixel_shader(1);
        rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, FALSE);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

        real_rectangle2d* p_rect = NULL;
        real_rectangle2d rectangle;
        if (rect)
        {
            rectangle.x0 = rect->left;
            rectangle.x1 = rect->right;
            rectangle.y0 = rect->top;
            rectangle.y1 = rect->bottom;
            p_rect = &rectangle;
        }

        rasterizer_dx9_render_fullscreen_overlay_geometry(p_rect, 0, rasterizer_fullscreen_effects_build_vertex_buffer_cb, 0, 0, 1, true);
        rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, render_state);
        rasterizer_dx9_set_texture(0, NULL);
        rasterizer_dx9_set_target(_rasterizer_target_render_primary, 0, true);
    }
    return;
}

e_rasterizer_target rasterizer_dx9_sun_glow_convolve(e_rasterizer_target primary_target, e_rasterizer_target secondary_target, int16 pass_count)
{
    ASSERT(VALID_INDEX(primary_target, k_rasterizer_target_count));
    ASSERT(VALID_INDEX(secondary_target, k_rasterizer_target_count));

    s_rasterizer_dx9_main_globals* dx9_globals = rasterizer_dx9_main_globals_get();

    IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();

    if (pass_count > 0)
    {
        rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DBLEND_ZERO);
        rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DBLEND_DESTALPHA);
        rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, D3DBLEND_ZERO);
        rasterizer_dx9_set_render_state(D3DRS_SRCBLEND, D3DBLEND_DESTALPHA);
        rasterizer_dx9_set_render_state(D3DRS_DESTBLEND, D3DBLEND_ZERO);
        rasterizer_dx9_set_render_state(D3DRS_BLENDOP, D3DBLEND_ZERO);
        rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, 0);
        rasterizer_dx9_set_render_state(D3DRS_ZENABLE, 0);
        rasterizer_dx9_set_render_state(D3DRS_DEPTHBIAS, 0);
        rasterizer_dx9_set_vertex_shader_permutation(1);
        rasterizer_dx9_submit_resolve();

        const real32 width_ratio = 1.f / (real32)dx9_globals->global_d3d_sun_width;
        const real32 top = width_ratio * 0.5f;
        const real32 bottom = width_ratio * -0.5f;

        real_vector4d geometry[8];
        geometry[0] = { 1.f, 0.f, 0.f, bottom };
        geometry[1] = { 0.f, 1.f, 0.f, bottom };
        geometry[2] = { 1.f, 0.f, 0.f, top };
        geometry[3] = { 0.f, 1.f, 0.f, top };
        geometry[4] = { 1.f, 0.f, 0.f, bottom };
        geometry[5] = { 0.f, 1.f, 0.f, top };
        geometry[6] = { 1.f, 0.f, 0.f, top };
        geometry[7] = { 0.f, 1.f, 0.f, bottom };

        if (rasterizer_get_main_vertex_shader_cache()->test_cache(18, geometry, NUMBEROF(geometry)))
        {
            global_d3d_device->SetVertexShaderConstantF(18, (real32*)geometry, NUMBEROF(geometry));
        }

        global_d3d_device->SetPixelShader(sun_glow_convolve_shader);

        for (uint8 pass_index = 0; pass_index < pass_count; pass_index++)
        {
            // Alternate between the targets at the different passes
            // We originally use the colour of the secondary target and store the convolution result in the first target
            // After doing this we use the first target as the source and second as the destination.
            // Keep alternating for the pass count specified in the parameters

            const bool alternate_targets = pass_index % 2;
            e_rasterizer_target target = (alternate_targets ? secondary_target : primary_target);

            for (uint8 texture_stage = 0; texture_stage < 4; texture_stage++)
            {
                rasterizer_dx9_set_target_as_texture(texture_stage, target);
                rasterizer_dx9_set_sampler_state(texture_stage, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
                rasterizer_dx9_set_sampler_state(texture_stage, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
                rasterizer_dx9_set_sampler_state(texture_stage, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
                rasterizer_dx9_set_sampler_state(texture_stage, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
                rasterizer_dx9_set_sampler_state(texture_stage, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
                rasterizer_dx9_set_sampler_state(texture_stage, D3DSAMP_MAXANISOTROPY, 1);
                rasterizer_dx9_set_sampler_state(texture_stage, D3DSAMP_MIPMAPLODBIAS, 0);
                rasterizer_dx9_set_sampler_state(texture_stage, D3DSAMP_MAXMIPLEVEL, 0);
            }

            target = (alternate_targets ? primary_target : secondary_target);
            rasterizer_dx9_set_target(target, 0, false);
            uint8 alpha = (pass_index > 0 ? 127 : 255);
            rasterizer_dx9_draw_overlay_rect(0, 0, D3DCOLOR_ARGB(alpha, 0, 0, 0));
        }
    }

    // Make sure we pass back the final sun target that we've performed the convolution on
    e_rasterizer_target final_sun_surface = (pass_count % 2 ? secondary_target : primary_target);
    return final_sun_surface;
}

