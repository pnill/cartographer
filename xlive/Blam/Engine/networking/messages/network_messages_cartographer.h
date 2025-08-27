#pragma once
#include "network_message_handler.h"
#include "network_message_type_collection.h"

/* structures */

struct s_network_message_custom_map_filename
{
	s_network_message_session_data session_data;
	wchar_t file_name[32];
	int map_download_id;
};

struct s_network_message_request_map_filename
{
	s_network_message_session_data session_data;
	unsigned long long player_id;
	int map_download_id;
};

struct s_network_message_rank_change
{
	s_network_message_session_data session_data;
	int8 rank;
};

struct s_network_message_anti_cheat
{
	s_network_message_session_data session_data;
	bool enabled;
};

/* prototypes */

void network_message_types_register_cartographer_types(class c_network_message_type_collection* message_collection);

void network_message_cartographer_send_request_map_filename(int32 map_download_id);

void network_message_cartographer_send_rank_change(int32 peer_index, int8 rank);

void network_message_cartographer_send_anti_cheat(int32 peer_index);
