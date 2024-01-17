#pragma once
#include "units.h"
#include "Blam/Engine/tag_files/string_id.h"

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

enum e_biped_flags : uint32
{

};

struct biped_datum
{
	unit_datum unit;
	e_biped_flags biped_flags;
	int8 gap364[124];
	bool field_3E0;
	int8 pad[3];
	real_vector3d vector_3E4;
	int8 gap3F0[4];
	e_biped_physics_mode biped_movement_type;
	int8 gap1[139];
};
CHECK_STRUCT_SIZE(biped_datum, 0x480);

void bipeds_apply_patches(void);

void __cdecl biped_get_sight_position(
	datum biped_index,
	e_unit_estimate_mode estimate_mode,
	real_point3d* estimated_body_position,
	real_vector3d* desired_facing_vector,
	real_vector3d* desired_gun_offset,
	real_point3d* object_origin);