#pragma once

#include "effects_creation.h"
#include "effect_location_definition.h"

class c_particle_definition_interface;
struct c_particle_definition_interface_vtable
{
	void* gap_0[7];
	int32(__thiscall* get_attached_particle_system)(c_particle_definition_interface*, int32);
	int32(__thiscall* get_particle_definiton_locations_size)(c_particle_definition_interface*);
	effect_location_definition* (__thiscall* get_particle_definiton_locations)(c_particle_definition_interface*);
	void* gap_28[3];
	bool(__thiscall* system_is_debug_only_or_looping_or_cinematic)(c_particle_definition_interface*);
	void* function_14;
	void* function_15;
	void* function_16;
	bool(__thiscall* system_is_v_mirrored_or_one_shot)(c_particle_definition_interface*);
	bool(__thiscall* system_is_one_shot)(c_particle_definition_interface*);
	void* function_19;
	void(__thiscall* sub_503B62)(c_particle_definition_interface_vtable*, int);
};

class c_particle_definition_interface
{
public:
	c_particle_definition_interface_vtable* vtbl;
	int8 gap_4[8];
};
CHECK_STRUCT_SIZE(c_particle_definition_interface, 12);

c_particle_definition_interface* __cdecl get_particle_system_interface_from_tag_index(datum tag_index);