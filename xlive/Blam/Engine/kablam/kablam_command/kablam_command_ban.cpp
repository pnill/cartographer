#include "stdafx.h"
#include "kablam_command_ban.h"

#include "kablam_command_util.h"

#include "kablam_strings.h"

#include <time.h>

/* public code */

void kablam_command_ban::execute_rpc_command(void)
{
	switch (type())
	{
	case _kablam_command_ban_ip:
		((kablam_command_ban_ip*)this)->execute_rpc_command();
		break;
	case _kablam_command_ban_nic:
		((kablam_command_ban_nic*)this)->execute_rpc_command();
		break;
	case _kablam_command_ban_gamer:
		((kablam_command_ban_gamer*)this)->execute_rpc_command();
		break;
	case _kablam_command_get_ban_ip:
		((kablam_command_ban_get_ip*)this)->execute_rpc_command();
		break;
	case _kablam_command_get_ban_nic:
		((kablam_command_ban_get_nic*)this)->execute_rpc_command();
		break;
	case _kablam_command_get_ban_gamer:
		((kablam_command_ban_get_gamer*)this)->execute_rpc_command();
		break;
	default:
		assert(false && "unknown command type reached");
		break;
	}

	return;
}

void kablam_command_ban::print_help_text(void)
{
	kablam_command_print_help_text(kablam_string_help_ban_desc, kablam_string_help_ban_usage);
	return;
}

void kablam_command_ban::parse_response(
	kablam_command* in_command)
{
	switch (in_command->type())
	{
	case _kablam_command_ban_ip:
		((kablam_command_ban_ip*)in_command)->parse_response(in_command);
		break;
	case _kablam_command_ban_nic:
		((kablam_command_ban_nic*)in_command)->parse_response(in_command);
		break;
	case _kablam_command_ban_gamer:
		((kablam_command_ban_gamer*)in_command)->parse_response(in_command);
		break;
	case _kablam_command_get_ban_ip:
		((kablam_command_ban_get_ip*)in_command)->parse_response(in_command);
		break;
	case _kablam_command_get_ban_nic:
		((kablam_command_ban_get_nic*)in_command)->parse_response(in_command);
		break;
	case _kablam_command_get_ban_gamer:
		((kablam_command_ban_get_gamer*)in_command)->parse_response(in_command);
		break;
	default:
		assert(false && "unknown command type reached");
		break;
	}
}

kablam_command* kablam_command_ban::create_instance(
	wchar_t const* const* arguments,
	uint32 argument_count,
	kablam_string* out_message)
{
	kablam_command* result = nullptr;
	if (argument_count < 2)
	{
		out_message->load(kablam_string_err_missing_argument);
	}
	else
	{
		if (!_wcsicmp(L"display", arguments[1]))
		{
			if (!_wcsicmp(L"ip", arguments[2]))
			{
				result = kablam_command_ban_get_ip::create_instance(arguments, argument_count, out_message);
			}
			if (!_wcsicmp(L"nic", arguments[2]))
			{
				result = kablam_command_ban_get_nic::create_instance(arguments, argument_count, out_message);
			}
			if (!_wcsicmp(L"gamer", arguments[2]))
			{
				result = kablam_command_ban_get_gamer::create_instance(arguments, argument_count, out_message);
			}
		}
		else if (!_wcsicmp(L"ip", arguments[1]))
		{
			result = kablam_command_ban_ip::create_instance(arguments, argument_count, out_message);
		}
		else if (!_wcsicmp(L"nic", arguments[1]))
		{
			result = kablam_command_ban_nic::create_instance(arguments, argument_count, out_message);
		}
		else if (!_wcsicmp(L"gamer", arguments[1]))
		{
			result = kablam_command_ban_gamer::create_instance(arguments, argument_count, out_message);
		}

		if (!result)
		{
			out_message->load(kablam_string_err_invalid_argument);
		}
	}
	
	return result;
}

void kablam_command_ban_ip::execute_rpc_command(void)
{
	kablam_command_ban_ip_rpc(this->ipv4_subnet.ipv4_address, this->ipv4_subnet.cidr, this->duration, &this->result_code);
	return;
}

void kablam_command_ban_ip::parse_response(
	kablam_command* in_command)
{
	kablam_command_ban_ip* command = (kablam_command_ban_ip*)in_command;

	int32 response_string_id;
	switch (command->result_code)
	{
	case ban_network_result_code_success:
		response_string_id = kablam_string_info_ban_added;
		break;
	case ban_network_result_code_lan_only:
		response_string_id = kablam_string_err_command_lan_only;
		break;
	case ban_network_result_code_ban_list_full:
		response_string_id = kablam_string_err_ban_list_full;
		break;
	case ban_network_result_code_ban_not_found:
		response_string_id = kablam_string_warn_unknown;
		break;
	default:
		response_string_id = 0;
		break;
	}

	kablam_string_quick_wprintf(L"%ws", response_string_id);
	return;
}

kablam_command* kablam_command_ban_ip::create_instance(
	wchar_t const* const* arguments,
	uint32 argument_count,
	kablam_string* out_message)
{
	kablam_command_ban_ip* result = nullptr;
	
	uint32 ip_addr = 0;
	int8 cidr = 0;
	int32 duration = 0;

	out_message->free();

	if (argument_count < 3)
	{
		out_message->load(kablam_string_err_missing_ip_or_cidr);
	}
	else if (!kablam_command_parse_ip_cidr(arguments[2], &ip_addr, &cidr))
	{
		out_message->load(kablam_string_err_invalid_ip_or_cidr);
	}
	else if (argument_count > 4)
	{
		out_message->load(kablam_string_err_too_many_args);
	}
	else if (argument_count == 4 && !kablam_command_parse_duration_string(arguments[3], &duration))
	{
		out_message->load(kablam_string_err_invalid_duration);
	}
	else
	{
		result = new kablam_command_ban_ip();

		result->set_type(_kablam_command_ban_ip);
		result->set_valid(true);

		result->ipv4_subnet.ipv4_address = ip_addr;
		result->ipv4_subnet.cidr = cidr;
		result->duration = duration;
		result->result_code = ban_network_result_code_success;
	}
	return result;
}

void kablam_command_ban_nic::execute_rpc_command(void)
{
	kablam_command_ban_nic_rpc(&this->mac_address, this->duration, &this->result_code);
	return;
}

void kablam_command_ban_nic::parse_response(
	kablam_command* in_command)
{
	kablam_command_ban_nic* command = (kablam_command_ban_nic*)in_command;

	int32 response_string_id;
	switch (command->result_code)
	{
	case ban_network_result_code_success:
		response_string_id = kablam_string_info_ban_added;
		break;
	case ban_network_result_code_lan_only:
		response_string_id = kablam_string_err_command_lan_only;
		break;
	case ban_network_result_code_ban_list_full:
		response_string_id = kablam_string_err_ban_list_full;
		break;
	case ban_network_result_code_ban_not_found:
		response_string_id = kablam_string_warn_unknown;
		break;
	default:
		response_string_id = 0;
		break;
	}

	kablam_string_quick_wprintf(L"%ws", response_string_id);
}

kablam_command* kablam_command_ban_nic::create_instance(
	wchar_t const* const* arguments,
	uint32 argument_count,
	kablam_string* out_message)
{
	out_message->free();

	if (argument_count > 4)
	{
		out_message->load(kablam_string_err_too_many_args);
		return nullptr;
	}

	if (argument_count < 3)
	{
		out_message->load(kablam_string_err_missing_ethernet);
		return nullptr;
	}

	s_mac_address mac_address{};

	if (!kablam_command_parse_mac_address(arguments[2], &mac_address))
	{
		out_message->load(kablam_string_err_invalid_ethernet);
		return nullptr;
	}

	int32 duration = 0;

	if (argument_count == 4 && !kablam_command_parse_duration_string(arguments[3], &duration))
	{
		out_message->load(kablam_string_err_invalid_duration);
		return nullptr;
	}

	kablam_command_ban_nic* instance = new kablam_command_ban_nic();

	instance->set_type(_kablam_command_ban_nic);
	instance->set_valid(true);

	memcpy(&instance->mac_address, &mac_address, sizeof(s_mac_address));
	instance->duration = duration;
	instance->result_code = ban_network_result_code_success;

	return instance;
}

void kablam_command_ban_gamer::execute_rpc_command(void)
{
	kablam_command_ban_gamer_rpc(this->gamertag, this->duration, &this->result_code);
	return;
}

void kablam_command_ban_gamer::parse_response(
	kablam_command* in_command)
{
	kablam_command_ban_gamer* command = (kablam_command_ban_gamer*)in_command;

	int32 response_string_id;
	switch (command->result_code)
	{
	case gamer_table_result_code_success:
		response_string_id = kablam_string_info_ban_added;
		break;
	case gamer_table_result_code_live_only:
		response_string_id = kablam_string_err_command_live_only;
		break;
	case gamer_table_result_code_table_full:
		response_string_id = kablam_string_err_ban_list_full;
		break;
	case gamer_table_result_code_gamer_not_found:
		response_string_id = kablam_string_warn_unknown;
		break;
	default:
		response_string_id = 0;
		break;
	}

	kablam_string_quick_wprintf(L"%ws", response_string_id);
}

kablam_command* kablam_command_ban_gamer::create_instance(
	wchar_t const* const* arguments,
	uint32 argument_count,
	kablam_string* out_message)
{
	kablam_command_ban_gamer* result = nullptr;

	int32 duration = 0;

	out_message->free();
	if (argument_count > 4)
	{
		out_message->load(kablam_string_err_too_many_args);
	}
	else if (argument_count < 3)
	{
		out_message->load(kablam_string_err_missing_gamertag);
	}
	else if (!wcslen(arguments[2]))
	{
		out_message->load(kablam_string_err_invalid_gamertag);
	}
	else if (argument_count == 4 && !kablam_command_parse_duration_string(arguments[3], &duration))
	{
		out_message->load(kablam_string_err_invalid_duration);
	}
	else
	{
		result = new kablam_command_ban_gamer();

		result->set_type(_kablam_command_ban_gamer);
		result->set_valid(true);

		wcsncpy_s(result->gamertag, NUMBEROF(result->gamertag), arguments[2], _TRUNCATE);
		result->duration = duration;
		result->result_code = gamer_table_result_code_success;
	}

	return result;
}

void kablam_command_ban_get_ip::execute_rpc_command(void)
{
	int32 returned_entry_size = 0;

	kablam_command_get_ban_ip_rpc(&this->m_entry_count, &returned_entry_size, ((s_exipration_entry_ipv4_subnet**)&this->m_entry_buffer));

	if (!returned_entry_size && this->m_entry_buffer)
	{
		free(this->m_entry_buffer);
		this->m_entry_buffer = nullptr;
	}

	return;
}

void kablam_command_ban_get_ip::parse_response(
	kablam_command* in_command)
{
	kablam_command_ban_get_ip* command = (kablam_command_ban_get_ip*)in_command;
	if (command->m_entry_count)
	{
		if (command->m_entry_buffer)
		{
			for (uint32 i = 0; i < command->m_entry_count; ++i)
			{
				s_exipration_entry_ipv4_subnet* entry = &((s_exipration_entry_ipv4_subnet*)command->m_entry_buffer)[i];

				wprintf(L"%d.%d.%d.%d", 
					(unsigned int)((entry->ipv4_address.ipv4_address >> 24) & 0xFF),
					(unsigned int)((entry->ipv4_address.ipv4_address >> 16) & 0xFF),
					(unsigned int)((entry->ipv4_address.ipv4_address >> 8) & 0xFF),
					(unsigned int)(entry->ipv4_address.ipv4_address & 0xFF));

				if (entry->ipv4_address.cidr < 32)
				{
					wprintf(L"/%d", entry->ipv4_address.cidr);
				}
				if (entry->expiry_time_stamp > 0)
				{
					kablam_string_quick_wprintf(L" %ws", kablam_string_until_label);

					wchar_t time_buffer[32];
					if (!_wctime64_s(time_buffer, NUMBEROF(time_buffer), &entry->expiry_time_stamp))
					{
						// remove the trailing new line at the end of a formatted time string
						wchar_t* p = wcschr(time_buffer, L'\n');
						if (p)
						{
							p[0] = L'\0';
						}

						wprintf(L" %ws", time_buffer);
					}
				}
				wprintf(L"\r\n");
			}
		}
		else
		{
			kablam_string_quick_wprintf(L"%ws", kablam_string_err_insufficient_memory);
		}
	}
	else
	{
		kablam_string_quick_wprintf(L"%ws", kablam_string_info_list_empty);
	}

	return;
}

kablam_command* kablam_command_ban_get_ip::create_instance(
	wchar_t const* const* arguments,
	uint32 argument_count,
	kablam_string* out_message)
{
	UNREFERENCED_PARAMETER(arguments);
	UNREFERENCED_PARAMETER(argument_count);

	out_message->free();

	kablam_command_ban_get_ip* instance = new kablam_command_ban_get_ip();
	instance->set_type(_kablam_command_get_ban_ip);
	instance->set_valid(true);
	instance->m_entry_count = 0;
	instance->m_entry_buffer = nullptr;
	return instance;
}

void kablam_command_ban_get_nic::execute_rpc_command(void)
{
	int32 returned_entry_size = 0;

	kablam_command_get_ban_nic_rpc(&this->m_entry_count, &returned_entry_size, ((s_expiration_entry_mac_address**)&this->m_entry_buffer));

	if (!returned_entry_size && this->m_entry_buffer)
	{
		free(this->m_entry_buffer);
		this->m_entry_buffer = nullptr;
	}

	return;
}

void kablam_command_ban_get_nic::parse_response(
	kablam_command* in_command)
{
	kablam_command_ban_get_nic* command = (kablam_command_ban_get_nic*)in_command;

	if (command->m_entry_count)
	{
		if (command->m_entry_buffer)
		{
			for (uint32 i = 0; i < command->m_entry_count; ++i)
			{
				s_expiration_entry_mac_address* entry = &((s_expiration_entry_mac_address*)command->m_entry_buffer)[i];

				wprintf(L"%02X-%02X-%02X-%02X-%02X-%02X",
					entry->mac_address.octets[0],
					entry->mac_address.octets[1],
					entry->mac_address.octets[2],
					entry->mac_address.octets[3],
					entry->mac_address.octets[4],
					entry->mac_address.octets[5]);

				if (entry->expiry_time_stamp > 0)
				{
					kablam_string_quick_wprintf(L" %ws", kablam_string_until_label);

					wchar_t time_buffer[32]{};
					if (!_wctime64_s(time_buffer, 32, &entry->expiry_time_stamp))
					{
						// remove the trailing new line at the end of a formatted time string
						*(wcschr(time_buffer, L'\n')) = L'\0';

						// dumb stupid bungie way
						// time_buffer[24] = L"\0";

						wprintf(L" %ws", time_buffer);
					}
				}
				wprintf(L"\r\n");
			}
		}
		else
		{
			kablam_string_quick_wprintf(L"%ws", kablam_string_err_insufficient_memory);
		}
	}
	else
	{
		kablam_string_quick_wprintf(L"%ws", kablam_string_info_list_empty);
	}

	return;
}

kablam_command* kablam_command_ban_get_nic::create_instance(
	wchar_t const* const* arguments,
	uint32 argument_count,
	kablam_string* out_message)
{
	UNREFERENCED_PARAMETER(arguments);
	UNREFERENCED_PARAMETER(argument_count);

	out_message->free();

	kablam_command_ban_get_nic* instance = new kablam_command_ban_get_nic();
	instance->set_type(_kablam_command_get_ban_nic);
	instance->set_valid(true);
	instance->m_entry_count = 0;
	instance->m_entry_buffer = nullptr;
	return instance;
}

void kablam_command_ban_get_gamer::execute_rpc_command(void)
{
	int32 returned_entry_size = 0;

	kablam_command_get_ban_gamer_rpc(&this->m_entry_count, &returned_entry_size, ((s_expiration_entry_gamer**)&this->m_entry_buffer));

	if (!returned_entry_size && this->m_entry_buffer)
	{
		free(this->m_entry_buffer);
		this->m_entry_buffer = nullptr;
	}

	return;
}

void kablam_command_ban_get_gamer::parse_response(
	kablam_command* in_command)
{
	kablam_command_ban_get_gamer* command = (kablam_command_ban_get_gamer*)in_command;
	if (command->m_entry_count)
	{
		if (command->m_entry_buffer)
		{
			for (uint32 i = 0; i < command->m_entry_count; ++i)
			{
				s_expiration_entry_gamer* entry = &((s_expiration_entry_gamer*)command->m_entry_buffer)[i];

				wprintf(L"%ws", entry->gamer_tag.gamertag);

				if (entry->expiry_time_stamp > 0)
				{
					kablam_string_quick_wprintf(L" %ws", kablam_string_until_label);

					wchar_t time_buffer[32]{};
					if (!_wctime64_s(time_buffer, 32, &entry->expiry_time_stamp))
					{
						// remove the trailing new line at the end of a formatted time string
						*(wcschr(time_buffer, L'\n')) = L'\0';

						// dumb stupid bungie way
						// time_buffer[24] = L"\0";

						wprintf(L" %ws", time_buffer);
					}
				}
				wprintf(L"\r\n");
			}
		}
		else
		{
			kablam_string_quick_wprintf(L"%ws", kablam_string_err_insufficient_memory);
		}
	}
	else
	{
		kablam_string_quick_wprintf(L"%ws", kablam_string_info_list_empty);
	}
}

kablam_command* kablam_command_ban_get_gamer::create_instance(
	wchar_t const* const* arguments,
	uint32 argument_count,
	kablam_string* out_message)
{
	UNREFERENCED_PARAMETER(arguments);
	UNREFERENCED_PARAMETER(argument_count);

	out_message->free();

	kablam_command_ban_get_gamer* instance = new kablam_command_ban_get_gamer();
	instance->set_type(_kablam_command_get_ban_gamer);
	instance->set_valid(true);
	instance->m_entry_count = 0;
	instance->m_entry_buffer = nullptr;
	return instance;
}
