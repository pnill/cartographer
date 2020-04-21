#pragma once
#ifndef CREA_H
#define CREA_H
#pragma pack(push,1)
#include "object_definition.hpp"
/*********************************************************************
* name: creature
* group_tag : crea
* header size : 384
* *********************************************************************/
struct s_creature_group_definition :TagGroup<'crea'>
{
	s_object_group_definition objectTag;
	enum class e_flags : __int32
	{
		unused = FLAG(0),
		infection_form = FLAG(1),
		immune_to_falling_damage = FLAG(2),
		rotate_while_airborne = FLAG(3),
		zapped_by_shields = FLAG(4),
		attach_upon_impact = FLAG(5),
		not_on_motion_sensor = FLAG(6),
	};
	e_flags flags;//0xBC
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
	enum class e_motion_sensor_blip_size : __int16
	{
		medium = 0,
		small = 1,
		large = 2,
	};
	e_motion_sensor_blip_size motion_sensor_blip_size;//0xC2
	angle turning_velocity_maximum;//0xC4
	angle turning_acceleration_maximum;//0xC8
	float casual_turning_modifier;//0xCC
	float autoaim_width;//0xD0
	enum class e_flags2 : __int32
	{
		centered_at_origin = FLAG(0),
		shape_spherical = FLAG(1),
		use_player_physics = FLAG(2),
		climb_any_surface = FLAG(3),
		flying = FLAG(4),
		not_physical = FLAG(5),
		dead_character_collision_group = FLAG(6),
	};
	e_flags2 flags2;//0xD4
	float height_standing;//0xD8
	float height_crouching;//0xDC
	float radius;//0xE0
	float mass;//0xE4
	string_id living_material_name;//0xE8
	string_id dead_material_name;//0xEC
	PAD(0x4);//0xF0
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
	tag_block<s_dead_sphere_shapes_block> dead_sphere_shapes;//0xF4
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
	tag_block<s_pill_shapes_block> pill_shapes;//0xFC
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
	tag_block<s_sphere_shapes_block> sphere_shapes;//0x104
	struct {
		angle maximum_slope_angle;//0x10C
		angle downhill_falloff_angle;//0x110
		angle downhill_cutoff_angle;//0x114
		angle uphill_falloff_angle;//0x118
		angle uphill_cutoff_angle;//0x11C
		float downhill_velocity_scale;//0x120
		float uphill_velocity_scale;//0x124
	}ground;

	PAD(0x14);//0x128

	struct
	{
		angle bank_angle;//0x13C
		float bank_apply_time;//0x140
		float bank_decay_time;//0x144
		float pitch_ratio;//0x148
		float max_velocity;//0x14C
		float max_sidestep_velocity;//0x150
		float acceleration;//0x154
		float deceleration;//0x158
		angle angular_velocity_maximum;//0x15C
		angle angular_acceleration_maximum;//0x160
		float crouch_velocity_modifier;//0x164

	}flying;

	struct {
		tag_reference impact_damage;//0x168
		tag_reference impact_shield_damage;//0x170
	}sentinel;

	struct {
		real_bounds destroy_after_death_time;//0x178	
	}death_and_destruction;

};
TAG_GROUP_SIZE_ASSERT(s_creature_group_definition, 0x180);

#pragma pack(pop)
#endif
