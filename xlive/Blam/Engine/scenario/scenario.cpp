#include "stdafx.h"
#include "scenario.h"

scenario* get_global_scenario(void) 
{
	return *Memory::GetAddress<scenario**>(0x479E74, 0x4A6430);
}

uint32 scenario_netgame_equipment_size(void)
{
	return get_global_scenario()->netgame_equipment.size;
}
