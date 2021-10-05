#pragma once
#include "Blam\Engine\Simulation\UnitControl.h"
#include "Blam\Common\Common.h"

struct s_simulation_actor
{
	int m_actor_index;	//index in actor data array
	datum m_unit_index;	//index in object data array
	void* m_simulation_world;
	int gap_C;
	DWORD start_ticks;
	int gap_14;
	s_unit_control_data m_control;
};
CHECK_STRUCT_SIZE(s_simulation_actor, 0x98);
