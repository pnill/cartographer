#pragma once

/* enums */

enum e_rasterizer_dx9_atmospheric_fog_output_type : int32
{
	_rasterizer_dx9_atmospheric_fog_output_type_position,
	_rasterizer_dx9_atmospheric_fog_output_type_texcoord,
	_rasterizer_dx9_atmospheric_fog_output_type_plane_position,
	_rasterizer_dx9_atmospheric_fog_output_type_secondary_plane_position,
	_rasterizer_dx9_atmospheric_fog_output_type_screen_coordinates,
	_rasterizer_dx9_atmospheric_fog_output_type_color
};

enum e_rasterizer_dx9_patchy_fog_output_type : int32
{
	_rasterizer_dx9_patchy_fog_output_type_position,
	_rasterizer_dx9_patchy_fog_output_type_texcoord,
	_rasterizer_dx9_patchy_fog_output_type_plane_position,
	_rasterizer_dx9_patchy_fog_output_type_screen_coordinates,
	_rasterizer_dx9_patchy_fog_output_type_color
};

enum e_rasterizer_dx9_fog_sky_only_output_type : int32
{
	_rasterizer_dx9_fog_sky_only_output_type_position,
	_rasterizer_dx9_fog_sky_only_output_type_color
};

/* globals */


/* prototypes */

void rasterizer_dx9_fog_apply_patches(void);

bool __cdecl rasterizer_dx9_atmospheric_fog_pipeline_setup(void* data);

bool rasterizer_dx9_patchy_fog_apply_separate_layers_pipeline_setup(void* data);

bool __cdecl rasterizer_dx9_draw_fog_sky_only(void* data);

bool __cdecl rasterizer_dx9_patchy_fog_apply_from_stencil_pipeline_setup(void* data);

bool __cdecl rasterizer_dx9_patchy_fog_composite_pipeline_setup(void* data);

bool __cdecl rasterizer_dx9_patchy_fog_composite_build_vertex_buffer(
	int32 output_type,
	real_rectangle2d* bounds,
	real_vector4d* location,
	void* output,
	void* ctx);

bool __cdecl rasterizer_dx9_atmospheric_fog_build_vertex_buffer(
	int32 output_type,
	real_rectangle2d* bounds,
	real_vector4d* location,
	void* output,
	void* ctx);

bool __cdecl rasterizer_dx9_patchy_fog_apply_build_vertex_buffer(
	int32 output_type,
	real_rectangle2d* bounds,
	real_vector4d* location,
	void* output,
	void* ctx);

bool __cdecl rasterizer_dx9_patchy_fog_apply_from_stencil_build_vertex_buffer(
	int32 output_type,
	real_rectangle2d* bounds,
	real_vector4d* location,
	void* output,
	void* ctx);

bool __cdecl rasterizer_dx9_sky_only_fog_build_vertex_buffer(
	int32 output_type,
	real_rectangle2d* bounds,
	real_vector4d* location,
	void* output,
	void* ctx);
