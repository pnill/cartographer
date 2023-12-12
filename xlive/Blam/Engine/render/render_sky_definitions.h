#pragma once
#include "Blam/Cache/DataTypes/TagBlock.h"
#include "Blam/Cache/DataTypes/TagRef.h"
#include "Blam/Engine/math/color_math.h"
#include "Blam/Engine/math/real_math.h"

#define MAXIMUM_ANIMATIONS_PER_SKY 8
#define MAXIMUM_LIGHTS_PER_SKY 8
#define MAXIMUM_SHADER_FUNCTIONS_PER_SKY 8

enum e_sky_flags
{
	_sky_fixed_in_world_space = FLAG(0),
	_sky_depreciated = FLAG(1),
	_sky_casts_light_from_below = FLAG(2),
	_sky_fog_only_affects_containing_clusters = FLAG(3),
	_sky_use_clear_color = FLAG(4)
};

enum e_sky_radiosity_light_flags
{
	_sky_radiosity_light_affects_exteriors = FLAG(0),
	_sky_radiosity_light_affects_interios = FLAG(1),
	_sky_radiosity_direct_illumination_in_lightmaps = FLAG(2),
	_sky_radiosity_indirect_illumination_in_lightmaps = FLAG(3)
};

// max_count:    MAXIMUM_ANIMATIONS_PER_SKY = 8
struct s_sky_animation
{
	int16 animation_index;
	int8 pad[2];
	real32 period_sec;
	int8 pad_2[28];
};
TAG_BLOCK_SIZE_ASSERT(s_sky_animation, 36);

// max_count:    MAXIMUM_SHADER_FUNCTIONS_PER_SKY = 8
struct s_sky_shader_function
{
	int8 pad[4];
	char global_function_name[32];
};
TAG_BLOCK_SIZE_ASSERT(s_sky_shader_function, 36);

// max count 1
struct sky_radiosity_light_block
{
	e_sky_radiosity_light_flags flags;
	real_rgb_color color;
	real32 power;
	real32 test_distance_world_units;
	int8 pad[12];
	real32 diameter_degrees;
};
TAG_BLOCK_SIZE_ASSERT(sky_radiosity_light_block, 0x28);

// max count 1
struct s_sky_light_fog
{
	real_rgb_color color;
	real32 maximum_density;
	real32 start_distance_sworld_units;
	real32 opaque_distance_world_units;
	real_bounds cone_degrees;
	real32 atmospheric_fog_influence;
	real32 secondary_fog_influence;
	real32 sky_fog_influence;
};
TAG_BLOCK_SIZE_ASSERT(s_sky_light_fog, 0x2C);

// max_count:    MAXIMUM_LIGHTS_PER_SKY = 8
struct s_sky_light
{
	real_vector3d direction_vector;
	real_vector2d direction;

	tag_reference lens_flare; // lens
	tag_block<s_sky_light_fog> fog;
	tag_block<s_sky_light_fog> fog_opposite;
	tag_block<sky_radiosity_light_block> radiosity;
};
TAG_BLOCK_SIZE_ASSERT(s_sky_light, 0x34);

// max count 1
struct s_sky_patchy_fog
{
	real_rgb_color color;
	int8 pad[12];
	real_vector2d density;
	real_vector2d distance_world_units;
	int8 pad_2[32];
	tag_reference patchy_fog; // fpch
};
TAG_BLOCK_SIZE_ASSERT(s_sky_patchy_fog, 0x50);

// max count 1
struct s_sky_fog
{
	real_rgb_color color;
	real32 density;
};
TAG_BLOCK_SIZE_ASSERT(s_sky_fog, 16);

// max count 1
struct s_sky_atmospheric_fog
{
	real_rgb_color color;
	real32 maximum_density;
	real32 start_distance_world_units;
	real32 opaque_distance_world_units;
};
TAG_BLOCK_SIZE_ASSERT(s_sky_atmospheric_fog, 24);

// max count 1
struct s_sky_cubemap
{
	tag_reference cube_map_reference;
	real32 power_scale;
};
TAG_BLOCK_SIZE_ASSERT(s_sky_cubemap, 0xC);

struct s_sky_definition
{
	tag_reference render_model; // mode
	tag_reference animation_graph; // jmad
	e_sky_flags flags;
	real32 render_model_scale;
	real32 movement_scale;
	tag_block<s_sky_cubemap> cube_map;

	// Explaination("AMBIENT LIGHT", "EMPTY STRING")
	real_rgb_color indoor_ambient_color;
	int8 pad[4];
	real_rgb_color outdoor_ambient_color;
	int8 pad_2[4];

	real32 fog_spread_distance_world_units;

	tag_block<s_sky_atmospheric_fog> atmospheric_fog;
	tag_block<s_sky_atmospheric_fog> secondary_fog;
	tag_block<s_sky_fog> sky_fog;
	tag_block<s_sky_patchy_fog> patchy_fog;

	real32 bloom_amount;
	real32 bloom_threshold;
	real32 bloom_brightness;
	real32 bloom_gamma_power;

	tag_block<s_sky_light> lights;

	real32 global_sky_rotation;
	tag_block<s_sky_shader_function> shader_functions;
	tag_block<s_sky_animation> animations;

	int8 pad_3[12];
	real_rgb_color clear_color;
};
TAG_GROUP_SIZE_ASSERT(s_sky_definition, 0xAC);