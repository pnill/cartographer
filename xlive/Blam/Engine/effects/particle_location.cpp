#include "stdafx.h"
#include "particle_location.h"

s_data_array* get_particle_location_table()
{
	return *Memory::GetAddress<s_data_array**>(0x4DD094, 0);
}
