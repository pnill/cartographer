#pragma once
#include "math/color_math.h"
#include "render/render_weather.h"
#include "tag_files/tag_reference.h"

#define MAX_WIND_PRIMITIVES 128
#define k_animated_background_plate_textures 3

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
ASSERT_STRUCT_SIZE(c_wind_primitive, 24);

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
ASSERT_STRUCT_SIZE(c_wind_model, 156);

struct s_animated_background_plate_window_color_data
{
	real32 alpha[k_animated_background_plate_textures];
	real32 field_C[k_animated_background_plate_textures];
	real32 field_18[k_animated_background_plate_textures];
};
ASSERT_STRUCT_SIZE(s_animated_background_plate_window_color_data, 36);

struct s_animated_background_plate_window_bound_data
{
	s_animated_background_plate_window_color_data field_0[2];
	real32 field_34[3];
	real_point2d texture_positions[k_animated_background_plate_textures];
	real32 field_6C[k_animated_background_plate_textures];
	real32 field_78;
	real32 field_7C;
	real32 field_80;
	real32 field_84;
	real32 field_88;
	real32 field_8C[11];
};
ASSERT_STRUCT_SIZE(s_animated_background_plate_window_bound_data, 184);

// max: 1
class c_animated_background_plate
{
public:
	tag_reference textures[k_animated_background_plate_textures]; // bitm

	real_point3d plate_position;

	real_vector3d texture_move_speed[k_animated_background_plate_textures];
	real32 texture_scale[k_animated_background_plate_textures];
	real_vector3d texture_jitter[k_animated_background_plate_textures];

	real_bounds plate_z_bounds;
	real_bounds depth_blend_z_bounds;

	real32 texture_opacity[k_animated_background_plate_textures];

	e_animated_background_plate_flags flags;

	real_rgb_color texture_tint_color[k_animated_background_plate_textures];
	real32 texture_mass[k_animated_background_plate_textures];

	s_animated_background_plate_window_bound_data window[4];
};
ASSERT_STRUCT_SIZE(c_animated_background_plate, 936);

// TODO: Figure out where this actually goes.
// max: 1

class c_weather_system
{
public:
	tag_block<c_particle_system_lite> m_particle_system;
	tag_block<c_animated_background_plate> m_background_plates;
	c_wind_model m_wind_model;
	real32 m_fade_radius;
};
ASSERT_STRUCT_SIZE(c_weather_system, 0xB0);