#pragma once
#include "render/weather_definitions.h"

/* enums */

enum e_rasterizer_dx9_weather_plate_output_type : int32
{
	_rasterizer_dx9_weather_plate_output_type_position,
	_rasterizer_dx9_weather_plate_output_type_screen_texcoord,
	_rasterizer_dx9_weather_plate_output_type_plane_position,
	_rasterizer_dx9_weather_plate_output_type_texture_0_texcoord,
	_rasterizer_dx9_weather_plate_output_type_texture_1_texcoord,
	_rasterizer_dx9_weather_plate_output_type_texture_2_texcoord,
	_rasterizer_dx9_weather_plate_output_type_color
};

/* prototypes */

void rasterizer_dx9_weather_apply_patches(void);

bool __cdecl rasterizer_dx9_weather_plate_setup_pipeline(const c_animated_background_plate* plate);

bool __cdecl rasterizer_dx9_weather_plate_build_vertex_buffer(
	int32 output_type,
	real_rectangle2d* bounds,
	real_vector4d* location,
	void* output,
	c_animated_background_plate* ctx);

bool rasterizer_dx9_draw_weather_particles(c_particle_system_lite* system);
