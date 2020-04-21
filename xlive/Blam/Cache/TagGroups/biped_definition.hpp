#pragma once

#pragma pack(push,1)
#include "unit_definition.hpp"
/*********************************************************************
* name: biped
* group_tag : bipd
* header size : 788
* *********************************************************************/
struct s_biped_group_definition :TagGroup<'bipd'>
{
	s_unit_group_definition unitTag;
	angle moving_turning_speed;//0x1EC
	enum class e_biped_flags : __int32
	{
		turns_without_animating = FLAG(0),
		passes_through_other_bipeds = FLAG(1),
		immune_to_falling_damage = FLAG(2),
		rotate_while_airborne = FLAG(3),
		uses_limp_body_physics = FLAG(4),
		unused = FLAG(5),
		random_speed_increase = FLAG(6),
		unused1 = FLAG(7),
		spawn_death_children_on_destroy = FLAG(8),
		stunned_by_emp_damage = FLAG(9),
		dead_physics_when_stunned = FLAG(10),
		always_ragdoll_when_dead = FLAG(11),
	};
	e_biped_flags biped_flags;//0x1F0
	angle stationary_turning_threshold;//0x1F4
	float jump_velocity;//0x1F8
	float maximum_soft_landing_time;//0x1FC
	float maximum_hard_landing_time;//0x200
	float minimum_soft_landing_velocity;//0x204
	float minimum_hard_landing_velocity;//0x208
	float maximum_hard_landing_velocity;//0x20C
	float death_hard_landing_velocity;//0x210
	float stun_duration;//0x214
	float standing_camera_height;//0x218
	float crouching_camera_height;//0x21C
	float crouch_transition_time;//0x220
	angle camera_interpolation_start;//0x224
	angle camera_interpolation_end;//0x228
	float camera_forward_movement_scale;//0x22C
	float camera_side_movement_scale;//0x230
	float camera_vertical_movement_scale;//0x234
	float camera_exclusion_distance;//0x238
	float autoaim_width;//0x23C
	enum class e_lock_on_flags : __int32
	{
		locked_by_human_targeting = FLAG(0),
		locked_by_plasma_targeting = FLAG(1),
		always_locked_by_plasma_targeting = FLAG(2),
	};
	e_lock_on_flags lock_on_flags;//0x240
	float lock_on_distance;//0x244
	PAD(0x10);//0x248
	float head_shot_acc_scale;//0x258
	tag_reference area_damage_effect;//0x25C
	enum class e_flags : __int32
	{
		centered_at_origin = FLAG(0),
		shape_spherical = FLAG(1),
		use_player_physics = FLAG(2),
		climb_any_surface = FLAG(3),
		flying = FLAG(4),
		not_physical = FLAG(5),
		dead_character_collision_group = FLAG(6),
	};
	e_flags flags;//0x264
	float height_standing;//0x268
	float height_crouching;//0x26C
	float radius;//0x270
	float mass;//0x274
	string_id living_material_name;//0x278
	string_id dead_material_name;//0x27C
	PAD(0x4);//0x280
	struct s_dead_sphere_shapes_block
	{
		string_id name;//0x0
		__int16 material;//0x4
		enum class e_flags : __int16
		{
			unused = FLAG(0),
		};
		e_flags flags;//0x6
		float relative_mass_scale;//0x8
		float friction;//0xC
		float restitution;//0x10
		float volume;//0x14
		float mass;//0x18
		PAD(0x2);//0x1C
		__int16 phantom;//0x1E
		PAD(0x4);//0x20
		__int16 size0;//0x24
		__int16 count0;//0x26
		PAD(0x4);//0x28
		float radius;//0x2C
		PAD(0x4);//0x30
		__int16 size1;//0x34
		__int16 count1;//0x36
		PAD(0x48);//0x38
	};
	TAG_BLOCK_SIZE_ASSERT(s_dead_sphere_shapes_block, 0x80);
	tag_block<s_dead_sphere_shapes_block> dead_sphere_shapes;//0x284
	struct s_pill_shapes_block
	{
		string_id name;//0x0
		__int16 material;//0x4
		enum class e_flags : __int16
		{
			unused = FLAG(0),
		};
		e_flags flags;//0x6
		float relative_mass_scale;//0x8
		float friction;//0xC
		float restitution;//0x10
		float volume;//0x14
		float mass;//0x18
		PAD(0x2);//0x1C
		__int16 phantom;//0x1E
		PAD(0x4);//0x20
		__int16 size;//0x24
		__int16 count;//0x26
		PAD(0x4);//0x28
		float radius;//0x2C
		PAD(0x20);//0x30
	};
	TAG_BLOCK_SIZE_ASSERT(s_pill_shapes_block, 0x50);
	tag_block<s_pill_shapes_block> pill_shapes;//0x28C
	struct s_sphere_shapes_block
	{
		string_id name;//0x0
		__int16 material;//0x4
		enum class e_flags : __int16
		{
			unused = FLAG(0),
		};
		e_flags flags;//0x6
		float relative_mass_scale;//0x8
		float friction;//0xC
		float restitution;//0x10
		float volume;//0x14
		float mass;//0x18
		PAD(0x2);//0x1C
		__int16 phantom;//0x1E
		PAD(0x4);//0x20
		__int16 size0;//0x24
		__int16 count0;//0x26
		PAD(0x4);//0x28
		float radius;//0x2C
		PAD(0x4);//0x30
		__int16 size1;//0x34
		__int16 count1;//0x36
		PAD(0x48);//0x38
	};
	TAG_BLOCK_SIZE_ASSERT(s_sphere_shapes_block, 0x80);
	tag_block<s_sphere_shapes_block> sphere_shapes;//0x294
	angle maximum_slope_angle;//0x29C
	angle downhill_falloff_angle;//0x2A0
	angle downhill_cutoff_angl;//0x2A4
	angle uphill_falloff_angle;//0x2A8
	angle uphill_cutoff_angle;//0x2AC
	float downhill_velocity_scale;//0x2B0
	float uphill_velocity_scale;//0x2B4
	PAD(0x14);//0x2B8
	angle bank_angle;//0x2CC
	float bank_apply_time;//0x2D0
	float bank_decay_time;//0x2D4
	float pitch_ratio;//0x2D8
	float max_velocity;//0x2DC
	float max_sidestep_velocity;//0x2E0
	float acceleration;//0x2E4
	float deceleration;//0x2E8
	angle angular_velocity_maximum;//0x2EC
	angle angular_acceleration_maximum;//0x2F0
	float crouch_velocity_modifier;//0x2F4
	struct s_contact_points_block
	{
	string_id marker_name_old_string_id;//0x0
	};
	TAG_BLOCK_SIZE_ASSERT(s_contact_points_block, 0x4);
	tag_block<s_contact_points_block> contact_points;//0x2F8
	tag_reference reanimation_character;//0x300
	tag_reference death_spawn_character;//0x308
	__int16 death_spawn_count;//0x310
	PAD(0x2);//0x312
};
TAG_GROUP_SIZE_ASSERT(s_biped_group_definition, 0x314);
#pragma pack(pop)

