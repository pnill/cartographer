#include "stdafx.h"
#include "kablam_command_unban.h"

#include "kablam_command_util.h"

#include "kablam_strings.h"

void kablam_command_unban::execute_rpc_command()
{
	switch (this->type)
	{
		case _kablam_command_unban_ip:
			((kablam_command_unban_ip*)this)->execute_rpc_command();
			break;
		case _kablam_command_unban_nic:
			((kablam_command_unban_nic*)this)->execute_rpc_command();
			break;
		case _kablam_command_unban_gamer:
			((kablam_command_unban_gamer*)this)->execute_rpc_command();
			break;
		case _kablam_command_unban_all:
			((kablam_command_unban_all*)this)->execute_rpc_command();
			break;
		default:
			//ASSERT();
			//No.
			break;
	}
}

void kablam_command_unban::print_help_text()
{
	kablam_command_print_help_text(kablam_string_help_unban_desc, kablam_string_help_unban_usage);
}

void kablam_command_unban::parse_response(kablam_command* in_command)
{
	switch (in_command->type)
	{
		case _kablam_command_unban_ip:
			((kablam_command_unban_ip*)in_command)->parse_response(in_command);
			break;
		case _kablam_command_unban_nic:
			((kablam_command_unban_nic*)in_command)->parse_response(in_command);
			break;
		case _kablam_command_unban_gamer:
			((kablam_command_unban_gamer*)in_command)->parse_response(in_command);
			break;
		case _kablam_command_unban_all:
			((kablam_command_unban_all*)in_command)->parse_response(in_command);
			break;
		default:
			//ASSERT();
			//No.
			break;
	}
}

kablam_command* kablam_command_unban::create_instance(wchar_t** arguments, uint32 argument_count, kablam_string* out_message)
{
	out_message->free();

	if (argument_count < 2)
	{
		out_message->load(kablam_string_err_missing_argument);
		return nullptr;
	}

	if (!_wcsicmp(L"all", arguments[1]))
	{
		return kablam_command_unban_all::create_instance(arguments, argument_count, out_message);
	}
	if (!_wcsicmp(L"ip", arguments[1]))
	{
		return kablam_command_unban_ip::create_instance(arguments, argument_count, out_message);
	}
	if (!_wcsicmp(L"nic", arguments[1]))
	{
		return kablam_command_unban_nic::create_instance(arguments, argument_count, out_message);
	}
	if (!_wcsicmp(L"gamer", arguments[1]))
	{
		return kablam_command_unban_gamer::create_instance(arguments, argument_count, out_message);
	}

	out_message->load(kablam_string_err_invalid_argument);
	return nullptr;
}

void kablam_command_unban_ip::execute_rpc_command()
{
	kablam_command_unban_ip_rpc(this->ipv4_subnet.ipv4_address, this->ipv4_subnet.cidr, &this->result_code);
}

void kablam_command_unban_ip::parse_response(kablam_command* in_command)
{
	kablam_command_unban_ip* command = (kablam_command_unban_ip*)in_command;

	int32 response_string_id = 0;

	switch (command->result_code)
	{
		case ban_network_result_code_success:
			response_string_id = kablam_string_info_ban_removed;
			break;
		case ban_network_result_code_lan_only:
			response_string_id = kablam_string_err_command_lan_only;
			break;
		case ban_network_result_code_ban_list_full:
			response_string_id = kablam_string_err_ban_list_full;
			break;
		case ban_network_result_code_ban_not_found:
			response_string_id = kablam_string_err_ban_not_found;
			break;
	}

	kablam_string_quick_wprintf(L"%ws", response_string_id);
}

kablam_command* kablam_command_unban_ip::create_instance(wchar_t** arguments, uint32 argument_count, kablam_string* out_message)
{
	out_message->free();

	if (argument_count < 3)
	{
		out_message->load(kablam_string_err_missing_ip_or_cidr);
		return nullptr;
	}

	if (argument_count > 3)
	{
		out_message->load(kablam_string_err_too_many_args);
		return nullptr;
	}

	s_ipv4_subnet ipv4_subnet {};

	if (!kablam_command_parse_ip_cidr(arguments[2],&ipv4_subnet))
	{
		out_message->load(kablam_string_err_invalid_ip_or_cidr);
		return nullptr;
	}

	kablam_command_unban_ip* instance = new kablam_command_unban_ip();

	instance->type = _kablam_command_unban_ip;
	instance->valid = true;
	instance->ipv4_subnet = ipv4_subnet;
	instance->result_code = ban_network_result_code_success;

	return instance;
}

void kablam_command_unban_nic::execute_rpc_command()
{
	kablam_command_unban_nic_rpc(&this->mac_address, &this->result_code);
}

void kablam_command_unban_nic::parse_response(kablam_command* in_command)
{
	kablam_command_unban_nic* command = (kablam_command_unban_nic*)in_command;

	int32 response_string_id = 0;

	switch (command->result_code)
	{
		case ban_network_result_code_success:
			response_string_id = kablam_string_info_ban_removed;
			break;
		case ban_network_result_code_lan_only:
			response_string_id = kablam_string_err_command_lan_only;
			break;
		case ban_network_result_code_ban_list_full:
			response_string_id = kablam_string_err_ban_list_full;
			break;
		case ban_network_result_code_ban_not_found:
			response_string_id = kablam_string_err_ban_not_found;
			break;
	}

	kablam_string_quick_wprintf(L"%ws", response_string_id);
}

kablam_command* kablam_command_unban_nic::create_instance(wchar_t** arguments, uint32 argument_count,	kablam_string* out_message)
{
	out_message->free();

	if (argument_count < 3)
	{
		out_message->load(kablam_string_err_missing_ip_or_cidr);
		return nullptr;
	}

	if (argument_count > 3)
	{
		out_message->load(kablam_string_err_too_many_args);
		return nullptr;
	}

	s_mac_address nic{};

	if (!kablam_command_parse_mac_address(arguments[2], &nic))
	{
		out_message->load(kablam_string_err_invalid_ethernet);
		return nullptr;
	}

	kablam_command_unban_nic* instance = new kablam_command_unban_nic();

	instance->type = _kablam_command_unban_nic;
	instance->valid = true;
	instance->mac_address = nic;
	instance->result_code = ban_network_result_code_success;

	return instance;
}

void kablam_command_unban_gamer::execute_rpc_command()
{
	kablam_command_unban_gamer_rpc(this->gamertag, &this->result_code);
}

void kablam_command_unban_gamer::parse_response(kablam_command* in_command)
{
	kablam_command_unban_gamer* command = (kablam_command_unban_gamer*)in_command;

	int32 response_string_id = 0;

	switch (command->result_code)
	{
		case gamer_table_result_code_success:
			response_string_id = kablam_string_info_ban_removed;
			break;
		case gamer_table_result_code_live_only:
			response_string_id = kablam_string_err_command_live_only;
			break;
		case gamer_table_result_code_table_full:
			response_string_id = kablam_string_err_ban_list_full;
			break;
		case gamer_table_result_code_gamer_not_found:
			response_string_id = kablam_string_err_player_not_found;
			break;
	}

	kablam_string_quick_wprintf(L"%ws", response_string_id);
}

kablam_command* kablam_command_unban_gamer::create_instance(wchar_t** arguments, uint32 argument_count,	kablam_string* out_message)
{
	out_message->free();

	if (argument_count < 3)
	{
		out_message->load(kablam_string_err_missing_ip_or_cidr);
		return nullptr;
	}

	if (argument_count > 3)
	{
		out_message->load(kablam_string_err_too_many_args);
		return nullptr;
	}

	if (!wcslen(arguments[2]))
	{
		out_message->load(kablam_string_err_invalid_gamertag);
		return nullptr;
	}

	kablam_command_unban_gamer* instance = new kablam_command_unban_gamer();

	instance->type = _kablam_command_unban_gamer;
	instance->valid = true;
	wcsncpy_s(instance->gamertag, 16, arguments[2], -1);
	instance->result_code = gamer_table_result_code_success;

	return instance;
}

void kablam_command_unban_all::execute_rpc_command()
{
	kablam_command_unban_all_rpc(this->clear_type, &this->result_code);
}

void kablam_command_unban_all::parse_response(kablam_command* in_command)
{
	kablam_command_unban_all* command = (kablam_command_unban_all*)in_command;

	int32 response_string_id = 0;

	switch (command->result_code)
	{
		case unban_all_result_code_success:
			response_string_id = kablam_string_info_ban_cleared;
			break;
		case unban_all_result_code_failed:
			if (command->clear_type == unban_all_gamertags)
				response_string_id = kablam_string_err_command_live_only;
			else
				response_string_id = kablam_string_err_command_lan_only;
			break;
	}

	kablam_string_quick_wprintf(L"%ws", response_string_id);
}

kablam_command* kablam_command_unban_all::create_instance(wchar_t** arguments, uint32 argument_count, kablam_string* out_message)
{
	out_message->free();

	if (argument_count == 2)
	{
		out_message->load(kablam_string_err_missing_argument);
		return nullptr;
	}

	if (argument_count > 3)
	{
		out_message->load(kablam_string_err_too_many_args);
		return nullptr;
	}

	kablam_command_unban_all* instance = new kablam_command_unban_all();

	instance->type = _kablam_command_unban_all;
	instance->valid = true;
	instance->result_code = unban_all_result_code_success;

	if (!_wcsicmp(L"ip", arguments[2]))
	{
		instance->clear_type = unban_all_ip;
	}
	else if (!_wcsicmp(L"nic", arguments[2]))
	{
		instance->clear_type = unban_all_nic;
	}
	else if (!_wcsicmp(L"gamer", arguments[2]))
	{
		instance->clear_type = unban_all_gamertags;
	}
	else
	{
		out_message->load(kablam_string_err_invalid_argument);
		return nullptr;
	}

	return instance;
}
