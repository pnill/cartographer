#include "stdafx.h"
#include "rasterizer_dx9_fog.h"

#include "rasterizer_dx9.h"
#include "rasterizer_dx9_fullscreen_passes.h"
#include "rasterizer_dx9_main.h"
#include "rasterizer_dx9_submit.h"
#include "rasterizer_dx9_shader_submit.h"

#include "rasterizer/rasterizer_globals.h"
#include "rasterizer/dx9/shaders/compiled/fog_atmospheric_apply.h"
#include "render/render_patchy_fog.h"

/* constants */

static real32 fog_wind_x_axis_velocity_factor1 = 30.0f;

/* typedefs */

typedef bool(__cdecl* rasterizer_dx9_create_fog_shaders_t)(void);

/* globals */

rasterizer_dx9_create_fog_shaders_t p_rasterizer_dx9_create_fog_shaders;

/* prototypes */

real32* patchy_fog_apply_scale_get(void);
bool __cdecl rasterizer_dx9_set_fog_pixel_shader(uint32 index);
void rasterizer_fog_constant_velocity_update();
void rasterizer_fog_speed_apply_patch(void);
bool __cdecl rasterizer_dx9_create_fog_shaders(void);
void rasterizer_dx9_atmospheric_fog_patch(void);

/* public code */

void rasterizer_dx9_fog_apply_patches(void)
{
	rasterizer_fog_speed_apply_patch();
	rasterizer_dx9_atmospheric_fog_patch();
	return;
}

bool __cdecl rasterizer_dx9_atmospheric_fog_pipeline_setup(void* data)
{
	const s_rasterizer_globals* rasterizer_globals = rasterizer_globals_get();

	rasterizer_dx9_set_target(*rasterizer_dx9_main_render_target_get(), 0, true);
	rasterizer_dx9_set_target_as_texture(0, rasterizer_globals->d3d9_sm3_supported ? _rasterizer_target_z_a8b8g8r8 : _rasterizer_target_backbuffer);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPFILTER, 0);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAXANISOTROPY, 1);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPMAPLODBIAS, 0);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAXMIPLEVEL, 0);

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

	rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
	rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, TRUE);
	rasterizer_dx9_set_render_state(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
	rasterizer_dx9_set_render_state(D3DRS_BLENDFACTOR, global_white_pixel32.color);
	rasterizer_dx9_set_render_state(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	rasterizer_dx9_set_render_state(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, 0);
	rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DCULL_NONE);
	if (!*rasterizer_dx9_disable_stencil_get())
	{
		rasterizer_dx9_set_render_state(D3DRS_STENCILENABLE, 0);
	}
	rasterizer_dx9_set_render_state(D3DRS_ZENABLE, 0);
	rasterizer_dx9_set_render_state(D3DRS_ZWRITEENABLE, 0);
	rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	rasterizer_dx9_set_render_state(D3DRS_DEPTHBIAS, 0);
	rasterizer_dx9_set_render_state(D3DRS_SLOPESCALEDEPTHBIAS, 0);

	const s_frame* global_window_parameters = global_window_parameters_get();
	real_vector4d ps_constants[3];

	real_rgb_color color;
	color.red = global_window_parameters->fog_result.atmospheric_color.red * global_window_parameters->fog_result.atmospheric_max_density;
	color.green = global_window_parameters->fog_result.atmospheric_color.green * global_window_parameters->fog_result.atmospheric_max_density;
	color.blue = global_window_parameters->fog_result.atmospheric_color.blue * global_window_parameters->fog_result.atmospheric_max_density;

	ps_constants[0].i = PIN(color.red, 0.f, 1.f);
	ps_constants[0].j = PIN(color.green, 0.f, 1.f);
	ps_constants[0].k = PIN(color.blue, 0.f, 1.f);
	ps_constants[0].l = PIN(global_window_parameters->fog_result.atmospheric_max_density, 0.f, 1.f);


	color.red = global_window_parameters->fog_result.secondary_color.red * global_window_parameters->fog_result.secondary_max_density;
	color.green = global_window_parameters->fog_result.secondary_color.green * global_window_parameters->fog_result.secondary_max_density;
	color.blue = global_window_parameters->fog_result.secondary_color.blue * global_window_parameters->fog_result.secondary_max_density;

	ps_constants[1].i = PIN(color.red, 0.f, 1.f);
	ps_constants[1].j = PIN(color.green, 0.f, 1.f);
	ps_constants[1].k = PIN(color.blue, 0.f, 1.f);
	ps_constants[1].l = PIN(global_window_parameters->fog_result.secondary_max_density, 0.f, 1.f);


	ps_constants[2].i = PIN(global_window_parameters->fog_result.sky_fog_color.red, 0.f, 1.f);
	ps_constants[2].j = PIN(global_window_parameters->fog_result.sky_fog_color.green, 0.f, 1.f);
	ps_constants[2].k = PIN(global_window_parameters->fog_result.sky_fog_color.blue, 0.f, 1.f);
	ps_constants[2].l = PIN(global_window_parameters->fog_result.sky_fog_alpha, 0.f, 1.f);

	if (rasterizer_get_main_pixel_shader_cache()->test_cache(0, ps_constants, NUMBEROF(ps_constants)))
	{
		rasterizer_dx9_device_get_interface()->SetPixelShaderConstantF(0, (const float*)ps_constants, NUMBEROF(ps_constants));
	}
	rasterizer_dx9_set_fog_pixel_shader(0);
	rasterizer_dx9_submit_resolve();
	return true;
}

bool rasterizer_dx9_patchy_fog_apply_separate_layers_pipeline_setup(void* data)
{
	const s_patchy_fog_globals* patchy_fog_globals = patchy_fog_globals_get();

	const e_rasterizer_target target = patchy_fog_globals->render_patchy_fog_parameters[1] ? *rasterizer_dx9_main_render_target_get() : _rasterizer_target_texaccum;
	rasterizer_dx9_set_target(target, 0, false);

	rasterizer_dx9_set_target_as_texture(0, rasterizer_globals_get()->d3d9_sm3_supported ? _rasterizer_target_z_a8b8g8r8 : _rasterizer_target_backbuffer);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPFILTER, 0);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAXANISOTROPY, 1);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_MIPMAPLODBIAS, 0);
	rasterizer_dx9_set_sampler_state(0, D3DSAMP_MAXMIPLEVEL, 0);

	rasterizer_dx9_set_target_as_texture(2, _rasterizer_target_active_camo);
	rasterizer_dx9_set_sampler_state(2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	rasterizer_dx9_set_sampler_state(2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	rasterizer_dx9_set_sampler_state(2, D3DSAMP_ADDRESSW, D3DTADDRESS_CLAMP);
	rasterizer_dx9_set_sampler_state(2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	rasterizer_dx9_set_sampler_state(2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	rasterizer_dx9_set_sampler_state(2, D3DSAMP_MIPFILTER, 0);
	rasterizer_dx9_set_sampler_state(2, D3DSAMP_MAXANISOTROPY, 1);
	rasterizer_dx9_set_sampler_state(2, D3DSAMP_MIPMAPLODBIAS, 0);
	rasterizer_dx9_set_sampler_state(2, D3DSAMP_MAXMIPLEVEL, 0);

	rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
	rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, TRUE);
	rasterizer_dx9_set_render_state(D3DRS_SRCBLEND, D3DBLEND_BLENDFACTOR);
	rasterizer_dx9_set_render_state(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	rasterizer_dx9_set_render_state(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	rasterizer_dx9_set_render_state(D3DRS_BLENDFACTOR, global_white_pixel32.color);
	rasterizer_dx9_set_render_state(D3DRS_ALPHATESTENABLE, false);
	rasterizer_dx9_set_render_state(D3DRS_CULLMODE, D3DCULL_NONE);
	rasterizer_dx9_set_render_state(D3DRS_ZENABLE, FALSE);
	rasterizer_dx9_set_render_state(D3DRS_ZWRITEENABLE, FALSE);
	rasterizer_dx9_set_render_state(D3DRS_ZFUNC, D3DBLEND_INVDESTALPHA);
	rasterizer_dx9_set_render_state(D3DRS_DEPTHBIAS, 0);
	rasterizer_dx9_set_render_state(D3DRS_SLOPESCALEDEPTHBIAS, 0);

	real_vector4d ps_constants[4];
	ps_constants[0].i = PIN(patchy_fog_globals->layers[1].field_14, 0.f, 1.f);
	ps_constants[0].j = PIN(patchy_fog_globals->layers[2].field_14, 0.f, 1.f);
	ps_constants[0].k = PIN(patchy_fog_globals->layers[3].field_14, 0.f, 1.f);
	ps_constants[0].l = PIN(patchy_fog_globals->layers[4].field_14, 0.f, 1.f);

	const s_scenario_fog_result* fog_result = &global_window_parameters_get()->fog_result;

	ps_constants[1].i = fog_result->patchy_fog_color.i;
	ps_constants[1].j = fog_result->patchy_fog_color.j;
	ps_constants[1].k = fog_result->patchy_fog_color.k;
	ps_constants[1].l = 1.f;

	ps_constants[2] = { PIN(fog_result->field_88.lower, 0.f, 1.f), 0.f, 0.f, 0.f };
	ps_constants[3] = { PIN(fog_result->field_88.upper, 0.f, 1.f), 0.f, 0.f, 0.f };

	IDirect3DDevice9Ex* global_d3d_device = rasterizer_dx9_device_get_interface();

	if (rasterizer_get_main_pixel_shader_cache()->test_cache(0, ps_constants, NUMBEROF(ps_constants)))
	{
		global_d3d_device->SetPixelShaderConstantF(0, (const float*)&ps_constants, NUMBEROF(ps_constants));
	}

	if (!patchy_fog_globals->render_patchy_fog_parameters[1])
	{
		rasterizer_dx9_set_render_state(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_BLUE);
		rasterizer_dx9_set_render_state(D3DRS_ALPHABLENDENABLE, FALSE);
		rasterizer_dx9_set_fog_pixel_shader(2 * patchy_fog_globals->separate_layer_depths + 2);
	}
	else
	{
		rasterizer_dx9_set_fog_pixel_shader(2 * patchy_fog_globals->separate_layer_depths + 1);
	}

	rasterizer_dx9_submit_resolve();
	return true;
}

bool __cdecl rasterizer_dx9_draw_fog_sky_only(void* data)
{
	return INVOKE(0x276912, 0x0, rasterizer_dx9_draw_fog_sky_only, data);
}

bool __cdecl rasterizer_dx9_patchy_fog_apply_from_stencil_pipeline_setup(void* data)
{
	return INVOKE(0x276FBE, 0x0, rasterizer_dx9_patchy_fog_apply_from_stencil_pipeline_setup, data);
}

bool __cdecl rasterizer_dx9_patchy_fog_composite_pipeline_setup(void* data)
{
	return INVOKE(0x276A4F, 0x0, rasterizer_dx9_patchy_fog_apply_from_stencil_pipeline_setup, data);
}

bool __cdecl rasterizer_dx9_patchy_fog_composite_build_vertex_buffer(
	int32 output_type,
	real_rectangle2d* bounds,
	real_vector4d* location,
	void* output,
	void* ctx)
{
	return INVOKE(0x2761D8, 0x0, rasterizer_dx9_patchy_fog_composite_build_vertex_buffer, output_type, bounds, location, output, ctx);
}

bool __cdecl rasterizer_dx9_atmospheric_fog_build_vertex_buffer(
	int32 output_type,
	real_rectangle2d* bounds,
	real_vector4d* location,
	void* output,
	void* ctx)
{
	s_frame* global_window_parameters;
	real32 atmospheric_distance;
	real32 val;

	bool result = true;

	switch (output_type)
	{
	case _rasterizer_dx9_atmospheric_fog_output_type_position:
		rasterizer_dx9_fullscreen_calculate_position(location, 1.f, (real_vector4d*)output);
		break;
	case _rasterizer_dx9_atmospheric_fog_output_type_texcoord:
		rasterizer_dx9_fullscreen_calculate_texcoords(bounds, (real_point2d*)location, (real_point2d*)output);
		break;
	case _rasterizer_dx9_atmospheric_fog_output_type_plane_position:
		global_window_parameters = global_window_parameters_get();
		atmospheric_distance = global_window_parameters->fog_result.atmospheric_max_distance - global_window_parameters->fog_result.atmospheric_min_distance;

		val = atmospheric_distance >= k_real_math_epsilon ? atmospheric_distance : k_real_math_epsilon;
		val = (1.f / val);
		((real_vector4d*)output)->i = global_window_parameters->camera.z_far * val;
		((real_vector4d*)output)->j = 0.f;
		((real_vector4d*)output)->k = -(global_window_parameters->fog_result.atmospheric_min_distance * val);
		((real_vector4d*)output)->l = 0.f;
		break;
	case _rasterizer_dx9_atmospheric_fog_output_type_secondary_plane_position:
		global_window_parameters = global_window_parameters_get();
		atmospheric_distance = global_window_parameters->fog_result.secondary_max_distance - global_window_parameters->fog_result.secondary_min_distance;

		val = atmospheric_distance >= k_real_math_epsilon ? atmospheric_distance : k_real_math_epsilon;
		val = (1.f / val);
		((real_vector4d*)output)->i = global_window_parameters->camera.z_far * val;
		((real_vector4d*)output)->j = 0.f;
		((real_vector4d*)output)->k = -(global_window_parameters->fog_result.secondary_min_distance * val);
		((real_vector4d*)output)->l = 0.f;
		break;
	case _rasterizer_dx9_atmospheric_fog_output_type_screen_coordinates:
		rasterizer_dx9_fullscreen_calculate_screen_coordinates(bounds, (real_point2d*)location, (real_point2d*)output);
		break;
	case _rasterizer_dx9_atmospheric_fog_output_type_color:
		*(pixel32*)output = global_white_pixel32;
		break;
	default:
		DISPLAY_ASSERT("unreachable");
		result = false;
	}

	return result;
}

bool __cdecl rasterizer_dx9_patchy_fog_apply_build_vertex_buffer(
	int32 output_type,
	real_rectangle2d* bounds,
	real_vector4d* location,
	void* output,
	void* ctx)
{
	s_frame* global_window_parameters;
	real32 patchy_distance;
	real32 val;

	bool result = true;

	switch (output_type)
	{
	case _rasterizer_dx9_patchy_fog_output_type_position:
		rasterizer_dx9_fullscreen_calculate_position(location, 1.f, (real_vector4d*)output);
		break;
	case _rasterizer_dx9_patchy_fog_output_type_texcoord:
		rasterizer_dx9_fullscreen_calculate_texcoords(bounds, (real_point2d*)location, (real_point2d*)output);
		break;
	case _rasterizer_dx9_patchy_fog_output_type_plane_position:
		global_window_parameters = global_window_parameters_get();
		patchy_distance = global_window_parameters->fog_result.patchy_max_distance - global_window_parameters->fog_result.patchy_min_distance;

		val = patchy_distance >= k_real_math_epsilon ? patchy_distance : k_real_math_epsilon;
		val = (1.f / val);
		((real_vector4d*)output)->i = global_window_parameters->camera.z_far * val;
		((real_vector4d*)output)->j = 0.f;
		((real_vector4d*)output)->k = -(global_window_parameters->fog_result.patchy_min_distance * val);
		((real_vector4d*)output)->l = 0.f;
		break;
	case _rasterizer_dx9_patchy_fog_output_type_screen_coordinates:
		rasterizer_dx9_fullscreen_calculate_screen_coordinates(bounds, (real_point2d*)location, (real_point2d*)output);
		((real_point2d*)(output))->u *= *patchy_fog_apply_scale_get();
		((real_point2d*)(output))->v *= *patchy_fog_apply_scale_get();
		break;
	case _rasterizer_dx9_patchy_fog_output_type_color:
		*(pixel32*)output = global_white_pixel32;
		break;
	default:
		result = 0;
		break;
	}
	return result;
}

bool __cdecl rasterizer_dx9_patchy_fog_apply_from_stencil_build_vertex_buffer(
	int32 output_type,
	real_rectangle2d* bounds,
	real_vector4d* location,
	void* output,
	void* ctx)
{
	bool result = true;
	ASSERT(output);

	switch (output_type)
	{
	case _vertex_output_type_position:
		// position normalized device coordinates
		// make sure to set the viewport, otherwise it'll represent the entire surface
		// which is not good during split-screen
		rasterizer_dx9_fullscreen_calculate_position(location, 1.f, (real_vector4d*)output);
		break;
	case _vertex_output_type_texcoord:
		rasterizer_dx9_fullscreen_calculate_texcoords(bounds, (real_point2d*)location, (real_point2d*)output);
		break;
	case _vertex_output_type_color:
		*(pixel32*)output = global_white_pixel32;
		break;
	default:
		DISPLAY_ASSERT("unreachable");
		result = false;
	}

	return result;
}

bool __cdecl rasterizer_dx9_sky_only_fog_build_vertex_buffer(
	int32 output_type,
	real_rectangle2d* bounds,
	real_vector4d* location,
	void* output,
	void* ctx)
{
	bool result = true;

	switch (output_type)
	{
	case _rasterizer_dx9_fog_sky_only_output_type_position:
		rasterizer_dx9_fullscreen_calculate_position(location, 1.f, (real_vector4d*)output);
		break;
	case _rasterizer_dx9_fog_sky_only_output_type_color:
		*(pixel32*)output = global_white_pixel32;
		break;
	default:
		DISPLAY_ASSERT("unreachable");
		result = false;
	}

	return result;
}

/* private code */

real32* patchy_fog_apply_scale_get(void)
{
	return Memory::GetAddress<real32*>(0xA56B50);
}

bool __cdecl rasterizer_dx9_set_fog_pixel_shader(uint32 index)
{
	return INVOKE(0x276018, 0x0, rasterizer_dx9_set_fog_pixel_shader, index);
}

void __declspec(naked) rasterizer_fog_constant_velocity_update()
{
#define k_delta_time_stack_location (-10h)
	__asm
	{
		pop eax

		// esi - atmospheric fog ptr, [esp+1Ch + k_delta_time_stack_location] - dt
		// edi - tag definition data
		// xmm0 to this point dt as well
		movss xmm1, dword ptr[edi+44h]
		mulss xmm1, fog_wind_x_axis_velocity_factor1
		movss xmm2, dword ptr[esi]
		mulss xmm2, dword ptr[esi + 10h]
		addss xmm1, xmm2
		mulss xmm1, xmm0 // multiply by dt
		movss dword ptr[ebp], xmm1

		// repeat the code block above 1 more time
		movss xmm1, dword ptr[edi + 44h + 4h]
		mulss xmm1, fog_wind_x_axis_velocity_factor1
		movss xmm2, dword ptr[esi + 4]
		mulss xmm2, dword ptr[esi + 10h]
		addss xmm1, xmm2
		mulss xmm1, xmm0 // multiply by dt
		movss dword ptr[ebp + 4], xmm1

		// this block without addss
		movss xmm1, dword ptr[edi + 44h + 8h]
		mulss xmm1, fog_wind_x_axis_velocity_factor1
		mulss xmm1, xmm0 // multiply by dt
		movss dword ptr[ebp + 8], xmm1

		pop edi
		pop esi

		// push return back
		push eax
		retn
	}
#undef k_delta_time_stack_location
}

void rasterizer_fog_speed_apply_patch(void)
{
	Codecave(Memory::GetAddress(0x2773B5), rasterizer_fog_constant_velocity_update, 42);
	// xorps xmm0, xmm0, 2 nops
	BYTE xmm0_clear[] = { 0x0F, 0x57, 0xC0, 0x90, 0x90 };
	WriteBytes(Memory::GetAddress(0x2776D4), xmm0_clear, sizeof(xmm0_clear));
	WriteBytes(Memory::GetAddress(0x2776E8), xmm0_clear, sizeof(xmm0_clear));
	WriteBytes(Memory::GetAddress(0x277700), xmm0_clear, sizeof(xmm0_clear));
	return;
}

bool __cdecl rasterizer_dx9_create_fog_shaders(void)
{
	const unsigned char* ps = rasterizer_globals_get()->d3d9_sm3_supported ? k_fog_atmospheric_apply_ps_3_0 : k_fog_atmospheric_apply_ps_2_0;
	WritePointer(Memory::GetAddress(0x468768), ps);
	return p_rasterizer_dx9_create_fog_shaders();
}

void rasterizer_dx9_atmospheric_fog_patch(void)
{
	DETOUR_ATTACH(p_rasterizer_dx9_create_fog_shaders, Memory::GetAddress<rasterizer_dx9_create_fog_shaders_t>(0x275F94), rasterizer_dx9_create_fog_shaders);
	return;
}
