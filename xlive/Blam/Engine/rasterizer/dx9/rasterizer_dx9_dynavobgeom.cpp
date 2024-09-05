#include "stdafx.h"
#include "rasterizer_dx9_dynavobgeom.h"

#include "rasterizer_dx9.h"
#include "rasterizer_dx9_dynamic_geometry.h"
#include "rasterizer_dx9_main.h"
#include "rasterizer_dx9_shader_submit.h"
#include "rasterizer_dx9_submit.h"


/* globals */

#ifndef NDEBUG
bool g_dynamic_geometry_print_message = true;
#endif

D3DPRIMITIVETYPE g_dynamic_primitive_types[5] =
{
	D3DPT_POINTLIST,
	D3DPT_LINELIST,
	D3DPT_TRIANGLELIST,
	D3DPT_TRIANGLESTRIP,
	(D3DPRIMITIVETYPE)0
};

/* prototypes */

IDirect3DPixelShader9** dynavobgeom_pixel_shaders_get(void);
int rasterizer_dx9_draw_primitive_dynamic_geometry(
	int32 type,
	int32 usage,
	int32 primitive_type,
	int32 vertex_declaration_index,
	uint32 vertex_size,
	int32 vertex_count,
	void(vertex_builder)(uint8*, uint32, const void*),
	int32 index_count,
	int32 unused,
	const void* old_vertex_data);
uint32 rasterizer_dx9_get_primitive_count(uint32 vertex_count, uint32 primitive_type);

/* public code */

void rasterizer_dx9_dynamic_screen_geometry_draw(const s_dynamic_geometry_parameters* parameters, const s_dynamic_vertex_data* vertex_data)
{
	// INVOKE(0x271C57, 0x0, rasterizer_dx9_dynamic_screen_geometry_draw, parameters, vertex_data);

	ASSERT(parameters);
	ASSERT(parameters->map[0]);
	ASSERT(!parameters->map[2] || parameters->map[1]);
	ASSERT(!parameters->map[1] || !parameters->meter_parameters);

	const s_frame* global_window_parameters = global_window_parameters_get();
	if (!global_window_parameters->render_type)
	{
		rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DBLEND_ZERO);
		rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DBLEND_DESTALPHA);
		rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, D3DBLEND_ZERO);
		rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, FALSE);
		rasterizer_dx9_set_render_state(D3DRS_ZENABLE, FALSE);
		rasterizer_dx9_set_render_state(D3DRS_DEPTHBIAS, 0);
		rasterizer_dx9_set_blend_render_state(parameters->blend_function);

		const int16 width = rectangle2d_width(&global_window_parameters->camera.viewport_bounds);
		const int16 height = rectangle2d_height(&global_window_parameters->camera.viewport_bounds);

		const bool f4_valid = parameters->field_4;
		const real32 v1 = (f4_valid ? (parameters->field_4->i * 2.f) / width : 0.f);
		const real32 v2 = (f4_valid ? (parameters->field_4->j * -2.f) / width : 0.f);

		const real32 width_ratio = 1.f / width;
		const real32 height_ratio = 1.f / height;

		real_vector4d vs_constants[5];

		vs_constants[0].i = width_ratio * 2.f;
		vs_constants[0].j = 0.f;
		vs_constants[0].k = 0.f;
		vs_constants[0].l = v1 - (width_ratio + 1.f);

		vs_constants[1].i = 0.f;
		vs_constants[1].j = height_ratio * -2.f;
		vs_constants[1].k = 0.f;
		vs_constants[1].l = (height_ratio + v2) + 1.f;

		vs_constants[2] = { 0.f, 0.f, 0.f, 0.5f };
		vs_constants[3] = { 0.f, 0.f, 0.f, 1.f };
		vs_constants[4] = { parameters->width_ratio, parameters->height_ratio, 0.f, 1.f };


		real_vector4d vs_constants_second[6];
		vs_constants_second[0] = parameters->vs_constants_second_0;

		vs_constants_second[1].i = (parameters->field_8 ? 1.f : 0.f);
		vs_constants_second[1].j = (parameters->field_8 ? 0.f : 1.f);
		vs_constants_second[1].k = (parameters->field_9 ? 1.f : 0.f);
		vs_constants_second[1].l = (parameters->field_9 ? 0.f : 1.f);

		vs_constants_second[2].i = (parameters->field_10 ? 1.f : 0.f);
		vs_constants_second[2].j = (parameters->field_10 ? 0.f : 1.f);
		vs_constants_second[2].k = (parameters->field_1C ? parameters->field_1C->i : 0.f);
		vs_constants_second[2].l = (parameters->field_1C ? parameters->field_1C->j : 0.f);

		vs_constants_second[3].i = (parameters->field_20 ? parameters->field_20->i : 0.f);
		vs_constants_second[3].j = (parameters->field_20 ? parameters->field_20->j : 0.f);
		vs_constants_second[3].k = (parameters->field_24 ? parameters->field_24->i : 0.f);
		vs_constants_second[3].l = (parameters->field_24 ? parameters->field_24->j : 0.f);
		
		vs_constants_second[4] = parameters->vs_constants_second_4;
		vs_constants_second[5] = { parameters->field_34.i, parameters->field_34.j, 0.f, 0.f };
		
		IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();

		c_rasterizer_constant_4f_cache<256>* g_vertex_shader_constants_cache = rasterizer_get_main_vertex_shader_cache();
		if (g_vertex_shader_constants_cache->test_cache(177, vs_constants, NUMBEROF(vs_constants)))
		{
			global_d3d_device->SetVertexShaderConstantF(177, (real32*)vs_constants, NUMBEROF(vs_constants));
		}
		if (g_vertex_shader_constants_cache->test_cache(182, vs_constants_second, NUMBEROF(vs_constants_second)))
		{
			global_d3d_device->SetVertexShaderConstantF(182, (real32*)vs_constants_second, NUMBEROF(vs_constants_second));
		}

		for (uint8 sampler = 0; sampler < k_dynamic_geometry_map_max_count; sampler++)
		{
			bitmap_data* bitmap = parameters->map[sampler];
			if (!bitmap)
			{
				break;
			}

			uint32 filter = (parameters->point_interpolation ? D3DTEXF_POINT : D3DTEXF_LINEAR);
			uint32 address = (parameters->wrap_uv[sampler] ? D3DTADDRESS_WRAP : D3DTADDRESS_MIRRORONCE);
			rasterizer_set_texture_bitmap_data(sampler, bitmap);
			rasterizer_dx9_set_sampler_state(sampler, D3DSAMP_ADDRESSU, address);
			rasterizer_dx9_set_sampler_state(sampler, D3DSAMP_ADDRESSV, address);
			rasterizer_dx9_set_sampler_state(sampler, D3DSAMP_MAGFILTER, filter);
			rasterizer_dx9_set_sampler_state(sampler, D3DSAMP_MINFILTER, filter);
			rasterizer_dx9_set_sampler_state(sampler, D3DSAMP_MIPFILTER, filter);
			rasterizer_dx9_set_sampler_state(sampler, D3DSAMP_MAXANISOTROPY, 1);
			rasterizer_dx9_set_sampler_state(sampler, D3DSAMP_MIPMAPLODBIAS, 0);
			rasterizer_dx9_set_sampler_state(sampler, D3DSAMP_MAXMIPLEVEL, 0);
		}

		rasterizer_dx9_set_vertex_shader_permutation(14);
		rasterizer_dx9_submit_vertex_declaration(35);
		rasterizer_dx9_submit_resolve();

		bool draw_geometry = false;
		if (parameters->meter_parameters || !parameters->map[0])
		{
			draw_geometry = true;
		}
		else
		{
			real_vector4d ps_constants[6] = { 0 };
			for (uint8 i = 0; i < k_dynamic_geometry_map_max_count; i++)
			{
				const s_dynamic_geometry_map_color_parameters* color = parameters->map_color[i];

				// Copy colour to vs constants if one is supplied, otherwise copy white
				memcpy(&ps_constants[i], (color != NULL ? color->color : global_real_rgb_white), sizeof(real_rgb_color));

				const s_dynamic_geometry_map_alpha* alpha = parameters->map_alpha[i];
				ps_constants[i].l = (alpha != NULL ? *alpha->alpha : 1.f);
			}

			if (parameters->map[1])
			{
				ps_constants[3].i = (real32)(parameters->field_90 == 0);
				ps_constants[3].j = (real32)(parameters->field_90 == 1);
				ps_constants[3].k = (real32)(parameters->field_90 == 2);
				ps_constants[3].l = (real32)(parameters->field_90 == 3);
				ps_constants[4].i = (real32)(parameters->field_90 == 4);
				ps_constants[4].j = (real32)(parameters->field_90 == 5);
			}

			if (parameters->map[2])
			{
				ps_constants[4].k = (real32)(parameters->field_92 == 0);
				ps_constants[4].l = (real32)(parameters->field_92 == 1);
				ps_constants[5].i = (real32)(parameters->field_92 == 2);
				ps_constants[5].j = (real32)(parameters->field_92 == 3);
				ps_constants[5].k = (real32)(parameters->field_92 == 4);
			}

			draw_geometry = SUCCEEDED(global_d3d_device->SetPixelShader(dynavobgeom_pixel_shaders_get()[0]));

			if (rasterizer_get_main_pixel_shader_cache()->test_cache(0, ps_constants, NUMBEROF(ps_constants)))
			{
				global_d3d_device->SetPixelShaderConstantF(0, (real32*)ps_constants, NUMBEROF(ps_constants));
			}
		}

		if (draw_geometry)
		{
			rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DBLEND_SRCCOLOR);
			rasterizer_dx9_set_vertex_shader_permutation(14);
			rasterizer_dx9_submit_vertex_declaration(35);
			rasterizer_dx9_submit_resolve();
			rasterizer_dx9_draw_primitive_dynamic_geometry(0, 0, D3DPT_LINESTRIP, 35, sizeof(s_dynamic_vertex_data), 4, rasterizer_dx9_primitive_builder, 0, 0, vertex_data);
		}
	}

	return;
}

/* private code */

IDirect3DPixelShader9** dynavobgeom_pixel_shaders_get(void)
{
	return Memory::GetAddress<IDirect3DPixelShader9**>(0xA4DDE0);
}

int rasterizer_dx9_draw_primitive_dynamic_geometry(
	int32 type,
	int32 usage,
	int32 primitive_type,
	int32 vertex_declaration_index,
	uint32 vertex_size,
	int32 vertex_count,
	void(vertex_builder)(uint8*, uint32, const void*),
	int32 index_count,
	int32 unused,
	const void* old_vertex_data)
{
	uint8 vertex_data[8192];

	if (!type)
	{
		if (vertex_count * vertex_size < 8192)
		{
			vertex_builder(vertex_data, vertex_count * vertex_size, old_vertex_data);
			int32 primitive_count = rasterizer_dx9_get_primitive_count(vertex_count, primitive_type);
			rasterizer_dx9_draw_primitive_up(g_dynamic_primitive_types[primitive_type], primitive_count, vertex_data, vertex_size);
		}
#ifndef NDEBUG
		else if (g_dynamic_geometry_print_message)
		{
			g_dynamic_geometry_print_message = false;
			error(2, "DrawPrimitiveUP was given too much data to swallow, skipping");
		}
#endif
	}
	return NONE;
}

uint32 rasterizer_dx9_get_primitive_count(uint32 vertex_count, uint32 primitive_type)
{
	uint32 result;
	switch (primitive_type)
	{
	case 0:
		result = vertex_count;
		break;
	case 1:
		result = vertex_count / 2;
		break;
	case 2:
		result = vertex_count / 3;
		break;
	case 3:
		result = vertex_count - 2;
		break;
	default:
		result = 0;
	}
	return result;
}