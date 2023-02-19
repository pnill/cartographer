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
	enum e_object_flags : __int16
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
	e_object_flags object_flags;//0x2
	float bounding_radius;//0x4
	float bounding_offset_x;//0x8
	float bounding_offset_y;//0xC
	float bounding_offset_z;//0x10
	float acceleration_scale;//0x14
	enum class e_lightmap_shadow_mode : __int16
	{
		default = 0,
		never = 1,
		always = 2,
	};
	e_lightmap_shadow_mode lightmap_shadow_mode;//0x18
	enum class e_sweetener_size : __int8
	{
		_small = 0,
		medium = 1,
		large = 2,
	};
	e_sweetener_size sweetener_size;//0x1A
	byte unk1B;
	enum e_object_group_runtime_flags : __int32
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
	float dynamic_light_sphere_radius;//0x20
	real_point3d dynamic_light_sphere_offset;//0x24
	string_id default_model_variant;//0x30
	tag_reference model;//0x34
	tag_reference crate_object;//0x3C
	tag_reference modifier_shader;//0x44
	tag_reference creation_effect;//0x4C
	tag_reference material_effects;//0x54
	struct s_ai_properties_block
	{
		enum class e_ai_flags : __int32
		{
			detroyable_cover = FLAG(0),
			pathfinding_ignore_when_dead = FLAG(1),
			dynamic_cover = FLAG(2),
		};
		e_ai_flags ai_flags;//0x0
		string_id ai_type_name;//0x4
		DWORD unk8;
		enum class e_ai_size : __int16
		{
			default = 0,
			tiny = 1,
			_small = 2,
			medium = 3,
			large = 4,
			huge = 5,
			immobile = 6,
		};
		e_ai_size ai_size;//0xC
		enum class e_leap_jump_speed : __int16
		{
			none = 0,
			down = 1,
			step = 2,
			crouch = 3,
			stand = 4,
			storey = 5,
			tower = 6,
			infinite = 7,
		};
		e_leap_jump_speed leap_jump_speed;//0xE
	};
	TAG_BLOCK_SIZE_ASSERT(s_ai_properties_block, 0x10);
	tag_block<s_ai_properties_block> ai_properties;//0x5C
	struct s_functions_block
	{
		enum class e_object_functions_flags : __int32
		{
			invert = FLAG(0),
			mapping_does_not_controls_active = FLAG(1),
			always_active = FLAG(2),
			random_time_offset = FLAG(3),
		};
		e_object_functions_flags flags;//0x0
		string_id import_name;//0x4
		string_id export_name;//0x8
		string_id turn_off_with;//0xC
		float min_value;//0x10

		data_block data;//0x14
		string_id scale_by;//0x1C
	};
	TAG_BLOCK_SIZE_ASSERT(s_functions_block, 0x20);
	tag_block<s_functions_block> functions;//0x64

	float apply_collision_damage_scale;//0x6C
	float min_game_acc_default;//0x70
	float max_game_acc_default;//0x74
	float min_game_scale_default;//0x78
	float max_game_scale_default;//0x7C
	float min_abs_acc_default;//0x80
	float max_abs_acc_default;//0x84
	float min_abs_scale_default;//0x88
	float max_abs_scale_default;//0x8C
	__int16 hud_text_message_index;//0x90
	PAD(0x2);//0x92
	struct s_attachments_block
	{
		tag_reference type;//0x0
		string_id marker_old_string_id;//0x8
		enum class e_attachments_change_color : __int16
		{
			none = 0,
			primary = 1,
			secondary = 2,
			tertiary = 3,
			quaternary = 4,
		};
		e_attachments_change_color change_color;//0xC
		PAD(0x2);//0xE
		string_id primary_scale;//0x10
		string_id secondary_scale;//0x14
	};
	TAG_BLOCK_SIZE_ASSERT(s_attachments_block, 0x18);
	tag_block<s_attachments_block> attachments;//0x94
	struct s_widgets_block
	{
		tag_reference type;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_widgets_block, 0x8);
	tag_block<s_widgets_block> widgets;//0x9C
	struct s_old_functions_block
	{
		PAD(0x4C);//0x0
		string_id old_string_id;//0x4C
	};
	TAG_BLOCK_SIZE_ASSERT(s_old_functions_block, 0x50);
	tag_block<s_old_functions_block> old_functions;//0xA4
	struct s_change_colors_block
	{
		struct s_change_colors_block_initial_permutations_block
		{
			float weight;//0x0
			real_color_rgb color_lower_bound;//0x4
			real_color_rgb color_upper_bound;//0x10
			string_id variant_name;//0x1C
		};
		TAG_BLOCK_SIZE_ASSERT(s_change_colors_block_initial_permutations_block, 0x20);
		tag_block<s_change_colors_block_initial_permutations_block> initial_permutations;//0x0
		struct s_change_colors_functions_block
		{
			unsigned long next_function_block;	// This value is stupid and dumb
			enum e_colors_function_scale_flags : __int32
			{
				blend_in_hsv = FLAG(0),
				more_colors = FLAG(1),
			};
			e_colors_function_scale_flags scale_flags;//0x4
			real_color_rgb color_lower_bound;//0x8
			real_color_rgb color_upper_bound;//0x14
			string_id darken_by;//0x20
			string_id scale_by;//0x24
		};
		TAG_BLOCK_SIZE_ASSERT(s_change_colors_functions_block, 0x28);
		tag_block<s_change_colors_functions_block> functions;//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_change_colors_block, 0x10);
	tag_block<s_change_colors_block> change_colors;//0xAC
	struct s_predicted_resources_block
	{
		enum class e_predicted_resource_type : __int16
		{
			bitmap = 0,
			sound = 1,
			render_model_geometry = 2,
			cluster_geometry = 3,
			cluster_instanced_geometry = 4,
			lightmap_geometry_object_buckets = 5,
			lightmap_geometry_instance_buckets = 6,
			lightmap_cluster_bitmaps = 7,
			lightmap_instance_bitmaps = 8,
		};
		e_predicted_resource_type type;//0x0
		__int16 resource_index;//0x2
		datum tag_index;//0x4
	};
	TAG_BLOCK_SIZE_ASSERT(s_predicted_resources_block, 0x8);
	tag_block<s_predicted_resources_block> predicted_resources;//0xB4
};
TAG_GROUP_SIZE_ASSERT(s_object_group_definition, 0xBC);
#pragma pack(pop)

