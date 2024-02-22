#include "stdafx.h"
#include "simulation_actors.h"

void c_simulation_actor::destroy(void)
{
	m_actor_index = NONE;
	m_unit_index = NONE;
	m_simulation_world = NULL;
	return;
}