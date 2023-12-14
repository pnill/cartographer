#pragma once

class c_particle_system_definition;
class c_particle_definition_interface
{
public:
	virtual c_particle_system_definition* get_attached_particle_system(int32 particle_system_index);
};

c_particle_definition_interface* __cdecl get_particle_system_interface_from_tag_index(datum tag_index);