#pragma once
#include "networking/network_constants.h"

// TODO reverse class
class c_replication_event_manager
{
public:
	void reset(void);

private:
	class c_replication_entity_manager* m_entity_manager;
	class c_simulation_event_handler* m_client;
	uint32 m_view_mask;
	class c_replication_event_manager_view* m_views[k_network_maximum_views_per_simulation];
	int32 m_outgoing_event_count;
	class c_replication_outgoing_event* m_outgoing_event_list;
};
ASSERT_STRUCT_SIZE(c_replication_event_manager, 84);