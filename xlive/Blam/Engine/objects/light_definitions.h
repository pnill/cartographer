#pragma once
#include "math/function_definitions.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"

enum e_light_placement_flags : short
{
    light_placement_flag_custom_geometry = FLAG(0),
    light_placement_flag_unused = FLAG(1),
    light_placement_flag_cinematicOnly = FLAG(2)
};

enum e_lightmap_type : short
{
    lightmap_type_use_light_tag_setting = 0,
    lightmap_type_dynamic_only = 1,
    lightmap_type_dynamic_with_lightmaps = 2,
    lightmap_type_lightmaps_only = 3
};

enum e_lightmap_flags: short
{
    lightmap_placement_flag_unused = FLAG(0),
};

enum e_light_definition_flags : uint32
{
    _light_definition_no_illumination_dont_cast_any_per_pixel_dynamic_light = 0,
	_light_definition_no_specular_dont_cast_any_specular_highlights = 1,
	_light_definition_force_cast_environment_shadows_through_portals = 2,
	_light_definition_no_shadow_dont_cast_any_stencil_shadows = 3,
	_light_definition_force_frustum_visibility_on_small_light = 4,
	_light_definition_only_render_in_first_person = 5,
	_light_definition_only_render_in_third_person = 6,
	_light_definition_dont_fade_when_invisible = 7,
	_light_definition_multiplayer_override = 8,
	_light_definition_animated_gel = 9,
	_light_definition_only_in_dynamic_envmap = 10,
	_light_definition_ignore_parent_object = 11,
	_light_definition_dont_shadow_parent = 12,
	_light_definition_ignore_all_parents = 13,
	_light_definition_march_milestone_hack = 14,
	_light_definition_force_light_inside_world = 15,
	_light_definition_environment_doesnt_cast_stencil_shadows = 16,
	_light_definition_objects_dont_cast_stencil_shadows = 17,
	_light_definition_first_person_from_camera = 18,
	_light_definition_texture_camera_gel = 19,
	_light_definition_light_framerate_killer = 20,
	_light_definition_allowed_in_split_screen = 21,
	_light_definition_only_on_parent_bipeds = 22,
	k_light_definition_flag_count
};

enum e_light_type : int16
{
	_light_type_sphere = 0,
	_light_type_orthogonal = 1,
	_light_type_projective = 2,
	_light_type_pyramid = 3,
	NUMBER_OF_LIGHT_TYPES
};

enum e_light_definition_shadow_tap_bias : uint16
{
	_light_definition_shadow_3_tap = 0,
	_light_definition_shadow_unused = 1,
	_light_definition_shadow_1_tap = 2
};

enum e_light_definition_interpolation_flags: uint32
{
	// blends colors in hsv rather than rgb space
	_light_definition_interpolation_blend_in_hsv = FLAG(0),

	// blends colors through more hues goes the long way around the color wheel
	_light_definition_interpolation_more_colors = FLAG(1)
};

enum e_light_definition_specular_mask : uint16
{
	_light_definition_specular_mask_default = 0,
	_light_definition_specular_mask_none_no_mask = 1,
	_light_definition_specular_mask_gel_alpha = 2,
	_light_definition_specular_mask_gel_color = 3
};

enum e_light_definition_fall_off_function : uint16
{
	_light_definition_fall_off_function_default = 0,
	_light_definition_fall_off_function_narrow = 1,
	_light_definition_fall_off_function_broad = 2,
	_light_definition_fall_off_function_very_broad = 3,
};

enum e_light_definition_diffuse_contrast : uint16
{
	_light_definition_diffuse_contrast_default_linear = 0,
	_light_definition_diffuse_contrast_high = 1,
	_light_definition_diffuse_contrast_low = 2,
	_light_definition_diffuse_contrast_very_low = 3
};

enum e_light_definition_specular_contrast : uint16
{
	_light_definition_diffuse_specular_contrast_default_one = 0,
	_light_definition_diffuse_specular_contrast_high_linear = 1,
	_light_definition_diffuse_specular_contrast_low = 2,
	_light_definition_diffuse_specular_contrast_very_low = 3
};

enum e_light_definition_fall_off_geometry : uint16
{
	_light_definition_fall_off_geometry_default = 0,
	_light_definition_fall_off_geometry_directional = 1,
	_light_definition_fall_off_geometry_spherical = 2 
};

enum e_light_definition_default_lightmap_setting : uint16
{
	_light_definitions_default_lightmap_dynamic_only = 0,
	_light_definitions_default_lightmap_dynamic_with_lightmaps = 1,
	_light_definitions_default_lightmap_lightmaps_only
};

enum e_light_definition_effect_fall_off_function : uint16
{
	_light_definition_effect_fall_off_function_linear = 0,
	_light_definition_effect_fall_off_function_late = 1,
	_light_definition_effect_fall_off_function_very_late = 2,
	_light_definition_effect_fall_off_function_early = 3,
	_light_definition_effect_fall_off_function_very_early = 4,
	_light_definition_effect_fall_off_function_cosine = 5,
	_light_definition_effect_fall_off_function_zero = 6,
	_light_definition_effect_fall_off_function_one = 7
};

enum e_light_definition_distance_fade : uint16
{
	_light_definition_distance_fade_very_far = 0,
	_light_definition_distance_fade_far = 1,
	_light_definition_distance_fade_medium = 2,
	_light_definition_distance_fade_close = 3,
	_light_definition_distance_fade_very_close = 4
};

enum e_light_definition_animation_flags : uint32
{
	_light_definition_animation_synchronized = FLAG(0)
};

struct s_scenario_light_datum
{
    e_light_type type;
    e_light_placement_flags flags;
	e_lightmap_type lightmap_type;
    e_lightmap_flags lightmap_flags;
    float lightmap_half_life;
    float lightmap_light_scale;
    real_point3d target_point;
    float width_world_units;
    float height_scale_world_units;
    float field_of_view_degrees;
    float falloff_distance_world_units;
    float cutoff_distance_world_units_from_far_plane;
};
ASSERT_STRUCT_SIZE(s_scenario_light_datum, 48);

struct light_definition
{
	c_flags<e_light_definition_flags, uint32, k_light_definition_flag_count> flags;
	e_light_type type;
	int16 pad_1;

	/// how the light's size changes with external scale
	real_bounds size_modifier;

	/// larger positive numbers improve quality, larger negative numbers improve speed
	real32 shadow_quality_bias;

	/// the less taps you use, the faster the light (but edges can look worse)
	e_light_definition_shadow_tap_bias shadow_tap_bias;

	int16 pad_2;

	// Explaination("SPHERE LIGHT", "default shape parameters for spherical lights")
	/// the radius at which illumination falls off to zero
	real32 radius_world_units;

	/// the radius at which specular highlights fall off to zero (if zero, same as maximum radius)
	real32 specular_radius_world_units;

	// Explaination("FRUSTUM LIGHT", "default shape parameters for frustum lights (orthogonal, projective or pyramid types)")
	/// width of the frustum light at its near plane
	real32 near_width_world_units;

	/// how much the gel is stretched vertically (0.0 or 1.0 = aspect ratio same as gel)
	real32 height_stretch;

	/// horizontal angle that the frustum light covers (0.0 = no spread, a parallel beam)
	real32 field_of_view_degrees;

	/// distance from near plane to where the light falloff starts
	real32 fall_off_distance;

	/// distance from near plane to where illumination falls off to zero
	real32 cut_off_distance;

	// Explaination("COLOR", "EMPTY STRING")
	e_light_definition_interpolation_flags interpolation_flags;

	real_bounds bloom_bounds;
	real_rgb_color specular_lower_bound;
	real_rgb_color specular_upper_bound;
	real_rgb_color diffuse_lower_bound;
	real_rgb_color diffuse_upper_bound;
	real_bounds brightness_bounds;

	// Explaination("GEL", "the gel map tints the light's illumination per-pixel")
	/// must be a cubemap for spherical light and a 2d texture for frustum light
	tag_reference gel_map; // bitm

	e_light_definition_specular_mask specular_mask;

	int8 pad_3[6];

	// Explaination("FALLOFF", "EMPTY STRING")
	e_light_definition_fall_off_function fall_off_function;
	e_light_definition_diffuse_contrast diffuse_contrast;
	e_light_definition_specular_contrast specular_contrast;
	e_light_definition_fall_off_geometry fall_off_geometry;

	// Explaination("LENS FLARE", "optional lens flare and light volume associated with this light")
	tag_reference lens_flare; // lens

	/// used to generate a bounding radius for lensflare-only lights
	real32 bounding_radius_world_units;

	tag_reference light_volume; // MSG2

	// Explaination("RADIOSITY", "how the light affects the lightmaps (ignored for dynamic lights)")
	e_light_definition_default_lightmap_setting lightmap_setting;

	int16 pad_4;

	real32 lightmap_half_light;
	real32 lightmap_light_scale;

	// Explaination("EFFECT PARAMETERS", "if the light is created by an effect, it will animate itself as follows")
	/// the light will last this long when created by an effect
	real32 duration_seconds;

	int16 pad_5;

	/// the scale of the light will diminish over time according to this function
	e_light_definition_effect_fall_off_function effect_fall_off_function;

	// Explaination("DISTANCE FADING PARAMETERS", "To fade the light's illumination and shadow casting abilities")
	e_light_definition_distance_fade illumination_fade;
	e_light_definition_distance_fade shadow_fade;
	e_light_definition_distance_fade specular_fade;

	int16 pad_6;

	// Explaination("ANIMATION PARAMETERS", "EMPTY STRING")
	e_light_definition_animation_flags animation_flags;
	tag_block<c_function_definition> brightness_animation;
	tag_block<c_function_definition> color_animation;
	tag_block<c_function_definition> gel_animation;

	// Explaination("SHADER", "EMPTY STRING")
	tag_reference shader; // shad
};
ASSERT_STRUCT_SIZE(light_definition, 228);

/* public code */

static light_definition* current_light_definition_get(void)
{
	return *Memory::GetAddress<light_definition**>(0xA3E234);
}

static e_light_type current_lght_type_get(void)
{
	return *Memory::GetAddress<e_light_type*>(0xA3E238);
}

static bool light_type_is_spherical(e_light_type light_type)
{
	ASSERT(VALID_INDEX(light_type, NUMBER_OF_LIGHT_TYPES));
	return light_type == _light_type_sphere;
}
