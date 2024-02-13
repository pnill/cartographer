#pragma once
#include "simulation_players.h"

#include "simulation_world.h"

class c_simulation_watcher
{
public:
	uint8 gap_0[4];
	c_simulation_world* m_sim_world;

	void __thiscall generate_player_updates(int32* player_update_count, int32 maximum_player_update_count, simulation_player_update* player_updates)
	{
		return INVOKE_TYPE(0x1D5D24, 0x1C2932, void(__thiscall*)(c_simulation_watcher*, int32*, int32, simulation_player_update*), this, player_update_count, maximum_player_update_count, player_updates);
	}

	bool __thiscall need_to_generate_updates(void)
	{
		bool result = INVOKE_TYPE(0x1D4B42, 0x1C188C, bool(__thiscall*)(c_simulation_watcher*), this);
		return result || !m_sim_world->simulation_queues_empty();
	}
};
