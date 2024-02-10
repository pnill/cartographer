#pragma once
#include "simulation_players.h"

class c_simulation_watcher
{
public:
	void __thiscall generate_player_updates(int32* player_update_count, int32 maximum_player_update_count, simulation_player_update* player_updates)
	{
		return INVOKE_TYPE(0x1ADE8D, 0x1C2932, void(__thiscall*)(c_simulation_watcher*, int32*, int32, simulation_player_update*), this, player_update_count, maximum_player_update_count, player_updates);
	}

	bool __thiscall need_to_generate_updates(void)
	{
		return INVOKE_TYPE(0x1D4B42, 0x1C188C, bool(__thiscall*)(c_simulation_watcher*), this);
	}
};
