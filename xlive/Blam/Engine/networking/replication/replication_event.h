#pragma once
#include "networking/network_constants.h"

/* structures */

class c_replication_outgoing_event
{
private:
	int32 m_state;
	int32 m_type;
	uint32 m_timestamp;
	int32 m_cancel_timer_milliseconds;
	int32 m_entity_references[k_entity_reference_indices_count_max];
	uint8* m_payload_data;
	int32 m_payload_data_size;
	uint32 m_outgoing_mask;
	uint32 m_pending_acknowledgement_mask;
	c_replication_outgoing_event* m_next;
};
ASSERT_STRUCT_SIZE(c_replication_outgoing_event, 44);