#pragma once
#include "units/units.h"

struct c_simulation_actor
{
	int32 m_actor_index;	//index in actor data array
	datum m_unit_index;		//index in object data array
	void* m_simulation_world;
	int32 m_gap_C;
	int32 m_start_ticks;
	int32 m_gap_14;
	unit_control_data m_control;
};
CHECK_STRUCT_SIZE(c_simulation_actor, 0x98);
