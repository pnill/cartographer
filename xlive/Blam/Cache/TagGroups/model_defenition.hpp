#pragma once
#include "Blam\Cache\DataTypes\BlamDataTypes.h"
#include "Blam/Math/BlamMath.h"

/*********************************************************************
* name: model
* group_tag_hlmt
* header size_252
* *********************************************************************/
struct s_model_group_definition
{
	tag_reference render_model;//0x0
	tag_reference collision_model;//0x8
	tag_reference animation;//0x10
	tag_reference physics;//0x18
	tag_reference physics_model;//0x20
	float disappear_distance_world_units;//0x28
	float begin_fade_distance_world_units;//0x2C
	PAD(0x4);//0x30
	float reduce_to_l1_world_units_super_low;//0x34
	float reduce_to_l2_world_units_low;//0x38
	float reduce_to_l3_world_units_medium;//0x3C
	float reduce_to_l4_world_units_high;//0x40
	float reduce_to_l5_world_units_super_high;//0x44
	PAD(0x4);//0x48
	enum class e_shadow_fade_distance : __int16
	{
		fade_at_super_high_detail_level = 0,
		fade_at_high_detail_level = 1,
		fade_at_medium_detail_level = 2,
		fade_at_low_detail_level = 3,
		fade_at_super_low_detail_level = 4,
		fade_never = 5,
	};
	e_shadow_fade_distance shadow_fade_distance;//0x4C
	PAD(0x2);//0x4E
	struct s_variants_block
	{
		string_id name;//0x0
		PAD(0x10);//0x4
		struct s_regions_block
		{
			string_id region_name;//0x0
			PAD(0x2);//0x4
			__int16 parent_variant;//0x6
			struct s_permutations_block
			{
				string_id permutation_name;//0x0
				PAD(0x1);//0x4
				enum class e_flags : __int8
				{
					copy_states_to_all_permutations = FLAG(0),
				};
				e_flags flags;//0x5
				PAD(0x2);//0x6
				float probability_0;//0x8
				struct s_states_block
				{
					string_id permutation_name;//0x0
					PAD(0x1);//0x4
					enum class e_property_flags : __int8
					{
						blurred = FLAG(0),
						hella_blurred = FLAG(1),
						shielded = FLAG(2),
					};
					e_property_flags property_flags;//0x5
					enum class e_state : __int16
					{
						default = 0,
						minor_damage = 1,
						medium_damage = 2,
						major_damage = 3,
						destroyed = 4,
					};
					e_state state;//0x6
					tag_reference looping_effect;//0x8
					string_id looping_effect_marker_name;//0x10
					float initial_probability;//0x14
				};
				TAG_BLOCK_SIZE_ASSERT(s_states_block, 0x18);
				tag_block<s_states_block> states;//0xC
				PAD(0xC);//0x14
			};
			TAG_BLOCK_SIZE_ASSERT(s_permutations_block, 0x20);
			tag_block<s_permutations_block> permutations;//0x8
			enum class e_sort_order : __int16
			{
			no_sorting = 0,
			NUM_1_closest = 1,
			NUM_2 = 2,
			NUM_3 = 3,
			NUM_4 = 4,
			NUM_5 = 5,
			NUM_0_same_as_model = 6,
			NUM_6 = 7,
			NUM_7 = 8,
			NUM_8 = 9,
			NUM_9 = 10,
			NUM_5_farthest = 11,
			};
			e_sort_order sort_order;//0x10
			PAD(0x2);//0x12
		};
		TAG_BLOCK_SIZE_ASSERT(s_regions_block, 0x14);
		tag_block<s_regions_block> regions;//0x14
		struct s_objects_block
		{
			string_id parent_marker;//0x0
			string_id child_marker;//0x4
			tag_reference child_object;//0x8
		};
		TAG_BLOCK_SIZE_ASSERT(s_objects_block, 0x10);
		tag_block<s_objects_block> objects;//0x1C
		PAD(0x8);//0x24
		string_id dialogue_sound_effect;//0x2C
		tag_reference dialogue;//0x30
	};
	TAG_BLOCK_SIZE_ASSERT(s_variants_block, 0x38);
	tag_block<s_variants_block> variants;//0x50
	struct s_materials_block
	{
		string_id material_name;//0x0
		enum class e_material_type : __int16
		{
			dirt = 0,
			sand = 1,
			stone = 2,
			snow = 3,
			wood = 4,
			metal_hollow = 5,
			metal_thin = 6,
			metal_thick = 7,
			rubber = 8,
			glass = 9,
			force_field = 10,
			grunt = 11,
			hunter_armor = 12,
			hunter_skin = 13,
			elite = 14,
			jackal = 15,
			jackal_energy_shield = 16,
			engineer_skin = 17,
			engineer_force_field = 18,
			flood_combat_form = 19,
			flood_carrier_form = 20,
			cyborg_armor = 21,
			cyborg_energy_shield = 22,
			human_armor = 23,
			human_skin = 24,
			sentinel = 25,
			monitor = 26,
			plastic = 27,
			water = 28,
			leaves = 29,
			elite_energy_shield = 30,
			ice = 31,
			hunter_shield = 32,
		};
		e_material_type material_type;//0x4
		__int16 damage_section;//0x6
		PAD(0x4);//0x8
		string_id global_material_name;//0xC
		PAD(0x4);//0x10
	};
	TAG_BLOCK_SIZE_ASSERT(s_materials_block, 0x14);
	tag_block<s_materials_block> materials;//0x58
	struct s_new_damage_info_block
	{
		enum class e_flags : __int32
		{
			takes_shield_damage_for_children = FLAG(0),
			takes_body_damage_for_children = FLAG(1),
			always_shields_friendly_damage = FLAG(2),
			passes_area_damage_to_children = FLAG(3),
			parent_never_takes_body_damage_for_us = FLAG(4),
			only_damaged_by_explosives = FLAG(5),
			parent_never_takes_shield_damage_for_us = FLAG(6),
			cannot_die_from_damage = FLAG(7),
			passes_attached_damage_to_riders = FLAG(8),
		};
		e_flags flags;//0x0
		string_id global_indirect_material_name;//0x4
		__int16 indirect_damage_section;//0x8
		PAD(0x6);//0xA
		enum class e_collision_damage_reporting_type : __int8
		{
			teh_guardians11 = 0,
			falling_damage = 1,
			generic_collision_damage = 2,
			generic_melee_damage = 3,
			generic_explosion = 4,
			magnum_pistol = 5,
			plasma_pistol = 6,
			needler = 7,
			smg = 8,
			plasma_rifle = 9,
			battle_rifle = 10,
			carbine = 11,
			shotgun = 12,
			sniper_rifle = 13,
			beam_rifle = 14,
			rocket_launcher = 15,
			flak_cannon = 16,
			brute_shot = 17,
			disintegrator = 18,
			brute_plasma_rifle = 19,
			energy_sword = 20,
			frag_grenade = 21,
			plasma_grenade = 22,
			flag_melee_damage = 23,
			bomb_melee_damage = 24,
			bomb_explosion_damage = 25,
			ball_melee_damage = 26,
			human_turret = 27,
			plasma_turret = 28,
			banshee = 29,
			ghost = 30,
			mongoose = 31,
			scorpion = 32,
			spectre_driver = 33,
			spectre_gunner = 34,
			warthog_driver = 35,
			warthog_gunner = 36,
			wraith = 37,
			tank = 38,
			sentinel_beam = 39,
			sentinel_rpg = 40,
			teleporter = 41,
		};
		e_collision_damage_reporting_type collision_damage_reporting_type;//0x10
		enum class e_response_damage_reporting_type : __int8
		{
			teh_guardians11 = 0,
			falling_damage = 1,
			generic_collision_damage = 2,
			generic_melee_damage = 3,
			generic_explosion = 4,
			magnum_pistol = 5,
			plasma_pistol = 6,
			needler = 7,
			smg = 8,
			plasma_rifle = 9,
			battle_rifle = 10,
			carbine = 11,
			shotgun = 12,
			sniper_rifle = 13,
			beam_rifle = 14,
			rocket_launcher = 15,
			flak_cannon = 16,
			brute_shot = 17,
			disintegrator = 18,
			brute_plasma_rifle = 19,
			energy_sword = 20,
			frag_grenade = 21,
			plasma_grenade = 22,
			flag_melee_damage = 23,
			bomb_melee_damage = 24,
			bomb_explosion_damage = 25,
			ball_melee_damage = 26,
			human_turret = 27,
			plasma_turret = 28,
			banshee = 29,
			ghost = 30,
			mongoose = 31,
			scorpion = 32,
			spectre_driver = 33,
			spectre_gunner = 34,
			warthog_driver = 35,
			warthog_gunner = 36,
			wraith = 37,
			tank = 38,
			sentinel_beam = 39,
			sentinel_rpg = 40,
			teleporter = 41,
		};
		e_response_damage_reporting_type response_damage_reporting_type;//0x11
		PAD(0x16);//0x12
		float maximum_vitality;//0x28
		float minimum_stun_damage;//0x2C
		float stun_time_seconds;//0x30
		float recharge_time_seconds;//0x34
		float recharge_fraction;//0x38
		PAD(0x40);//0x3C
		tag_reference shield_damaged_first_person_shader;//0x7C
		tag_reference shield_damaged_shader;//0x84
		float maximum_shield_vitality;//0x8C
		string_id global_shield_material_name;//0x90
		float minimum_stun_damage_1;//0x94
		float stun_time_seconds_1;//0x98
		float recharge_time_seconds_1;//0x9C
		float shield_damaged_threshold;//0xA0
		tag_reference shield_damaged_effect;//0xA4
		tag_reference shield_depleted_effect;//0xAC
		tag_reference shield_recharging_effect;//0xB4
		struct s_damage_sections_block
		{
			string_id name;//0x0
			enum class e_flags : __int32
			{
				absorbs_body_damage = FLAG(0),
				takes_full_dmg_when_object_dies = FLAG(1),
				cannot_die_with_riders = FLAG(2),
				takes_full_dmg_when_obj_dstryd = FLAG(3),
				restored_on_ressurection = FLAG(4),
				unused = FLAG(5),
				unused_1 = FLAG(6),
				heatshottable = FLAG(7),
				ignores_shields = FLAG(8),
			};
			e_flags flags;//0x4
			float vitality_percentage_01;//0x8
			struct s_instant_responses_block
			{
				enum class e_response_type : __int16
				{
					receives_all_damage = 0,
					receives_area_effect_damage = 1,
					receives_local_damage = 2,
				};
				e_response_type response_type;//0x0
				enum class e_constraint_damage_type : __int16
				{
					none = 0,
					destroy_one_of_group = 1,
					destroy_entire_group = 2,
					loosen_one_of_group = 3,
					loosen_entire_group = 4,
				};
				e_constraint_damage_type constraint_damage_type;//0x2
				enum class e_flags : __int32
				{
					kills_object = FLAG(0),
					inhibits_melee_attack = FLAG(1),
					inhibits_weapon_attack = FLAG(2),
					inhibits_walking = FLAG(3),
					forces_drop_weapon = FLAG(4),
					kills_weapon_primary_trigger = FLAG(5),
					kills_weapon_secondary_trigger = FLAG(6),
					destroys_object = FLAG(7),
					damages_weapon_primary_trigger = FLAG(8),
					damages_weapon_secondary_trigger = FLAG(9),
					light_damage_left_turn = FLAG(10),
					major_damage_left_turn = FLAG(11),
					light_damage_right_turn = FLAG(12),
					major_damage_right_turn = FLAG(13),
					light_damage_engine = FLAG(14),
					major_damage_engine = FLAG(15),
					kills_object_no_player_solo = FLAG(16),
					causes_detonation = FLAG(17),
					destroy_all_group_constraints = FLAG(18),
					kills_variant_objects = FLAG(19),
					force_unattached_effects = FLAG(20),
					fires_under_threshold = FLAG(21),
					triggers_special_death = FLAG(22),
					only_on_special_death = FLAG(23),
					only_not_on_special_death = FLAG(24),
				};
				e_flags flags;//0x4
				float damage_threshold;//0x8
				tag_reference transition_effect;//0xC
				tag_reference transition_damage_effect;//0x14
				string_id region;//0x1C
				enum class e_new_state : __int16
				{
					default = 0,
					minor_damage = 1,
					medium_damage = 2,
					major_damage = 3,
					destroyed = 4,
				};
				e_new_state new_state;//0x20
				__int16 runtime_region_index;//0x22
				string_id effect_marker_name;//0x24
				string_id damage_effect_marker_name;//0x28
				float response_delay;//0x2C
				tag_reference delay_effect;//0x30
				string_id delay_effect_marker_name;//0x38
				string_id constraintgroup_name;//0x3C
				string_id ejecting_seat_label;//0x40
				float skip_fraction;//0x44
				string_id destroyed_child_object_marker_name;//0x48
				float total_damage_threshold;//0x4C
			};
			TAG_BLOCK_SIZE_ASSERT(s_instant_responses_block, 0x50);
			tag_block<s_instant_responses_block> instant_responses;//0xC
			PAD(0x10);//0x14
			float stun_time_seconds;//0x24
			float recharge_time_seconds;//0x28
			PAD(0x4);//0x2C
			string_id resurrection_restored_region_name;//0x30
			PAD(0x4);//0x34
		};
		TAG_BLOCK_SIZE_ASSERT(s_damage_sections_block, 0x38);
		tag_block<s_damage_sections_block> damage_sections;//0xBC
		struct s_nodes_block
		{
			PAD(0x10);//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_nodes_block, 0x10);
		tag_block<s_nodes_block> nodes;//0xC4
		PAD(0xC);//0xCC
		struct s_damage_seats_block
		{
			string_id seat_label;//0x0
			float direct_damage_scale;//0x4
			float damage_transfer_falloff_radius;//0x8
			float maximum_transfer_damage_scale;//0xC
			float minimum_transfer_damage_scale;//0x10
		};
		TAG_BLOCK_SIZE_ASSERT(s_damage_seats_block, 0x14);
		tag_block<s_damage_seats_block> damage_seats;//0xD8
		struct s_damage_constraints_block
		{
			string_id physics_model_constraint_name;//0x0
			string_id damage_constraint_name;//0x4
			string_id damage_constraint_group_name;//0x8
			float group_probability_scale;//0xC
			PAD(0x4);//0x10
		};
		TAG_BLOCK_SIZE_ASSERT(s_damage_constraints_block, 0x14);
		tag_block<s_damage_constraints_block> damage_constraints;//0xE0
		tag_reference overshield_first_person_shader;//0xE8
		tag_reference overshield_shader;//0xF0
	};
	TAG_BLOCK_SIZE_ASSERT(s_new_damage_info_block, 0xF8);
	tag_block<s_new_damage_info_block> new_damage_info;//0x60
	struct s_targets_block
	{
		string_id marker_name;//0x0
		float size;//0x4
		angle cone_angle;//0x8
		__int16 damage_section;//0xC
		__int16 variant;//0xE
		float targeting_relevance;//0x10
		enum class e_flags : __int32
		{
			locked_by_human_tracking = FLAG(0),
			locked_by_plasma_tracking = FLAG(1),
			headshot = FLAG(2),
			vulnerable = FLAG(3),
			alwas_locked_by_plasma_tracking = FLAG(4),
		};
		e_flags flags;//0x14
		float lock_on_distance;//0x18
	};
	TAG_BLOCK_SIZE_ASSERT(s_targets_block, 0x1C);
	tag_block<s_targets_block> targets;//0x68
	struct s_NUM__block
	{
		string_id name;//0x0
		__int8 collision_region_index;//0x4
		__int8 physics_region_index;//0x5
		PAD(0x2);//0x6
		struct s_permutations_block
		{
			string_id name;//0x0
			enum class e_flags : __int8
			{
				cannot_be_chosen_randomly = FLAG(0),
			};
			e_flags flags;//0x4
			__int8 collision_permutation_index;//0x5
			PAD(0x2);//0x6
		};
		TAG_BLOCK_SIZE_ASSERT(s_permutations_block, 0x8);
		tag_block<s_permutations_block> permutations;//0x8
	};
	TAG_BLOCK_SIZE_ASSERT(s_NUM__block, 0x10);
	tag_block<s_NUM__block> NUM_;//0x70
	struct s_NUM__block_1
	{
		string_id name;//0x0
		__int16 parent_node;//0x4
		__int16 first_child_node;//0x6
		__int16 next_sibling_node;//0x8
		PAD(0x2);//0xA
		float default_translation_x;//0xC
		float default_translation_y;//0x10
		float default_translation_z;//0x14
		PAD(0xC);//0x18
		float default_rotation_w;//0x24
		float default_inverse_scale;//0x28
		PAD(0x24);//0x2C
		float default_inverse_position_x;//0x50
		float default_inverse_position_y;//0x54
		float default_inverse_position_z;//0x58
	};
	TAG_BLOCK_SIZE_ASSERT(s_NUM__block_1, 0x5C);
	tag_block<s_NUM__block_1> NUM_1;//0x78
	PAD(0x4);//0x80
	struct s_model_object_data_block
	{
		enum class e_type : __int16
		{
			not_set = 0,
			user_defined = 1,
			auto_generated = 2,
		};
		e_type type;//0x0
		PAD(0x2);//0x2
		float offset_x;//0x4
		float offset_y;//0x8
		float offset_z;//0xC
		float radius;//0x10
	};
	TAG_BLOCK_SIZE_ASSERT(s_model_object_data_block, 0x14);
	tag_block<s_model_object_data_block> model_object_data;//0x84
	tag_reference default_dialogue;//0x8C
	tag_reference unused;//0x94
	enum class e_flags : __int32
	{
		active_camo_always_on = FLAG(0),
		active_camo_always_merge = FLAG(1),
		active_camo_never_merge = FLAG(2),
	};
	e_flags flags;//0x9C
	string_id default_dialogue_effect;//0xA0
	__int8 NUM_0;//0xA4
	__int8 NUM_64;//0xA5
	__int8 NUM_2;//0xA6
	__int8 NUM_3;//0xA7
	__int8 NUM_4;//0xA8
	__int8 NUM_5;//0xA9
	__int8 NUM_6;//0xAA
	__int8 NUM_7;//0xAB
	__int8 NUM_8;//0xAC
	__int8 NUM_9;//0xAD
	__int8 NUM_10;//0xAE
	__int8 NUM_11;//0xAF
	__int8 NUM_12;//0xB0
	__int8 NUM_13;//0xB1
	__int8 NUM_14;//0xB2
	__int8 NUM_15;//0xB3
	__int8 NUM_16;//0xB4
	__int8 NUM_17;//0xB5
	__int8 NUM_18;//0xB6
	__int8 NUM_19;//0xB7
	__int8 NUM_20;//0xB8
	__int8 NUM_21;//0xB9
	__int8 NUM_22;//0xBA
	__int8 NUM_23;//0xBB
	__int8 NUM_24;//0xBC
	__int8 NUM_25;//0xBD
	__int8 NUM_26;//0xBE
	__int8 NUM_27;//0xBF
	__int8 NUM_28;//0xC0
	__int8 NUM_29;//0xC1
	__int8 NUM_30;//0xC2
	__int8 NUM_31;//0xC3
	__int8 NUM_32;//0xC4
	__int8 NUM_33;//0xC5
	__int8 NUM_34;//0xC6
	__int8 NUM_35;//0xC7
	__int8 NUM_36;//0xC8
	__int8 NUM_37;//0xC9
	__int8 NUM_38;//0xCA
	__int8 NUM_39;//0xCB
	__int8 NUM_40;//0xCC
	__int8 NUM_41;//0xCD
	__int8 NUM_42;//0xCE
	__int8 NUM_43;//0xCF
	__int8 NUM_44;//0xD0
	__int8 NUM_45;//0xD1
	__int8 NUM_46;//0xD2
	__int8 NUM_47;//0xD3
	__int8 NUM_48;//0xD4
	__int8 NUM_49;//0xD5
	__int8 NUM_50;//0xD6
	__int8 NUM_51;//0xD7
	__int8 NUM_52;//0xD8
	__int8 NUM_53;//0xD9
	__int8 NUM_54;//0xDA
	__int8 NUM_55;//0xDB
	__int8 NUM_56;//0xDC
	__int8 NUM_57;//0xDD
	__int8 NUM_58;//0xDE
	__int8 NUM_59;//0xDF
	__int8 NUM_60;//0xE0
	__int8 NUM_61;//0xE1
	__int8 NUM_62;//0xE2
	__int8 NUM_63;//0xE3
	enum class e_runtime_flags : __int32
	{
		contains_runtime_nodes = FLAG(0),
	};
	e_runtime_flags runtime_flags;//0xE4
	struct s_scenario_load_parameters_block
	{
		tag_reference scenario;//0x0
		data_block parameters;//0x8
		PAD(0x20);//0x10
	};
	TAG_BLOCK_SIZE_ASSERT(s_scenario_load_parameters_block, 0x30);
	tag_block<s_scenario_load_parameters_block> scenario_load_parameters;//0xE8
	tag_reference hologram_shader;//0xF0
	string_id hologram_control_function;//0xF8
};
TAG_GROUP_SIZE_ASSERT(s_model_group_definition, 0xFC);
