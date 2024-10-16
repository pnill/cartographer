#include "stdafx.h"
#include "rasterizer_dx9_dof.h"

#include "rasterizer_dx9.h"
#include "rasterizer_dx9_main.h"
#include "rasterizer_dx9_submit.h"
#include "rasterizer_dx9_shader_submit.h"

/* structures */

struct s_convolution_d3d_vertex
{
    real32 pos_x;
    real32 pos_y;
    real32 pos_z;
    real32 bounds_x;
    real32 bounds_y;
    D3DCOLOR color;
};

struct s_rasterizer_dx9_dof_overlay_data
{
    real_vector4d constants[4];
    e_rasterizer_target target;
    int16 pad;
};

/* prototypes */

IDirect3DPixelShader9** get_local_pixel_shaders(void);

e_rasterizer_target __cdecl sub_87E570(real32 unused1, real32 a1, real32 unused2, e_rasterizer_target a2, e_rasterizer_target a3, bool a4);

bool __cdecl rasterizer_dx9_depth_of_field_pipeline_setup(s_rasterizer_dx9_dof_overlay_data* dof_data);

/* public code */

void rasterizer_dx9_dof_apply_patches(void)
{
    // Fix DOF rendering when using SM3
    WriteValue(Memory::GetAddress(0x26CCA2 + 1), rasterizer_dx9_depth_of_field_pipeline_setup);
    return;
}

s_rasterizer_bloom_globals** rasterizer_bloom_globals_get(void)
{
    return Memory::GetAddress<s_rasterizer_bloom_globals**>(0xA3C63C);
}

void __cdecl rasterizer_dx9_create_blur_pixel_shaders(void)
{
    INVOKE(0x26BB08, 0x0, rasterizer_dx9_create_blur_pixel_shaders);
    return;
}

void __cdecl rasterizer_bloom_globals_initialize(void)
{
    INVOKE(0x26BBDF, 0x0, rasterizer_bloom_globals_initialize);
    return;
}

// TODO: Rewrite to fix bloom in splitscreen
void __cdecl rasterizer_dx9_dof_render_bloom(real32 blur_amount, real32 threshold, real32 brightness, real32 box_factor, real32 max_factor, bool a6, bool a7)
{
	INVOKE(0x26C3EB, 0x0, rasterizer_dx9_dof_render_bloom, blur_amount, threshold, brightness, box_factor, max_factor, a6, a7);
	return;
}

void __cdecl rasterizer_dx9_render_depth_of_field(real32 lower, real32 upper, real32 a3)
{
	INVOKE(0x26CABC, 0x0, rasterizer_dx9_render_depth_of_field, lower, upper, a3);
	return;
}

e_rasterizer_target rasterizer_dx9_convolve_screen_surfaces(
    real32 a1,
    real32 a2,
    real32 a3,
    e_rasterizer_target source,
    e_rasterizer_target destination,
    e_rasterizer_target stretch_source,
    e_rasterizer_target stretch_destination,
    int32 a8,
    int32 a9,
    real32 a10,
    real32 a11)
{
    e_rasterizer_target source_target = source;
    e_rasterizer_target stretch = (stretch_source == _rasterizer_target_none ? source : stretch_source);
    real32 accum = 0.f;

    if (a1 > 0.f)
    {
        e_rasterizer_target dst = destination;
        do
        {
            real32 res = a1 - accum;
            res = MIN(res, 1.f);

            if (res > 0.f)
            {
                source_target = sub_87E570(res, a2, a3, source_target, dst, a8);
                
                // Swap temp and stretch values
                e_rasterizer_target temp = stretch;
                stretch = dst;
                dst = temp;
            }
            a2 *= (1.f - a3 + a3 + a3);
            accum += 1.f;
        }
        while (accum < a1);
    }

    if (stretch_destination != source_target && stretch_destination != _rasterizer_target_none)
    {
        rasterizer_dx9_copy_target(source_target, stretch_destination);
        source_target = stretch_destination;
    }

    return source_target;

    /*
    return INVOKE(0x26C2CA, 0x0, rasterizer_dx9_convolve_screen_surfaces,
        a1,
        a2,
        a3,
        source,
        destination,
        stretch_source,
        stretch_destination,
        a8,
        a9,
        a10,
        a11);
    */
}

void rasterizer_dx9_draw_overlay_rect(uint32 screen_resolution_x, uint32 screen_resolution_y, D3DCOLOR color)
{
    s_convolution_d3d_vertex vertex_stream[4];

    rasterizer_dx9_submit_vertex_declaration(39);
    rasterizer_dx9_submit_resolve();
    
    const real32 x = (screen_resolution_x ? 1.f / screen_resolution_x : 0.f);
    const real32 y = (screen_resolution_y ? 1.f / screen_resolution_y : 0.f);

    const real32 left = -1.f - x;
    const real32 right = 1.f + x;
    const real32 top = 1.f + y;
    const real32 bottom = -1.f - y;

    vertex_stream[0] = { left, top, 0.f, 0.f, 0.f, color };
    vertex_stream[1] = { left, bottom, 0.f, 0.f, 1.f, color };
    vertex_stream[2] = { right, top, 0.f, 1.f, 0.f, color };
    vertex_stream[3] = { right, bottom, 0.f, 1.f, 1.f, color };

    rasterizer_dx9_draw_primitive_up(D3DPT_TRIANGLESTRIP, 2, vertex_stream, sizeof(s_convolution_d3d_vertex));
    return;
}

/* private code */

IDirect3DPixelShader9** get_local_pixel_shaders(void)
{
    return Memory::GetAddress<IDirect3DPixelShader9**>(0xA4AFF0);
}


e_rasterizer_target __cdecl sub_87E570(real32 unused1, real32 a1, real32 unused2, e_rasterizer_target a2, e_rasterizer_target a3, bool a4)
{
    return INVOKE(0x26BFBD, 0x0, sub_87E570, unused1, a1, unused2, a2, a3, a4);
}


bool __cdecl rasterizer_dx9_depth_of_field_pipeline_setup(s_rasterizer_dx9_dof_overlay_data* dof_data)
{
    const s_rasterizer_dx9_main_globals* rasterizer_dx9_globals = rasterizer_dx9_main_globals_get();

    rasterizer_dx9_set_render_target_internal(rasterizer_dx9_globals->global_d3d_surface_render_resolved, (IDirect3DSurface9*)NONE, true);
    rasterizer_dx9_set_target_as_texture(0, dof_data->target);
    rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
    rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    rasterizer_dx9_set_sampler_state(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAXANISOTROPY, 1);
    rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPMAPLODBIAS, 0);
    rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAXMIPLEVEL, 0);

    const e_rasterizer_target target = rasterizer_globals_get()->d3d9_sm3_supported ? _rasterizer_target_z_a8b8g8r8 : _rasterizer_target_backbuffer;
    rasterizer_dx9_set_target_as_texture(1, target);
    rasterizer_dx9_set_sampler_state(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    rasterizer_dx9_set_sampler_state(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    rasterizer_dx9_set_sampler_state(1, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
    rasterizer_dx9_set_sampler_state(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    rasterizer_dx9_set_sampler_state(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    rasterizer_dx9_set_sampler_state(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    rasterizer_dx9_set_sampler_state(1, D3DSAMP_MAXANISOTROPY, 1);
    rasterizer_dx9_set_sampler_state(1, D3DSAMP_MIPMAPLODBIAS, 0);
    rasterizer_dx9_set_sampler_state(1, D3DSAMP_MAXMIPLEVEL, 0);

    rasterizer_dx9_set_texture_direct(2, rasterizer_globals_get_data()->gradients.index, 0, 0.f);
    rasterizer_dx9_set_sampler_state(2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    rasterizer_dx9_set_sampler_state(2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    rasterizer_dx9_set_sampler_state(2, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
    rasterizer_dx9_set_sampler_state(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    rasterizer_dx9_set_sampler_state(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    rasterizer_dx9_set_sampler_state(2, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
    rasterizer_dx9_set_sampler_state(2, D3DSAMP_MAXANISOTROPY, 1);
    rasterizer_dx9_set_sampler_state(2, D3DSAMP_MIPMAPLODBIAS, 0);
    rasterizer_dx9_set_sampler_state(2, D3DSAMP_MAXMIPLEVEL, 0);

    rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
    rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, TRUE);
    rasterizer_dx9_set_render_state(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    rasterizer_dx9_set_render_state(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    rasterizer_dx9_set_render_state(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, FALSE);
    rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DCULL_NONE);
    rasterizer_dx9_set_render_state(D3DRS_ZENABLE, FALSE);
    rasterizer_dx9_set_render_state(D3DRS_ZWRITEENABLE, FALSE);
    rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DCMP_ALWAYS);
    rasterizer_dx9_set_render_state(D3DRS_DEPTHBIAS, 0);
    rasterizer_dx9_set_render_state(D3DRS_SLOPESCALEDEPTHBIAS, 0);
    if (!*rasterizer_dx9_disable_stencil_get())
    {
        rasterizer_dx9_set_render_state(D3DRS_STENCILENABLE, FALSE);
    }
    rasterizer_dx9_submit_resolve();

    if (rasterizer_get_main_pixel_shader_cache()->test_cache(0, dof_data->constants, NUMBEROF(dof_data->constants)))
    {
        rasterizer_dx9_globals->global_d3d_device->SetPixelShaderConstantF(0, (const float*)&dof_data->constants, NUMBEROF(dof_data->constants));
    }
    rasterizer_dx9_globals->global_d3d_device->SetPixelShader(get_local_pixel_shaders()[5]);
    return true;
}
