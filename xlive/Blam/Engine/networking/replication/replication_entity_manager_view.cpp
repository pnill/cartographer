#include "stdafx.h"
#include "replication_entity_manager_view.h"

#include "memory/bitstream.h"


bool c_replication_entity_manager_view::has_data_to_transmit()
{
	bool result = false;

	if (this->m_replicating)
		result = this->m_statistics.creations_pending != 0
				|| this->m_statistics.updates_pending != 0
				|| this->m_statistics.deletions_pending != 0;

	return result;
}

bool c_replication_entity_manager_view::build_outgoing_requests(const s_simulation_view_telemetry_data* telemetry_data,	int32 maximum_number_of_requests, void* requests)
{
	return INVOKE_TYPE(0x1D180C, 0x1D6A98, bool(__thiscall*)(c_replication_entity_manager_view*, const s_simulation_view_telemetry_data*, int32, void*), 
		this, telemetry_data, maximum_number_of_requests, requests);
}

int32 c_replication_entity_manager_view::terminator_required_bits()
{
	// todo: determine if its a bits_required for a specific structure or a constant
	return 3;
}

void c_replication_entity_manager_view::write_to_packet(void* request_identifier, int32 request_type, void* telemetry_data, int32 packet_sequence_number, c_bitstream* packet, int32 must_leave_space_bits)
{
	INVOKE_TYPE(0x1D2C24, 0x1D7EB4, void(__thiscall*)(c_replication_entity_manager_view*, void*, int32, void*, int32, c_bitstream*, int32),
		this, request_identifier, request_type, telemetry_data, packet_sequence_number, packet, must_leave_space_bits);
}

void c_replication_entity_manager_view::write_terminator_to_packet(c_bitstream* packet)
{
	packet->write_integer("code", 0, 3);
	this->m_outgoing_packet = nullptr;
}

int32 c_replication_entity_manager_view::read_from_packet(c_bitstream* packet, int32 maximum_number_of_requests, void* requests, int32* out_number_of_requests)
{
	return INVOKE_TYPE(0x1D1C46, 0x1D6ED2, int32(__thiscall*)(c_replication_entity_manager_view*, c_bitstream*, int32, void*, int32*),
		this, packet, maximum_number_of_requests, requests, out_number_of_requests);
}

void c_replication_entity_manager_view::process_incoming_request(void* request)
{
	INVOKE_TYPE(0x1D29BA, 0x1D7C46, void(__thiscall*)(c_replication_entity_manager_view*, void*),
		this, request);
}

void c_replication_entity_manager_view::notify_packet_acknowledged()
{
	// empty function
}

void c_replication_entity_manager_view::mark_packet_delivered(bool delivered)
{
	INVOKE_TYPE(0x1D236B, 0x1D75F7, void(__thiscall*)(c_replication_entity_manager_view*, bool), this, delivered);
}

void c_replication_entity_manager_view::initialize(int32 world_view_index,	class c_replication_entity_manager* entity_manager)
{
	this->m_entity_manager = entity_manager;
	this->m_view_index = world_view_index;
	this->m_view_mask = 1 << world_view_index;
	this->m_packet_list = nullptr;
	this->m_outgoing_packet = nullptr;

	for (uint32 i = 0; i < k_replication_entity_manager_view_max_entities; ++i)
	{
		s_replication_entity_view_data* data = &this->m_entity_data[i];

		data->entity_index = NONE;
		data->update_mask = 0;
		data->state = 0;
		data->next_index = NONE;
		data->update_timestamp = 0;
		data->flags = 0;
	}

	this->m_current_absolute_index_position = NONE;
	this->m_statistics.creations_pending = 0;
	this->m_statistics.creations_sent = 0;
	this->m_statistics.deletions_pending = 0;
	this->m_statistics.deletions_sent = 0;
	this->m_statistics.updates_pending = 0;
	this->m_statistics.updates_sent = 0;
	this->m_replicating = false;
	this->m_fatal_error = false;
	this->m_initialized = true;
}

void c_replication_entity_manager_view::reset(void)
{
	typedef void(__thiscall* reset_t)(c_replication_entity_manager_view*);
	INVOKE_TYPE(0x1D1718, 0x1D69A4, reset_t, this);
	return;
}
