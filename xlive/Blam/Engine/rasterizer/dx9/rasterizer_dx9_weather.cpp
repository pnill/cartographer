#include "stdafx.h"
#include "rasterizer_dx9_weather.h"

#include "rasterizer_dx9.h"
#include "rasterizer_dx9_fullscreen_passes.h"
#include "rasterizer_dx9_main.h"
#include "rasterizer_dx9_submit.h"
#include "rasterizer_dx9_shader_submit.h"

#include "render/render_weather.h"

#include "cache/pc_geometry_cache.h"

/* structures */

struct s_dx9_weather_particle_primitive
{
	real_point3d position;
	real32 scale;
	pixel32 color;
};

/* globals */

// Multiply max count by 3 since there can be 3 primitives in a rain particle
s_dx9_weather_particle_primitive g_weather_particle_primitives[MAX_PARTICLES_LITE * 3] = {};

/* prototypes */


/* public code */

void rasterizer_dx9_weather_apply_patches(void)
{
	// Needed to fix splitscreen
	WritePointer(Memory::GetAddress(0x199708) + 1, rasterizer_dx9_weather_plate_build_vertex_buffer);
	WritePointer(Memory::GetAddress(0x19970D) + 1, rasterizer_dx9_weather_plate_setup_pipeline);
	return;
}

bool __cdecl rasterizer_dx9_weather_plate_setup_pipeline(const c_animated_background_plate* plate)
{
	rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, 1);
	rasterizer_dx9_set_render_state(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
	rasterizer_dx9_set_render_state(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	rasterizer_dx9_set_render_state(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	rasterizer_dx9_set_render_state(D3DRS_BLENDFACTOR, global_white_pixel32.color);
	rasterizer_dx9_set_render_state(D3DRS_ZENABLE, 0);
	rasterizer_dx9_set_render_state(D3DRS_ZWRITEENABLE, 0);
	rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DCMP_ALWAYS);	// D3DCMP_LESSEQUAL on xbox?
	rasterizer_dx9_set_render_state(D3DRS_DEPTHBIAS, 0);
	rasterizer_dx9_set_render_state(D3DRS_SLOPESCALEDEPTHBIAS, 0);
	rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DBLEND_INVBLENDFACTOR);
	rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, 0);
	rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DCULL_NONE);

	if (!*rasterizer_dx9_disable_stencil_get())
	{
		rasterizer_dx9_set_render_state(D3DRS_STENCILENABLE, 0);
	}

	rasterizer_dx9_set_target(_rasterizer_target_render_primary, 0, false);
	rasterizer_dx9_set_target_as_texture(0, _rasterizer_target_backbuffer);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAXANISOTROPY, 1);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPMAPLODBIAS, 0);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAXMIPLEVEL, 0);
	
	for (uint8 sampler = 1; sampler < k_animated_background_plate_textures+1; ++sampler)
	{	
		// Subtract sampler by 1 since we want to start off at 0 for the texture indices
		rasterizer_dx9_set_texture_direct(sampler, plate->textures[sampler-1].index, 0, 0.f);
		rasterizer_dx9_set_sampler_state(sampler, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		rasterizer_dx9_set_sampler_state(sampler, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		rasterizer_dx9_set_sampler_state(sampler, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		rasterizer_dx9_set_sampler_state(sampler, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		rasterizer_dx9_set_sampler_state(sampler, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		rasterizer_dx9_set_sampler_state(sampler, D3DSAMP_MAXANISOTROPY, 1);
		rasterizer_dx9_set_sampler_state(sampler, D3DSAMP_MIPMAPLODBIAS, 0);
		rasterizer_dx9_set_sampler_state(sampler, D3DSAMP_MAXMIPLEVEL, 0);
	}

	//rasterizer_dx9_set_vertex_shader_permutation(21, 0, 0);
	//rasterizer_dx9_submit_resolve();

	const int16 window_bound_index = global_window_parameters_get()->window_bound_index;
	const real32 alpha = plate->window[window_bound_index].field_80 * plate->window[window_bound_index].field_84;

	real_vector4d ps_constants[k_animated_background_plate_textures * 2];
	for (uint8 background_plate_num = 0; background_plate_num < k_animated_background_plate_textures; ++background_plate_num)
	{
		// Tint colors
		ps_constants[0+background_plate_num].i = PIN(plate->texture_tint_color[background_plate_num].red, 0.f, 1.f);
		ps_constants[0+background_plate_num].j = PIN(plate->texture_tint_color[background_plate_num].green, 0.f, 1.f);
		ps_constants[0+background_plate_num].k = PIN(plate->texture_tint_color[background_plate_num].blue, 0.f, 1.f);
		ps_constants[0+background_plate_num].l = 1.f;
		
		// Dot factors
		ps_constants[3+background_plate_num].i = 0.f;
		ps_constants[3+background_plate_num].j = 0.f;
		ps_constants[3+background_plate_num].k = 0.f;
		// doesn't really matter where this is set
		// because it always returns the scalar/dot product in the shader, and the rest of the vector is 0
		// but keep it in the last component for consistency with the original code
		ps_constants[3+background_plate_num].l = PIN(alpha * plate->window[window_bound_index].field_0[1].alpha[background_plate_num], 0.f, 1.f);
	}

	if (rasterizer_get_main_pixel_shader_cache()->test_cache(0, ps_constants, NUMBEROF(ps_constants)))
	{
		IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();
		global_d3d_device->SetPixelShaderConstantF(0, (real32*)ps_constants, NUMBEROF(ps_constants));
		global_d3d_device->SetPixelShader(g_d3d9_weather_shaders[_dx9_weather_shader_weather_plate]);
	}
	return true;
}

bool __cdecl rasterizer_dx9_weather_plate_build_vertex_buffer(
	int32 output_type,
	real_rectangle2d* bounds,
	real_vector4d* location,
	void* output,
	c_animated_background_plate* ctx)
{
	s_frame* global_window_parameters;
	s_animated_background_plate_window_bound_data* window_data;
	real32 val;
	uint32 tex_num;

	bool result = true;

	switch (output_type)
	{
	case _rasterizer_dx9_weather_plate_output_type_position:
		rasterizer_dx9_fullscreen_calculate_position(location, 0.f, (real_vector4d*)output);
		break;
	case _rasterizer_dx9_weather_plate_output_type_screen_texcoord:
		rasterizer_dx9_fullscreen_calculate_texcoords(bounds, (real_point2d*)location, (real_point2d*)output);
		break;
	case _rasterizer_dx9_weather_plate_output_type_plane_position:
		global_window_parameters = global_window_parameters_get();

		val = MAX(k_real_math_epsilon, ctx->depth_blend_z_bounds.upper - ctx->depth_blend_z_bounds.lower);
		val = (1.f / val);

		((real_rectangle2d*)output)->x0 = global_window_parameters->camera.z_far * val;
		((real_rectangle2d*)output)->x1 = 0.f;
		((real_rectangle2d*)output)->y0 = -(ctx->depth_blend_z_bounds.lower * val);
		((real_rectangle2d*)output)->y1 = 0.f;
		break;
	case _rasterizer_dx9_weather_plate_output_type_texture_0_texcoord:
	case _rasterizer_dx9_weather_plate_output_type_texture_1_texcoord:
	case _rasterizer_dx9_weather_plate_output_type_texture_2_texcoord:
		global_window_parameters = global_window_parameters_get();
		tex_num = output_type % k_animated_background_plate_textures;
		window_data = &ctx->window[global_window_parameters->window_bound_index];
		val = window_data->field_6C[tex_num];

		((real_point2d*)output)->u = window_data->texture_positions[tex_num].u * ctx->texture_scale[tex_num] + val * location->i;
		((real_point2d*)output)->v = window_data->texture_positions[tex_num].v * ctx->texture_scale[tex_num] + val * location->j;
		break;
	case _rasterizer_dx9_weather_plate_output_type_color:
		*(pixel32*)output = global_white_pixel32;
		break;
	default:
		DISPLAY_ASSERT("unreachable");
		result = false;
	}

	return result;
}

bool rasterizer_dx9_draw_weather_particles(c_particle_system_lite* system)
{
	rasterizer_dx9_perf_event_begin("weather particles", NULL);

	bool is_rain = system->m_type == _particle_system_lite_type_rain;
	if (pc_geometry_cache_preload_geometry(
		&system->m_geometry, 
		(e_pc_geometry_cache_preload_flags)(_pc_geometry_cache_preload_flag_2 | _pc_geometry_cache_preload_blocking)))
	{
		const particle_system_data* data = system->m_particle_system_data[0];

		const s_frame* global_window_parameters = global_window_parameters_get();

		rasterizer_dx9_reset_depth_buffer();
		rasterizer_dx9_set_vertex_shader_permutation(22, 0, 0);
		rasterizer_dx9_initialize_camera_projection(false, &global_window_parameters->camera, &global_window_parameters->projection, *rasterizer_dx9_main_render_target_get());

		real_vector2d rain_vector;
		pixel32 lower_rain_color;
		pixel32 upper_rain_color;
		if (is_rain)
		{
			rain_vector.i = global_window_parameters->camera.forward.i;
			rain_vector.j = global_window_parameters->camera.forward.j;
			
			lower_rain_color = real_alpha_to_pixel32(system->m_opacity.lower);
			upper_rain_color = real_alpha_to_pixel32(system->m_opacity.upper);
		}
		else
		{
			rasterizer_dx9_set_render_state(D3DRS_POINTSPRITEENABLE, 1);
			rasterizer_dx9_set_render_state(D3DRS_POINTSCALEENABLE, 1);
			rasterizer_dx9_set_render_state(D3DRS_POINTSCALE_A, 0);
			rasterizer_dx9_set_render_state(D3DRS_POINTSCALE_B, 0);
			rasterizer_dx9_set_render_state(D3DRS_POINTSCALE_C, 0x3F800000);
		}

		rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, 1);
		rasterizer_dx9_set_render_state(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		rasterizer_dx9_set_render_state(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, 1);
		rasterizer_dx9_set_render_state(D3DRS_ALPHAREF, 0);
		rasterizer_dx9_set_render_state(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		rasterizer_dx9_set_render_state(D3DRS_ZWRITEENABLE, 0);
		rasterizer_dx9_set_render_state(D3DRS_ZENABLE, 1);
		rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

		if (!is_rain)
		{
			rasterizer_dx9_set_texture_direct(3, system->m_sprites.index, 0, 0.f);
		}

		const int16 player_count = PIN(local_player_count(), 1, k_number_of_users);
		const uint32 count_per_user = data->render_data.count / player_count;
		const uint32 size = count_per_user * sizeof(s_dx9_weather_particle_primitive) * (2 * is_rain + 1);
		
		uint32 used_vertices = 0;
		s_dx9_weather_particle_primitive* current_primitive = g_weather_particle_primitives;
		
		for (uint32 i = 0; i < count_per_user; ++i)
		{
			const c_particle_lite_render* render_data = data->render_data[i];
			const c_particle_lite_data* update_data = data->update_data[i];
			if (update_data->m_creation_time_stamp > 0.f)
			{
				if (is_rain)
				{
					used_vertices += 2;
					const real32 scale = system->m_rain_streak_scale;
					const real32 dt = *weather_dt_global_get();

					current_primitive->position.x = render_data->m_position.x - update_data->m_velocity.i * scale * dt;
					current_primitive->position.y = render_data->m_position.y - update_data->m_velocity.j * scale * dt;
					current_primitive->position.z = render_data->m_position.z - update_data->m_velocity.k * scale * dt;
					current_primitive->scale = 1.f;
					current_primitive->color = { lower_rain_color.color | render_data->m_color.color & 0x00FFFFFF };
					++current_primitive;

					current_primitive->position = render_data->m_position;
					current_primitive->scale = 1.f;
					current_primitive->color = { upper_rain_color.color | render_data->m_color.color & 0x00FFFFFF };

					++current_primitive;

					current_primitive->position.x = render_data->m_position.x - system->m_rain_line_width * rain_vector.j;
					current_primitive->position.y = render_data->m_position.y + system->m_rain_line_width * rain_vector.i;
					current_primitive->position.z = render_data->m_position.z;
					current_primitive->scale = 1.f;
					current_primitive->color = { upper_rain_color.color | render_data->m_color.color & 0x00FFFFFF };
				}
				else
				{
					const real32 viewport_height = (real32)rectangle2d_height(&global_window_parameters->camera.viewport_bounds);

					current_primitive->position = render_data->m_position;
					current_primitive->scale = render_data->m_size * (viewport_height / 480.f) * 0.5f;	// TODO: figure out what the constant size is?
					current_primitive->color = render_data->m_color;
				}
				++used_vertices;
				++current_primitive;
			}
		}

		rasterizer_dx9_submit_vertex_declaration(61);
		if (is_rain)
		{
			rasterizer_dx9_device_get_interface()->SetPixelShader(g_d3d9_weather_shaders[_dx9_weather_shader_particle_rain]);
			rasterizer_dx9_draw_primitive_up(D3DPT_TRIANGLELIST, used_vertices / 3, g_weather_particle_primitives, sizeof(s_dx9_weather_particle_primitive));
		}
		else
		{
			rasterizer_dx9_device_get_interface()->SetPixelShader(g_d3d9_weather_shaders[_dx9_weather_shader_particle_generic]);
			rasterizer_dx9_draw_primitive_up(D3DPT_POINTLIST, used_vertices, g_weather_particle_primitives, sizeof(s_dx9_weather_particle_primitive));
		}

		rasterizer_dx9_set_render_state(D3DRS_POINTSPRITEENABLE, 0);
		rasterizer_dx9_set_render_state(D3DRS_POINTSCALEENABLE, 0);
		rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, 0);
		rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, 0);
		rasterizer_dx9_reset_depth_buffer();
	}
	
	rasterizer_dx9_perf_event_end("weather particles");
	return true;
}
