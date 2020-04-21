#pragma once
#ifndef VEHI_H
#define VEHI_H
#pragma pack(push,1)
#include "unit_definition.hpp"
/*********************************************************************
* name: vehicle
* group_tag : vehi
* header size : 768
* *********************************************************************/
struct s_vehicle_group_definition :TagGroup<'vehi'>
{
	s_unit_group_definition unitTag;
	enum class e_flags : __int32
	{
		speed_wakes_physics = FLAG(0),
		turn_wakes_physics = FLAG(1),
		driver_power_wakes_physics = FLAG(2),
		gunner_power_wakes_physics = FLAG(3),
		control_opposite_speed_sets_brake = FLAG(4),
		slide_wakes_physics = FLAG(5),
		kills_riders_at_terminal_velocity = FLAG(6),
		causes_collision_damage = FLAG(7),
		ai_weapon_cannot_rotate = FLAG(8),
		ai_does_not_require_driver = FLAG(9),
		ai_unused = FLAG(10),
		ai_driver_enable = FLAG(11),
		ai_driver_flying = FLAG(12),
		ai_driver_cansidestep = FLAG(13),
		ai_driver_hovering = FLAG(14),
		vehicle_steers_directly = FLAG(15),
		unused = FLAG(16),
		has_ebrake = FLAG(17),
		noncombat_vehicle = FLAG(18),
		no_friction_wdriver = FLAG(19),
		can_trigger_automatic_opening_doors = FLAG(20),
		autoaim_when_teamless = FLAG(21),
	};
	e_flags flags;//0x1EC
	enum class e_type : __int16
	{
		human_tank = 0,
		human_jeep = 1,
		human_boat = 2,
		human_plane = 3,
		alien_scout = 4,
		alien_fighter = 5,
		turret = 6,
	};
	e_type type;//0x1F0
	enum class e_control : __int16
	{
		vehicle_control_normal = 0,
		vehicle_control_unused = 1,
		vehicle_control_tank = 2,
	};
	e_control control;//0x1F2
	float maximum_forward_speed;//0x1F4
	float maximum_reverse_speed;//0x1F8
	float speed_acceleration;//0x1FC
	float speed_deceleration;//0x200
	float maximum_left_turn;//0x204
	float maximum_right_turn_negative;//0x208
	float wheel_circumference;//0x20C
	float turn_rate;//0x210
	float blur_speed;//0x214
	enum class e_specific_type : __int16
	{
		none = 0,
		ghost = 1,
		wraith = 2,
		spectre = 3,
		sentinel_enforcer = 4,
	};
	e_specific_type specific_type;//0x218
	enum class e_player_training_vehicle_type : __int16
	{
		none = 0,
		warthog = 1,
		warthog_turret = 2,
		ghost = 3,
		banshee = 4,
		tank = 5,
		wraith = 6,
	};
	e_player_training_vehicle_type player_training_vehicle_type;//0x21A
	string_id flip_message;//0x21C
	float turn_scale;//0x220
	float speed_turn_penalty_power;//0x224
	float speed_turn_penalty;//0x228
	float maximum_left_slide;//0x22C
	float maximum_right_slide;//0x230
	float slide_acceleration;//0x234
	float slide_deceleration;//0x238
	float minimum_flipping_angular_velocity;//0x23C
	float maximum_flipping_angular_velocity;//0x240
	enum class e_vehicle_size : __int16
	{
		small = 0,
		large = 1,
	};
	e_vehicle_size vehicle_size;//0x244
	PAD(0x2);//0x246
	float fixed_gun_yaw;//0x248
	float fixed_gun_pitch;//0x24C
	float overdampen_cusp_angle;//0x250
	float overdampen_exponent;//0x254
	float crouch_transition_time;//0x258
	PAD(0x4);//0x25C
	float engine_moment;//0x260
	float engine_max_angular_velocity;//0x264
	struct s_gears_block
	{
		struct
		{
			float min_torque;//0x0
			float max_torque;//0x4
			float peak_torque_scale;//0x8
			float past_peak_torque_exponent;//0xC
			float torque_at_max_angular_velocity;//0x10
			float torque_at_2x_max_angular_velocity;//0x14

		}loaded_torque;

		struct
		{
			float min_torque;//0x18
			float max_torque;//0x1C
			float peak_torque_scale;//0x20
			float past_peak_torque_exponent;//0x24
			float torque_at_max_angular_velocity;//0x28
			float torque_at_2x_max_angular_velocity;//0x2C
		}cruising_torque;

		struct
		{
			float min_time_to_upshift;//0x30
			float engine_upshift_scale;//0x34
			float gear_ratio;//0x38
			float min_time_to_downshift;//0x3C
			float engine_downshift_scale;//0x40
		}gearing;

	};
	TAG_BLOCK_SIZE_ASSERT(s_gears_block, 0x44);
	tag_block<s_gears_block> gears;//0x268

	float flying_torque_scale;//0x270
	float seat_enterance_acceleration_scale;//0x274
	float seat_exit_accelersation_scale;//0x278
	float air_friction_deceleration;//0x27C
	float thrust_scale;//0x280

	tag_reference suspension_sound;//0x284
	tag_reference crash_sound;//0x28C
	tag_reference unused;//0x294
	tag_reference special_effect;//0x29C
	tag_reference unused_effect;//0x2A4

	struct
	{
		enum class e_flags : __int32
		{
			invalid = FLAG(0),
		};
		e_flags flags;//0x2AC
		float ground_friction;//0x2B0
		float ground_depth;//0x2B4
		float ground_damp_factor;//0x2B8
		float ground_moving_friction;//0x2BC
		float ground_maximum_slope_0;//0x2C0
		float ground_maximum_slope_1;//0x2C4
		PAD(0x10);//0x2C8
		float anti_gravity_bank_lift;//0x2D8
		float steering_bank_reaction_scale;//0x2DC
		float gravity_scale;//0x2E0
		float radius;//0x2E4
	}physics;


	struct s_anti_gravity_points_block
	{
		string_id marker_name;//0x0
		enum class e_flags : __int32
		{
			gets_damage_from_region = FLAG(0),
		};
		e_flags flags;//0x4
		float antigrav_strength;//0x8
		float antigrav_offset;//0xC
		float antigrav_height;//0x10
		float antigrav_damp_factor;//0x14
		float antigrav_normal_k1;//0x18
		float antigrav_normal_k0;//0x1C
		float radius;//0x20
		PAD(0x10);//0x24
		string_id damage_source_region_name;//0x34
		float default_state_error;//0x38
		float minor_damage_error;//0x3C
		float medium_damage_error;//0x40
		float major_damage_error;//0x44
		float destroyed_state_error;//0x48
	};
	TAG_BLOCK_SIZE_ASSERT(s_anti_gravity_points_block, 0x4C);
	tag_block<s_anti_gravity_points_block> anti_gravity_points;//0x2E8

	struct s_friction_points_block
	{
		string_id marker_name;//0x0
		enum class e_flags : __int32
		{
			gets_damage_from_region = FLAG(0),
			powered = FLAG(1),
			front_turning = FLAG(2),
			rear_turning = FLAG(3),
			attached_to_ebrake = FLAG(4),
			can_be_destroyed = FLAG(5),
		};
		e_flags flags;//0x4
		float fraction_of_total_mass;//0x8
		float radius;//0xC
		float damaged_radius;//0x10
		enum class e_friction_type : __int16
		{
			point = 0,
			forward = 1,
		};
		e_friction_type friction_type;//0x14
		PAD(0x2);//0x16
		float moving_friction_velocity_diff;//0x18
		float ebrake_moving_friction;//0x1C
		float ebrake_friction;//0x20
		float ebrake_moving_friction_vel_diff;//0x24
		PAD(0x14);//0x28
		string_id collision_global_material_name;//0x3C
		PAD(0x2);//0x40
		enum class e_model_state_destroyed : __int16
		{
			default = 0,
			minor_damage = 1,
			medium_damage = 2,
			major_damage = 3,
			destroyed = 4,
		};
		e_model_state_destroyed model_state_destroyed;//0x42
		string_id region_name;//0x44
		PAD(0x4);//0x48
	};
	TAG_BLOCK_SIZE_ASSERT(s_friction_points_block, 0x4C);
	tag_block<s_friction_points_block> friction_points;//0x2F0
	struct s_shape_phantom_shape_block
	{
		PAD(0x4);//0x0
		__int16 size;//0x4
		__int16 count;//0x6
		PAD(0x8);//0x8
		__int32 child_shapes_size;//0x10
		__int32 child_shapes_capacity;//0x14
		enum class e_shape_type : __int16
		{
			sphere = 0,
			pill = 1,
			box = 2,
			triangle = 3,
			polyhedron = 4,
			multi_sphere = 5,
			unused_0 = 6,
			unused_1 = 7,
			unused_2 = 8,
			unused_3 = 9,
			unused_4 = 10,
			unused_5 = 11,
			unused_6 = 12,
			unused_7 = 13,
			list = 14,
			mopp = 15,
		};
		e_shape_type shape_type0;//0x18
		__int16 shape0;//0x1A
		__int32 collision_filter0;//0x1C

		e_shape_type shape_type1;//0x20
		__int16 shape1;//0x22
		__int32 collision_filter1;//0x24

		e_shape_type shape_type2;//0x28
		__int16 shape2;//0x2A
		__int32 collision_filter2;//0x2C

		e_shape_type shape_type3;//0x30
		__int16 shape3;//0x32

		__int32 collision_filter3;//0x34
		__int32 multisphere_count;//0x38

		enum class e_flags : __int32
		{
			has_aabb_phantom = FLAG(0),
			NUM_ = FLAG(1),
		};
		e_flags flags;//0x3C
		PAD(0x8);//0x40
		float x0;//0x48
		float x1;//0x4C
		float y0;//0x50
		float y1;//0x54
		float z0;//0x58
		float z1;//0x5C
		PAD(0x4);//0x60
		__int16 size0;//0x64
		__int16 count0;//0x66
		PAD(0x4);//0x68
		__int32 num_spheres0;//0x6C
		PAD(0x84);//0x70
		__int16 size1;//0xF4
		__int16 count1;//0xF6
		PAD(0x4);//0xF8
		__int32 num_spheres1;//0xFC
		PAD(0x84);//0x100
		__int16 size2;//0x184
		__int16 count2;//0x186
		PAD(0x4);//0x188
		__int32 num_spheres2;//0x18C
		PAD(0x84);//0x190
		__int16 size3;//0x214
		__int16 count3;//0x216
		PAD(0x4);//0x218
		__int32 num_spheres3;//0x21C
		PAD(0x80);//0x220
	};
	TAG_BLOCK_SIZE_ASSERT(s_shape_phantom_shape_block, 0x2A0);
	tag_block<s_shape_phantom_shape_block> shape_phantom_shape;//0x2F8
};
TAG_GROUP_SIZE_ASSERT(s_vehicle_group_definition, 0x300);

#pragma pack(pop)
#endif
