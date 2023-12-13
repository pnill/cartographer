#include "stdafx.h"
#include "particle.h"

s_data_array* get_particle_table()
{
	// TODO Server offset
	return *Memory::GetAddress<s_data_array**>(0x4DD08C);
}
