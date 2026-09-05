#pragma once
#include "units.h"
#include "physics/character_physics.h"

#define k_contact_point_count 3

enum e_biped_physics_mode : uint8
{
	_mode_ground = 1,
	_mode_flying = 2,
	_mode_dead = 3,
	_mode_posture = 4,
	_mode_climbing = 5,
	_mode_melee = 6,
	_mode_last = 7
};

enum e_biped_flags : uint16
{
	_biped_flags_bit_1,
	_biped_flags_bit_2,
	_biped_flags_bit_3,
	_biped_flags_bit_4,
	_biped_flags_bit_5,
	_biped_flags_bit_6,
	_biped_flags_bit_7,
	_biped_flags_bit_8,
	_biped_flags_bit_9,
	_biped_flags_bit_10,
	_biped_flags_bit_11,
	_biped_flags_bit_12,
	_biped_flags_bit_13,
	_biped_flags_bit_14,
	_biped_flags_bit_15,
	_biped_flag_is_falling_to_its_death
};

struct _biped_datum
{
	e_biped_flags flags;
	int8 movement_category;
	int8 ragdoll_flag;
	int16 pathfinding_structure_index;
	int32 pathfinding_surface_index;
	int32 pathfinding_sector;
	int32 pathfinding_instanced_geometry_index;
	int32 pathfinding_object_index;
	uint32 pathfinding_bsp_reference;
	int32 pathfinding_time;
	real_point3d pathfinding_point;
	real_vector3d pathfinding_ground_collision_normal;
	real_point3d pathfinding_ground_collision_point;
	uint32 last_falling_communication_time;
	uint32 melee_target_unit_index;
	uint32 bump_object_index;
	uint8 bump_ticks;
	uint8 jump_control_ticks;
	uint8 slip_control_ticks;
	uint8 last_known_speed_scale;
	uint8 airborne_ticks;
	uint8 stun_ticks;
	uint8 jump_delay_ticks;
	real32 lean;
	real32 lean_fraction;
	uint8 stationary_ticks;
	uint8 physics_movement_category;
	uint8 crouch_transition_ticks;
	uint8 falling_danger_ticks;
	real_vector3d max_analog_movement_controller;
	real_vector3d next_analog_movement_controller;
	int32 analog_movement_controller_age;
	bool first_person_camera_initialized;
	real_vector3d first_person_camera_offset;
	int32 field_90;
	c_character_physics_component physics;
	object_header_block_reference simulation_interpolation_storage;
};

struct biped_datum
{
	datum definition_index;
	_object_datum object;
	_unit_datum unit;
	_biped_datum biped;
};
ASSERT_STRUCT_SIZE(biped_datum, 0x480);

/* prototypes */

void bipeds_apply_patches(void);

void __cdecl biped_get_sight_position(
	datum biped_index,
	e_unit_estimate_mode estimate_mode,
	const real_point3d* estimated_body_position,
	const real_vector3d* desired_facing_vector,
	const real_vector3d* desired_gun_offset,
	real_point3d* object_origin);

/* macros */

#define biped_get(index) ((biped_datum*)object_get_and_verify_type(index, _object_mask_biped))
#define biped_try_and_get(index) ((biped_datum*)object_try_and_get_and_verify_type(index, _object_mask_biped))
