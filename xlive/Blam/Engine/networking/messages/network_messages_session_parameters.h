#pragma once
#include "network_message_type_collection.h"

#include "game/game_options.h"
#include "saved_games/game_variant.h"
#include "networking/transport/transport_security.h"

/* structures */

struct s_session_remote_join_data
{
	int32 platform;
	s_transport_secure_identifier session_id;
	s_transport_secure_key secure_key;
	s_transport_secure_address host_secure_address;
	int32 field_40;
};
ASSERT_STRUCT_SIZE(s_session_remote_join_data, 0x44);


#pragma pack(push, 1)
struct s_network_message_parameters_update
{
	s_transport_secure_identifier session_id;
	int32 update_number;
	int32 incremental_update_number;
	bool server_hosted;
	bool session_mode_valid;
	bool session_mode_request_ack;
	int8 gap_13;
	int32 session_mode;
	int32 session_mode_sequence;
	bool language_valid;
	int8 gap_1D[3];
	int32 language;
	bool privacy_mode_valid;
	int8 gap_25[3];
	int32 privacy_mode;
	bool maximum_counts_valid;
	int8 gap_2D[3];
	int32 maximum_peer_count;
	int32 maximum_player_count;
	bool gateway_session_valid;
	bool gateway_session_exists;
	s_transport_secure_identifier gateway_session_id;
	bool start_mode_valid;
	int8 gap_43;
	int32 start_mode;
	bool end_game_valid;
	bool end_game;
	bool map_valid;
	int8 gap_4B;
	int32 campaign_id;
	wchar_t scenario_path[128];
	int32 map_count;
	char gap[200];
	bool game_instance_valid;
	int8 gap_421[3];
	int64 game_instance;
	bool random_seed_valid;
	int8 gap_431[3];
	int32 random_seed;
	bool simulation_protocol_valid;
	int8 gap_439[3];
	int32 simulation_protocol;
	bool variant_valid;
	int8 variant_count;
	int8 gap_23A[2];
	s_game_variant game_variant[2];
	bool initial_participants_valid;
	bool initial_participants_exist;
	game_machine_options machine_data;
	game_player_options initial_players[16];
	char gap_1252[2];
	bool ui_state_valid;
	bool game_setup_changing;
	bool campaign_difficulty_valid;
	int8 gap_1257;
	int16 campaign_difficulty;
	bool countdown_valid;
	bool countdown_running;
	int32 countdown_timer;
	int32 countdown_delayed_reason;
	int32 countdown_delayed_timer;
	s_player_identifier countdown_delayed_culprit;
	bool remote_join_data_valid;
	bool remote_join_data_exists;
	int8 gap_1272[2];
	s_session_remote_join_data remote_join_data;
	bool voice_repeater_peer_index_valid;
	int8 gap_12B9[3];
	int32 voice_repeater_peer_index;
	bool text_chat_repeater_peer_index_valid;
	int8 gap_12C1[3];
	int32 text_chat_repeater_peer_index;
};
#pragma pack(pop)
ASSERT_STRUCT_SIZE(s_network_message_parameters_update, 0x12C8);

struct s_network_message_mode_acknowledge
{
	s_transport_secure_identifier session_id;
	int32 session_mode;
	int32 session_mode_sequence;
};
ASSERT_STRUCT_SIZE(s_network_message_mode_acknowledge, 16);

/* prototypes */

void __cdecl network_message_types_register_session_parameters(c_network_message_type_collection* message_collection);
