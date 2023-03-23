#pragma once
#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Cache/TagGroups.hpp"
#include "Blam/Engine/objects/objects.h"
#include "Blam/Math/BlamMath.h"

/*********************************************************************
* name: object
* group_tag : obje
* header size : 188
* *********************************************************************/

#pragma pack(push,1)
struct s_object_group_definition :TagGroup<'obje'>
{
	WORD object_type;
	enum e_object_flags : short
	{
		does_not_cast_shadow = FLAG(0),
		search_cardinal_direction_lightmaps_on_failure = FLAG(1),
		unused = FLAG(2),
		not_a_pathfinding_obstacle = FLAG(3),
		extension_of_parent = FLAG(4),
		does_not_cause_collision_damage = FLAG(5),
		early_mover = FLAG(6),
		early_mover_localized_physics = FLAG(7),
		use_static_massive_lightmap_sample = FLAG(8),
		object_scales_attachments = FLAG(9),
		inherits_players_appearance = FLAG(10),
		dead_bipeds_cant_localize = FLAG(11),
		attach_to_clusters_by_dynamic_sphere = FLAG(12),
		effects_created_by_this_object_do_not_spawn_objects_in_multiplayer = FLAG(13),
	};
	e_object_flags object_flags;
	float bounding_radius;
	real_point3d bouding_offset;
	float acceleration_scale;
	enum e_lightmap_shadow_mode : short
	{
		_lightmap_shadow_mode_default = 0,
		_lightmap_shadow_mode_never = 1,
		_lightmap_shadow_mode_always = 2,
	};
	e_lightmap_shadow_mode lightmap_shadow_mode;
	enum e_sweetener_size : byte
	{
		_sweetener_size_small = 0,
		_sweetener_size_medium = 1,
		_sweetener_size_large = 2,
	};
	e_sweetener_size sweetener_size;
	byte unk1B;
	enum e_object_group_runtime_flags : int
	{
		e_object_group_runtime_runtime_change_colors_allowed = FLAG(0),
		e_object_group_runtime_bit_1 = FLAG(1),
		e_object_group_runtime_bit_2 = FLAG(2),
		e_object_group_runtime_bit_3 = FLAG(3),
		e_object_group_runtime_bit_4 = FLAG(4),
		e_object_group_runtime_bit_5 = FLAG(5),
		e_object_group_runtime_bit_6 = FLAG(6),
		e_object_group_runtime_bit_7 = FLAG(7),
		e_object_group_runtime_bit_8 = FLAG(8),
		e_object_group_runtime_bit_9 = FLAG(9),
		e_object_group_runtime_bit_10 = FLAG(10),
		e_object_group_runtime_bit_11 = FLAG(11),
		e_object_group_runtime_bit_12 = FLAG(12),
		e_object_group_runtime_bit_13 = FLAG(13),
		e_object_group_runtime_bit_14 = FLAG(14),
		e_object_group_runtime_bit_15 = FLAG(15),
		e_object_group_runtime_bit_16 = FLAG(16),
		e_object_group_runtime_bit_17 = FLAG(17),
		e_object_group_runtime_bit_18 = FLAG(18),
		e_object_group_runtime_bit_19 = FLAG(19),
		e_object_group_runtime_bit_20 = FLAG(20),
		e_object_group_runtime_bit_21 = FLAG(21),
		e_object_group_runtime_bit_22 = FLAG(22),
		e_object_group_runtime_bit_23 = FLAG(23),
		e_object_group_runtime_bit_24 = FLAG(24),
		e_object_group_runtime_bit_25 = FLAG(25),
		e_object_group_runtime_bit_26 = FLAG(26),
		e_object_group_runtime_bit_27 = FLAG(27),
		e_object_group_runtime_bit_28 = FLAG(28),
		e_object_group_runtime_bit_29 = FLAG(29),
		e_object_group_runtime_bit_30 = FLAG(30),
		e_object_group_runtime_bit_31 = FLAG(31),
	};
	e_object_group_runtime_flags runtime_flags;
	float dynamic_light_sphere_radius;
	real_point3d dynamic_light_sphere_offset;
	string_id default_model_variant;
	tag_reference model;
	tag_reference crate_object;
	tag_reference modifier_shader;
	tag_reference creation_effect;
	tag_reference material_effects;
	struct s_ai_properties_block
	{
		enum e_ai_flags : int
		{
			_ai_flags_detroyable_cover_bit = FLAG(0),
			_ai_flags_pathfinding_ignore_when_dead_bit = FLAG(1),
			_ai_flags_dynamic_cover_bit = FLAG(2),
		};
		e_ai_flags ai_flags;
		string_id ai_type_name;
		DWORD unk8;
		enum e_ai_size : short
		{
			_ai_size_default = 0,
			_ai_size_tiny = 1,
			_ai_size_small = 2,
			_ai_size_medium = 3,
			_ai_size_large = 4,
			_ai_size_huge = 5,
			_ai_size_immobile = 6,
		};
		e_ai_size ai_size;
		enum e_leap_jump_speed : short
		{
			_leap_jump_speed_none = 0,
			_leap_jump_speed_down = 1,
			_leap_jump_speed_step = 2,
			_leap_jump_speed_crouch = 3,
			_leap_jump_speed_stand = 4,
			_leap_jump_speed_storey = 5,
			_leap_jump_speed_tower = 6,
			_leap_jump_speed_infinite = 7,
		};
		e_leap_jump_speed leap_jump_speed;
	};
	TAG_BLOCK_SIZE_ASSERT(s_ai_properties_block, 0x10);
	tag_block<s_ai_properties_block> ai_properties;
	struct s_functions_block
	{
		enum class e_object_functions_flags : int
		{
			invert = FLAG(0),
			mapping_does_not_controls_active = FLAG(1),
			always_active = FLAG(2),
			random_time_offset = FLAG(3),
		};
		e_object_functions_flags flags;
		string_id import_name;
		string_id export_name;
		string_id turn_off_with;
		float min_value;

		data_block data;
		string_id scale_by;
	};
	TAG_BLOCK_SIZE_ASSERT(s_functions_block, 0x20);
	tag_block<s_functions_block> functions;

	float apply_collision_damage_scale;
	float min_game_acc_default;
	float max_game_acc_default;
	float min_game_scale_default;
	float max_game_scale_default;
	float min_abs_acc_default;
	float max_abs_acc_default;
	float min_abs_scale_default;
	float max_abs_scale_default;
	short hud_text_message_index;
	WORD unk_92;
	struct s_attachments_block
	{
		tag_reference type;
		string_id marker_old_string_id;
		enum e_attachments_change_color : short
		{
			_attachments_change_color_none = 0,
			_attachments_change_color_primary = 1,
			_attachments_change_color_secondary = 2,
			_attachments_change_color_tertiary = 3,
			_attachments_change_color_quaternary = 4,
		};
		e_attachments_change_color change_color;
		WORD unkE;
		string_id primary_scale;
		string_id secondary_scale;
	};
	TAG_BLOCK_SIZE_ASSERT(s_attachments_block, 0x18);
	tag_block<s_attachments_block> attachments;
	struct s_widgets_block
	{
		tag_reference type;
	};
	TAG_BLOCK_SIZE_ASSERT(s_widgets_block, 0x8);
	tag_block<s_widgets_block> widgets;
	struct s_old_functions_block
	{
		DWORD unk0[19];
		string_id old_string_id;
	};
	TAG_BLOCK_SIZE_ASSERT(s_old_functions_block, 0x50);
	tag_block<s_old_functions_block> old_functions;
	struct s_change_colors_block
	{
		struct s_change_colors_block_initial_permutations_block
		{
			float weight;
			real_color_rgb color_lower_bound;
			real_color_rgb color_upper_bound;
			string_id variant_name;
		};
		TAG_BLOCK_SIZE_ASSERT(s_change_colors_block_initial_permutations_block, 0x20);
		tag_block<s_change_colors_block_initial_permutations_block> initial_permutations;
		struct s_change_colors_functions_block
		{
			unsigned long next_function_block;	// This value is stupid and dumb
			enum e_colors_function_scale_flags : int
			{
				_colors_function_scale_flags_blend_in_hsv_bit = FLAG(0),
				_colors_function_scale_flags_more_colors_bit = FLAG(1),
			};
			e_colors_function_scale_flags scale_flags;
			real_color_rgb color_lower_bound;
			real_color_rgb color_upper_bound;
			string_id darken_by;
			string_id scale_by;
		};
		TAG_BLOCK_SIZE_ASSERT(s_change_colors_functions_block, 0x28);
		tag_block<s_change_colors_functions_block> functions;
	};
	TAG_BLOCK_SIZE_ASSERT(s_change_colors_block, 0x10);
	tag_block<s_change_colors_block> change_colors;
	struct s_predicted_resources_block
	{
		enum e_predicted_resource_type : short
		{
			_predicted_resource_type_bitmap_bit = 0,
			_predicted_resource_type_sound_bit = 1,
			_predicted_resource_type_render_model_geometry_bit = 2,
			_predicted_resource_type_cluster_geometry_bit = 3,
			_predicted_resource_type_cluster_instanced_geometry_bit = 4,
			_predicted_resource_type_lightmap_geometry_object_buckets_bit = 5,
			_predicted_resource_type_lightmap_geometry_instance_buckets_bit = 6,
			_predicted_resource_type_lightmap_cluster_bitmaps_bit = 7,
			_predicted_resource_type_lightmap_instance_bitmaps_bit = 8,
		};
		e_predicted_resource_type type;
		short resource_index;
		datum tag_index;
	};
	TAG_BLOCK_SIZE_ASSERT(s_predicted_resources_block, 0x8);
	tag_block<s_predicted_resources_block> predicted_resources;
};
TAG_GROUP_SIZE_ASSERT(s_object_group_definition, 0xBC);
#pragma pack(pop)

