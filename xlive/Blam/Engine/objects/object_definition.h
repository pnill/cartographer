#pragma once
#include "Blam/Cache/DataTypes/TagBlock.h"
#include "tag_files/tag_reference.h"

#include "Blam/Engine/cache/predicted_resources.h"
#include "Blam/Engine/math/color_math.h"
#include "Blam/Engine/math/function_definitions.h"

#include "Blam/Engine/objects/object_type_list.h"
#include "Blam/Engine/tag_files/string_id.h"

#define k_maximum_object_functions 256
#define k_maximum_number_of_attachments_per_object 16
#define MAXIMUM_WIDGETS_PER_OBJECT 4
#define k_object_change_color_count 4
#define MAXIMUM_NUMBER_OF_OBJECT_CHANGE_COLOR_INITIAL_PERMUTATIONS 32
#define MAXIMUM_NUMBER_OF_OBJECT_CHANGE_COLOR_FUNCTIONS 4

enum e_object_definition_flags : int16
{
	_object_definition_does_not_cast_shadow = FLAG(0),
	_object_definition_search_cardinal_direction_lightmaps_on_failure = FLAG(1),
	_object_definition_unused = FLAG(2),
	_object_definition_not_a_pathfinding_obstacle = FLAG(3),
	_object_definition_extension_of_parent = FLAG(4),	// Object passes all function values to parent and uses parents markers
	_object_definition_does_not_cause_collision_damage = FLAG(5),
	_object_definition_early_mover = FLAG(6),
	_object_definition_early_mover_localized_physics = FLAG(7),
	_object_definition_use_static_massive_lightmap_sample = FLAG(8),
	_object_definition_object_scales_attachments = FLAG(9),
	_object_definition_inherits_players_appearance = FLAG(10),
	_object_definition_dead_bipeds_cant_localize = FLAG(11),
	_object_definition_attach_to_clusters_by_dynamic_sphere = FLAG(12),	// Use this for the mac gun on spacestation
	_object_definition_effects_created_by_this_object_do_not_spawn_objects_in_multiplayer = FLAG(13),
};

enum e_object_definition_runtime_flags : int32
{
	_object_definition_runtime_change_colors_allowed = FLAG(0)
};

enum e_lightmap_shadow_mode : int16
{
	_lightmap_shadow_mode_default = 0,
	_lightmap_shadow_mode_never = 1,
	_lightmap_shadow_mode_always = 2
};

enum e_sweetener_size : int8
{
	_sweetener_size_small = 0,
	_sweetener_size_medium = 1,
	_sweetener_size_large = 2
};

enum e_object_ai_flags : int32
{
	_object_ai_flag_detroyable_cover = FLAG(0),
	_object_ai_flag_pathfinding_ignore_when_dead = FLAG(1),
	_object_ai_flag_dynamic_cover = FLAG(2)
};

enum e_object_ai_size : int16
{
	_object_ai_size_default = 0,
	_object_ai_size_tiny = 1,
	_object_ai_size_small = 2,
	_object_ai_size_medium = 3,
	_object_ai_size_large = 4,
	_object_ai_size_huge = 5,
	_object_ai_size_immobile = 6
};

enum e_object_leap_jump_speed : int16
{
	_object_leap_jump_speed_none = 0,
	_object_leap_jump_speed_down = 1,
	_object_leap_jump_speed_step = 2,
	_object_leap_jump_speed_crouch = 3,
	_object_leap_jump_speed_stand = 4,
	_object_leap_jump_speed_storey = 5,
	_object_leap_jump_speed_tower = 6,
	_object_leap_jump_speed_infinite = 7
};

enum e_object_function_definition_flags : int32
{
	_object_function_definition_invert = FLAG(0),							// Result of function is one minus actual result
	_object_function_definition_mapping_does_not_controls_active = FLAG(1), // The curve mapping can make the function active inactive
	_object_function_definition_always_active = FLAG(2),					// Function does not deactivate when at or below lower bound
	_object_function_definition_random_time_offset = FLAG(3)				// Function offsets periodic function input by random value between 0 and 1 
};

enum e_change_color : int16
{
	_change_color_none = 0,
	_change_color_primary = 1,
	_change_color_secondary = 2,
	_change_color_tertiary = 3,
	_change_color_quaternary = 4
};

enum e_object_change_color_function_scale_flags : int32
{
	_object_change_color_function_scale_flag_blend_in_hsv = FLAG(0),	// Blends colors in hsv rather than rgb space
	_object_change_color_function_scale_flag_more_colors = FLAG(1),		// Blends colors through more hues goes the long way around the color wheel
};

// max count: 1
struct object_ai_properties
{
	e_object_ai_flags ai_flags;
	string_id ai_type_name;				// used for combat dialogue, etc.
	int32 pad;
	e_object_ai_size ai_size;
	e_object_leap_jump_speed leap_jump_speed;
};
TAG_BLOCK_SIZE_ASSERT(object_ai_properties, 16);

// max count: k_maximum_object_functions
struct s_object_function_definition
{
	e_object_function_definition_flags flags;
	string_id import_name;
	string_id export_name;
	string_id turn_off_with;		// if the specified function is off, so is this function
	real32 min_value;				// function must exceed this value (after mapping) to be active 0. means do nothing
	c_function_definition default_function;		// fned
	string_id scale_by;
};
TAG_BLOCK_SIZE_ASSERT(s_object_function_definition, 32);

// max count: k_maximum_number_of_attachments_per_object
struct object_attachment_definition
{
	tag_reference type;
	string_id marker;
	e_change_color change_color;
	int16 pad;
	string_id primary_scale;
	string_id secondary_scale;
};
TAG_BLOCK_SIZE_ASSERT(object_attachment_definition, 24);

// max count: MAXIMUM_WIDGETS_PER_OBJECT
struct object_definition_widget
{
	tag_reference type;
};
TAG_BLOCK_SIZE_ASSERT(object_definition_widget, 8);

// max count: 4
struct s_old_object_function_definition
{
	int8 pad[76];
	string_id unk;
};
TAG_BLOCK_SIZE_ASSERT(s_old_object_function_definition, 80);

// max count: MAXIMUM_NUMBER_OF_OBJECT_CHANGE_COLOR_INITIAL_PERMUTATIONS
struct object_change_color_initial_permutation
{
	real32 weight;
	real_rgb_color color_lower_bound;
	real_rgb_color color_upper_bound;
	string_id variant_name;		// if empty, may be used by any model variant
};
TAG_BLOCK_SIZE_ASSERT(object_change_color_initial_permutation, 32);

// max count: MAXIMUM_NUMBER_OF_OBJECT_CHANGE_COLOR_FUNCTIONS
struct object_change_color_function
{
	int32 pad;
	e_object_change_color_function_scale_flags scale_flags;
	real_rgb_color color_lower_bound;
	real_rgb_color color_upper_bound;
	string_id darken_by;
	string_id scale_by;
};
TAG_BLOCK_SIZE_ASSERT(object_change_color_function, 40);

struct object_change_color_definition
{
	tag_block<object_change_color_initial_permutation> initialPermutations;
	tag_block<object_change_color_function> functions;
};
TAG_BLOCK_SIZE_ASSERT(object_change_color_definition, 16);

struct object_definition
{
	e_object_type object_type;
	int8 pad;
	e_object_definition_flags flags;
	real32 bounding_radius;				// World Units
	real_point3d bounding_offset;
	real32 acceleration_scale;			// marine 1.0, grunt 1.4, elite 0.9, hunter 0.5, etc.

	e_lightmap_shadow_mode lightmap_shadow_mode;
	e_sweetener_size sweetener_size;
	int8 pad1;
	e_object_definition_runtime_flags runtime_flags;

	real32 dynamic_light_sphere_radius;			// sphere to use for dynamic lights and shadows. only used if not 0
	real_point3d dynamic_light_sphere_offset;	// sphere to use for dynamic lights and shadows. only used if not 0
	string_id default_model_variant;
	tag_reference model;						// hlmt
	tag_reference crate_object;					// bloc
	tag_reference modifier_shader;				// shad
	tag_reference creation_effect;				// effe
	tag_reference material_effects;				// foot

	tag_block<object_ai_properties> ai_properties;
	tag_block<s_object_function_definition> functions;

	// Explaination("Applying collision damage", "for things that want to cause more or less collision damage")
	
	real32 apply_collision_damage_scale;	// 0 means 1.  1 is standard scale.  Some things may want to apply more damage
	
	// Explaination("Game collision damage parameters", "0 - means take default value from globals.globals")
	
	real_bounds game_acceleration;	// 0-oo
	real_bounds game_scale;			// 0-oo

	// Explaination("Absolute collision damage parameters", "0 - means take default value from globals.globals")
	
	real_bounds abs_acceleration;	// 0-oo
	real_bounds abs_scale;			// 0-1

	int16 hud_text_message_index;
	int16 pad2;

	tag_block<object_attachment_definition> attachments;
	tag_block<object_definition_widget> widgets;
	tag_block<s_old_object_function_definition> old_functions;
	tag_block<object_change_color_definition> change_colors;
	tag_block<predicted_resource> predicted_resources;
};
TAG_GROUP_SIZE_ASSERT(object_definition, 188);
