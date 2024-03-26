#pragma once

// TODO move this into object_types.h but currently getting issues
// Has to do with header recursion and errors that pop up with tag definitions?

enum e_object_type : int8
{
	_object_type_biped = 0,
	_object_type_vehicle = 1,
	_object_type_weapon = 2,
	_object_type_equipment = 3,
	_object_type_garbage = 4,
	_object_type_projectile = 5,
	_object_type_scenery = 6,
	_object_type_machine = 7,
	_object_type_control = 8,
	_object_type_light_fixture = 9,
	_object_type_sound_scenery = 10,
	_object_type_crate = 11,
	_object_type_creature = 12,
	k_object_types_count,

	_object_type_flag_unit = FLAG(_object_type_biped) | FLAG(_object_type_vehicle)
};
