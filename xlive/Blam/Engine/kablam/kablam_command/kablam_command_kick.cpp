#include "stdafx.h"
#include "kablam_command_kick.h"

#include "kablam_command_util.h"

#include "kablam_strings.h"

/* public code */

void kablam_command_kick::execute_rpc_command(void)
{
	kablam_command_kick_rpc(this->gamertag, &this->result_code);
	return;
}

void kablam_command_kick::print_help_text(void)
{
	kablam_command_print_help_text(kablam_string_help_kick_desc, kablam_string_help_kick_usage);
	return;
}

void kablam_command_kick::parse_response(
	kablam_command* in_command)
{
	kablam_command_kick* command = (kablam_command_kick*)in_command;

	int32 response_string_id;
	switch (command->result_code)
	{
	case kick_player_response_player_kicked:
		response_string_id = kablam_string_info_player_booted;
		break;
	case kick_player_response_server_not_running:
		response_string_id = kablam_string_status_server_not_active;
		break;
	case kick_player_response_player_not_found:
		response_string_id = kablam_string_err_player_not_found;
		break;
	default:
		response_string_id = 0;
		break;
	}

	kablam_string_quick_wprintf(L"%s", response_string_id);
	return;
}

kablam_command* kablam_command_kick::create_instance(
	wchar_t const* const* arguments,
	uint32 argument_count,
	kablam_string* out_message)
{
	out_message->free();

	if (argument_count > 2)
	{
		out_message->load(kablam_string_err_too_many_args);
		return nullptr;
	}

	if (argument_count < 2)
	{
		out_message->load(kablam_string_err_missing_player_name);
		return nullptr;
	}

	kablam_command_kick* instance = new kablam_command_kick();

	if (!instance)
	{
		out_message->load(kablam_string_err_insufficient_memory);
		return nullptr;
	}

	instance->set_type(_kablam_command_kick);
	instance->set_valid(true);

	wcsncpy_s(instance->gamertag, 32, arguments[1], _TRUNCATE);

	instance->result_code = kick_player_response_player_kicked;

	return instance;
}
