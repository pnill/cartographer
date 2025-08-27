#pragma once

/* enums */

enum e_network_message_type : int32
{
	_network_message_type_ping,
	_network_message_type_pong,
	_network_message_type_broadcast_search,
	_network_message_type_broadcast_reply,
	_network_message_type_connect_request,
	_network_message_type_connect_refuse,
	_network_message_type_connect_establish,
	_network_message_type_connect_closed,
	_network_message_type_join_request,
	_network_message_type_join_abort,
	_network_message_type_join_refuse,
	_network_message_type_leave_session,
	_network_message_type_leave_acknowledge,
	_network_message_type_session_disband,
	_network_message_type_session_boot,
	_network_message_type_host_handoff,
	_network_message_type_peer_handoff,
	_network_message_type_host_transition,
	_network_message_type_host_reestablish,
	_network_message_type_host_decline,
	_network_message_type_peer_reestablish,
	_network_message_type_peer_establish,
	_network_message_type_election,
	_network_message_type_election_refuse,
	_network_message_type_time_synchronize,
	_network_message_type_membership_update,
	_network_message_type_peer_properties,
	_network_message_type_delegate_leadership,
	_network_message_type_boot_machine,
	_network_message_type_player_add,
	_network_message_type_player_refuse,
	_network_message_type_player_remove,
	_network_message_type_player_properties,
	_network_message_type_parameters_update,
	_network_message_type_parameters_request,
	_network_message_type_countdown_timer,
	_network_message_type_mode_acknowledge,
	_network_message_type_virtual_couch_update,
	_network_message_type_virtual_couch_request,
	_network_message_type_vote_update,
	_network_message_type_view_establishment,
	_network_message_type_player_acknowledge,
	_network_message_type_synchronous_update,
	_network_message_type_synchronous_actions,
	_network_message_type_synchronous_join,
	_network_message_type_synchronous_gamestate,
	_network_message_type_game_results,
	_network_message_type_text_chat,
	_network_message_type_test,

	// custom network meesages below
	_network_message_type_request_map_filename,
	_network_message_type_custom_map_filename,
	_network_message_type_rank_change,
	_network_message_type_anti_cheat,

	k_network_message_type_count
};

/* structures */

struct s_network_message_type
{
	bool initialized;
	uint8 pad[3];
	const char* name;
	uint32 flags;
	int32 message_size;
	int32 message_size_maximum;
	void* encode_function;
	void* decode_function;
	void* unknown_function;
};
ASSERT_STRUCT_SIZE(s_network_message_type, 32);

/* classes */

class c_network_message_type_collection
{
public:

	void clear_message_types(void);
	void check_message_types(void) const;

	void register_message_type(e_network_message_type type, const char* message_type_name, uint32 flags, int32 message_size, int32 message_size_maximum, void* encode_function, void* decode_function, void* unk_callback)
	{
		INVOKE_TYPE(0x1E81D6, 0x1CA199, void(__thiscall*)(void*, int32, const char*, int32, int32, int32, void*, void*, void*),
			this, type, message_type_name, flags, message_size, message_size_maximum, encode_function, decode_function, unk_callback);
	}
private:
	s_network_message_type m_message_types[k_network_message_type_count];
};

/* globals */

extern c_network_message_type_collection g_network_message_types_mem;

/* prototypes */

const char* get_network_message_description(e_network_message_type type);

bool is_message_custom(e_network_message_type type);

void network_message_type_collection_apply_patches(void);
