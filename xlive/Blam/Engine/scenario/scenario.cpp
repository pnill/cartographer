#include "stdafx.h"
#include "scenario.h"

s_scenario_group_definition* get_global_scenario() 
{
	return *Memory::GetAddress<s_scenario_group_definition**>(0x479E74, 0x4A6430);
}