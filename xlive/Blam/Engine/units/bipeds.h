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
	byte gap0[144];
	e_biped_physics_mode biped_movement_type;
	byte gap1[139];
};
CHECK_STRUCT_SIZE(biped_datum, 0x480);
