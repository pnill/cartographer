#include "stdafx.h"
#include "particle_emitter.h"

#include "Blam/Engine/camera/camera.h"

s_data_array* get_particle_emitter_table()
{
	return *Memory::GetAddress<s_data_array**>(0x4DD090, 0x5053B8);
}
