#pragma once

/* prototypes */

void rasterizer_dx9_fog_apply_patches(void);

bool __cdecl rasterizer_dx9_draw_atmospheric_fog(int32 a1);

bool __cdecl rasterizer_dx9_draw_fog_sky_only(int32 a1);

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
