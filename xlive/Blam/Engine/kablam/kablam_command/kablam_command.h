#pragma once
#include "kablam_rpc/kablam_commands_h.h"

enum e_kablam_command_type : int
{
	_kablam_command_exit = 0x0,
	_kablam_command_skip = 0x1,
	_kablam_command_kick = 0x2,
	_kablam_command_set_name = 0x3,
	_kablam_command_get_name = 0x4,
	_kablam_command_set_description = 0x5,
	_kablam_command_get_description = 0x6,
	_kablam_command_status = 0x7,
	_kablam_command_send_message = 0x8,
	_kablam_command_play = 0x9,
	_kablam_command_playing = 0xA,
	_kablam_command_live_key = 0xB,
	_kablam_command_live_auto_signin = 0xC,
	_kablam_command_live_signin = 0xD,
	_kablam_command_live_signout = 0xE,
	_kablam_command_ban_ip = 0xF,
	_kablam_command_ban_nic = 0x10,
	_kablam_command_ban_gamer = 0x11,
	_kablam_command_unban_ip = 0x12,
	_kablam_command_unban_nic = 0x13,
	_kablam_command_unban_gamer = 0x14,
	_kablam_command_unban_all = 0x15,
	_kablam_command_get_ban_ip = 0x16,
	_kablam_command_get_ban_nic = 0x17,
	_kablam_command_get_ban_gamer = 0x18,
	_kablam_command_vip_add = 0x19,
	_kablam_command_vip_remove = 0x1A,
	_kablam_command_vip_clear = 0x1B,
	_kablam_command_vip_get = 0x1C,
	_kablam_command_stats_folder_get = 0x1D,
	_kablam_command_stats_folder_set = 0x1E,
	_kablam_command_privacy_set = 0x1F,
	_kablam_command_privacy_get = 0x20,
	_kablam_command_players_set = 0x21,
	_kablam_command_players_get = 0x22,

	k_kablam_command_type_count
};


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