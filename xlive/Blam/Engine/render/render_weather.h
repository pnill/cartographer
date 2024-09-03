#pragma once
#include "geometry/geometry_block.h"
#include "math/color_math.h"
#include "math/real_math.h"
#include "tag_files/tag_reference.h"
#include "tag_files/tag_block.h"

/* constants */

#define MAX_PARTICLES_LITE 4096

/* enums */

enum e_particle_system_lite_type : uint16
{
	_particle_system_lite_type_generic = 0,
	_particle_system_lite_type_snow = 1,
	_particle_system_lite_type_rain = 2,
	_particle_system_lite_type_rain_splash = 3,
	_particle_system_lite_type_bugs = 4,
	_particle_system_lite_type_sand_storm = 5,
	_particle_system_lite_type_debris = 6,
	_particle_system_lite_type_bubbles = 7
};

enum e_dx9_weather_shader_type
{
	_dx9_weather_shader_particle_generic,
	_dx9_weather_shader_particle_rain,
	_dx9_weather_shader_weather_plate,
	k_dx9_weather_shader_type_count
};

/* structures */

class c_particle_lite_render
{
public:
	real_point3d m_position;
	real32 m_size;
	pixel32 m_color;
};
ASSERT_STRUCT_SIZE(c_particle_lite_render, 20);

class c_particle_lite_data
{
public:
	real_vector3d m_velocity;
	int32 m_pad[3];
	real32 m_mass;
	real32 m_creation_time_stamp;
};
ASSERT_STRUCT_SIZE(c_particle_lite_data, 32);

struct particle_system_data
{
	tag_block<c_particle_lite_render> render_data;
	tag_block<c_particle_lite_data> update_data;
	int8 pad[32];
};
ASSERT_STRUCT_SIZE(particle_system_data, 48);

class c_particle_system_lite
{
public:
	// TODO: move the initialize and dispose functions out of here once we own rasterizer_initialize
	static void __cdecl initialize(void);
	static void __cdecl dispose(void);

	// TODO: make this a member function once we own the function that calls this
	static bool __fastcall draw(c_particle_system_lite* _this);

	tag_reference m_sprites; // bitm
	real_bounds m_view_box; // width, height
	real32 m_view_box_depth;
	real32 m_exclusion_radius;
	real32 m_max_velocity;
	real_bounds m_mass;
	real_bounds m_size;
	int32 m_maximum_particle_count;
	real_vector3d m_initial_velocity;
	real32 m_bitmap_animation_speed;
	geometry_block_info m_geometry;
	tag_block<particle_system_data> m_particle_system_data;
	e_particle_system_lite_type m_type;
	int8 m_pad_1[2];
	real_bounds m_opacity;
	real32 m_rain_streak_scale;
	real32 m_rain_line_width;
	int32 m_pad_2[3];
};
ASSERT_STRUCT_SIZE(c_particle_system_lite, 140);

/* globals */

extern IDirect3DPixelShader9* g_d3d9_weather_shaders[k_dx9_weather_shader_type_count];

/* prototypes */

void render_weather_apply_patches(void);

real32* weather_dt_global_get(void);
