#pragma once
#include "Blam/Cache/DataTypes.h"
#include "Blam/Common/Common.h"
#include "Blam/Cache/DataTypes/TagBlock.h"
#include "Blam/Cache/DataTypes/TagRef.h"
#include "Blam/Maths/real_math.h"

/*********************************************************************
* name: light
* group_tag : ligh
* header size : 228
* *********************************************************************/
struct s_light_group_definition
{
	enum class e_flags : __int32
	{
		no_illumination = FLAG(0),
		no_specular = FLAG(1),
		force_cast_environment_shadows_through_portals = FLAG(2),
		no_shadow = FLAG(3),
		force_frustum_visibility_on_small_light = FLAG(4),
		only_render_in_first_person = FLAG(5),
		only_render_in_third_person = FLAG(6),
		dont_fade_when_invisible = FLAG(7),
		multiplayer_override = FLAG(8),
		animated_gel = FLAG(9),
		only_in_dynamic_envmap = FLAG(10),
		ignore_parent_object = FLAG(11),
		dont_shadow_parent = FLAG(12),
		ignore_all_parents = FLAG(13),
		march_milestone_hack = FLAG(14),
		force_light_inside_world = FLAG(15),
		environment_doesnt_cast_stencil_shadows = FLAG(16),
		objects_dont_cast_stencil_shadows = FLAG(17),
		first_person_from_camera = FLAG(18),
		texture_camera_gel = FLAG(19),
		light_framerate_killer = FLAG(20),
		allowed_in_split_screen = FLAG(21),
		only_on_parent_bipeds = FLAG(22),
	};
	e_flags flags;//0x0
	enum class e_type : __int16
	{
		sphere = 0,
		orthogonal = 1,
		projective = 2,
		pyramid = 3,
	};
	e_type type;//0x4
	PAD(0x2);//0x6
	float size_modifer;//0x8
	float size_modifer_2;//0xC
	float shadow_quality_bias;//0x10
	enum class e_shadow_tap_bias : __int16
	{
		NUM_3_tap = 0,
		unused = 1,
		NUM_1_tap = 2,
	};
	e_shadow_tap_bias shadow_tap_bias;//0x14
	PAD(0x2);//0x16
	float radius;//0x18
	float specular_radius;//0x1C
	float near_width;//0x20
	float height_stretch;//0x24
	angle field_of_view;//0x28
	float falloff_distance;//0x2C
	float cutoff_distance;//0x30
	enum class e_interpolation_flags : __int32
	{
		blend_in_hsv = FLAG(0),
		more_colors = FLAG(1),
	};
	e_interpolation_flags interpolation_flags;//0x34
	float bloom_bounds02_lower;//0x38
	float bloom_bound02_upper;//0x3C
	real_color_rgb specular_lower_bound;//0x40
	real_color_rgb specular_upper_bound;//0x4C
	real_color_rgb diffuse_lower_bound;//0x58
	real_color_rgb diffuse_upper_bound;//0x64
	float brightness_bounds02_lower;//0x70
	float brightness_bounds_02_upper;//0x74
	tag_reference gel_map;//0x78
	enum class e_specular_mask : __int16
	{
		default = 0,
		none_no_mask = 1,
		gel_alpha = 2,
		gel_color = 3,
	};
	e_specular_mask specular_mask;//0x80
	PAD(0x6);//0x82
	enum class e_falloff_function : __int16
	{
		default = 0,
		narrow = 1,
		broad = 2,
		very_broad = 3,
	};
	e_falloff_function falloff_function;//0x88
	enum class e_diffuse_contrast : __int16
	{
		default_linear = 0,
		high = 1,
		low = 2,
		very_low = 3,
	};
	e_diffuse_contrast diffuse_contrast;//0x8A
	enum class e_specular_contrast : __int16
	{
		default_one = 0,
		high_linear = 1,
		low = 2,
		very_low = 3,
	};
	e_specular_contrast specular_contrast;//0x8C
	enum class e_falloff_geometry : __int16
	{
		default = 0,
		directional = 1,
		spherical = 2,
	};
	e_falloff_geometry falloff_geometry;//0x8E
	tag_reference lens_flare;//0x90
	float bounding_radius;//0x98
	tag_reference light_volume;//0x9C
	enum class e_default_lightmap_setting : __int16
	{
		dynamic_only = 0,
		dynamic_with_lightmaps = 1,
		lightmaps_only = 2,
	};
	e_default_lightmap_setting default_lightmap_setting;//0xA4
	PAD(0x2);//0xA6
	float lightmap_half_life;//0xA8
	float lightmap_light_scale;//0xAC
	float duration;//0xB0
	PAD(0x2);//0xB4
	enum class e_falloff_function_2 : __int16
	{
		linear = 0,
		late = 1,
		very_late = 2,
		early = 3,
		very_early = 4,
		cosine = 5,
		zero = 6,
		one = 7,
	};
	e_falloff_function_2 falloff_function_2;//0xB6
	enum class e_illumination_fade : __int16
	{
		fade_very_far = 0,
		fade_far = 1,
		fade_medium = 2,
		fade_close = 3,
		fade_very_close = 4,
	};
	e_illumination_fade illumination_fade;//0xB8
	enum class e_shadow_fade : __int16
	{
		fade_very_far = 0,
		fade_far = 1,
		fade_medium = 2,
		fade_close = 3,
		fade_very_close = 4,
	};
	e_shadow_fade shadow_fade;//0xBA
	enum class e_specular_fade : __int16
	{
		fade_very_far = 0,
		fade_far = 1,
		fade_medium = 2,
		fade_close = 3,
		fade_very_close = 4,
	};
	e_specular_fade specular_fade;//0xBC
	PAD(0x2);//0xBE
	enum class e_flags_2 : __int32
	{
		synchronized = FLAG(0),
	};
	e_flags_2 flags_2;//0xC0
	struct s_brightness_animation_block
	{
		struct s_data_block
		{
			__int8 value;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_data_block, 0x1);
		tag_block<s_data_block> data;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_brightness_animation_block, 0x8);
	tag_block<s_brightness_animation_block> brightness_animation;//0xC4
	struct s_color_animation_block
	{
		struct s_data_block
		{
			__int8 value;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_data_block, 0x1);
		tag_block<s_data_block> data;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_color_animation_block, 0x8);
	tag_block<s_color_animation_block> color_animation;//0xCC
	struct s_gel_animation_block
	{
		struct s_data_block
		{
			__int8 value;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_data_block, 0x1);
		tag_block<s_data_block> data;//0x0
		tag_block<s_data_block> data_2;//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_gel_animation_block, 0x10);
	tag_block<s_gel_animation_block> gel_animation;//0xD4
	tag_reference shader;//0xDC
};
TAG_GROUP_SIZE_ASSERT(s_light_group_definition, 0xE4);