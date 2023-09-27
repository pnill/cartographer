#pragma once
#include "units.h"
#include "Blam/Engine/tag_files/string_id.h"

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
struct s_biped_data_definition : s_unit_data_definition
{
	PAD(0x3F4 - sizeof(s_unit_data_definition));
	e_biped_physics_mode biped_mode;//0x3F4
	PAD(0x480 - 0x3F5);

	// NEW DATA
	string_id variant_name;
};
CHECK_STRUCT_SIZE(s_biped_data_definition, 0x480 + 4);
