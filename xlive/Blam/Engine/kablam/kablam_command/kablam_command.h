#pragma once
#include "kablam_rpc/kablam_commands_h.h"

/* enums */

enum e_kablam_command_type
{
	_kablam_command_exit = 0,
	_kablam_command_skip,
	_kablam_command_kick,
	_kablam_command_set_name,
	_kablam_command_get_name,
	_kablam_command_set_description,
	_kablam_command_get_description,
	_kablam_command_status,
	_kablam_command_send_message,
	_kablam_command_play,
	_kablam_command_playing,
	_kablam_command_live_key,
	_kablam_command_live_auto_signin,
	_kablam_command_live_signin,
	_kablam_command_live_signout,
	_kablam_command_ban_ip,
	_kablam_command_ban_nic,
	_kablam_command_ban_gamer,
	_kablam_command_unban_ip,
	_kablam_command_unban_nic,
	_kablam_command_unban_gamer,
	_kablam_command_unban_all,
	_kablam_command_get_ban_ip,
	_kablam_command_get_ban_nic,
	_kablam_command_get_ban_gamer,
	_kablam_command_vip_add,
	_kablam_command_vip_remove,
	_kablam_command_vip_clear,
	_kablam_command_vip_get,
	_kablam_command_stats_folder_get,
	_kablam_command_stats_folder_set,
	_kablam_command_privacy_set,
	_kablam_command_privacy_get,
	_kablam_command_players_set,
	_kablam_command_players_get,

	k_kablam_command_type_count
};

/* classes */

class kablam_command
{
public:
	e_kablam_command_type type;
	bool valid;

	virtual void execute_rpc_command() = 0;
	virtual ~kablam_command() = default;
};

class kablam_command_dynamic_array : public kablam_command
{
public:
	uint32 entry_count;
	void* entry_buffer;
};
