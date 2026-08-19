#include "stdafx.h"
#include "kablam_command_players.h"

#include "kablam_command_util.h"

#include "kablam_strings.h"

/* public code */

void kablam_command_players::execute_rpc_command(void)
{
	if (type() == _kablam_command_players_get)
	{
		kablam_command_get_max_players_rpc(&this->max_players);
	}
	else if (type() == _kablam_command_players_set)
	{
		kablam_command_set_max_players_rpc(this->max_players, &this->result_code);
	}

	return;
}

void kablam_command_players::print_help_text(void)
{
	kablam_command_print_help_text(kablam_string_help_players_desc, kablam_string_help_players_usage);
	return;
}

void kablam_command_players::parse_response(
	kablam_command* in_command)
{
	kablam_command_players* command = (kablam_command_players*)in_command;

	if (command->type() == _kablam_command_players_get)
	{
		wprintf(L"%ld", command->max_players);
	}

	if (command->type() == _kablam_command_players_set)
	{
		kablam_string response_string;
		response_string.load(command->result_code == set_max_players_result_code_out_of_range ? kablam_string_err_player_count_out_of_range : kablam_string_info_player_count_changed);

		wprintf(L"%s", response_string.get());

		response_string.free();
	}

	return;
}

kablam_command* kablam_command_players::create_instance(
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

	kablam_command_players* instance = new kablam_command_players();

	instance->set_valid(true);

	if (argument_count == 1)
	{
		instance->set_type(_kablam_command_players_get);
		instance->max_players = 0;
	}
	else
	{
		instance->set_type(_kablam_command_players_set);
		instance->max_players = _wtoi(arguments[1]);
	}

	return instance;
}
