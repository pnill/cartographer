#include "stdafx.h"
#include "structures.h"

s_structure_globals* structure_globals_get(void)
{
	return Memory::GetAddress<s_structure_globals*>(0x4D12A1, 0x4F5159);
}

int16 get_global_structure_bsp_index(void)
{
	return *Memory::GetAddress<int16*>(0x4119A4, 0x3B5290);
}

s_scenario_structure_bsp_group_definition* get_global_structure_bsp(void)
{
	return *Memory::GetAddress<s_scenario_structure_bsp_group_definition**>(0x479E6C, 0x4A6428);
}