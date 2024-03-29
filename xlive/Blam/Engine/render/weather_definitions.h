#pragma once
#include "geometry/geometry_block.h"
#include "math/color_math.h"
#include "tag_files/tag_reference.h"

#define MAX_PARTICLES_LITE = 4096
#define MAX_WIND_PRIMITIVES = 128

enum  e_wind_primitive_type : uint16
{
	_wind_primitive_type_vortex = 0,
	_wind_primitive_type_gust = 1,
	_wind_primitive_type_implosion = 2,
	_wind_primitive_type_explosion = 3
};

enum e_animated_background_plate_flags : int32
{
	_animated_background_plate_forward_motion = FLAG(0),
	_animated_background_plate_auto_position_planes = FLAG(1),
	_animated_background_plate_auto_scale_planes_auto_update_speed = FLAG(2)
};

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

// max: MAX_WIND_PRIMITIVES
class c_wind_primitive 
{
public:
	real_vector3d position;
	real32 radius;
	real32 strength;
	e_wind_primitive_type type;
	int8 pad_1[2];
};
TAG_BLOCK_SIZE_ASSERT(c_wind_primitive, 24);

// max: 1
class c_wind_model 
{
public:
	real32 wind_tiling_scale;
	real_vector3d wind_primary_heading_pitch_strength;
	real32 primary_rate_of_change;
	real32 primary_min_strength;
	int8 pad_1[20];
	real_vector3d wind_gusting_heading_pitch_strength;
	real32 gust_directional_rate_of_change;
	real32 gust_strength_rate_of_change;
	real32 gust_cone_angle;
	int8 pad_2[56];
	real32 turbulence_rate_of_change;
	real_vector3d turbulence_scale;
	real32 gravity_constant;
	tag_block<c_wind_primitive> wind_primitives;
	int32 pad_3;
};
TAG_BLOCK_SIZE_ASSERT(c_wind_model, 156);

// max: 1
class c_animated_background_plate
{
public:
	tag_reference textures[3]; // bitm

	real_point3d plate_position;

	real_vector3d texture_move_speed[3];

	real32 texture_scale[3];

	real_vector3d texture_jitter[3];

	real_bounds plate_z_bounds;
	real_bounds depth_blend_z_bounds;

	real32 texture_opacity[3];

	e_animated_background_plate_flags flags;

	real_rgb_color texture_tint_color[3];
	real32 texture_mass[3];

	int8 pad[736];
};
TAG_BLOCK_SIZE_ASSERT(c_animated_background_plate, 936);

// TODO: Figure out where this actually goes.
// max: MAX_PARTICLES_LITE
class c_particle_lite_render 
{
public:
	real_vector3d position;
	real32 size;
	pixel32 color;
};
TAG_BLOCK_SIZE_ASSERT(c_particle_lite_render, 20);

// TODO: Figure out where this actually goes.
// max: MAX_PARTICLES_LITE
class c_particle_lite_data 
{
public:
	real_vector3d velocity;
	int32 pad_1[3];
	real32 mass;
	real32 creation_time_stamp;
};
TAG_BLOCK_SIZE_ASSERT(c_particle_lite_data, 32);

// TODO: Figure out where this actually goes.
// max: 1
struct particle_system_data
{
	tag_block<c_particle_lite_render> render_data;
	tag_block<c_particle_lite_data> update_data;
	int8 pad_1[32];
};
TAG_BLOCK_SIZE_ASSERT(particle_system_data, 48);

// TODO: Figure out where this actually goes.
// max: 1
class c_particle_system_lite
{
public:
	tag_reference sprites; // bitm
	real_bounds view_box; // width, height
	real32 view_box_depth;
	real32 exclusion_radius;
	real32 max_velocity;
	real_bounds mass;
	real_bounds size;
	int32 maximum_particle_count;
	real_vector3d initial_velocity;
	real32 bitmap_animation_speed;
	geometry_block_info geometry;
	tag_block<particle_system_data> particle_system_data;
	e_particle_system_lite_type type;
	int8 pad_1[2];
	real_bounds opacity;
	real32 rain_streak_scale;
	real32 rain_line_width;
	int32 pad_2[3];
};
TAG_BLOCK_SIZE_ASSERT(c_particle_system_lite, 140);

class c_weather_system
{
public:
	tag_block<c_particle_system_lite> particle_system;
	tag_block< c_animated_background_plate> background_plates;
	c_wind_model wind_model;
	real32 fade_radius;
};
TAG_BLOCK_SIZE_ASSERT(c_weather_system, 0xB0);