#pragma once
#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam\Cache\TagGroups.hpp"
#include "Blam\Math\BlamMath.h"

/*********************************************************************
* name: scenario
* group_tag : scnr
* header size : 992
* *********************************************************************/

#pragma pack(push,1)
struct s_scenario_group_definition :TagGroup<'scnr'>
{
	tag_reference do_not_use;//0x0
	struct s_skies_block
	{
		tag_reference sky;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_skies_block, 0x8);
	tag_block<s_skies_block> skies;//0x8
	enum class e_type : __int16
	{
		single_player = 0,
		multiplayer = 1,
		main_menu = 2,
		multiplayer_shared = 3,
		single_player_shared = 4,
	};
	e_type type;//0x10
	enum class e_flags : __int16
	{
		cortana_hack = FLAG(0),
		always_draw_sky = FLAG(1),
		dont_strip_pathfinding = FLAG(2),
		symmetric_multiplayer_map = FLAG(3),
		quickloading_cinematiconly_scenario = FLAG(4),
		characters_use_previous_mission_weapons = FLAG(5),
		lightmaps_smooth_palettes_with_neighbors = FLAG(6),
		snap_to_white_at_start = FLAG(7),
	};
	e_flags flags;//0x12
	struct s_child_scenarios_block
	{
		tag_reference child_scenario;//0x0
		PAD(0x10);//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_child_scenarios_block, 0x18);
	tag_block < s_child_scenarios_block> child_scenarios;//0x14
	angle local_north;//0x1C
	struct s_predicted_resources_block
	{
		enum class e_type : __int16
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
		e_type type;//0x0
		__int16 resource_index;//0x2
		__int32 tag_index;//0x4
	};
	TAG_BLOCK_SIZE_ASSERT(s_predicted_resources_block, 0x8);
	tag_block<s_predicted_resources_block> predicted_resources;//0x20
	struct s_functions_block
	{
		enum class e_flags : __int32
		{
			scripted = FLAG(0),
			invert = FLAG(1),
			additive = FLAG(2),
			always_active = FLAG(3),
		};
		e_flags flags;//0x0
		tag_string32 name;//0x4
		float period;//0x24
		__int16 scale_period_by;//0x28
		enum class e_function : __int16
		{
			one = 0,
			zero = 1,
			cosine = 2,
			cosine_variable_period = 3,
			diagonal_wave = 4,
			diagonal_wave_variable_period = 5,
			slide = 6,
			slide_variable_period = 7,
			noise = 8,
			jitter = 9,
			wander = 10,
			spark = 11,
		};
		e_function function;//0x2A
		__int16 scale_function_by;//0x2C
		enum class e_wobble_function : __int16
		{
			one = 0,
			zero = 1,
			cosine = 2,
			cosine_variable_period = 3,
			diagonal_wave = 4,
			diagonal_wave_variable_period = 5,
			slide = 6,
			slide_variable_period = 7,
			noise = 8,
			jitter = 9,
			wander = 10,
			spark = 11,
		};
		e_wobble_function wobble_function;//0x2E
		float wobble_period;//0x30
		float wobble_magnitude;//0x34
		float square_wave_threshold;//0x38
		__int16 step_count;//0x3C
		enum class e_map_to : __int16
		{
			linear = 0,
			early = 1,
			very_early = 2,
			late = 3,
			very_late = 4,
			cosine = 5,
			one = 6,
			zero = 7,
		};
		e_map_to map_to;//0x3E
		__int16 sawtooth_count;//0x40
		PAD(0x2);//0x42
		__int16 scale_result_by;//0x44
		enum class e_bounds_mode : __int16
		{
			clip = 0,
			clip_and_normalize = 1,
			scale_to_fit = 2,
		};
		e_bounds_mode bounds_mode;//0x46
		float bounds_lower;//0x48
		float bounds_upper;//0x4C
		PAD(0x6);//0x50
		__int16 turn_off_with;//0x56
		PAD(0x20);//0x58
	};
	TAG_BLOCK_SIZE_ASSERT(s_functions_block, 0x78);
	tag_block<s_functions_block> functions;//0x28
	data_block editor_scenario_data;//0x30
	struct s_comments_block
	{
		float position_x;//0x0
		float position_y;//0x4
		float position_z;//0x8
		enum class e_type : __int32
		{
			generic = 0,
		};
		e_type type;//0xC
		tag_string32 name;//0x10
		tag_string256 comment;//0x30
	};
	TAG_BLOCK_SIZE_ASSERT(s_comments_block, 0x130);
	tag_block<s_comments_block> comments;//0x38
	struct s_dont_use_environment_objects__block
	{
		__int16 bsp;//0x0
		__int16 unk;//0x2
		__int32 unique_id;//0x4
		PAD(0x4);//0x8
		unsigned __int32 object_definition_tag;//0xC
		__int32 object;//0x10
		PAD(0x2C);//0x14
	};
	TAG_BLOCK_SIZE_ASSERT(s_dont_use_environment_objects__block, 0x40);
	tag_block<s_dont_use_environment_objects__block> dont_use_environment_objects;//0x40
	struct s_object_names_block
	{
		tag_string32 name;//0x0
		enum class e_type : __int16
		{
			biped = 0,
			vehicle = 1,
			weapon = 2,
			equipment = 3,
			garbage = 4,
			projectile = 5,
			scenery = 6,
			machine = 7,
			control = 8,
			light_fixture = 9,
			sound_scenery = 10,
			crate = 11,
			creature = 12,
		};
		e_type type;//0x20
		__int16 placement_index;//0x22
	};
	TAG_BLOCK_SIZE_ASSERT(s_object_names_block, 0x24);
	tag_block<s_object_names_block> object_names;//0x48
	struct s_scenery_block
	{
		__int16 type;//0x0
		__int16 name;//0x2
		enum class e_placement_flags : __int32
		{
			not_automatically = FLAG(0),
			unused = FLAG(1),
			unused1 = FLAG(2),
			unused2 = FLAG(3),
			lock_type_to_env_object = FLAG(4),
			lock_transform_to_env_object = FLAG(5),
			never_placed = FLAG(6),
			lock_name_to_env_object = FLAG(7),
			create_at_rest = FLAG(8),
		};
		e_placement_flags placement_flags;//0x4
		float position_x;//0x8
		float position_y;//0xC
		float position_z;//0x10
		angle rotation_yaw;//0x14
		angle rotation_pitch;//0x18
		angle rotation_roll;//0x1C
		float scale;//0x20
		enum class e_transform_flags : __int16
		{
			mirrored = FLAG(0),
		};
		e_transform_flags transform_flags;//0x24
		__int16 manual_bsp_flags;//0x26
		__int32 unique_id;//0x28
		__int16 origin_bsp_index;//0x2C
		enum class e_type : __int8
		{
			biped = 0,
			vehicle = 1,
			weapon = 2,
			equipment = 3,
			garbage = 4,
			projectile = 5,
			scenery = 6,
			machine = 7,
			control = 8,
			light_fixture = 9,
			sound_scenery = 10,
			crate = 11,
			creature = 12,
		};
		e_type type1;//0x2E
		enum class e_source : __int8
		{
			structure = 0,
			editor = 1,
			dynamic = 2,
			legacy = 3,
		};
		e_source source;//0x2F
		enum class e_bsp_policy : __int8
		{
			default = 0,
			always_placed = 1,
			manual_bsp_placement = 2,
		};
		e_bsp_policy bsp_policy;//0x30
		PAD(0x1);//0x31
		__int16 editor_folder;//0x32
		string_id variant_name;//0x34
		enum class e_active_change_colors : __int32
		{
			primary = FLAG(0),
			secondary = FLAG(1),
			tertiary = FLAG(2),
			quaternary = FLAG(3),
		};
		e_active_change_colors active_change_colors;//0x38
		PAD(0x10);//0x3C
		enum class e_pathfinding_policy : __int16
		{
			tag_default = 0,
			pathfinding_dynamic = 1,
			pathfinding_cutout = 2,
			pathfinding_static = 3,
			pathfinding_none = 4,
		};
		e_pathfinding_policy pathfinding_policy;//0x4C
		enum class e_lightmapping_policy : __int16
		{
			tag_default = 0,
			dynamic = 1,
			pervertex = 2,
		};
		e_lightmapping_policy lightmapping_policy;//0x4E
		struct s_pathfinding_references_block
		{
			__int16 bsp_index;//0x0
			__int16 pathfinding_object_index;//0x2
		};
		TAG_BLOCK_SIZE_ASSERT(s_pathfinding_references_block, 0x4);
		tag_block<s_pathfinding_references_block> pathfinding_references;//0x50
		PAD(0x2);//0x58
		enum class e_valid_multiplayer_games : __int16
		{
			capture_the_flag = FLAG(0),
			slayer = FLAG(1),
			oddball = FLAG(2),
			king_of_the_hill = FLAG(3),
			juggernaut = FLAG(4),
			territories = FLAG(5),
			assault = FLAG(6),
		};
		e_valid_multiplayer_games valid_multiplayer_games;//0x5A
	};
	TAG_BLOCK_SIZE_ASSERT(s_scenery_block, 0x5C);
	tag_block<s_scenery_block> scenery;//0x50
	struct s_scenery_palette_block
	{
		tag_reference name;//0x0
		PAD(0x20);//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_scenery_palette_block, 0x28);
	tag_block<s_scenery_palette_block> scenery_palette;//0x58
	struct s_bipeds_block
	{
		__int16 type;//0x0
		__int16 name;//0x2
		enum class e_placement_flags : __int32
		{
			not_automatically = FLAG(0),
			unused = FLAG(1),
			unused1 = FLAG(2),
			unused2 = FLAG(3),
			lock_type_to_env_object = FLAG(4),
			lock_transform_to_env_object = FLAG(5),
			never_placed = FLAG(6),
			lock_name_to_env_object = FLAG(7),
			create_at_rest = FLAG(8),
		};
		e_placement_flags placement_flags;//0x4
		real_point3d position_x;//0x8		
		real_euler_angles3d rotation_yaw;//0x14		
		float scale;//0x20
		enum class e_transform_flags : __int16
		{
			mirrored = FLAG(0),
		};
		e_transform_flags transform_flags;//0x24
		__int16 manual_bsp_flags;//0x26
		__int32 unique_id;//0x28
		__int16 origin_bsp_index;//0x2C
		enum class e_type : __int8
		{
			biped = 0,
			vehicle = 1,
			weapon = 2,
			equipment = 3,
			garbage = 4,
			projectile = 5,
			scenery = 6,
			machine = 7,
			control = 8,
			light_fixture = 9,
			sound_scenery = 10,
			crate = 11,
			creature = 12,
		};
		e_type type1;//0x2E
		enum class e_source : __int8
		{
			structure = 0,
			editor = 1,
			dynamic = 2,
			legacy = 3,
		};
		e_source source;//0x2F
		enum class e_bsp_policy : __int8
		{
			default = 0,
			always_placed = 1,
			manual_bsp_placement = 2,
		};
		e_bsp_policy bsp_policy;//0x30
		PAD(0x1);//0x31
		__int16 editor_folder;//0x32
		string_id variant_name;//0x34
		enum class e_active_change_colors : __int32
		{
			primary = FLAG(0),
			secondary = FLAG(1),
			tertiary = FLAG(2),
			quaternary = FLAG(3),
		};
		e_active_change_colors active_change_colors;//0x38
		PAD(0x10);//0x3C
		float body_vitality;//0x4C
		enum class e_flags : __int32
		{
			dead = FLAG(0),
			closed = FLAG(1),
			not_enterable_by_player = FLAG(2),
		};
		e_flags flags;//0x50
	};
	TAG_BLOCK_SIZE_ASSERT(s_bipeds_block, 0x54);
	tag_block<s_bipeds_block> bipeds;//0x60
	struct s_biped_palette_block
	{
		tag_reference name;//0x0
		PAD(0x20);//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_biped_palette_block, 0x28);
	tag_block<s_biped_palette_block> biped_palette;//0x68
	struct s_vehicles_block
	{
		__int16 type;//0x0
		__int16 name;//0x2
		enum class e_placement_flags : __int32
		{
			not_automatically = FLAG(0),
			unused = FLAG(1),
			unused1 = FLAG(2),
			unused2 = FLAG(3),
			lock_type_to_env_object = FLAG(4),
			lock_transform_to_env_object = FLAG(5),
			never_placed = FLAG(6),
			lock_name_to_env_object = FLAG(7),
			create_at_rest = FLAG(8),
		};
		e_placement_flags placement_flags;//0x4
		float position_x;//0x8
		float position_y;//0xC
		float position_z;//0x10
		angle rotation_yaw;//0x14
		angle rotation_pitch;//0x18
		angle rotation_roll;//0x1C
		float scale;//0x20
		enum class e_transform_flags : __int16
		{
			mirrored = FLAG(0),
		};
		e_transform_flags transform_flags;//0x24
		__int16 manual_bsp_flags;//0x26
		__int32 unique_id;//0x28
		__int16 origin_bsp_index;//0x2C
		enum class e_type : __int8
		{
			biped = 0,
			vehicle = 1,
			weapon = 2,
			equipment = 3,
			garbage = 4,
			projectile = 5,
			scenery = 6,
			machine = 7,
			control = 8,
			light_fixture = 9,
			sound_scenery = 10,
			crate = 11,
			creature = 12,
		};
		e_type type1;//0x2E
		enum class e_source : __int8
		{
			structure = 0,
			editor = 1,
			dynamic = 2,
			legacy = 3,
		};
		e_source source;//0x2F
		enum class e_bsp_policy : __int8
		{
			default = 0,
			always_placed = 1,
			manual_bsp_placement = 2,
		};
		e_bsp_policy bsp_policy;//0x30
		PAD(0x1);//0x31
		__int16 editor_folder;//0x32
		string_id variant_name;//0x34
		enum class e_active_change_colors : __int32
		{
			primary = FLAG(0),
			secondary = FLAG(1),
			tertiary = FLAG(2),
			quaternary = FLAG(3),
		};
		e_active_change_colors active_change_colors;//0x38
		PAD(0x10);//0x3C
		float body_vitality;//0x4C
		enum class e_flags : __int32
		{
			dead = FLAG(0),
			closed = FLAG(1),
			not_enterable_by_player = FLAG(2),
		};
		e_flags flags;//0x50
	};
	TAG_BLOCK_SIZE_ASSERT(s_vehicles_block, 0x54);
	tag_block<s_vehicles_block> vehicles;//0x70
	struct s_vehicle_palette_block
	{
		tag_reference name;//0x0
		PAD(0x20);//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_vehicle_palette_block, 0x28);
	tag_block<s_vehicle_palette_block> vehicle_palette;//0x78
	struct s_equipment_block
	{
		__int16 type;//0x0
		__int16 name;//0x2
		enum class e_placement_flags : __int32
		{
			not_automatically = FLAG(0),
			unused = FLAG(1),
			unused1 = FLAG(2),
			unused2 = FLAG(3),
			lock_type_to_env_object = FLAG(4),
			lock_transform_to_env_object = FLAG(5),
			never_placed = FLAG(6),
			lock_name_to_env_object = FLAG(7),
			create_at_rest = FLAG(8),
		};
		e_placement_flags placement_flags;//0x4
		float position_x;//0x8
		float position_y;//0xC
		float position_z;//0x10
		angle rotation_yaw;//0x14
		angle rotation_pitch;//0x18
		angle rotation_roll;//0x1C
		float scale;//0x20
		enum class e_transform_flags : __int16
		{
			mirrored = FLAG(0),
		};
		e_transform_flags transform_flags;//0x24
		__int16 manual_bsp_flags;//0x26
		__int32 unique_id;//0x28
		__int16 origin_bsp_index;//0x2C
		enum class e_type : __int8
		{
			biped = 0,
			vehicle = 1,
			weapon = 2,
			equipment = 3,
			garbage = 4,
			projectile = 5,
			scenery = 6,
			machine = 7,
			control = 8,
			light_fixture = 9,
			sound_scenery = 10,
			crate = 11,
			creature = 12,
		};
		e_type type1;//0x2E
		enum class e_source : __int8
		{
			structure = 0,
			editor = 1,
			dynamic = 2,
			legacy = 3,
		};
		e_source source;//0x2F
		enum class e_bsp_policy : __int8
		{
			default = 0,
			always_placed = 1,
			manual_bsp_placement = 2,
		};
		e_bsp_policy bsp_policy;//0x30
		PAD(0x1);//0x31
		__int16 editor_folder;//0x32
		enum class e_equipment_flags : __int32
		{
			initially_at_rest_does_not_fall = FLAG(0),
			obsolete = FLAG(1),
			does_accelerate_moves_due_to_explosions = FLAG(2),
		};
		e_equipment_flags equipment_flags;//0x34
	};
	TAG_BLOCK_SIZE_ASSERT(s_equipment_block, 0x38);
	tag_block<s_equipment_block> equipment;//0x80
	struct s_equipment_palette_block
	{
		tag_reference name;//0x0
		PAD(0x20);//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_equipment_palette_block, 0x28);
	tag_block<s_equipment_palette_block> equipment_palette;//0x88
	struct s_weapons_block
	{
		__int16 type;//0x0
		__int16 name;//0x2
		enum class e_placement_flags : __int32
		{
			not_automatically = FLAG(0),
			unused = FLAG(1),
			unused1 = FLAG(2),
			unused2 = FLAG(3),
			lock_type_to_env_object = FLAG(4),
			lock_transform_to_env_object = FLAG(5),
			never_placed = FLAG(6),
			lock_name_to_env_object = FLAG(7),
			create_at_rest = FLAG(8),
		};
		e_placement_flags placement_flags;//0x4
		float position_x;//0x8
		float position_y;//0xC
		float position_z;//0x10
		angle rotation_yaw;//0x14
		angle rotation_pitch;//0x18
		angle rotation_roll;//0x1C
		float scale;//0x20
		enum class e_transform_flags : __int16
		{
			mirrored = FLAG(0),
		};
		e_transform_flags transform_flags;//0x24
		__int16 manual_bsp_flags;//0x26
		__int32 unique_id;//0x28
		__int16 origin_bsp_index;//0x2C
		enum class e_type : __int8
		{
			biped = 0,
			vehicle = 1,
			weapon = 2,
			equipment = 3,
			garbage = 4,
			projectile = 5,
			scenery = 6,
			machine = 7,
			control = 8,
			light_fixture = 9,
			sound_scenery = 10,
			crate = 11,
			creature = 12,
		};
		e_type type1;//0x2E
		enum class e_source : __int8
		{
			structure = 0,
			editor = 1,
			dynamic = 2,
			legacy = 3,
		};
		e_source source;//0x2F
		enum class e_bsp_policy : __int8
		{
			default = 0,
			always_placed = 1,
			manual_bsp_placement = 2,
		};
		e_bsp_policy bsp_policy;//0x30
		PAD(0x1);//0x31
		__int16 editor_folder;//0x32
		string_id variant_name;//0x34
		enum class e_active_change_colors : __int32
		{
			primary = FLAG(0),
			secondary = FLAG(1),
			tertiary = FLAG(2),
			quaternary = FLAG(3),
		};
		e_active_change_colors active_change_colors;//0x38
		PAD(0x10);//0x3C
		__int16 rounds_left;//0x4C
		__int16 rounds_loaded;//0x4E
		enum class e_flags : __int32
		{
			initially_at_rest_does_not_fall = FLAG(0),
			obsolete = FLAG(1),
			does_accelerate_moves_due_to_explosions = FLAG(2),
		};
		e_flags flags;//0x50
	};
	TAG_BLOCK_SIZE_ASSERT(s_weapons_block, 0x54);
	tag_block<s_weapons_block> weapons;//0x90
	struct s_weapon_palette_block
	{
		tag_reference name;//0x0
		PAD(0x20);//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_weapon_palette_block, 0x28);
	tag_block<s_weapon_palette_block> weapon_palette;//0x98
	struct s_device_groups_block
	{
		tag_string32 name;//0x0
		float initial_value;//0x20
		enum class e_flags : __int32
		{
			can_change_only_once = FLAG(0),
		};
		e_flags flags;//0x24
	};
	TAG_BLOCK_SIZE_ASSERT(s_device_groups_block, 0x28);
	tag_block<s_device_groups_block> device_groups;//0xA0
	struct s_machines_block
	{
		__int16 type;//0x0
		__int16 name;//0x2
		enum class e_placement_flags : __int32
		{
			not_automatically = FLAG(0),
			unused = FLAG(1),
			unused1 = FLAG(2),
			unused2 = FLAG(3),
			lock_type_to_env_object = FLAG(4),
			lock_transform_to_env_object = FLAG(5),
			never_placed = FLAG(6),
			lock_name_to_env_object = FLAG(7),
			create_at_rest = FLAG(8),
		};
		e_placement_flags placement_flags;//0x4
		float position_x;//0x8
		float position_y;//0xC
		float position_z;//0x10
		angle rotation_yaw;//0x14
		angle rotation_pitch;//0x18
		angle rotation_roll;//0x1C
		float scale;//0x20
		enum class e_transform_flags : __int16
		{
			mirrored = FLAG(0),
		};
		e_transform_flags transform_flags;//0x24
		__int16 manual_bsp_flags;//0x26
		__int32 unique_id;//0x28
		__int16 origin_bsp_index;//0x2C
		enum class e_type : __int8
		{
			biped = 0,
			vehicle = 1,
			weapon = 2,
			equipment = 3,
			garbage = 4,
			projectile = 5,
			scenery = 6,
			machine = 7,
			control = 8,
			light_fixture = 9,
			sound_scenery = 10,
			crate = 11,
			creature = 12,
		};
		e_type type1;//0x2E
		enum class e_source : __int8
		{
			structure = 0,
			editor = 1,
			dynamic = 2,
			legacy = 3,
		};
		e_source source;//0x2F
		enum class e_bsp_policy : __int8
		{
			default = 0,
			always_placed = 1,
			manual_bsp_placement = 2,
		};
		e_bsp_policy bsp_policy;//0x30
		PAD(0x1);//0x31
		__int16 editor_folder;//0x32
		__int16 power_group;//0x34
		__int16 position_group;//0x36
		enum class e_flags : __int32
		{
			initially_open_10 = FLAG(0),
			initially_off_00 = FLAG(1),
			can_change_only_once = FLAG(2),
			position_reversed = FLAG(3),
			not_usable_from_any_side = FLAG(4),
		};
		e_flags flags;//0x38
		enum class e_flags1 : __int32
		{
			does_not_operate_automatically = FLAG(0),
			onesided = FLAG(1),
			never_appears_locked = FLAG(2),
			opened_by_melee_attack = FLAG(3),
			onesided_for_player = FLAG(4),
			does_not_close_automatically = FLAG(5),
		};
		e_flags1 flags1;//0x3C
		struct s_pathfinding_references_block
		{
			__int16 bsp_index;//0x0
			__int16 pathfinding_object_index;//0x2
		};
		TAG_BLOCK_SIZE_ASSERT(s_pathfinding_references_block, 0x4);
		tag_block<s_pathfinding_references_block> pathfinding_references;//0x40
	};
	TAG_BLOCK_SIZE_ASSERT(s_machines_block, 0x48);
	tag_block<s_machines_block> machines;//0xA8
	struct s_machine_palette_block
	{
		tag_reference name;//0x0
		PAD(0x20);//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_machine_palette_block, 0x28);
	tag_block<s_machine_palette_block> machine_palette;//0xB0
	struct s_controls_block
	{
		__int16 type;//0x0
		__int16 name;//0x2
		enum class e_placement_flags : __int32
		{
			not_automatically = FLAG(0),
			unused = FLAG(1),
			unused1 = FLAG(2),
			unused2 = FLAG(3),
			lock_type_to_env_object = FLAG(4),
			lock_transform_to_env_object = FLAG(5),
			never_placed = FLAG(6),
			lock_name_to_env_object = FLAG(7),
			create_at_rest = FLAG(8),
		};
		e_placement_flags placement_flags;//0x4
		float position_x;//0x8
		float position_y;//0xC
		float position_z;//0x10
		angle rotation_yaw;//0x14
		angle rotation_pitch;//0x18
		angle rotation_roll;//0x1C
		float scale;//0x20
		enum class e_transform_flags : __int16
		{
			mirrored = FLAG(0),
		};
		e_transform_flags transform_flags;//0x24
		__int16 manual_bsp_flags;//0x26
		__int32 unique_id;//0x28
		__int16 origin_bsp_index;//0x2C
		enum class e_type : __int8
		{
			biped = 0,
			vehicle = 1,
			weapon = 2,
			equipment = 3,
			garbage = 4,
			projectile = 5,
			scenery = 6,
			machine = 7,
			control = 8,
			light_fixture = 9,
			sound_scenery = 10,
			crate = 11,
			creature = 12,
		};
		e_type type1;//0x2E
		enum class e_source : __int8
		{
			structure = 0,
			editor = 1,
			dynamic = 2,
			legacy = 3,
		};
		e_source source;//0x2F
		enum class e_bsp_policy : __int8
		{
			default = 0,
			always_placed = 1,
			manual_bsp_placement = 2,
		};
		e_bsp_policy bsp_policy;//0x30
		PAD(0x1);//0x31
		__int16 editor_folder;//0x32
		__int16 power_group;//0x34
		__int16 position_group;//0x36
		enum class e_flags : __int32
		{
			initially_open_10 = FLAG(0),
			initially_off_00 = FLAG(1),
			can_change_only_once = FLAG(2),
			position_reversed = FLAG(3),
			not_usable_from_any_side = FLAG(4),
		};
		e_flags flags;//0x38
		enum class e_flags2 : __int32
		{
			usable_from_both_sides = FLAG(0),
		};
		e_flags2 flags2;//0x3C
		__int16 dont_touch_this;//0x40
		PAD(0x2);//0x42
	};
	TAG_BLOCK_SIZE_ASSERT(s_controls_block, 0x44);
	tag_block<s_controls_block> controls;//0xB8
	struct s_control_palette_block
	{
		tag_reference name;//0x0
		PAD(0x20);//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_control_palette_block, 0x28);
	tag_block<s_control_palette_block> control_palette;//0xC0
	struct s_light_fixtures_block
	{
		__int16 type;//0x0
		__int16 name;//0x2
		enum class e_placement_flags : __int32
		{
			not_automatically = FLAG(0),
			unused = FLAG(1),
			unused1 = FLAG(2),
			unused2 = FLAG(3),
			lock_type_to_env_object = FLAG(4),
			lock_transform_to_env_object = FLAG(5),
			never_placed = FLAG(6),
			lock_name_to_env_object = FLAG(7),
			create_at_rest = FLAG(8),
		};
		e_placement_flags placement_flags;//0x4
		float position_x;//0x8
		float position_y;//0xC
		float position_z;//0x10
		angle rotation_yaw;//0x14
		angle rotation_pitch;//0x18
		angle rotation_roll;//0x1C
		float scale;//0x20
		enum class e_transform_flags : __int16
		{
			mirrored = FLAG(0),
		};
		e_transform_flags transform_flags;//0x24
		__int16 manual_bsp_flags;//0x26
		__int32 unique_id;//0x28
		__int16 origin_bsp_index;//0x2C
		enum class e_type : __int8
		{
			biped = 0,
			vehicle = 1,
			weapon = 2,
			equipment = 3,
			garbage = 4,
			projectile = 5,
			scenery = 6,
			machine = 7,
			control = 8,
			light_fixture = 9,
			sound_scenery = 10,
			crate = 11,
			creature = 12,
		};
		e_type type1;//0x2E
		enum class e_source : __int8
		{
			structure = 0,
			editor = 1,
			dynamic = 2,
			legacy = 3,
		};
		e_source source;//0x2F
		enum class e_bsp_policy : __int8
		{
			default = 0,
			always_placed = 1,
			manual_bsp_placement = 2,
		};
		e_bsp_policy bsp_policy;//0x30
		PAD(0x1);//0x31
		__int16 editor_folder;//0x32
		__int16 power_group;//0x34
		__int16 position_group;//0x36
		enum class e_flags : __int32
		{
			initially_open_10 = FLAG(0),
			initially_off_00 = FLAG(1),
			can_change_only_once = FLAG(2),
			position_reversed = FLAG(3),
			not_usable_from_any_side = FLAG(4),
		};
		e_flags flags;//0x38
		real_color_rgb color;//0x3C
		float intensity;//0x48
		angle falloff_angle;//0x4C
		angle cutoff_angle;//0x50
	};
	TAG_BLOCK_SIZE_ASSERT(s_light_fixtures_block, 0x54);
	tag_block<s_light_fixtures_block> light_fixtures;//0xC8
	struct s_light_fixtures_palette_block
	{
		tag_reference name;//0x0
		PAD(0x20);//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_light_fixtures_palette_block, 0x28);
	tag_block<s_light_fixtures_palette_block> light_fixtures_palette;//0xD0
	struct s_sound_scenery_block
	{
		__int16 type;//0x0
		__int16 name;//0x2
		enum class e_placement_flags : __int32
		{
			not_automatically = FLAG(0),
			unused = FLAG(1),
			unused1 = FLAG(2),
			unused2 = FLAG(3),
			lock_type_to_env_object = FLAG(4),
			lock_transform_to_env_object = FLAG(5),
			never_placed = FLAG(6),
			lock_name_to_env_object = FLAG(7),
			create_at_rest = FLAG(8),
		};
		e_placement_flags placement_flags;//0x4
		float position_x;//0x8
		float position_y;//0xC
		float position_z;//0x10
		angle rotation_yaw;//0x14
		angle rotation_pitch;//0x18
		angle rotation_roll;//0x1C
		float scale;//0x20
		enum class e_transform_flags : __int16
		{
			mirrored = FLAG(0),
		};
		e_transform_flags transform_flags;//0x24
		__int16 manual_bsp_flags;//0x26
		__int32 unique_id;//0x28
		__int16 origin_bsp_index;//0x2C
		enum class e_type : __int8
		{
			biped = 0,
			vehicle = 1,
			weapon = 2,
			equipment = 3,
			garbage = 4,
			projectile = 5,
			scenery = 6,
			machine = 7,
			control = 8,
			light_fixture = 9,
			sound_scenery = 10,
			crate = 11,
			creature = 12,
		};
		e_type type1;//0x2E
		enum class e_source : __int8
		{
			structure = 0,
			editor = 1,
			dynamic = 2,
			legacy = 3,
		};
		e_source source;//0x2F
		enum class e_bsp_policy : __int8
		{
			default = 0,
			always_placed = 1,
			manual_bsp_placement = 2,
		};
		e_bsp_policy bsp_policy;//0x30
		PAD(0x1);//0x31
		__int16 editor_folder;//0x32
		enum class e_volume_type : __int32
		{
			sphere = 0,
			vertical_cylinder = 1,
		};
		e_volume_type volume_type;//0x34
		float height;//0x38
		float override_distance_bounds_lower;//0x3C
		float override_distance_bounds_upper;//0x40
		angle override_cone_angle_bounds_lower;//0x44
		angle override_cone_angle_bounds_upper;//0x48
		float override_outer_cone_gain;//0x4C
	};
	TAG_BLOCK_SIZE_ASSERT(s_sound_scenery_block, 0x50);
	tag_block<s_sound_scenery_block> sound_scenery;//0xD8
	struct s_sound_scenery_palette_block
	{
		tag_reference name;//0x0
		PAD(0x20);//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_sound_scenery_palette_block, 0x28);
	tag_block<s_sound_scenery_palette_block> sound_scenery_palette;//0xE0
	struct s_light_volumes_block
	{
		__int16 type;//0x0
		__int16 name;//0x2
		enum class e_placement_flags : __int32
		{
			not_automatically = FLAG(0),
			unused = FLAG(1),
			unused1 = FLAG(2),
			unused2 = FLAG(3),
			lock_type_to_env_object = FLAG(4),
			lock_transform_to_env_object = FLAG(5),
			never_placed = FLAG(6),
			lock_name_to_env_object = FLAG(7),
			create_at_rest = FLAG(8),
		};
		e_placement_flags placement_flags;//0x4
		real_point3d position;//0x8		
		real_euler_angles3d rotation_;//0x14

		float scale;//0x20
		enum class e_transform_flags : __int16
		{
			mirrored = FLAG(0),
		};
		e_transform_flags transform_flags;//0x24
		__int16 manual_bsp_flags;//0x26
		__int32 unique_id;//0x28
		__int16 origin_bsp_index;//0x2C
		enum class e_obj_type : __int8
		{
			biped = 0,
			vehicle = 1,
			weapon = 2,
			equipment = 3,
			garbage = 4,
			projectile = 5,
			scenery = 6,
			machine = 7,
			control = 8,
			light_fixture = 9,
			sound_scenery = 10,
			crate = 11,
			creature = 12,
		};
		e_obj_type obj_type;//0x2E
		enum class e_source : __int8
		{
			structure = 0,
			editor = 1,
			dynamic = 2,
			legacy = 3,
		};
		e_source source;//0x2F
		enum class e_bsp_policy : __int8
		{
			default = 0,
			always_placed = 1,
			manual_bsp_placement = 2,
		};
		e_bsp_policy bsp_policy;//0x30
		PAD(0x1);//0x31
		__int16 editor_folder;//0x32
		__int16 power_group;//0x34
		__int16 position_group;//0x36
		enum class e_flags : __int32
		{
			initially_open_10 = FLAG(0),
			initially_off_00 = FLAG(1),
			can_change_only_once = FLAG(2),
			position_reversed = FLAG(3),
			not_usable_from_any_side = FLAG(4),
		};
		e_flags flags;//0x38
		enum class e_type : __int16
		{
			sphere = 0,
			orthogonal = 1,
			projective = 2,
			pyramid = 3,
		};
		e_type type1;//0x3C
		enum class e_flags1 : __int16
		{
			custom_geometry = FLAG(0),
			unused = FLAG(1),
			cinematic_only = FLAG(2),
		};
		e_flags1 flags1;//0x3E
		enum class e_lightmap_type : __int16
		{
			use_light_tag_setting = 0,
			dynamic_only = 1,
			dynamic_with_lightmaps = 2,
			lightmaps_only = 3,
		};
		e_lightmap_type lightmap_type;//0x40
		enum class e_lightmap_flags : __int16
		{
			unused = FLAG(0),
		};
		e_lightmap_flags lightmap_flags;//0x42
		float lightmap_half_life;//0x44
		float lightmap_light_scale;//0x48
		float target_point_x;//0x4C
		float target_point_y;//0x50
		float target_point_z;//0x54
		float width;//0x58
		float height_scale;//0x5C
		angle field_of_view;//0x60
		float falloff_distance;//0x64
		float cutoff_distance;//0x68
	};
	TAG_BLOCK_SIZE_ASSERT(s_light_volumes_block, 0x6C);

	tag_block<s_light_volumes_block> light_volumes;//0xE8
	struct s_light_volumes_palette_block
	{
		tag_reference name;//0x0
		PAD(0x20);//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_light_volumes_palette_block, 0x28);
	tag_block<s_light_volumes_palette_block> light_volumes_palette;//0xF0
	struct s_player_starting_profile_block
	{
		tag_string32 name;//0x0
		float starting_health_damage;//0x20
		float starting_shield_damage;//0x24
		tag_reference primary_weapon;//0x28
		__int16 primary_rounds_loaded;//0x30
		__int16 primary_rounds_total;//0x32
		tag_reference secondary_weapon;//0x34
		__int16 secondary_rounds_loaded;//0x3C
		__int16 secondary_rounds_total;//0x3E
		__int8 starting_fragmentation_grenade_count;//0x40
		__int8 starting_plasma_grenade_count;//0x41
		__int8 starting_unknown_grenade_count;//0x42
		__int8 starting_unknown_grenade_count1;//0x43
	};
	TAG_BLOCK_SIZE_ASSERT(s_player_starting_profile_block, 0x44);
	tag_block<s_player_starting_profile_block> player_starting_profile;//0xF8
	struct s_player_starting_locations_block
	{
		float position_x;//0x0
		float position_y;//0x4
		float position_z;//0x8
		angle facing;//0xC
		enum class e_team_designator : __int16
		{
			red_alpha = 0,
			blue_bravo = 1,
			yellow_charlie = 2,
			green_delta = 3,
			purple_echo = 4,
			orange_foxtrot = 5,
			brown_golf = 6,
			pink_hotel = 7,
			neutral = 8,
		};
		e_team_designator team_designator;//0x10
		__int16 bsp_index;//0x12
		enum class e_game_type : __int16
		{
			none = 0,
			capture_the_flag = 1,
			slayer = 2,
			oddball = 3,
			king_of_the_hill = 4,
			race = 5,
			headhunter = 6,
			juggernaut = 7,
			territories = 8,
			stub = 9,
			ignored3 = 10,
			ignored4 = 11,
			all_game_types = 12,
			all_except_ctf = 13,
			all_except_ctf_and_race = 14,
		};
		e_game_type game_type_1;//0x14		
		e_game_type game_type_2;//0x16		
		e_game_type game_type_3;//0x18		
		e_game_type game_type_4;//0x1A

		enum class e_spawn_type : __int16
		{
			both = 0,
			initial_spawn_only = 1,
			respawn_only = 2,
		};
		e_spawn_type spawn_type_0;//0x1C		
		e_spawn_type spawn_type_1;//0x1E		
		e_spawn_type spawn_type_2;//0x20		
		e_spawn_type spawn_type_3;//0x22

		PAD(0x8);
		enum class e_campaign_player_type : __int16
		{
			masterchief = 0,
			dervish = 1,
			chief_multiplayer = 2,
			elite_multiplayer = 3,
		};
		e_campaign_player_type campaign_player_type;//0x2C
		PAD(0x6);//0x2E
	};
	TAG_BLOCK_SIZE_ASSERT(s_player_starting_locations_block, 0x34);
	tag_block<s_player_starting_locations_block> player_starting_locations;//0x100
	struct s_kill_trigger_volumes_block
	{
		string_id name;//0x0
		__int16 object_name;//0x4
		PAD(0x2);//0x6
		string_id node_name;//0x8
		PAD(6 * 4);//0xC
		real_point3d position;//0x24
		real_point3d extents;//0x30

		PAD(0x4);//0x3C
		__int16 kill_trigger_volume;//0x40
		PAD(0x2);//0x42
	};
	TAG_BLOCK_SIZE_ASSERT(s_kill_trigger_volumes_block, 0x44);
	tag_block<s_kill_trigger_volumes_block> kill_trigger_volumes;//0x108
	struct s_recorded_animations_block
	{
		tag_string32 name;//0x0
		__int8 version;//0x20
		__int8 raw_animation_data;//0x21
		__int8 unit_control_data_version;//0x22
		PAD(0x1);//0x23
		__int16 length_of_animation;//0x24
		PAD(0x6);//0x26
		data_block recorded_animation_event_stream;//0x2C
	};
	TAG_BLOCK_SIZE_ASSERT(s_recorded_animations_block, 0x34);
	tag_block<s_recorded_animations_block> recorded_animations;//0x110
	struct s_netgame_flags_block
	{
		real_point3d position;//0x0
		angle facing;//0xC
		enum class e_type : __int16
		{
			ctf_flag_spawn = 0,
			ctf_flag_return = 1,
			assault_bomb_spawn = 2,
			assault_bomb_return = 3,
			oddball_spawn = 4,
			unused = 5,
			race_checkpoint = 6,
			teleporter_src = 7,
			teleporter_dest = 8,
			headhunter_bin = 9,
			territories_flag = 10,
			king_hill_0 = 11,
			king_hill_1 = 12,
			king_hill_2 = 13,
			king_hill_3 = 14,
			king_hill_4 = 15,
			king_hill_5 = 16,
			king_hill_6 = 17,
			king_hill_7 = 18,
		};
		e_type type;//0x10
		enum class e_team_designator : __int16
		{
			red_alpha = 0,
			blue_bravo = 1,
			yellow_charlie = 2,
			green_delta = 3,
			purple_echo = 4,
			orange_foxtrot = 5,
			brown_golf = 6,
			pink_hotel = 7,
			neutral = 8,
		};
		e_team_designator team_designator;//0x12
		__int16 identifier;//0x14
		enum class e_flags : __int16
		{
			multiple_flagbomb = FLAG(0),
			single_flagbomb = FLAG(1),
			neutral_flagbomb = FLAG(2),
		};
		e_flags flags;//0x16
		PAD(0x8);//0x18

	};
	TAG_BLOCK_SIZE_ASSERT(s_netgame_flags_block, 0x20);
	tag_block<s_netgame_flags_block> netgame_flags;//0x118
	struct s_netgame_equipment_block
	{
		enum class e_flags : __int32
		{
			levitate = FLAG(0),
			destroy_existing_on_new_spawn = FLAG(1),
		};
		e_flags flags;//0x0
		enum class e_game_type : __int16
		{
			none = 0,
			capture_the_flag = 1,
			slayer = 2,
			oddball = 3,
			king_of_the_hill = 4,
			race = 5,
			headhunter = 6,
			juggernaut = 7,
			territories = 8,
			stub = 9,
			ignored3 = 10,
			ignored4 = 11,
			all_game_types = 12,
			all_except_ctf = 13,
			all_except_ctf_and_race = 14,
		};
		e_game_type game_type_1;//0x4		
		e_game_type game_type_2;//0x6
		e_game_type game_type_3;//0x8		
		e_game_type game_type_4;//0xA
		PAD(0x2);//0xC
		__int16 spawn_time_in_seconds_;//0xE
		__int16 respawn_on_empty_time;//0x10
		enum class e_respawn_timer_starts : __int16
		{
			on_pick_up = 0,
			on_body_depletion = 1,
		};
		e_respawn_timer_starts respawn_timer_starts;//0x12
		enum class e_classification : __int8
		{
			weapon = 0,
			primary_light_land = 1,
			secondary_light_land = 2,
			primary_heavy_land = 3,
			primary_flying = 4,
			secondary_heavy_land = 5,
			primary_turret = 6,
			secondary_turret = 7,
			grenade = 8,
			powerup = 9,
		};
		e_classification classification;//0x14
		PAD(0x2B);//0x15
		real_point3d position;//0x40		
		real_euler_angles3d orientation_yaw;//0x4C		
		tag_reference itemvehicle_collection;//0x58
		PAD(0x30);//0x60
	};
	TAG_BLOCK_SIZE_ASSERT(s_netgame_equipment_block, 0x90);
	tag_block<s_netgame_equipment_block> netgame_equipment;//0x120
	struct s_starting_equipment_block
	{
		enum class e_flags : __int32
		{
			no_grenades = FLAG(0),
			plasma_grenades = FLAG(1),
		};
		e_flags flags;//0x0
		enum class e_game_type : __int16
		{
			none = 0,
			capture_the_flag = 1,
			slayer = 2,
			oddball = 3,
			king_of_the_hill = 4,
			race = 5,
			headhunter = 6,
			juggernaut = 7,
			territories = 8,
			stub = 9,
			ignored3 = 10,
			ignored4 = 11,
			all_game_types = 12,
			all_except_ctf = 13,
			all_except_ctf_and_race = 14,
		};
		e_game_type game_type_1;//0x4		
		e_game_type game_type_2;//0x6		
		e_game_type game_type_3;//0x8		
		e_game_type game_type_4;//0xA
		PAD(0x30);//0xC
		tag_reference item_collection_1;//0x3C
		tag_reference item_collection_2;//0x44
		tag_reference item_collection_3;//0x4C
		tag_reference item_collection_4;//0x54
		tag_reference item_collection_5;//0x5C
		tag_reference item_collection_6;//0x64
		PAD(0x30);//0x6C
	};
	TAG_BLOCK_SIZE_ASSERT(s_starting_equipment_block, 0x9C);
	tag_block<s_starting_equipment_block> starting_equipment;//0x128
	struct s_bsp_switch_trigger_volumes_block
	{
		__int16 trigger_volume;//0x0
		__int16 source;//0x2
		__int16 destination;//0x4
		PAD(0x8);//0x6
	};
	TAG_BLOCK_SIZE_ASSERT(s_bsp_switch_trigger_volumes_block, 0xE);
	tag_block<s_bsp_switch_trigger_volumes_block> bsp_switch_trigger_volumes;//0x130
	struct s_decals_block
	{
		__int16 decal_type;//0x0
		signed __int8 yaw;//0x2
		signed __int8 pitch;//0x3
		float position_x;//0x4
		float position_y;//0x8
		float position_z;//0xC
	};
	TAG_BLOCK_SIZE_ASSERT(s_decals_block, 0x10);
	tag_block<s_decals_block> decals;//0x138
	struct s_decals_palette_block
	{
		tag_reference reference;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_decals_palette_block, 0x8);
	tag_block<s_decals_palette_block> decals_palette;//0x140
	struct s_detail_object_collection_palette_block
	{
		tag_reference name;//0x0
		PAD(0x20);//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_detail_object_collection_palette_block, 0x28);
	tag_block<s_detail_object_collection_palette_block> detail_object_collection_palette;//0x148
	struct s_style_palette_block
	{
		tag_reference reference;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_style_palette_block, 0x8);
	tag_block<s_style_palette_block> style_palette;//0x150
	struct s_squad_groups_block
	{
		tag_string32 name;//0x0
		__int16 parent;//0x20
		__int16 initial_orders;//0x22
	};
	TAG_BLOCK_SIZE_ASSERT(s_squad_groups_block, 0x24);
	tag_block<s_squad_groups_block> squad_groups;//0x158
	struct s_squads_block
	{
		tag_string32 name;//0x0
		enum class e_flags : __int32
		{
			unused = FLAG(0),
			never_search = FLAG(1),
			start_timer_immediately = FLAG(2),
			no_timer_delay_forever = FLAG(3),
			magic_sight_after_timer = FLAG(4),
			automatic_migration = FLAG(5),
			deprecated = FLAG(6),
			respawn_enabled = FLAG(7),
			blind = FLAG(8),
			deaf = FLAG(9),
			braindead = FLAG(10),
			_3d_firing_positions = FLAG(11),
			initially_placed = FLAG(12),
			units_not_enterable_by_player = FLAG(13),
		};
		e_flags flags;//0x20
		enum class e_team : __int16
		{
			default = 0,
			player = 1,
			human = 2,
			covenant = 3,
			flood = 4,
			sentinel = 5,
			heretic = 6,
			prophet = 7,
			unused8 = 8,
			unused9 = 9,
			unused10 = 10,
			unused11 = 11,
			unused12 = 12,
			unused13 = 13,
			unused14 = 14,
			unused15 = 15,
		};
		e_team team;//0x24
		__int16 parent;//0x26
		float squad_delay_time;//0x28
		__int16 normal_diff_count;//0x2C
		__int16 insane_diff_count;//0x2E
		enum class e_major_upgrade : __int16
		{
			normal = 0,
			few = 1,
			many = 2,
			none = 3,
			all = 4,
		};
		e_major_upgrade major_upgrade;//0x30
		PAD(0x2);//0x32
		__int16 vehicle_type;//0x34
		__int16 character_type;//0x36
		__int16 initial_zone;//0x38
		PAD(0x2);//0x3A
		__int16 initial_weapon;//0x3C
		__int16 initial_secondary_weapon;//0x3E
		enum class e_grenade_type : __int16
		{
			none = 0,
			human_grenade = 1,
			covenant_plasma = 2,
		};
		e_grenade_type grenade_type;//0x40
		__int16 initial_order;//0x42
		string_id vehicle_variant;//0x44
		struct s_starting_locations_block
		{
			string_id name_old_string_id;//0x0
			float position_x;//0x4
			float position_y;//0x8
			float position_z;//0xC
			__int16 reference_frame;//0x10
			PAD(0x2);//0x12
			real_euler_angles2d facing;//0x14			
			enum class e_flags : __int32
			{
				initially_asleep = FLAG(0),
				infection_form_explode = FLAG(1),
				na = FLAG(2),
				always_place = FLAG(3),
				initially_hidden = FLAG(4),
			};
			e_flags flags;//0x1C
			__int16 character_type;//0x20
			__int16 initial_weapon;//0x22
			__int16 initial_secondary_weapon;//0x24
			PAD(0x2);//0x26
			__int16 vehicle_type;//0x28
			enum class e_seat_type : __int16
			{
				default = 0,
				passenger = 1,
				gunner = 2,
				driver = 3,
				small_cargo = 4,
				large_cargo = 5,
				no_driver = 6,
				no_vehicle = 7,
			};
			e_seat_type seat_type;//0x2A
			enum class e_grenade_type : __int16
			{
				none = 0,
				human_grenade = 1,
				covenant_plasma = 2,
			};
			e_grenade_type grenade_type;//0x2C
			__int16 swarm_count;//0x2E
			string_id actor_variant_name;//0x30
			string_id vehicle_variant_name;//0x34
			float initial_movement_distance;//0x38
			__int16 emitter_vehicle;//0x3C
			enum class e_initial_movement_mode : __int16
			{
				default = 0,
				climbing = 1,
				flying = 2,
			};
			e_initial_movement_mode initial_movement_mode;//0x3E
			tag_string32 placement_script;//0x40
			PAD(0x4);//0x60
		};
		TAG_BLOCK_SIZE_ASSERT(s_starting_locations_block, 0x64);
		tag_block<s_starting_locations_block> starting_locations;//0x48
		tag_string32 placement_script;//0x50
		PAD(0x4);//0x70
	};
	TAG_BLOCK_SIZE_ASSERT(s_squads_block, 0x74);
	tag_block<s_squads_block> squads;//0x160
	struct s_zones_block
	{
		tag_string32 name;//0x0
		enum class e_flags : __int32
		{
			manual_bsp_index = FLAG(0),
		};
		e_flags flags;//0x20
		__int16 manual_bsp;//0x24
		PAD(0x2);//0x26
		struct s_firing_positions_block
		{
			real_point3d position_local;//0x0			
			__int16 reference_frame;//0xC
			enum class e_flags : __int16
			{
				open = FLAG(0),
				partial = FLAG(1),
				closed = FLAG(2),
				mobile = FLAG(3),
				wall_lean = FLAG(4),
				perch = FLAG(5),
				ground_point = FLAG(6),
				dynamic_cover_point = FLAG(7),
			};
			e_flags flags;//0xE
			__int16 area;//0x10
			__int16 cluster_index;//0x12
			PAD(0x4);//0x14
			angle normal_yaw;//0x18
			angle normal_pitch;//0x1C
		};
		TAG_BLOCK_SIZE_ASSERT(s_firing_positions_block, 0x20);
		tag_block<s_firing_positions_block> firing_positions;//0x28
		struct s_areas_block
		{
			tag_string32 name;//0x0
			enum class e_area_flags : __int32
			{
				vehicle_area = FLAG(0),
				perch = FLAG(1),
				manual_reference_frame = FLAG(2),
			};
			e_area_flags area_flags;//0x20
			PAD(0x58);//0x24
			__int16 manual_reference_frame;//0x7C
			PAD(0x2);//0x7E
			struct s_flight_hints_block
			{
				__int16 flight_hint_index;//0x0
				__int16 poit_index;//0x2
			};
			TAG_BLOCK_SIZE_ASSERT(s_flight_hints_block, 0x4);
			tag_block<s_flight_hints_block> flight_hints;//0x80
		};
		TAG_BLOCK_SIZE_ASSERT(s_areas_block, 0x88);
		tag_block<s_areas_block> areas;//0x30
	};
	TAG_BLOCK_SIZE_ASSERT(s_zones_block, 0x38);
	tag_block<s_zones_block> zones;//0x168
	struct s_mission_scenes_block
	{
		string_id name;//0x0
		enum class e_flags : __int32
		{
			scene_can_play_multiple_times = FLAG(0),
			enable_combat_dialogue = FLAG(1),
		};
		e_flags flags;//0x4
		struct s_trigger_conditions_block
		{
			enum class e_combination_rule : __int16
			{
				or = 0,
				and = 1,
			};
			e_combination_rule combination_rule;//0x0
			PAD(0x2);//0x2
			struct s_triggers_block
			{
				enum class e_trigger_flags : __int32
				{
					not = FLAG(0),
				};
				e_trigger_flags trigger_flags;//0x0
				__int16 trigger;//0x4
				PAD(0x2);//0x6
			};
			TAG_BLOCK_SIZE_ASSERT(s_triggers_block, 0x8);
			tag_block<s_triggers_block> triggers;//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_trigger_conditions_block, 0xC);
		tag_block<s_trigger_conditions_block> trigger_conditions;//0x8
		struct s_roles_block
		{
			string_id name;//0x0
			enum class e_group : __int16
			{
				group_1 = 0,
				group_2 = 1,
				group_3 = 2,
			};
			e_group group;//0x4
			PAD(0x2);//0x6
			struct s_role_variants_block
			{
				string_id variant_designation;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_role_variants_block, 0x4);
			tag_block<s_role_variants_block> role_variants;//0x8
		};
		TAG_BLOCK_SIZE_ASSERT(s_roles_block, 0x10);
		tag_block<s_roles_block> roles;//0x10
	};
	TAG_BLOCK_SIZE_ASSERT(s_mission_scenes_block, 0x18);
	tag_block<s_mission_scenes_block> mission_scenes;//0x170
	struct s_character_palette_block
	{
		tag_reference reference;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_character_palette_block, 0x8);
	tag_block<s_character_palette_block> character_palette;//0x178
	struct s_ai_pathfinding_data_block
	{
		struct s_sectors_block
		{
			enum class e_pathfinding_sector_flags : __int16
			{
				sector_walkable = FLAG(0),
				sector_breakable = FLAG(1),
				sector_mobile = FLAG(2),
				sector_bsp_source = FLAG(3),
				floor = FLAG(4),
				ceiling = FLAG(5),
				wall_north = FLAG(6),
				wall_south = FLAG(7),
				wall_east = FLAG(8),
				wall_west = FLAG(9),
				crouchable = FLAG(10),
				aligned = FLAG(11),
				sector_step = FLAG(12),
				sector_interior = FLAG(13),
			};
			e_pathfinding_sector_flags pathfinding_sector_flags;//0x0
			__int16 hint_index;//0x2
			__int32 first_link_do_not_set_manually;//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_sectors_block, 0x8);
		tag_block<s_sectors_block> sectors;//0x0
		struct s_links_block
		{
			__int16 vertex_1;//0x0
			__int16 vertex_2;//0x2
			enum class e_link_flags : __int16
			{
				sector_link_from_collision_edge = FLAG(0),
				sector_intersection_link = FLAG(1),
				sector_link_bsp2d_creation_error = FLAG(2),
				sector_link_topology_error = FLAG(3),
				sector_link_chain_error = FLAG(4),
				sector_link_both_sectors_walkable = FLAG(5),
				sector_link_magic_hanging_link = FLAG(6),
				sector_link_threshold = FLAG(7),
				sector_link_crouchable = FLAG(8),
				sector_link_wall_base = FLAG(9),
				sector_link_ledge = FLAG(10),
				sector_link_leanable = FLAG(11),
				sector_link_start_corner = FLAG(12),
				sector_link_end_corner = FLAG(13),
			};
			e_link_flags link_flags;//0x4
			__int16 hint_index;//0x6
			__int16 forward_link;//0x8
			__int16 reverse_link;//0xA
			__int16 left_sector;//0xC
			__int16 right_sector;//0xE
		};
		TAG_BLOCK_SIZE_ASSERT(s_links_block, 0x10);
		tag_block<s_links_block> links;//0x8
		struct s_refs_block
		{
			__int32 node_ref_or_sector_ref;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_refs_block, 0x4);
		tag_block<s_refs_block> refs;//0x10
		struct s_bsp2d_nodes_block
		{
			real_plane2d plane;//0x0			
			__int32 left_child;//0xC
			__int32 right_child;//0x10
		};
		TAG_BLOCK_SIZE_ASSERT(s_bsp2d_nodes_block, 0x14);
		tag_block<s_bsp2d_nodes_block> bsp2d_nodes;//0x18
		struct s_surface_flags_block
		{
			__int32 flags;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_surface_flags_block, 0x4);
		tag_block<s_surface_flags_block> surface_flags;//0x20
		struct s_vertices_block
		{
			float point_x;//0x0
			float point_y;//0x4
			float point_z;//0x8
		};
		TAG_BLOCK_SIZE_ASSERT(s_vertices_block, 0xC);
		tag_block<s_vertices_block> vertices;//0x28
		struct s_object_refs_block
		{
			enum class e_flags : __int16
			{
				mobile = FLAG(0),
			};
			e_flags flags;//0x0
			PAD(0x2);//0x2
			__int32 first_sector;//0x4
			__int32 last_sector;//0x8
			struct s_bsps_block
			{
				__int32 bsp_reference;//0x0
				__int32 first_sector;//0x4
				__int32 last_sector;//0x8
				__int16 node_index;//0xC
				PAD(0x2);//0xE
			};
			TAG_BLOCK_SIZE_ASSERT(s_bsps_block, 0x10);
			tag_block<s_bsps_block> bsps;//0xC
			struct s_nodes_block
			{
				__int16 reference_frame_index;//0x0
				__int8 projection_axis;//0x2
				enum class e_projection_sign : __int8
				{
					projection_sign = FLAG(0),
				};
				e_projection_sign projection_sign;//0x3
			};
			TAG_BLOCK_SIZE_ASSERT(s_nodes_block, 0x4);
			tag_block<s_nodes_block> nodes;//0x14
		};
		TAG_BLOCK_SIZE_ASSERT(s_object_refs_block, 0x1C);
		tag_block<s_object_refs_block> object_refs;//0x30
		struct s_pathfinding_hints_block
		{
			enum class e_hint_type : __int16
			{
				intersection_link = 0,
				jump_link = 1,
				climb_link = 2,
				vault_link = 3,
				mount_link = 4,
				hoist_link = 5,
				wall_jump_link = 6,
				breakable_floor = 7,
			};
			e_hint_type hint_type;//0x0
			__int16 next_hint_index;//0x2
			__int16 hint_data_0;//0x4
			__int16 hint_data_1;//0x6
			__int16 hint_data_2;//0x8
			__int16 hint_data_3;//0xA
			__int16 hint_data_4;//0xC
			__int16 hint_data_5;//0xE
			__int16 hint_data_6;//0x10
			__int16 hint_data_7;//0x12
		};
		TAG_BLOCK_SIZE_ASSERT(s_pathfinding_hints_block, 0x14);
		tag_block<s_pathfinding_hints_block> pathfinding_hints;//0x38
		struct s_instanced_geometry_refs_block
		{
			__int16 pathfinding_object_index;//0x0
			PAD(0x2);//0x2
		};
		TAG_BLOCK_SIZE_ASSERT(s_instanced_geometry_refs_block, 0x4);
		tag_block<s_instanced_geometry_refs_block> instanced_geometry_refs;//0x40
		__int32 structure_checksum;//0x48
		PAD(0x20);//0x4C
		struct s_userplaced_hints_block
		{
			struct s_point_geometry_block
			{
				float point_x;//0x0
				float point_y;//0x4
				float point_z;//0x8
				__int16 reference_frame;//0xC
				PAD(0x2);//0xE
			};
			TAG_BLOCK_SIZE_ASSERT(s_point_geometry_block, 0x10);
			tag_block<s_point_geometry_block> point_geometry;//0x0
			struct s_ray_geometry_block
			{
				float point_x;//0x0
				float point_y;//0x4
				float point_z;//0x8
				__int16 reference_frame;//0xC
				PAD(0xE);//0xE
			};
			TAG_BLOCK_SIZE_ASSERT(s_ray_geometry_block, 0x1C);
			tag_block<s_ray_geometry_block> ray_geometry;//0x8
			struct s_line_segment_geometry_block
			{
				enum class e_flags : __int32
				{
					bidirectional = FLAG(0),
					closed = FLAG(1),
				};
				e_flags flags;//0x0
				float point_0_x;//0x4
				float point_0_y;//0x8
				float point_0_z;//0xC
				__int16 reference_frame0;//0x10
				PAD(0x2);//0x12
				float point_1_x;//0x14
				float point_1_y;//0x18
				float point_1_z;//0x1C
				__int16 reference_frame1;//0x20
				PAD(0x2);//0x22
			};
			TAG_BLOCK_SIZE_ASSERT(s_line_segment_geometry_block, 0x24);
			tag_block<s_line_segment_geometry_block> line_segment_geometry;//0x10
			struct s_parallelogram_geometry_block
			{
				enum class e_flags : __int32
				{
					bidirectional = FLAG(0),
					closed = FLAG(1),
				};
				e_flags flags;//0x0
				float point_0_x;//0x4
				float point_0_y;//0x8
				float point_0_z;//0xC
				__int16 reference_frame0;//0x10
				PAD(0x2);//0x12
				float point_1_x;//0x14
				float point_1_y;//0x18
				float point_1_z;//0x1C
				__int16 reference_frame1;//0x20
				PAD(0x2);//0x22
				float point_2_x;//0x24
				float point_2_y;//0x28
				float point_2_z;//0x2C
				__int16 reference_frame2;//0x30
				PAD(0x2);//0x32
				float point_3_x;//0x34
				float point_3_y;//0x38
				float point_3_z;//0x3C
				__int16 reference_frame3;//0x40
				PAD(0x2);//0x42
			};
			TAG_BLOCK_SIZE_ASSERT(s_parallelogram_geometry_block, 0x44);
			tag_block<s_parallelogram_geometry_block> parallelogram_geometry;//0x18
			struct s_polygon_geometry_block
			{
				enum class e_flags : __int32
				{
					bidirectional = FLAG(0),
					closed = FLAG(1),
				};
				e_flags flags;//0x0
				struct s_points_block
				{
					float point_x;//0x0
					float point_y;//0x4
					float point_z;//0x8
					__int16 reference_frame;//0xC
					PAD(0x2);//0xE
				};
				TAG_BLOCK_SIZE_ASSERT(s_points_block, 0x10);
				tag_block<s_points_block> points;//0x4
			};
			TAG_BLOCK_SIZE_ASSERT(s_polygon_geometry_block, 0xC);
			tag_block<s_polygon_geometry_block> polygon_geometry;//0x20
			struct s_jump_hints_block
			{
				enum class e_flags : __int16
				{
					bidirectional = FLAG(0),
					closed = FLAG(1),
				};
				e_flags flags;//0x0
				__int16 geometry_index;//0x2
				enum class e_force_jump_height : __int16
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
				e_force_jump_height force_jump_height;//0x4
				enum class e_control_flags : __int16
				{
					magic_lift = FLAG(0),
				};
				e_control_flags control_flags;//0x6
			};
			TAG_BLOCK_SIZE_ASSERT(s_jump_hints_block, 0x8);
			tag_block<s_jump_hints_block> jump_hints;//0x28
			struct s_climb_hints_block
			{
				enum class e_flags : __int16
				{
					bidirectional = FLAG(0),
					closed = FLAG(1),
				};
				e_flags flags;//0x0
				__int16 geometry_index;//0x2
			};
			TAG_BLOCK_SIZE_ASSERT(s_climb_hints_block, 0x4);
			tag_block<s_climb_hints_block> climb_hints;//0x30
			struct s_well_hints_block
			{
				enum class e_flags : __int32
				{
					bidirectional = FLAG(0),
				};
				e_flags flags;//0x0
				struct s_points_block
				{
					enum class e_type : __int16
					{
						jump = 0,
						climb = 1,
						hoist = 2,
					};
					e_type type;//0x0
					PAD(0xE);//0x2
					__int16 reference_frame;//0x10
					PAD(0x2);//0x12
					__int32 sector_index;//0x14
					angle normal_yaw;//0x18
					angle normal_pitch;//0x1C
				};
				TAG_BLOCK_SIZE_ASSERT(s_points_block, 0x20);
				tag_block<s_points_block> points;//0x4
			};
			TAG_BLOCK_SIZE_ASSERT(s_well_hints_block, 0xC);
			tag_block<s_well_hints_block> well_hints;//0x38
			struct s_flight_hints_block
			{
				struct s_points_block
				{
					PAD(0xC);//0x0
				};
				TAG_BLOCK_SIZE_ASSERT(s_points_block, 0xC);
				tag_block<s_points_block> points;//0x0
			};
			TAG_BLOCK_SIZE_ASSERT(s_flight_hints_block, 0x8);
			tag_block<s_flight_hints_block> flight_hints;//0x40
		};
		TAG_BLOCK_SIZE_ASSERT(s_userplaced_hints_block, 0x48);
		tag_block<s_userplaced_hints_block> userplaced_hints;//0x6C
	};
	TAG_BLOCK_SIZE_ASSERT(s_ai_pathfinding_data_block, 0x74);
	tag_block<s_ai_pathfinding_data_block> ai_pathfinding_data;//0x180
	struct s_ai_animation_references_block
	{
		tag_string32 animation_name;//0x0
		tag_reference animation_graph;//0x20
		PAD(0xC);//0x28
	};
	TAG_BLOCK_SIZE_ASSERT(s_ai_animation_references_block, 0x34);
	tag_block<s_ai_animation_references_block> ai_animation_references;//0x188
	struct s_ai_script_references_block
	{
		tag_string32 script_name;//0x0
		PAD(0x8);//0x20
	};
	TAG_BLOCK_SIZE_ASSERT(s_ai_script_references_block, 0x28);
	tag_block<s_ai_script_references_block> ai_script_references;//0x190
	struct s_ai_recording_references_block
	{
		tag_string32 recording_name;//0x0
		PAD(0x8);//0x20
	};
	TAG_BLOCK_SIZE_ASSERT(s_ai_recording_references_block, 0x28);
	tag_block<s_ai_recording_references_block> ai_recording_references;//0x198
	struct s_ai_conversations_block
	{
		tag_string32 name;//0x0
		enum class e_flags : __int16
		{
			stop_if_death = FLAG(0),
			stop_if_damaged = FLAG(1),
			stop_if_visible_enemy = FLAG(2),
			stop_if_alerted_to_enemy = FLAG(3),
			player_must_be_visible = FLAG(4),
			stop_other_actions = FLAG(5),
			keep_trying_to_play = FLAG(6),
			player_must_be_looking = FLAG(7),
		};
		e_flags flags;//0x20
		PAD(0x2);//0x22
		float trigger_distance;//0x24
		float runtoplayer_dist;//0x28
		PAD(0x24);//0x2C
		struct s_participants_block
		{
			PAD(0x8);//0x0
			__int16 use_this_object;//0x8
			__int16 set_new_name;//0xA
			PAD(0x18);//0xC
			tag_string32 encounter_name;//0x24
			PAD(0x10);//0x44
		};
		TAG_BLOCK_SIZE_ASSERT(s_participants_block, 0x54);
		tag_block<s_participants_block> participants;//0x50
		struct s_lines_block
		{
			enum class e_flags : __int16
			{
				addressee_look_at_speaker = FLAG(0),
				everyone_look_at_speaker = FLAG(1),
				everyone_look_at_addressee = FLAG(2),
				wait_after_until_told_to_advance = FLAG(3),
				wait_until_speaker_nearby = FLAG(4),
				wait_until_everyone_nearby = FLAG(5),
			};
			e_flags flags;//0x0
			__int16 participant;//0x2
			enum class e_addressee : __int16
			{
				none = 0,
				player = 1,
				participant = 2,
			};
			e_addressee addressee;//0x4
			__int16 addressee_participant;//0x6
			PAD(0x4);//0x8
			float line_delay_time;//0xC
			PAD(0xC);//0x10
			tag_reference variant_1;//0x1C
			tag_reference variant_2;//0x24
			tag_reference variant_3;//0x2C
			tag_reference variant_4;//0x34
			tag_reference variant_5;//0x3C
			tag_reference variant_6;//0x44
		};
		TAG_BLOCK_SIZE_ASSERT(s_lines_block, 0x4C);
		tag_block<s_lines_block> lines;//0x58		
		PAD(0x8);//0x60
	};
	TAG_BLOCK_SIZE_ASSERT(s_ai_conversations_block, 0x68);
	tag_block<s_ai_conversations_block> ai_conversations;//0x1A0
	data_block script_syntax_data;//0x1A8
	data_block script_string_data;//0x1B0
	struct s_scripts_block
	{
		tag_string32 name;//0x0
		enum class e_script_type : __int16
		{
			startup = 0,
			dormant = 1,
			continuous = 2,
			_static = 3,
			stub = 4,
			command_script = 5,
		};
		e_script_type script_type;//0x20
		enum class e_return_type : __int16
		{
			unparsed = 0,
			special_form = 1,
			function_name = 2,
			passthrough = 3,
			_void = 4,
			boolean = 5,
			real = 6,
			_short = 7,
			_long = 8,
			string = 9,
			script = 10,
			string_id = 11,
			unit_seat_mapping = 12,
			trigger_volume = 13,
			cutscene_flag = 14,
			cutscene_camera_point = 15,
			cutscene_title = 16,
			cutscene_recording = 17,
			device_group = 18,
			ai = 19,
			ai_command_list = 20,
			ai_command_script = 21,
			ai_behavior = 22,
			ai_orders = 23,
			starting_profile = 24,
			conversation = 25,
			structure_bsp = 26,
			navpoint = 27,
			point_reference = 28,
			style = 29,
			hud_message = 30,
			object_list = 31,
			sound = 32,
			effect = 33,
			damage = 34,
			looping_sound = 35,
			animation_graph = 36,
			damage_effect = 37,
			object_definition = 38,
			bitmap = 39,
			shader = 40,
			render_model = 41,
			structure_definition = 42,
			lightmap_definition = 43,
			game_difficulty = 44,
			team = 45,
			actor_type = 46,
			hud_corner = 47,
			model_state = 48,
			network_event = 49,
			object = 50,
			unit = 51,
			vehicle = 52,
			weapon = 53,
			device = 54,
			scenery = 55,
			object_name = 56,
			unit_name = 57,
			vehicle_name = 58,
			weapon_name = 59,
			device_name = 60,
			scenery_name = 61,
		};
		e_return_type return_type;//0x22
		__int16 root_expression_index;//0x24
		__int16 root_expression_salt;//0x26
	};
	TAG_BLOCK_SIZE_ASSERT(s_scripts_block, 0x28);
	tag_block<s_scripts_block> scripts;//0x1B8
	struct s_globals_block
	{
		tag_string32 name;//0x0
		enum class e_type : __int16
		{
			unparsed = 0,
			special_form = 1,
			function_name = 2,
			passthrough = 3,
			_void = 4,
			boolean = 5,
			real = 6,
			_short = 7,
			_long = 8,
			string = 9,
			script = 10,
			string_id = 11,
			unit_seat_mapping = 12,
			trigger_volume = 13,
			cutscene_flag = 14,
			cutscene_camera_point = 15,
			cutscene_title = 16,
			cutscene_recording = 17,
			device_group = 18,
			ai = 19,
			ai_command_list = 20,
			ai_command_script = 21,
			ai_behavior = 22,
			ai_orders = 23,
			starting_profile = 24,
			conversation = 25,
			structure_bsp = 26,
			navpoint = 27,
			point_reference = 28,
			style = 29,
			hud_message = 30,
			object_list = 31,
			sound = 32,
			effect = 33,
			damage = 34,
			looping_sound = 35,
			animation_graph = 36,
			damage_effect = 37,
			object_definition = 38,
			bitmap = 39,
			shader = 40,
			render_model = 41,
			structure_definition = 42,
			lightmap_definition = 43,
			game_difficulty = 44,
			team = 45,
			actor_type = 46,
			hud_corner = 47,
			model_state = 48,
			network_event = 49,
			object = 50,
			unit = 51,
			vehicle = 52,
			weapon = 53,
			device = 54,
			scenery = 55,
			object_name = 56,
			unit_name = 57,
			vehicle_name = 58,
			weapon_name = 59,
			device_name = 60,
			scenery_name = 61,
		};
		e_type type;//0x20
		PAD(0x2);//0x22
		__int32 initialization_expression_index;//0x24
	};
	TAG_BLOCK_SIZE_ASSERT(s_globals_block, 0x28);
	tag_block<s_globals_block> globals;//0x1C0
	struct s_references_block
	{
		tag_reference reference;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_references_block, 0x8);
	tag_block<s_references_block> references;//0x1C8
	struct s_source_files_block
	{
		tag_string32 name;//0x0
		data_block source;//0x20
	};
	TAG_BLOCK_SIZE_ASSERT(s_source_files_block, 0x28);
	tag_block<s_source_files_block> source_files;//0x1D0
	struct s_scripting_data_block
	{
		struct s_point_sets_block
		{
			tag_string32 name;//0x0
			struct s_points_block
			{
				tag_string32 name;//0x0
				float position_x;//0x20
				float position_y;//0x24
				float position_z;//0x28
				__int16 reference_frame;//0x2C
				PAD(0x2);//0x2E
				__int32 surface_index;//0x30
				angle facing_direction_yaw;//0x34
				angle facing_direction_pitch;//0x38
			};
			TAG_BLOCK_SIZE_ASSERT(s_points_block, 0x3C);
			tag_block<s_points_block> points;//0x20
			__int16 bsp_index;//0x28
			__int16 manual_reference_frame;//0x2A
			enum class e_flags : __int32
			{
				manual_reference_frame = FLAG(0),
				turret_deployment = FLAG(1),
			};
			e_flags flags;//0x2C
		};
		TAG_BLOCK_SIZE_ASSERT(s_point_sets_block, 0x30);
		tag_block<s_point_sets_block> point_sets;//0x0
		PAD(0x78);//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_scripting_data_block, 0x80);
	tag_block<s_scripting_data_block> scripting_data;//0x1D8
	struct s_cutscene_flags_block
	{
		PAD(0x4);//0x0
		tag_string32 name;//0x4
		float position_x;//0x24
		float position_y;//0x28
		float position_z;//0x2C
		angle facing_yaw;//0x30
		angle facing_pitch;//0x34
	};
	TAG_BLOCK_SIZE_ASSERT(s_cutscene_flags_block, 0x38);
	tag_block<s_cutscene_flags_block> cutscene_flags;//0x1E0
	struct s_cutscene_camera_points_block
	{
		enum class e_flags : __int16
		{
			edit_as_relative = FLAG(0),
		};
		e_flags flags;//0x0
		enum class e_type : __int16
		{
			normal = 0,
			ignore_target_orientation = 1,
			dolly = 2,
			ignore_target_updates = 3,
		};
		e_type type;//0x2
		tag_string32 name;//0x4
		float position_x;//0x24
		float position_y;//0x28
		float position_z;//0x2C
		angle orientation_yaw;//0x30
		angle orientation_pitch;//0x34
		angle orientation_roll;//0x38
		angle unused;//0x3C
	};
	TAG_BLOCK_SIZE_ASSERT(s_cutscene_camera_points_block, 0x40);
	tag_block<s_cutscene_camera_points_block> cutscene_camera_points;//0x1E8
	struct s_cutscene_titles_block
	{
		string_id name;//0x0
		__int16 text_bounds_on_screentop;//0x4
		__int16 text_bounds_on_screenleft;//0x6
		__int16 text_bounds_on_screenbottom;//0x8
		__int16 text_bounds_on_screenright;//0xA
		enum class e_justification : __int16
		{
			left = 0,
			right = 1,
			center = 2,
			custom_text_entry = 3,
		};
		e_justification justification;//0xC
		enum class e_font : __int16
		{
			terminal_font = 0,
			body_text_font = 1,
			title_font = 2,
			super_large_font = 3,
			large_body_text_font = 4,
			split_screen_hud_message_font = 5,
			full_screen_hud_message_font = 6,
			english_body_text_font = 7,
			hud_number_font = 8,
			subtitle_font = 9,
			main_menu_font = 10,
			text_chat_font = 11,
		};
		e_font font;//0xE
		PAD(0x8);//0x10
		float fade_in_time;//0x18
		float up_time;//0x1C
		float fade_out_time;//0x20
	};
	TAG_BLOCK_SIZE_ASSERT(s_cutscene_titles_block, 0x24);
	tag_block<s_cutscene_titles_block> cutscene_titles;//0x1F0
	tag_reference custom_object_names;//0x1F8
	tag_reference chapter_title_text;//0x200
	tag_reference hud_messages;//0x208
	struct s_structure_bsps_block
	{
		PAD(0x10);//0x0
		tag_reference structure_bsp;//0x10
		tag_reference structure_lightmap;//0x18
		PAD(0x4);//0x20
		float unused_radiance_est_search_distance;//0x24
		PAD(0x4);//0x28
		float unused_luminels_per_world_unit;//0x2C
		float unused_output_white_reference;//0x30
		PAD(0x8);//0x34
		enum class e_flags : __int16
		{
			default_sky_enabled = FLAG(0),
		};
		e_flags flags;//0x3C
		PAD(0x2);//0x3E
		__int16 default_sky;//0x40
		PAD(0x2);//0x42
	};
	TAG_BLOCK_SIZE_ASSERT(s_structure_bsps_block, 0x44);
	tag_block<s_structure_bsps_block> structure_bsps;//0x210
	struct s_scenario_resources_block
	{
		struct s_references_block
		{
			tag_reference reference;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_references_block, 0x8);
		tag_block<s_references_block> references;//0x0
		struct s_script_source_block
		{
			tag_reference reference;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_script_source_block, 0x8);
		tag_block<s_script_source_block> script_source;//0x8
		struct s_ai_resources_block
		{
			tag_reference reference;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_ai_resources_block, 0x8);
		tag_block<s_ai_resources_block> ai_resources;//0x10
	};
	TAG_BLOCK_SIZE_ASSERT(s_scenario_resources_block, 0x18);
	tag_block<s_scenario_resources_block> scenario_resources;//0x218
	struct s_scenario_resources_block1
	{
		data_block mopp_code;//0x0
		struct s_evironment_object_identifiers_block
		{
			__int32 unique_id;//0x0
			__int16 origin_bsp_index;//0x4
			enum class e_type : __int8
			{
				biped = 0,
				vehicle = 1,
				weapon = 2,
				equipment = 3,
				garbage = 4,
				projectile = 5,
				scenery = 6,
				machine = 7,
				control = 8,
				light_fixture = 9,
				sound_scenery = 10,
				crate = 11,
				creature = 12,
			};
			e_type type;//0x6
			enum class e_source : __int8
			{
				structure = 0,
				editor = 1,
				dynamic = 2,
				legacy = 3,
			};
			e_source source;//0x7
		};
		TAG_BLOCK_SIZE_ASSERT(s_evironment_object_identifiers_block, 0x8);
		tag_block<s_evironment_object_identifiers_block> evironment_object_identifiers;//0x8
		PAD(0x4);//0x10
		float mopp_bounds_min_x;//0x14
		float mopp_bounds_min_y;//0x18
		float mopp_bounds_min_z;//0x1C
		float mopp_bounds_max_x;//0x20
		float mopp_bounds_max_y;//0x24
		float mopp_bounds_max_z;//0x28
	};
	TAG_BLOCK_SIZE_ASSERT(s_scenario_resources_block1, 0x2C);
	tag_block<s_scenario_resources_block1> scenario_resources1;//0x220
	struct s_hs_unit_seats_block
	{
		datum unit;//0x0		
		enum class e_seats : __int32
		{
			seat_0 = FLAG(0),
			seat_1 = FLAG(1),
			seat_2 = FLAG(2),
			seat_3 = FLAG(3),
			seat_4 = FLAG(4),
			seat_5 = FLAG(5),
			seat_6 = FLAG(6),
			seat_7 = FLAG(7),
			seat_8 = FLAG(8),
			seat_9 = FLAG(9),
			seat_10 = FLAG(10),
			seat_11 = FLAG(11),
			seat_12 = FLAG(12),
			seat_13 = FLAG(13),
			seat_14 = FLAG(14),
			seat_15 = FLAG(15),
			seat_16 = FLAG(16),
			seat_17 = FLAG(17),
			seat_18 = FLAG(18),
			seat_19 = FLAG(19),
			seat_20 = FLAG(20),
			seat_21 = FLAG(21),
			seat_22 = FLAG(22),
			seat_23 = FLAG(23),
			seat_24 = FLAG(24),
			seat_25 = FLAG(25),
			seat_26 = FLAG(26),
			seat_27 = FLAG(27),
			seat_28 = FLAG(28),
			seat_29 = FLAG(29),
			seat_30 = FLAG(30),
			seat_31 = FLAG(31),
		};
		e_seats seats;//0x4

	};
	TAG_BLOCK_SIZE_ASSERT(s_hs_unit_seats_block, 0x8);
	tag_block<s_hs_unit_seats_block> hs_unit_seats;//0x228
	struct s_scenario_kill_triggers_block
	{
		__int16 trigger_volume;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_scenario_kill_triggers_block, 0x2);
	tag_block<s_scenario_kill_triggers_block> scenario_kill_triggers;//0x230
	struct s_script_expressions_block
	{
		unsigned __int16 salt;//0x0
		__int16 opcode;//0x2
		enum class e_value_type : __int16
		{
			unparsed = 0,
			special_form = 1,
			function_name = 2,
			passthrough = 3,
			_void = 4,
			boolean = 5,
			real = 6,
			_short = 7,
			_long = 8,
			string = 9,
			script = 10,
			string_id = 11,
			unit_seat_mapping = 12,
			trigger_volume = 13,
			cutscene_flag = 14,
			cutscene_camera_point = 15,
			cutscene_title = 16,
			cutscene_recording = 17,
			device_group = 18,
			ai = 19,
			ai_command_list = 20,
			ai_command_script = 21,
			ai_behavior = 22,
			ai_orders = 23,
			starting_profile = 24,
			conversation = 25,
			structure_bsp = 26,
			navpoint = 27,
			point_reference = 28,
			style = 29,
			hud_message = 30,
			object_list = 31,
			sound = 32,
			effect = 33,
			damage = 34,
			looping_sound = 35,
			animation_graph = 36,
			damage_effect = 37,
			object_definition = 38,
			bitmap = 39,
			shader = 40,
			render_model = 41,
			structure_definition = 42,
			lightmap_definition = 43,
			game_difficulty = 44,
			team = 45,
			actor_type = 46,
			hud_corner = 47,
			model_state = 48,
			network_event = 49,
			object = 50,
			unit = 51,
			vehicle = 52,
			weapon = 53,
			device = 54,
			scenery = 55,
			object_name = 56,
			unit_name = 57,
			vehicle_name = 58,
			weapon_name = 59,
			device_name = 60,
			scenery_name = 61,
		};
		e_value_type value_type;//0x4
		enum class e_expression_type : __int16
		{
			primitive = FLAG(0),
			user_function = FLAG(1),
			global_reference = FLAG(2),
			bit_4 = FLAG(3),
			bit_5 = FLAG(4),
			bit_6 = FLAG(5),
			bit_7 = FLAG(6),
			bit_8 = FLAG(7),
			bit_9 = FLAG(8),
			bit_10 = FLAG(9),
			bit_11 = FLAG(10),
			bit_12 = FLAG(11),
			bit_13 = FLAG(12),
			bit_14 = FLAG(13),
			bit_15 = FLAG(14),
			bit_16 = FLAG(15),
		};
		e_expression_type expression_type;//0x6		
		datum next_expression;//0x8			
		__int32 data;//0xC
		datum source_offset;//0x10		
	};
	TAG_BLOCK_SIZE_ASSERT(s_script_expressions_block, 0x14);
	tag_block<s_script_expressions_block> script_expressions;//0x238
	struct s_orders_block
	{
		tag_string32 name;//0x0
		__int16 style;//0x20
		PAD(0x2);//0x22
		enum class e_flags : __int32
		{
			locked = FLAG(0),
			always_active = FLAG(1),
			debug_on = FLAG(2),
			strict_area_def = FLAG(3),
			follow_closest_player = FLAG(4),
			follow_squad = FLAG(5),
			active_camo = FLAG(6),
			suppress_combat_until_engaged = FLAG(7),
			inhibit_vehicle_use = FLAG(8),
		};
		e_flags flags;//0x24
		enum class e_force_combat_status : __int16
		{
			none = 0,
			asleep = 1,
			idle = 2,
			alert = 3,
			combat = 4,
		};
		e_force_combat_status force_combat_status;//0x28
		PAD(0x2);//0x2A
		tag_string32 entry_script;//0x2C
		PAD(0x2);//0x4C
		__int16 follow_squad;//0x4E
		float follow_radius;//0x50
		struct s_primary_area_set_block
		{
			enum class e_area_type : __int16
			{
				deault = 0,
				search = 1,
				goal = 2,
			};
			e_area_type area_type;//0x0
			PAD(0x2);//0x2
			__int16 zone;//0x4
			__int16 area;//0x6
		};
		TAG_BLOCK_SIZE_ASSERT(s_primary_area_set_block, 0x8);
		tag_block<s_primary_area_set_block> primary_area_set;//0x54
		struct s_secondary_area_set_block
		{
			enum class e_area_type : __int16
			{
				deault = 0,
				search = 1,
				goal = 2,
			};
			e_area_type area_type;//0x0
			PAD(0x2);//0x2
			__int16 zone;//0x4
			__int16 area;//0x6
		};
		TAG_BLOCK_SIZE_ASSERT(s_secondary_area_set_block, 0x8);
		tag_block<s_secondary_area_set_block> secondary_area_set;//0x5C
		struct s_secondary_set_trigger_block
		{
			enum class e_combination_rule : __int16
			{
				or = 0,
				and = 1,
			};
			e_combination_rule combination_rule;//0x0
			enum class e_dialogue_type : __int16
			{
				none = 0,
				advance = 1,
				charge = 2,
				fall_back = 3,
				retreat = 4,
				moveone = 5,
				arrival = 6,
				enter_vehicle = 7,
				exit_vehicle = 8,
				follow_player = 9,
				leave_player = 10,
				support = 11,
			};
			e_dialogue_type dialogue_type;//0x2
			struct s_triggers_block
			{
				enum class e_trigger_flags : __int32
				{
					not = FLAG(0),
				};
				e_trigger_flags trigger_flags;//0x0
				__int16 trigger;//0x4
				PAD(0x2);//0x6
			};
			TAG_BLOCK_SIZE_ASSERT(s_triggers_block, 0x8);
			tag_block<s_triggers_block> triggers;//0x4
		};
		TAG_BLOCK_SIZE_ASSERT(s_secondary_set_trigger_block, 0xC);
		tag_block<s_secondary_set_trigger_block> secondary_set_trigger;//0x64
		struct s_special_movement_block
		{
			enum class e_special_movement_1 : __int32
			{
				jump = FLAG(0),
				climb = FLAG(1),
				vault = FLAG(2),
				mount = FLAG(3),
				hoist = FLAG(4),
				wall_jump = FLAG(5),
				na = FLAG(6),
			};
			e_special_movement_1 special_movement_1;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_special_movement_block, 0x4);
		tag_block<s_special_movement_block> special_movement;//0x6C
		struct s_order_endings_block
		{
			__int16 next_order;//0x0
			enum class e_combination_rule : __int16
			{
				or = 0,
				and = 1,
			};
			e_combination_rule combination_rule;//0x2
			float delay_time;//0x4
			enum class e_dialogue_type : __int16
			{
				none = 0,
				advance = 1,
				charge = 2,
				fall_back = 3,
				retreat = 4,
				moveone = 5,
				arrival = 6,
				enter_vehicle = 7,
				exit_vehicle = 8,
				follow_player = 9,
				leave_player = 10,
				support = 11,
			};
			e_dialogue_type dialogue_type;//0x8
			PAD(0x2);//0xA
			struct s_triggers_block
			{
				enum class e_trigger_flags : __int32
				{
					not = FLAG(0),
				};
				e_trigger_flags trigger_flags;//0x0
				__int16 trigger;//0x4
				PAD(0x2);//0x6
			};
			TAG_BLOCK_SIZE_ASSERT(s_triggers_block, 0x8);
			tag_block<s_triggers_block> triggers;//0xC
		};
		TAG_BLOCK_SIZE_ASSERT(s_order_endings_block, 0x14);
		tag_block<s_order_endings_block> order_endings;//0x74
	};
	TAG_BLOCK_SIZE_ASSERT(s_orders_block, 0x7C);
	tag_block<s_orders_block> orders;//0x240
	struct s_triggers_block
	{
		tag_string32 name;//0x0
		enum class e_trigger_flags : __int32
		{
			latch_on_when_triggered = FLAG(0),
		};
		e_trigger_flags trigger_flags;//0x20
		enum class e_combination_rule : __int16
		{
			or = 0,
			and = 1,
		};
		e_combination_rule combination_rule;//0x24
		PAD(0x2);//0x26
		struct s_conditions_block
		{
			enum class e_rule_type : __int16
			{
				a_or_greater_alive = 0,
				a_or_fewer_alive = 1,
				x_or_greater_strength = 2,
				x_or_less_strength = 3,
				if_enemy_sighted = 4,
				after_a_ticks = 5,
				if_alerted_by_squad_a = 6,
				script_ref_true = 7,
				script_ref_false = 8,
				if_player_in_trigger_volume = 9,
				if_all_players_in_trigger_volume = 10,
				combat_status_a_or_more = 11,
				combat_status_a_or_less = 12,
				arrived = 13,
				in_vehicle = 14,
				sighted_player = 15,
				a_or_greater_fighting = 16,
				a_or_fewer_fighting = 17,
				player_within_x_world_units = 18,
				player_shot_more_than_x_seconds_ago = 19,
				game_safe_to_save = 20,
			};
			e_rule_type rule_type;//0x0
			__int16 squad;//0x2
			__int16 squad_group;//0x4
			__int16 a;//0x6
			float x;//0x8
			__int16 trigger_volume;//0xC
			PAD(0x2);//0xE
			tag_string32 exit_condition_script;//0x10
			__int16 NUM_;//0x30
			PAD(0x2);//0x32
			enum class e_flags : __int32
			{
				not = FLAG(0),
			};
			e_flags flags;//0x34
		};
		TAG_BLOCK_SIZE_ASSERT(s_conditions_block, 0x38);
		tag_block<s_conditions_block> conditions;//0x28
	};
	TAG_BLOCK_SIZE_ASSERT(s_triggers_block, 0x30);
	tag_block<s_triggers_block> triggers;//0x248
	struct s_background_sound_palette_block
	{
		tag_string32 name;//0x0
		tag_reference background_sound;//0x20
		tag_reference inside_cluster_sound;//0x28
		PAD(0x14);//0x30
		float cutoff_distance;//0x44
		enum class e_scale_flags : __int32
		{
			override_default_scale = FLAG(0),
			use_adjacent_cluster_as_portal_scale = FLAG(1),
			use_adjacent_cluster_as_exterior_scale = FLAG(2),
			scale_with_weather_intensity = FLAG(3),
		};
		e_scale_flags scale_flags;//0x48
		float interior_scale;//0x4C
		float portal_scale;//0x50
		float exterior_scale;//0x54
		float interpolation_speed;//0x58
		PAD(0x8);//0x5C
	};
	TAG_BLOCK_SIZE_ASSERT(s_background_sound_palette_block, 0x64);
	tag_block<s_background_sound_palette_block> background_sound_palette;//0x250
	struct s_sound_environment_palette_block
	{
		tag_string32 name;//0x0
		tag_reference sound_environment;//0x20
		float cutoff_distance;//0x28
		float interpolation_speed;//0x2C
		PAD(0x18);//0x30
	};
	TAG_BLOCK_SIZE_ASSERT(s_sound_environment_palette_block, 0x48);
	tag_block<s_sound_environment_palette_block> sound_environment_palette;//0x258
	struct s_weather_palette_block
	{
		tag_string32 name;//0x0
		tag_reference weather_system;//0x20
		PAD(0x24);//0x28
		tag_reference wind;//0x4C
		PAD(0xC);//0x54
		float wind_magnitude;//0x60
		PAD(0x4);//0x64
		tag_string32 wind_scale_function;//0x68
	};
	TAG_BLOCK_SIZE_ASSERT(s_weather_palette_block, 0x88);
	tag_block<s_weather_palette_block> weather_palette;//0x260
	PAD(0x8 * 5);//0x268
	struct s_scenario_cluster_data_block
	{
		tag_reference bsp;//0x0
		struct s_background_sounds_block
		{
			__int16 type;//0x0
			PAD(0x2);//0x2
		};
		TAG_BLOCK_SIZE_ASSERT(s_background_sounds_block, 0x4);
		tag_block<s_background_sounds_block> background_sounds;//0x8
		struct s_sound_environments_block
		{
			__int16 type;//0x0
			PAD(0x2);//0x2
		};
		TAG_BLOCK_SIZE_ASSERT(s_sound_environments_block, 0x4);
		tag_block<s_sound_environments_block> sound_environments;//0x10
		__int32 bsp_checksum;//0x18
		struct s_cluster_centroids_block
		{
			float centroid_x;//0x0
			float centroid_y;//0x4
			float centroid_z;//0x8
		};
		TAG_BLOCK_SIZE_ASSERT(s_cluster_centroids_block, 0xC);
		tag_block<s_cluster_centroids_block> cluster_centroids;//0x1C
		struct s_weather_properties_block
		{
			__int16 type;//0x0
			PAD(0x2);//0x2
		};
		TAG_BLOCK_SIZE_ASSERT(s_weather_properties_block, 0x4);
		tag_block<s_weather_properties_block> weather_properties;//0x24
		struct s_atmospheric_fog_properties_block
		{
			__int16 type;//0x0
			PAD(0x2);//0x2
		};
		TAG_BLOCK_SIZE_ASSERT(s_atmospheric_fog_properties_block, 0x4);
		tag_block<s_atmospheric_fog_properties_block> atmospheric_fog_properties;//0x2C
	};
	TAG_BLOCK_SIZE_ASSERT(s_scenario_cluster_data_block, 0x34);
	tag_block<s_scenario_cluster_data_block> scenario_cluster_data;//0x290
	PAD(0x4 * 32);//0x298
	struct s_spawn_data_block
	{
		float dynamic_spawn_lower_height;//0x0
		float dynamic_spawn_upper_height;//0x4
		float game_object_reset_height;//0x8
		PAD(0x3C);//0xC
		struct s_dynamic_spawn_overloads_block
		{
			enum class e_overload_type : __int16
			{
				enemy = 0,
				_friend = 1,
				enemy_vehicle = 2,
				friendly_vehicle = 3,
				empty_vehicle = 4,
				oddball_inclusion = 5,
				oddball_exclusion = 6,
				hill_inclusion = 7,
				hill_exclusion = 8,
				last_race_flag = 9,
				dead_ally = 10,
				controlled_territory = 11,
			};
			e_overload_type overload_type;//0x0
			PAD(0x2);//0x2
			float inner_radius;//0x4
			float outer_radius;//0x8
			float weight;//0xC
		};
		TAG_BLOCK_SIZE_ASSERT(s_dynamic_spawn_overloads_block, 0x10);
		tag_block<s_dynamic_spawn_overloads_block> dynamic_spawn_overloads;//0x48
		struct s_static_respawn_zones_block
		{
			string_id name;//0x0
			enum class e_relevant_team : __int32
			{
				red_alpha = FLAG(0),
				blue_bravo = FLAG(1),
				yellow_charlie = FLAG(2),
				green_delta = FLAG(3),
				purple_echo = FLAG(4),
				orange_foxtrot = FLAG(5),
				brown_golf = FLAG(6),
				pink_hotel = FLAG(7),
				neutral = FLAG(8),
			};
			e_relevant_team relevant_team;//0x4
			enum class e_relevant_games : __int32
			{
				slayer = FLAG(0),
				oddball = FLAG(1),
				king_of_the_hill = FLAG(2),
				capture_the_flag = FLAG(3),
				race = FLAG(4),
				headhunter = FLAG(5),
				juggernaut = FLAG(6),
				territories = FLAG(7),
			};
			e_relevant_games relevant_games;//0x8
			enum class e_flags : __int32
			{
				disabled_if_flag_home = FLAG(0),
				disabled_if_flag_away = FLAG(1),
				disabled_if_bomb_home = FLAG(2),
				disabled_if_bomb_away = FLAG(3),
			};
			e_flags flags;//0xC
			float position_x;//0x10
			float position_y;//0x14
			float position_z;//0x18
			float lower_height;//0x1C
			float upper_height;//0x20
			float inner_radius;//0x24
			float outer_radius;//0x28
			float weight;//0x2C
		};
		TAG_BLOCK_SIZE_ASSERT(s_static_respawn_zones_block, 0x30);
		tag_block<s_static_respawn_zones_block> static_respawn_zones;//0x50
		struct s_static_initial_spawn_zones_block
		{
			string_id name;//0x0
			enum class e_relevant_team : __int32
			{
				red_alpha = FLAG(0),
				blue_bravo = FLAG(1),
				yellow_charlie = FLAG(2),
				green_delta = FLAG(3),
				purple_echo = FLAG(4),
				orange_foxtrot = FLAG(5),
				brown_golf = FLAG(6),
				pink_hotel = FLAG(7),
				neutral = FLAG(8),
			};
			e_relevant_team relevant_team;//0x4
			enum class e_relevant_games : __int32
			{
				slayer = FLAG(0),
				oddball = FLAG(1),
				king_of_the_hill = FLAG(2),
				capture_the_flag = FLAG(3),
				race = FLAG(4),
				headhunter = FLAG(5),
				juggernaut = FLAG(6),
				territories = FLAG(7),
			};
			e_relevant_games relevant_games;//0x8
			enum class e_flags : __int32
			{
				disabled_if_flag_home = FLAG(0),
				disabled_if_flag_away = FLAG(1),
				disabled_if_bomb_home = FLAG(2),
				disabled_if_bomb_away = FLAG(3),
			};
			e_flags flags;//0xC
			float position_x;//0x10
			float position_y;//0x14
			float position_z;//0x18
			float lower_height;//0x1C
			float upper_height;//0x20
			float inner_radius;//0x24
			float outer_radius;//0x28
			float weight;//0x2C
		};
		TAG_BLOCK_SIZE_ASSERT(s_static_initial_spawn_zones_block, 0x30);
		tag_block<s_static_initial_spawn_zones_block> static_initial_spawn_zones;//0x58
	};
	TAG_BLOCK_SIZE_ASSERT(s_spawn_data_block, 0x60);
	tag_block<s_spawn_data_block> spawn_data;//0x318
	tag_reference sound_effect_collection;//0x320
	struct s_crates_block
	{
		__int16 type;//0x0
		__int16 name;//0x2
		enum class e_placement_flags : __int32
		{
			not_automatically = FLAG(0),
			unused = FLAG(1),
			unused1 = FLAG(2),
			unused2 = FLAG(3),
			lock_type_to_env_object = FLAG(4),
			lock_transform_to_env_object = FLAG(5),
			never_placed = FLAG(6),
			lock_name_to_env_object = FLAG(7),
			create_at_rest = FLAG(8),
		};
		e_placement_flags placement_flags;//0x4
		float position_x;//0x8
		float position_y;//0xC
		float position_z;//0x10
		angle rotation_yaw;//0x14
		angle rotation_pitch;//0x18
		angle rotation_roll;//0x1C
		float scale;//0x20
		enum class e_transform_flags : __int16
		{
			mirrored = FLAG(0),
		};
		e_transform_flags transform_flags;//0x24
		__int16 manual_bsp_flags;//0x26
		__int32 unique_id;//0x28
		__int16 origin_bsp_index;//0x2C
		enum class e_type : __int8
		{
			biped = 0,
			vehicle = 1,
			weapon = 2,
			equipment = 3,
			garbage = 4,
			projectile = 5,
			scenery = 6,
			machine = 7,
			control = 8,
			light_fixture = 9,
			sound_scenery = 10,
			crate = 11,
			creature = 12,
		};
		e_type type1;//0x2E
		enum class e_source : __int8
		{
			structure = 0,
			editor = 1,
			dynamic = 2,
			legacy = 3,
		};
		e_source source;//0x2F
		enum class e_bsp_policy : __int8
		{
			default = 0,
			always_placed = 1,
			manual_bsp_placement = 2,
		};
		e_bsp_policy bsp_policy;//0x30
		PAD(0x1);//0x31
		__int16 editor_folder;//0x32
		string_id variant_name;//0x34
		enum class e_active_change_colors : __int32
		{
			primary = FLAG(0),
			secondary = FLAG(1),
			tertiary = FLAG(2),
			quaternary = FLAG(3),
		};
		e_active_change_colors active_change_colors;//0x38
		PAD(0x10);//0x3C
	};
	TAG_BLOCK_SIZE_ASSERT(s_crates_block, 0x4C);
	tag_block<s_crates_block> crates;//0x328
	struct s_crates_palette_block
	{
		tag_reference name;//0x0
		PAD(0x20);//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_crates_palette_block, 0x28);
	tag_block<s_crates_palette_block> crates_palette;//0x330
	tag_reference global_lighting;//0x338
	struct s_atmospheric_fog_palette_block
	{
		string_id name;//0x0
		struct {

			real_color_rgb color;//0x4
			float spread_distance;//0x10
			PAD(0x4);//0x14
			float maximum_density;//0x18
			float start_distance;//0x1C
			float opaque_distance;//0x20

		}atmospheric_fog;

		struct {

			real_color_rgb color;//0x24
			PAD(0x4);//0x30
			float maximum_density;//0x34
			float start_distance;//0x38
			float opaque_distance;//0x3C
		}secondary_fog;

		//plane fog override
		PAD(0x4);//0x40
		real_color_rgb planar_color;//0x44
		float planar_max_density;//0x50
		float planar_override_amount;//0x54
		float planar_min_distance_bias;//0x58
		PAD(0x2C);//0x5C

		//patchy fog
		real_color_rgb patchy_color;//0x88
		PAD(0xC);//0x94
		real_bounds patchy_density;//0xA0
		real_bounds patchy_distance;//0xA8
		PAD(0x20);//0xB0
		tag_reference patchy_fog;//0xD0

		struct s_mixers_block
		{
			PAD(0x4);//0x0
			string_id atmospheric_fog_source;//0x4
			string_id interpolator;//0x8
			PAD(0x4);//0xC
		};
		TAG_BLOCK_SIZE_ASSERT(s_mixers_block, 0x10);
		tag_block<s_mixers_block> mixers;//0xD8
		float amount;//0xE0
		float threshold;//0xE4
		float brightness;//0xE8
		float gamma_power;//0xEC
		enum class e_camera_immersion_flags : __int16
		{
			disable_atmospheric_fog = FLAG(0),
			disable_secondary_fog = FLAG(1),
			disable_planar_fog = FLAG(2),
			invert_planar_fog_priorities = FLAG(3),
			disable_water = FLAG(4),
		};
		e_camera_immersion_flags camera_immersion_flags;//0xF0
		PAD(0x2);//0xF2
	};
	TAG_BLOCK_SIZE_ASSERT(s_atmospheric_fog_palette_block, 0xF4);
	tag_block<s_atmospheric_fog_palette_block> atmospheric_fog_palette;//0x340
	struct s_planar_fog_palette_block
	{
		string_id name;//0x0
		tag_reference planar_fog;//0x4
		PAD(0x4);//0xC
	};
	TAG_BLOCK_SIZE_ASSERT(s_planar_fog_palette_block, 0x10);
	tag_block<s_planar_fog_palette_block> planar_fog_palette;//0x348
	struct s_flocks_block
	{
		__int16 bsp;//0x0
		PAD(0x2);//0x2
		__int16 bounding_volume;//0x4
		enum class e_flags : __int16
		{
			hard_boundaries_on_volume = FLAG(0),
			flock_initially_stopped = FLAG(1),
		};
		e_flags flags;//0x6
		float ecology_margin;//0x8
		struct s_sources_block
		{
			PAD(0xC);//0x0
			real_euler_angles2d starting_yaw_pitch;//0xC			
			float radius;//0x14
			float weight;//0x18
		};
		TAG_BLOCK_SIZE_ASSERT(s_sources_block, 0x1C);
		tag_block<s_sources_block> sources;//0xC
		struct s_sinks_block
		{
			PAD(0xC);//0x0
			float radius;//0xC
		};
		TAG_BLOCK_SIZE_ASSERT(s_sinks_block, 0x10);
		tag_block<s_sinks_block> sinks;//0x14
		float production_frequency;//0x1C
		float scale_lower;//0x20
		float scale_upper;//0x24
		tag_reference creature;//0x28
		__int16 boid_count_lower;//0x30
		__int16 boid_count_upper;//0x32
		float neighborhood_radius;//0x34
		float avoidance_radius;//0x38
		float forward_scale;//0x3C
		float alignment_scale;//0x40
		float avoidance_scale;//0x44
		float leveling_force_scale;//0x48
		float sink_scale;//0x4C
		angle perception_angle;//0x50
		float average_throttle;//0x54
		float maximum_throttle;//0x58
		float position_scale;//0x5C
		float position_min_radius;//0x60
		float position_max_radius;//0x64
		float movement_weight_threshold;//0x68
		float danger_radius;//0x6C
		float danger_scale;//0x70
		float random_offset_scale;//0x74
		real_bounds random_offset_period;//0x78	
		string_id flock_name;//0x80
	};
	TAG_BLOCK_SIZE_ASSERT(s_flocks_block, 0x84);
	tag_block<s_flocks_block> flocks;//0x350
	tag_reference subtitles;//0x358
	struct s_decorators_block
	{
		float grid_origin_x;//0x0
		float grid_origin_y;//0x4
		float grid_origin_z;//0x8
		__int32 cell_count_per_dimension;//0xC
		struct s_cache_blocks_block
		{
			__int32 block_offset;//0x0
			__int32 block_size;//0x4
			__int32 section_data_size;//0x8
			__int32 resource_data_size;//0xC
			struct s_resources_block
			{
				enum class e_type : __int8
				{
					tag_block = 0,
					tag_data = 1,
					vertex_buffer = 2,
				};
				e_type type;//0x0
				PAD(0x3);//0x1
				__int16 primary_locator;//0x4
				__int16 secondary_locator;//0x6
				__int32 resource_data_size;//0x8
				__int32 resource_data_offset;//0xC
			};
			TAG_BLOCK_SIZE_ASSERT(s_resources_block, 0x10);
			tag_block<s_resources_block> resources;//0x10
			PAD(0x4);//0x18
			__int16 owner_tag_section_offset;//0x1C
			PAD(0x6);//0x1E
			struct s_cache_block_data_block
			{
				struct s_placements_block
				{
					__int32 internal_data_1;//0x0
					__int32 compressed_position;//0x4
					PAD(0x8);//0x8
					__int32 compressed_light_direction;//0x10
					__int32 compressed_light_2_direction;//0x14
				};
				TAG_BLOCK_SIZE_ASSERT(s_placements_block, 0x18);
				tag_block<s_placements_block> placements;//0x0
				struct s_decal_vertices_block
				{
					float position_x;//0x0
					float position_y;//0x4
					float position_z;//0x8
					float texcoord_0_x;//0xC
					float texcoord_0_y;//0x10
					float texcoord_1_x;//0x14
					float texcoord_1_y;//0x18
					PAD(0x4);//0x1C
				};
				TAG_BLOCK_SIZE_ASSERT(s_decal_vertices_block, 0x20);
				tag_block<s_decal_vertices_block> decal_vertices;//0x8
				struct s_decal_indices_block
				{
					__int16 index;//0x0
				};
				TAG_BLOCK_SIZE_ASSERT(s_decal_indices_block, 0x2);
				tag_block<s_decal_indices_block> decal_indices;//0x10
				PAD(0x30);//0x18
				struct s_sprite_vertices_block
				{
					float position_x;//0x0
					float position_y;//0x4
					float position_z;//0x8
					PAD(0x18);//0xC
					float texcoord_x;//0x24
					float texcoord_y;//0x28
					PAD(0x4);//0x2C
				};
				TAG_BLOCK_SIZE_ASSERT(s_sprite_vertices_block, 0x30);
				tag_block<s_sprite_vertices_block> sprite_vertices;//0x48
				struct s_sprite_indices_block
				{
					__int16 index;//0x0
				};
				TAG_BLOCK_SIZE_ASSERT(s_sprite_indices_block, 0x2);
				tag_block<s_sprite_indices_block> sprite_indices;//0x50
				PAD(0x30);//0x58
			};
			TAG_BLOCK_SIZE_ASSERT(s_cache_block_data_block, 0x88);
			tag_block<s_cache_block_data_block> cache_block_data;//0x24
			PAD(0x8);//0x2C
		};
		TAG_BLOCK_SIZE_ASSERT(s_cache_blocks_block, 0x34);
		tag_block<s_cache_blocks_block> cache_blocks;//0x10
		struct s_groups_block
		{
			__int8 decorator_set;//0x0
			enum class e_decorator_type : __int8
			{
				model = 0,
				floating_decal = 1,
				projected_decal = 2,
				screen_facing_quad = 3,
				axis_rotating_quad = 4,
				cross_quad = 5,
			};
			e_decorator_type decorator_type;//0x1
			__int8 shader_index;//0x2
			__int8 compressed_radius;//0x3
			__int16 cluster;//0x4
			__int16 cache_block;//0x6
			__int16 decorator_start_index;//0x8
			__int16 decorator_count;//0xA
			__int16 vertex_start_offset;//0xC
			__int16 vertex_count;//0xE
			__int16 index_start_offset;//0x10
			__int16 index_count;//0x12
			__int32 compressed_bounding_center;//0x14
		};
		TAG_BLOCK_SIZE_ASSERT(s_groups_block, 0x18);
		tag_block<s_groups_block> groups;//0x18
		struct s_cells_block
		{
			__int16 child_index[8];//0x0			
			__int16 cache_block_index;//0x10
			__int16 group_count;//0x12
			__int32 group_start_index;//0x14
		};
		TAG_BLOCK_SIZE_ASSERT(s_cells_block, 0x18);
		tag_block<s_cells_block> cells;//0x20
		struct s_decals_block
		{
			__int8 decorator_set;//0x0
			__int8 decorator_class;//0x1
			__int8 decorator_permutation;//0x2
			__int8 sprite_index;//0x3
			real_point3d position;//0x4			
			PAD(0x24);//0x10
			real_point3d previous_position;//0x34
		};
		TAG_BLOCK_SIZE_ASSERT(s_decals_block, 0x40);
		tag_block<s_decals_block> decals;//0x28
	};
	TAG_BLOCK_SIZE_ASSERT(s_decorators_block, 0x30);
	tag_block<s_decorators_block> decorators;//0x360
	struct s_creatures_block
	{
		__int16 type;//0x0
		__int16 name;//0x2
		enum class e_placement_flags : __int32
		{
			not_automatically = FLAG(0),
			unused = FLAG(1),
			unused1 = FLAG(2),
			unused2 = FLAG(3),
			lock_type_to_env_object = FLAG(4),
			lock_transform_to_env_object = FLAG(5),
			never_placed = FLAG(6),
			lock_name_to_env_object = FLAG(7),
			create_at_rest = FLAG(8),
		};
		e_placement_flags placement_flags;//0x4
		float position_x;//0x8
		float position_y;//0xC
		float position_z;//0x10
		angle rotation_yaw;//0x14
		angle rotation_pitch;//0x18
		angle rotation_roll;//0x1C
		float scale;//0x20
		enum class e_transform_flags : __int16
		{
			mirrored = FLAG(0),
		};
		e_transform_flags transform_flags;//0x24
		__int16 manual_bsp_flags;//0x26
		__int32 unique_id;//0x28
		__int16 origin_bsp_index;//0x2C
		enum class e_type : __int8
		{
			biped = 0,
			vehicle = 1,
			weapon = 2,
			equipment = 3,
			garbage = 4,
			projectile = 5,
			scenery = 6,
			machine = 7,
			control = 8,
			light_fixture = 9,
			sound_scenery = 10,
			crate = 11,
			creature = 12,
		};
		e_type type1;//0x2E
		enum class e_source : __int8
		{
			structure = 0,
			editor = 1,
			dynamic = 2,
			legacy = 3,
		};
		e_source source;//0x2F
		enum class e_bsp_policy : __int8
		{
			default = 0,
			always_placed = 1,
			manual_bsp_placement = 2,
		};
		e_bsp_policy bsp_policy;//0x30
		PAD(0x1);//0x31
		__int16 editor_folder;//0x32
	};
	TAG_BLOCK_SIZE_ASSERT(s_creatures_block, 0x34);
	tag_block<s_creatures_block> creatures;//0x368
	struct s_creatures_palette_block
	{
		tag_reference name;//0x0
		PAD(0x20);//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_creatures_palette_block, 0x28);
	tag_block<s_creatures_palette_block> creatures_palette;//0x370
	struct s_decorators_palette_block
	{
		tag_reference decorator_set;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_decorators_palette_block, 0x8);
	tag_block<s_decorators_palette_block> decorators_palette;//0x378
	struct s_bsp_transition_volumes_block
	{
		__int32 bsp_index_key;//0x0
		__int16 trigger_volume;//0x4
		PAD(0x2);//0x6
	};
	TAG_BLOCK_SIZE_ASSERT(s_bsp_transition_volumes_block, 0x8);
	tag_block<s_bsp_transition_volumes_block> bsp_transition_volumes;//0x380
	struct s_structure_bsp_lighting_block
	{
		tag_reference bsp;//0x0
		struct s_lighting_points_block
		{
			float position_x;//0x0
			float position_y;//0x4
			float position_z;//0x8
		};
		TAG_BLOCK_SIZE_ASSERT(s_lighting_points_block, 0xC);
		tag_block<s_lighting_points_block> lighting_points;//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_structure_bsp_lighting_block, 0x10);
	tag_block<s_structure_bsp_lighting_block> structure_bsp_lighting;//0x388
	struct s_editor_folders_block
	{
		__int32 parent_folder;//0x0
		tag_string256 name;//0x4
	};
	TAG_BLOCK_SIZE_ASSERT(s_editor_folders_block, 0x104);
	tag_block<s_editor_folders_block> editor_folders;//0x390
	struct s_level_data_block
	{
		tag_reference level_description;//0x0
		struct s_campaign_level_data_block
		{
			__int32 campaign_id;//0x0
			__int32 map_id;//0x4
			tag_reference bitmap;//0x8
			PAD(0xB40);//0x10
		};
		TAG_BLOCK_SIZE_ASSERT(s_campaign_level_data_block, 0xB50);
		tag_block<s_campaign_level_data_block> campaign_level_data;//0x8
		struct s_multiplayer_block
		{
			__int32 map_id;//0x0
			tag_reference bitmap;//0x4
			tag_unicode32 english_name;//0xC
			tag_unicode32 japanese_name;//0x4C
			tag_unicode32 german_name;//0x8C
			tag_unicode32 french_name;//0xCC
			tag_unicode32 spanish_name;//0x10C
			tag_unicode32 italian_name;//0x14C
			tag_unicode32 korean_name;//0x18C
			tag_unicode32 chinese_name;//0x1CC
			tag_unicode32 portuguese_name;//0x20C
			tag_unicode128 english_description;//0x24C
			tag_unicode128 japanese_description;//0x34C
			tag_unicode128 german_description;//0x44C
			tag_unicode128 french_description;//0x54C
			tag_unicode128 spanish_description;//0x64C
			tag_unicode128 italian_description;//0x74C
			tag_unicode128 korean_description;//0x84C
			tag_unicode128 chinese_description;//0x94C
			tag_unicode128 portuguese_description;//0xA4C
			tag_string256 path;//0xB4C
			__int32 sort_order;//0xC4C
			enum class e_flags : __int8
			{
				unlockable = FLAG(0),
			};
			e_flags flags;//0xC50
			PAD(0x3);//0xC51
			__int8 max_teams_none;//0xC54
			__int8 max_teams_ctf;//0xC55
			__int8 max_teams_slayer;//0xC56
			__int8 max_teams_oddball;//0xC57
			__int8 max_teams_koth;//0xC58
			__int8 max_teams_race;//0xC59
			__int8 max_teams_headhunter;//0xC5A
			__int8 max_teams_juggernaut;//0xC5B
			__int8 max_teams_territories;//0xC5C
			__int8 max_teams_assault;//0xC5D
			__int8 max_teams_stub_10;//0xC5E
			__int8 max_teams_stub_11;//0xC5F
			__int8 max_teams_stub_12;//0xC60
			__int8 max_teams_stub_13;//0xC61
			__int8 max_teams_stub_14;//0xC62
			__int8 max_teams_stub_15;//0xC63
		};
		TAG_BLOCK_SIZE_ASSERT(s_multiplayer_block, 0xC64);
		tag_block<s_multiplayer_block> multiplayer;//0x10
	};
	TAG_BLOCK_SIZE_ASSERT(s_level_data_block, 0x18);
	tag_block<s_level_data_block> level_data;//0x398
	tag_reference territory_location_names;//0x3A0
	PAD(0x8);//0x3A8
	struct s_mission_dialogue_block
	{
		tag_reference mission_dialogue;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_mission_dialogue_block, 0x8);
	tag_block<s_mission_dialogue_block> mission_dialogue;//0x3B0
	tag_reference objectives;//0x3B8
	struct s_interpolators_block
	{
		string_id name;//0x0
		string_id accelerator_name;//0x4
		string_id multiplier_name;//0x8
		struct s_data_block
		{
			__int8 value;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_data_block, 0x1);
		tag_block<s_data_block> data;//0xC
		PAD(0x4);//0x14
	};
	TAG_BLOCK_SIZE_ASSERT(s_interpolators_block, 0x18);
	tag_block<s_interpolators_block> interpolators;//0x3C0
	struct s_shared_references_block
	{
		tag_reference reference;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_shared_references_block, 0x8);
	tag_block<s_shared_references_block> shared_references;//0x3C8
	struct s_screen_effect_references_block
	{
		PAD(0x10);//0x0
		tag_reference screen_effect;//0x10
		string_id primary_input;//0x18
		string_id secondary_input;//0x1C
		PAD(0x4);//0x20
	};
	TAG_BLOCK_SIZE_ASSERT(s_screen_effect_references_block, 0x24);
	tag_block<s_screen_effect_references_block> screen_effect_references;//0x3D0
	struct s_simulation_definition_table_block
	{
		datum tag;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_simulation_definition_table_block, 0x4);
	tag_block<s_simulation_definition_table_block> simulation_definition_table;//0x3D8
};
TAG_GROUP_SIZE_ASSERT(s_scenario_group_definition, 0x3E0);
#pragma pack(pop)

