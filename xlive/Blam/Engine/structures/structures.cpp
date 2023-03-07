#include "stdafx.h"
#include "structures.h"

__int16 get_global_structure_bsp_index()
{
	return *Memory::GetAddress<__int16*>(0x4119A4, 0x3B5290);
}

s_scenario_structure_bsp_group_definition* get_global_structure_bsp()
{
	return *Memory::GetAddress<s_scenario_structure_bsp_group_definition**>(0x479E6C);
}
