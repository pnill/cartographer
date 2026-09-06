#pragma once
#include "simulation_players.h"
#include "simulation_world.h"

#include "networking/delivery/network_channel.h"

/* structures */

class c_simulation_watcher : c_network_channel_owner
{
public:
	void __thiscall generate_player_updates(int32* player_update_count, int32 maximum_player_update_count, simulation_player_update* player_updates)
	{
		return INVOKE_TYPE(0x1D5D24, 0x1C2932, void(__thiscall*)(c_simulation_watcher*, int32*, int32, simulation_player_update*), this, player_update_count, maximum_player_update_count, player_updates);
	}

	bool __thiscall need_to_generate_updates(void)
	{
		ASSERT(m_world->exists());

		bool result = INVOKE_TYPE(0x1D4B42, 0x1C188C, bool(__thiscall*)(c_simulation_watcher*), this);
		return (result || !m_world->simulation_queues_empty()) && m_world->is_distributed() && m_world->is_authority();
	}

	void maintain_connection()
	{
		return INVOKE_TYPE(0x1D6531, 0x0, void(__thiscall*)(c_simulation_watcher*), this);
	}

private:
	c_simulation_world* m_world;
	class c_network_observer* m_observer;
	class c_network_session* m_session;
	int32 m_machine_last_local_membership_update_number;
	int32 m_machine_last_membership_update_number;
	int32 m_player_last_local_membership_update_number;
	uint32 m_machine_valid_mask;
	int32 m_local_machine_index;
	s_machine_identifier m_machine_identifiers[k_network_maximum_machines_per_session];
	bool m_machine_update_pending;
	s_player_collection m_player_collection;
	uint32 m_player_collection_machine_valid_mask;
	s_machine_identifier m_player_collection_machine_identifiers[k_network_maximum_machines_per_session];
	bool m_changes_pending_acknowledgement;
};
ASSERT_STRUCT_SIZE(c_simulation_watcher, 0xB3C);
