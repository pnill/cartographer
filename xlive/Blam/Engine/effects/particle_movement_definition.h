#pragma once
#include "particle_property.h"
#include "Blam/Cache/DataTypes/TagRef.h"

#define k_particle_movement_definition_controller_max_count 4
#define k_particle_movement_definition_controller_parameter_count 9
enum e_particle_movement_flags
{
	_particle_movement_physics = FLAG(0),
	_particle_movement_collide_with_structure = FLAG(1),
	_particle_movement_collide_with_media = FLAG(2),
	_particle_movement_collide_with_scenery = FLAG(3),
	_particle_movement_collide_with_vehicles = FLAG(4),
	_particle_movement_collide_with_bipeds = FLAG(5),
	_particle_movement_swarm = FLAG(6),
	_particle_movement_wind = FLAG(7)
};

enum e_particle_controller_type : int16
{
	_particle_controller_physics,
	_particle_controller_collider,
	_particle_controller_swarm,
	_particle_controller_swind
};

// max_count:    c_particle_movement_definition_controller_parameter_count = 9
class c_particle_controller_parameter
{
	int32 parameter_id;
	c_particle_property property;
};

// max_count:    c_particle_movement_definition_controller_max_count = 4
class c_particle_controller
{
public:
	e_particle_controller_type type;
	int8 pad[2];
	tag_block<c_particle_controller_parameter> parameters;
	int8 pad_2[8];
};

class c_particle_movement_definition
{
public:
	tag_reference template_; // pmov
	e_particle_movement_flags flags;
	tag_block<c_particle_controller> movements;
};
