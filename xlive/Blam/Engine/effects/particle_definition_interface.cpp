#include "stdafx.h"
#include "particle_definition_interface.h"

c_particle_definition_interface* __cdecl get_particle_system_interface_from_tag_index(datum tag_index)
{
	//TODO: Server offset
	return INVOKE(0xc3292, 0, get_particle_system_interface_from_tag_index, tag_index);
}