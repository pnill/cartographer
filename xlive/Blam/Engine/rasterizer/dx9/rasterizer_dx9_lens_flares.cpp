#include "stdafx.h"
#include "rasterizer_dx9_lens_flares.h"

#include "rasterizer_dx9_main.h"
#include "rasterizer_dx9_shader_submit.h"
#include "rasterizer_dx9_submit.h"
#include "rasterizer_dx9_targets.h"

#include "effects/lens_flares.h"
#include "math/color_math.h"
#include "rasterizer/rasterizer_globals.h"
#include "render/render.h"
#include "shell/shell_windows.h"

/* constants */
#define k_sun_quad_samples 16

/* globals */

D3DSURFACE_DESC g_sun_surface;

/* prototypes */

int32 __cdecl rasterizer_dx9_sun_glow_occlude(datum tag_index, real_point3d* point, int32 a3);
void __cdecl rasterizer_dx9_sun_glow_draw(datum tag_index, real_point3d* point, e_rasterizer_target rasterizer_target);

/* public code */

void rasterizer_dx9_lens_flares_apply_patches(void)
{
    PatchCall(Memory::GetAddress(0x26CF58), rasterizer_dx9_sun_glow_occlude);
    PatchCall(Memory::GetAddress(0x26CFA7), rasterizer_dx9_sun_glow_draw);
    return;
}

IDirect3DTexture9* rasterizer_dx9_sun_texture_get(void)
{
    return *Memory::GetAddress<IDirect3DTexture9**>(0xA3C678);
}

IDirect3DSurface9* rasterizer_dx9_sun_alpha_surface_get(void)
{
    return *Memory::GetAddress<IDirect3DSurface9**>(0xA3C684);
}


/* private code */

IDirect3DPixelShader9** lens_flare_pixel_shaders_get(void)
{
    return Memory::GetAddress<IDirect3DPixelShader9**>(0xA4B008);
}

// Is the sun outside of our viewport
bool rasterizer_dx9_sun_is_in_bounds(const real_rectangle2d* rect)
{
    return rect->x0 < 1.f && rect->y0 < 1.f && rect->x1 > -1.f && rect->y1 > -1.f;
}

int32 __cdecl rasterizer_dx9_sun_glow_occlude(datum tag_index, real_point3d* point, int32 a3)
{
    rasterizer_dx9_sun_texture_get()->GetLevelDesc(0, &g_sun_surface);
    
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
    real_rectangle2d rect;
    if (render_projection_point_to_screen(&sun_position, definition->occlusion_radius, &rect, &bounds))
    {        
        real32 x = rectangle2d_width(&global_window_parameters->camera.viewport_bounds);
        real32 y = rectangle2d_height(&global_window_parameters->camera.viewport_bounds);

        real_bounds window_bounds =
        {
            (real32)*global_window_bounds_x_get(),
            (real32)*global_window_bounds_y_get()
        };

        real32 x_stretch_factor = window_bounds.lower / x;
        real32 y_stretch_factor = window_bounds.upper / y;

        real32 x_result = g_sun_surface.Width / (x * 2.f * x_stretch_factor);
        real32 y_result = g_sun_surface.Height / (y * 2.f * y_stretch_factor);

        real_rectangle2d sun_occlusion_rect;
        sun_occlusion_rect.x0 = rect.x0 - x_result;
        sun_occlusion_rect.x1 = rect.x0 + x_result;
        sun_occlusion_rect.y0 = rect.x1 - y_result;
        sun_occlusion_rect.y1 = rect.x1 + y_result;


        bool in_bounds = rasterizer_dx9_sun_is_in_bounds(&sun_occlusion_rect);
        if (in_bounds)
        {
            rasterizer_dx9_set_shader_code(10);
            rasterizer_dx9_set_render_state(D3DRS_ZENABLE, D3DBLEND_ZERO);
            rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DBLEND_INVSRCCOLOR);
            rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, (D3DBLEND)0);
            rasterizer_dx9_set_stencil_mode(7);
            rasterizer_dx9_draw_rect(&sun_occlusion_rect, 1.f, global_yellow_pixel32);
        }
    }
    return a3;
}

void __cdecl rasterizer_dx9_sun_glow_draw(datum tag_index, real_point3d* point, e_rasterizer_target rasterizer_target)
{
    IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();
    ASSERT(global_d3d_device);

    const s_lens_flare_definition* definition = (s_lens_flare_definition*)tag_get_fast(tag_index);
    ASSERT(definition);

    const s_frame* global_window_parameters = global_window_parameters_get();

    real_vector3d direction;
    vector_from_points3d(point, &global_window_parameters->camera.point, &direction);
    normalize3d(&direction);
    real32 product = dot_product3d(&global_window_parameters->camera.forward, &direction);

    real32 product_magic = (product - 0.70710677f) / 0.29289323f;
    product_magic = PIN(product_magic, 0.f, 1.f);

    real32 length = global_window_parameters->camera.z_far / product;
    real_point3d position;
    point_from_line3d(&global_window_parameters->camera.point, &direction, length, &position);
   
    real_bounds bounds;
    real_rectangle2d rect;
    if (render_projection_point_to_screen(&position, definition->occlusion_radius, &rect, &bounds))
    {
        real_bounds window_bounds =
        {
            (real32)*global_window_bounds_x_get(),
            (real32)*global_window_bounds_y_get()
        };

        real32 x = rectangle2d_width(&global_window_parameters->camera.viewport_bounds);
        real32 y = rectangle2d_height(&global_window_parameters->camera.viewport_bounds);

        real32 x_stretch_factor = window_bounds.lower / x;
        real32 y_stretch_factor = window_bounds.upper / y;

        real32 x_result = g_sun_surface.Width / (x * 2.f * x_stretch_factor);
        real32 y_result = g_sun_surface.Height / (y * 2.f * y_stretch_factor);

        real_rectangle2d sun_draw_rect;
        sun_draw_rect.x0 = rect.x0 - x_result;    // Leftmost point on the screen
        sun_draw_rect.x1 = rect.x0 + x_result;    // Rightmost point on the screen
        sun_draw_rect.y0 = rect.x1 - y_result;    // Topmost point on the screen
        sun_draw_rect.y1 = rect.x1 + y_result;    // Bottommost point on the screen

        //LOG_INFO_GAME("x0: {}", vector.x0);
        //LOG_INFO_GAME("x1: {}", vector.x1);
        //LOG_INFO_GAME("nx0: {}", normalized_x);
        //LOG_INFO_GAME("nx1: {}", normalized_y);

        bool in_bounds = rasterizer_dx9_sun_is_in_bounds(&sun_draw_rect);
        if (in_bounds)
        {
            // clear_alpha

            IDirect3DPixelShader9** lens_flare_pixel_shaders = lens_flare_pixel_shaders_get();

            rasterizer_dx9_set_shader_code(10);

            x_result = x / 2;
            y_result = y / 2;

            int16 left = global_window_parameters->camera.viewport_bounds.left;
            int16 bottom = global_window_parameters->camera.viewport_bounds.bottom;

            RECT rect;
            rect.left = (x * sun_draw_rect.x0 * 0.5) + x_result + left;
            rect.top = bottom - ((y * sun_draw_rect.y1 * 0.5) + y_result);

            rect.right = (x * sun_draw_rect.x1 * 0.5) + x_result + left;
            rect.bottom = bottom - ((y * sun_draw_rect.y0 * 0.5) + y_result);
            
            IDirect3DSurface9* global_d3d_surface_render_primary = global_d3d_surface_render_primary_get();
            rasterizer_dx9_set_render_target(global_d3d_surface_render_primary, (int32)global_d3d_surface_render_primary_z_get(), true);
            rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DBLEND_ZERO);
            rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DBLEND_INVDESTALPHA);
            rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, (D3DBLEND)0);
            rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, (D3DBLEND)0);
            rasterizer_dx9_set_render_state(D3DRS_ZENABLE, (D3DBLEND)0);
            rasterizer_dx9_set_render_state(D3DRS_DEPTHBIAS, (D3DBLEND)0);
            global_d3d_device->SetPixelShader(lens_flare_pixel_shaders[1]);

            x_result = 10.f / x;
            y_result = 10.f / y;

            real_rectangle2d sun_alpha_rect;
            sun_alpha_rect.x0 = sun_draw_rect.x0 - x_result;
            sun_alpha_rect.x1 = sun_draw_rect.x1 + x_result;
            sun_alpha_rect.y0 = sun_draw_rect.y0 - y_result;
            sun_alpha_rect.y1 = sun_draw_rect.y1 + y_result;
            rasterizer_dx9_draw_rect(&sun_alpha_rect, 1.f, { 0 });


            // write_alpha
            rasterizer_dx9_set_texture_stage(0, rasterizer_globals_get_data()->glow.index, 0, 0.f);
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
            rasterizer_dx9_draw_rect(&sun_draw_rect, 1.f, global_yellow_pixel32);
            rasterizer_dx9_set_stencil_mode(0);
            global_d3d_device->StretchRect(global_d3d_surface_render_primary, &rect, rasterizer_dx9_sun_alpha_surface_get(), NULL, D3DTEXF_LINEAR);

            // copy_to_sun_glow_primary

            rasterizer_dx9_set_render_state(D3DRS_ZENABLE, (D3DBLEND)0);
            e_rasterizer_target target = sub_66C2CA(1.f, 1.f, 0.f, 15, 14, -1, -1, 1, 0, 1.f, 1.f);
            rasterizer_dx9_set_target(rasterizer_target, 0, 0);
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
            rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, (D3DBLEND)0);
            rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DBLEND_INVDESTALPHA);
            rasterizer_dx9_set_render_state(D3DRS_DEPTHBIAS, (D3DBLEND)0);
            global_d3d_device->SetPixelShader(lens_flare_pixel_shaders[4]);

            real32 alpha = 1.f - global_window_parameters->fog_result.sky_fog_alpha;
            alpha = PIN(alpha, 0.f, 1.f);

            real32 alpha_result = alpha * product_magic;
            
            for (uint32 sample_num = 0; sample_num < k_sun_quad_samples; sample_num++)
            {
                real32 calculation = sample_num * (1.f / (real32)k_sun_quad_samples);
                calculation *= 80.f;
                calculation -= 4.f;
                calculation /= 640.f;   // Resolution

                sun_alpha_rect.x0 = sun_draw_rect.x0 - calculation;
                sun_alpha_rect.x1 = sun_draw_rect.x1 + calculation;
                sun_alpha_rect.y0 = sun_draw_rect.y1 + calculation;
                sun_alpha_rect.y1 = sun_draw_rect.y0 - calculation;

                real32 result_div = alpha_result / (sample_num + 1);

                real_argb_color real_color;
                real_color.red = result_div * 0.733333f;
                real_color.green = result_div * 0.733333f;
                real_color.blue = result_div * 0.5f;
                real_color.alpha = 1.f;
                pixel32 color = real_argb_color_to_pixel32(&real_color);
                rasterizer_dx9_draw_rect(&sun_alpha_rect, 0.f, color);
            }
        }
    }

    return;
}