#include "stdafx.h"

#include "network_message_type_collection.h"
#include "network_message_handler.h"

/* constants */

static const char* k_network_message_type_collection_description[] = {
	"ping",
	"pong",
	"broadcast_search",
	"broadcast_reply",
	"connect_request",
	"connect_refuse",
	"connect_establish",
	"connect_closed",
	"join_request",
	"join_abort",
	"join_refuse",
	"leave_session",
	"leave_acknowledge",
	"session_disband",
	"session_boot",
	"host_handoff",
	"peer_handoff",
	"host_transition",
	"host_reestablish",
	"host_decline",
	"peer_reestablish",
	"peer_establish",
	"election",
	"election_refuse",
	"time_synchronize",
	"membership_update",
	"peer_properties",
	"delegate_leadership",
	"boot_machine",
	"player_add",
	"player_refuse",
	"player_remove",
	"player_properties",
	"parameters_update",
	"parameters_request",
	"countdown_timer",
	"mode_acknowledge",
	"virtual_couch_update",
	"virtual_couch_request",
	"vote_update",
	"view_establishment",
	"player_acknowledge",
	"synchronous_update",
	"synchronous_actions",
	"synchronous_join",
	"synchronous_gamestate",
	"game_results",
	"text_chat",
	"test",

	//custom packets below
	"request_map_filename",
	"map_file_name",
	"team_change",
	"rank_change",
	TEST_N_DEF(PC13)
	TEST_N_DEF(PC14)

	"end"
};

/* globals */

c_network_message_type_collection g_network_message_types_mem;

/* prototypes */


/* public code */

void c_network_message_type_collection::clear_message_types(void)
{
	csmemset(m_message_types, 0, sizeof(m_message_types));
	return;
}

void c_network_message_type_collection::check_message_types(void) const
{
	for (size_t message_type_index = 0; message_type_index < k_network_message_type_count; ++message_type_index)
	{
		ASSERT(m_message_types[message_type_index].initialized);
	}
	return;
}

const char* get_network_message_description(e_network_message_type type)
{
	return k_network_message_type_collection_description[type];
}

bool is_message_custom(e_network_message_type type)
{
	return type > _network_message_type_test;
}

void network_message_type_collection_apply_patches(void)
{
	WritePointer(Memory::GetAddress(0x1AC8F8, 0x1ACAC6), &g_network_message_types_mem);
	WriteValue<uint8>(Memory::GetAddress(0x1E825E, 0x1CA221), e_network_message_type::k_network_message_type_count);
	network_message_handler_apply_patches();
	return;
}

/* private code */

