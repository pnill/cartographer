#include "stdafx.h"
#include "particle.h"

#include "particle_system.h"
#include "Blam/Engine/game/game_time.h"


s_data_array* get_particle_table()
{
	return *Memory::GetAddress<s_data_array**>(0x4DD08C, 0x5053B4);
}
