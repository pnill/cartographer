#include "stdafx.h"
#include "particle_location.h"

s_data_array* get_particle_location_table()
{
	return *Memory::GetAddress<s_data_array**>(0x4DD094, 0);
}

void c_particle_location::frame_advance(c_particle_system* particle_system, c_particle_system_definition* particle_system_definition, real32 delta)
{
	return INVOKE_BY_TYPE(0x106055, 0x0, void(__thiscall*)(c_particle_location*, c_particle_system*, c_particle_system_definition*, real32), this, particle_system, particle_system_definition, delta);
}
