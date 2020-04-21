#pragma once
#pragma pack(push,1)
#include "object_definition.hpp"
/*********************************************************************
* name: unit
* group_tag : unit
* header size : 492
* *********************************************************************/
struct s_unit_group_definition :TagGroup<'unit'>
{
	s_object_group_definition objectTag;
	enum class e_unit_flags : __int32
	{
		circular_aiming = FLAG(0),
		destroyed_after_dying = FLAG(1),
		halfspeed_interpolation = FLAG(2),
		fires_from_camera = FLAG(3),
		entrance_inside_bounding_sphere = FLAG(4),
		doesnt_show_readied_weapon = FLAG(5),
		causes_passenger_dialogue = FLAG(6),
		resists_pings = FLAG(7),
		melee_attack_is_fatal = FLAG(8),
		dont_reface_during_pings = FLAG(9),
		has_no_aiming = FLAG(10),
		simple_creature = FLAG(11),
		impact_melee_attaches_to_unit = FLAG(12),
		impact_melee_dies_on_shields = FLAG(13),
		cannot_open_doors_automatically = FLAG(14),
		melee_attackers_cannot_attach = FLAG(15),
		not_instantly_killed_by_melee = FLAG(16),
		shield_sapping = FLAG(17),
		runs_around_flaming = FLAG(18),
		inconsequential = FLAG(19),
		special_cinematic_unit = FLAG(20),
		ignored_by_autoaiming = FLAG(21),
		shields_fry_infection_forms = FLAG(22),
		unused = FLAG(23),
		unused1 = FLAG(24),
		acts_as_gunner_for_parent = FLAG(25),
		controlled_by_parent_gunner = FLAG(26),
		parents_primary_weapon = FLAG(27),
		unit_has_boost = FLAG(28),
	};
	e_unit_flags unit_flags;//0xBC
	enum class e_default_team : __int16
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
	e_default_team default_team;//0xC0
	enum class e_constant_sound_volume : __int16
	{
		silent = 0,
		medium = 1,
		loud = 2,
		shout = 3,
		quiet = 4,
	};
	e_constant_sound_volume constant_sound_volume;//0xC2
	tag_reference integrated_light_toggle;//0xC4
	angle camera_field_of_view;//0xCC degrees
	float camera_stiffness;//0xD0
	string_id camera_marker_name_old_string_id;//0xD4
	string_id camera_submerged_marker_name_old_string_id;//0xD8
	angle pitch_autolevel;//0xDC
	angle pitch_range_lower;//0xE0
	angle pitch_range_upper;//0xE4
	struct s_camera_tracks_block
	{
		tag_reference track;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_camera_tracks_block, 0x8);
	tag_block<s_camera_tracks_block> camera_tracks;//0xE8
	PAD(0xC);//0xF0
	float accel_action_scale;//0xFC
	float accel_attach_scale;//0x100
	float soft_ping_threshold;//0x104
	float soft_ping_interrupt_time;//0x108
	float hard_ping_threshold;//0x10C
	float hard_ping_interrupt_time;//0x110
	float hard_death_threshold;//0x114
	float feign_death_threshold;//0x118
	float feign_death_time;//0x11C
	float distance_of_evade_anim;//0x120
	float distance_of_dive_anim;//0x124
	float stunned_movement_threshold;//0x128
	float feign_death_chance;//0x12C
	float feign_repeat_chance;//0x130
	tag_reference spawned_turret_character;//0x134
	__int16 spawned_actor_count_lower;//0x13C
	__int16 spawned_actor_count_upper;//0x13E
	float spawned_velocity;//0x140
	angle aiming_velocity_maximum;//0x144
	angle aiming_acceleration_maximum;//0x148
	float casual_aiming_modifier;//0x14C
	angle looking_velocity_maximum;//0x150
	angle looking_acceleration_maximum;//0x154
	string_id right_hand_node;//0x158
	string_id left_hand_node;//0x15C
	string_id preferred_gun_noder;//0x160
	tag_reference melee_damage;//0x164
	tag_reference boarding_melee_damage;//0x16C
	tag_reference boarding_melee_response;//0x174
	tag_reference landing_melee_damage;//0x17C
	tag_reference flurry_melee_damage;//0x184
	tag_reference obstacle_smash_damage;//0x18C
	enum class e_motion_sensor_blip_size : __int16
	{
		medium = 0,
		small = 1,
		large = 2,
	};
	e_motion_sensor_blip_size motion_sensor_blip_size;//0x194
	PAD(0x2);//0x196
	struct s_postures_block
	{
		string_id name;//0x0
		PAD(0xC);//0x4
	};
	TAG_BLOCK_SIZE_ASSERT(s_postures_block, 0x10);
	tag_block<s_postures_block> postures;//0x198
	struct s_new_hud_interfaces_block
	{
		tag_reference new_unit_hud_interface;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_new_hud_interfaces_block, 0x8);
	tag_block<s_new_hud_interfaces_block> new_hud_interfaces;//0x1A0
	struct s_dialogue_variants_block
	{
		__int16 variant_number;//0x0
		PAD(0x2);//0x2
		tag_reference dialogue;//0x4
	};
	TAG_BLOCK_SIZE_ASSERT(s_dialogue_variants_block, 0xC);
	tag_block<s_dialogue_variants_block> dialogue_variants;//0x1A8
	float grenade_velocity;//0x1B0
	enum class e_grenade_type : __int16
	{
		human_fragmentation = 0,
		covenant_plasma = 1,
	};
	e_grenade_type grenade_type;//0x1B4
	__int16 grenade_count;//0x1B6
	struct s_powered_seats_block
	{
		float driver_powerup_time;//0x0
		float driver_powerdown_time;//0x4
	};
	TAG_BLOCK_SIZE_ASSERT(s_powered_seats_block, 0x8);
	tag_block<s_powered_seats_block> powered_seats;//0x1B8
	struct s_weapons_block
	{
		tag_reference weapon;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_weapons_block, 0x8);
	tag_block<s_weapons_block> weapons;//0x1C0
	struct s_seats_block
	{
		enum class e_flags : __int32
		{
			invisible = FLAG(0),
			locked = FLAG(1),
			driver = FLAG(2),
			gunner = FLAG(3),
			third_person_camera = FLAG(4),
			allows_weapons = FLAG(5),
			third_person_on_enter = FLAG(6),
			first_person_camera_slaved_to_gun = FLAG(7),
			allow_vehicle_communication_animations = FLAG(8),
			not_valid_without_driver = FLAG(9),
			allow_ai_noncombatants = FLAG(10),
			boarding_seat = FLAG(11),
			ai_firing_disabled_by_max_acceleration = FLAG(12),
			boarding_enters_seat = FLAG(13),
			boarding_need_any_passenger = FLAG(14),
			controls_open_and_close = FLAG(15),
			invalid_for_player = FLAG(16),
			invalid_for_nonplayer = FLAG(17),
			gunner_player_only = FLAG(18),
			invisible_under_major_damage = FLAG(19),
		};
		e_flags flags;//0x0
		string_id label_old_string_id;//0x4
		string_id marker_name_old_string_id;//0x8
		string_id entry_markers_name;//0xC
		string_id boarding_grenade_marker;//0x10
		string_id boarding_grenade_string;//0x14
		string_id boarding_melee_string;//0x18
		float ping_scale;//0x1C
		float turnover_time;//0x20
		PAD(0xC);//0x24
		float accel_action_scale;//0x30
		float accel_attach_scale;//0x34
		float ai_scariness;//0x38
		enum class e_ai_seat_type : __int16
		{
			none = 0,
			passenger = 1,
			gunner = 2,
			small_cargo = 3,
			large_cargo = 4,
			driver = 5,
		};
		e_ai_seat_type ai_seat_type;//0x3C
		__int16 boarding_seat;//0x3E
		float listener_interpolation_factor;//0x40
		float yaw_rate_bounds_lower;//0x44
		float yaw_rate_bounds_upper;//0x48
		float pitch_rate_bounds_lower;//0x4C
		float pitch_rate_bounds_upper;//0x50
		float min_speed_reference;//0x54
		float max_speed_reference;//0x58
		float speed_exponent;//0x5C
		string_id camera_marker_name_old_string_id;//0x60
		string_id camera_submerged_marker_name_old_string_id;//0x64
		angle pitch_autolevel;//0x68
		angle pitch_range_lower;//0x6C
		angle pitch_range_upper;//0x70
		struct s_camera_tracks_block
		{
			tag_reference track;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_camera_tracks_block, 0x8);
		tag_block<s_camera_tracks_block> camera_tracks;//0x74
		struct s_unit_hud_interface_block
		{
			tag_reference new_unit_hud_interface;//0x0
		};
		TAG_BLOCK_SIZE_ASSERT(s_unit_hud_interface_block, 0x8);
		tag_block<s_unit_hud_interface_block> unit_hud_interface;//0x7C
		string_id enter_seat_string;//0x84
		angle yaw_minimum;//0x88
		angle yaw_maximum;//0x8C
		tag_reference builtin_gunner;//0x90
		float entry_radius;//0x98
		angle entry_marker_cone_angle;//0x9C
		angle entry_marker_facing_angle;//0xA0
		float maximum_relative_velocity;//0xA4
		string_id invisible_seat_region;//0xA8
		__int32 runtime_invisible_seat_region_index;//0xAC
	};
	TAG_BLOCK_SIZE_ASSERT(s_seats_block, 0xB0);
	tag_block<s_seats_block> seats;//0x1C8
	float boost_peak_power;//0x1D0
	float boost_rise_power;//0x1D4
	float boost_peak_time;//0x1D8
	float boost_fall_power;//0x1DC
	float dead_time;//0x1E0
	float attack_weight;//0x1E4
	float decay_weight;//0x1E8
};
TAG_GROUP_SIZE_ASSERT(s_unit_group_definition, 0x1EC);
#pragma pack(pop)

