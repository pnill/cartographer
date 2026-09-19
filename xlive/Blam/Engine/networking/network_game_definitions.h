#pragma once
#include "memory/bitstream.h"
#include "networking/transport/transport.h"
#include "saved_games/game_variant.h"

/* enums */

enum e_life_cycle_state
{
	_life_cycle_state_none = 0,
	_life_cycle_state_pre_game,
	_life_cycle_state_start_game,
	_life_cycle_state_in_game,
	_life_cycle_state_post_game,
	_life_cycle_state_joining,
	_life_cycle_state_matchmaking,
	k_life_cycle_state_count
};

enum e_network_session_qos_status_data_type
{
	_network_session_qos_status_data_none = 0,
	_network_session_qos_status_data_lobby_or_game,
	_network_session_qos_status_data_matchmaking,
	k_network_session_qos_status_data_type_count,
	k_network_session_qos_status_data_type_bits = 2,
};

enum e_network_game_simulation_protocol
{
	_network_game_simulation_protocol_offline = 0,
	_network_game_simulation_protocol_synchronous,
	_network_game_simulation_protocol_distributed,

	k_network_game_simulation_protocol_count
};

/* structures */

#pragma pack(push, 1)
struct s_network_squad_status_data_game_details
{
	int16 field;
	int16 n2;
	wchar_t session_name[32];
	int8 gap[70];
	s_transport_session_description description;
	int8 gap1[14];
	int16 open_public_slot_count;
	int16 open_private_slot_count;
	int16 current_public_players;
	int16 current_private_players;
	int16 game_mode;
	int16 game_state;
	int16 network_game_type;
	int16 connection_quality;
	int8 gap2[176];
	int32 game_engine_type;
	int8 gap3[644];
	int16 player_count;
	int16 field_41E;
};
#pragma pack(pop)


struct s_network_squad_status_data
{
	struct s_header
	{
		int16 network_protocol_version;
		int16 network_platform;
		int32 executable_type;
		int32 executable_version;
		int32 compatible_version;
	};

	s_network_squad_status_data::s_header header;
	e_network_session_qos_status_data_type session_mode;
	s_network_squad_status_data_game_details game_details;
	int8 gap[1538];
};
ASSERT_STRUCT_SIZE(s_network_squad_status_data, 2616);


/* public code */
void network_game_definitions_apply_patches();

void __cdecl network_game_definitions_encode_game_variant(c_bitstream* packet, s_game_variant* variant);

bool __cdecl network_game_definitions_decode_game_variant(c_bitstream* packet, s_game_variant* variant);