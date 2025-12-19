#include "stdafx.h"
#include "kablam_command_vip.h"

#include "kablam_command_util.h"

#include "kablam_strings.h"

#include "kablam_rpc/kablam_commands_h.h"

void kablam_command_vip::execute_rpc_command()
{
	switch (this->type)
	{
		case _kablam_command_vip_add:
			((kablam_command_vip_add*)this)->execute_rpc_command();
			break;
		case _kablam_command_vip_remove:
			((kablam_command_vip_remove*)this)->execute_rpc_command();
			break;
		case _kablam_command_vip_get:
			((kablam_command_vip_get*)this)->execute_rpc_command();
			break;
		case _kablam_command_vip_clear:
			((kablam_command_vip_clear*)this)->execute_rpc_command();
			break;
	}
}

void kablam_command_vip::print_help_text()
{
	kablam_command_print_help_text(kablam_string_help_vip_desc, kablam_string_help_vip_usage);
}

void kablam_command_vip::parse_response(kablam_command* in_command)
{
	switch (in_command->type)
	{
		case _kablam_command_vip_add:
			((kablam_command_vip_add*)in_command)->parse_response(in_command);
			break;
		case _kablam_command_vip_remove:
			((kablam_command_vip_remove*)in_command)->parse_response(in_command);
			break;
		case _kablam_command_vip_get:
			((kablam_command_vip_get*)in_command)->parse_response(in_command);
			break;
		case _kablam_command_vip_clear:
			((kablam_command_vip_clear*)in_command)->parse_response(in_command);
			break;
	}
}

kablam_command* kablam_command_vip::create_instance(wchar_t** arguments, uint32 argument_count, kablam_string* out_message)
{
	out_message->free();

	if (argument_count < 2)
	{
		out_message->load(kablam_string_err_missing_argument);
		return nullptr;
	}

	if (!_wcsicmp(L"add", arguments[1]))
	{
		return kablam_command_vip_add::create_instance(arguments, argument_count, out_message);
	}

	if (!_wcsicmp(L"remove", arguments[1]))
	{
		return kablam_command_vip_remove::create_instance(arguments, argument_count, out_message);
	}

	if (!_wcsicmp(L"display", arguments[1]))
	{
		return kablam_command_vip_get::create_instance(arguments, argument_count, out_message);
	}

	if (!_wcsicmp(L"clear", arguments[1]))
	{
		return kablam_command_vip_clear::create_instance(arguments, argument_count, out_message);
	}

	out_message->load(kablam_string_err_invalid_argument);
	return nullptr;
}

void kablam_command_vip_add::execute_rpc_command()
{
	kablam_command_vip_add_rpc(this->gamertag, &this->result_code);
}

void kablam_command_vip_add::parse_response(kablam_command* in_command)
{
	kablam_command_vip_add* command = (kablam_command_vip_add*)in_command;

	int32 response_string_id = 0;

	switch (command->result_code)
	{
		case gamer_table_result_code_success:
			response_string_id = kablam_string_info_vip_added;
			break;
		case gamer_table_result_code_live_only:
			response_string_id = kablam_string_err_command_live_only;
			break;
		case gamer_table_result_code_table_full:
			response_string_id = kablam_string_err_vip_list_full;
			break;
		case gamer_table_result_code_gamer_not_found:
			response_string_id = kablam_string_err_vip_not_found;
			break;
	}

	kablam_string_quick_wprintf(L"%ws", response_string_id);
}

kablam_command* kablam_command_vip_add::create_instance(wchar_t** arguments, uint32 argument_count, kablam_string* out_message)
{
	out_message->free();

	if (argument_count < 3)
	{
		out_message->load(kablam_string_err_missing_gamertag);
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

	kablam_command_vip_add* instance = new kablam_command_vip_add();

	instance->type = _kablam_command_vip_add;
	instance->valid = true;
	instance->result_code = gamer_table_result_code_success;

	wcsncpy_s(instance->gamertag, 16, arguments[2], -1);

	return instance;
}

void kablam_command_vip_remove::execute_rpc_command()
{
	kablam_command_vip_remove_rpc(this->gamertag, &this->result_code);
}

void kablam_command_vip_remove::parse_response(kablam_command* in_command)
{
	kablam_command_vip_remove* command = (kablam_command_vip_remove*)in_command;

	int32 response_string_id = 0;

	switch (command->result_code)
	{
		case gamer_table_result_code_success:
			response_string_id = kablam_string_info_vip_removed;
			break;
		case gamer_table_result_code_live_only:
			response_string_id = kablam_string_err_command_live_only;
			break;
		case gamer_table_result_code_table_full:
			response_string_id = kablam_string_err_vip_list_full;
			break;
		case gamer_table_result_code_gamer_not_found:
			response_string_id = kablam_string_err_vip_not_found;
			break;
	}

	kablam_string_quick_wprintf(L"%ws", response_string_id);
}

kablam_command* kablam_command_vip_remove::create_instance(wchar_t** arguments, uint32 argument_count, kablam_string* out_message)
{
	out_message->free();

	if (argument_count < 3)
	{
		out_message->load(kablam_string_err_missing_gamertag);
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

	kablam_command_vip_remove* instance = new kablam_command_vip_remove();

	instance->type = _kablam_command_vip_remove;
	instance->valid = true;
	instance->result_code = gamer_table_result_code_success;

	wcsncpy_s(instance->gamertag, 16, arguments[2], -1);

	return instance;
}

void kablam_command_vip_get::execute_rpc_command()
{
	int32 returned_entry_size = 0;

	kablam_command_get_vip_rpc(&this->entry_count, &returned_entry_size, ((s_gamertag**)&this->entry_buffer));

	if (!returned_entry_size && this->entry_buffer)
	{
		free(this->entry_buffer);
		this->entry_buffer = nullptr;
	}
}

void kablam_command_vip_get::parse_response(kablam_command* in_command)
{
	kablam_command_vip_get* command = (kablam_command_vip_get*)in_command;

	if (command->entry_count)
	{
		if (command->entry_buffer)
		{

			for (uint32 i = 0; i < command->entry_count; ++i)
			{
				s_gamertag* entry = &((s_gamertag*)command->entry_buffer)[i];

				wprintf(L"%ws\r\n", entry->gamertag);
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

kablam_command* kablam_command_vip_get::create_instance(wchar_t** arguments, uint32 argument_count, kablam_string* out_message)
{
	out_message->free();

	if (argument_count > 2)
	{
		out_message->load(kablam_string_err_too_many_args);
		return nullptr;
	}

	kablam_command_vip_get* instance = new kablam_command_vip_get();

	instance->type = _kablam_command_vip_get;
	instance->valid = true;
	instance->entry_count = 0;
	instance->entry_buffer = nullptr;

	return instance;
}

void kablam_command_vip_clear::execute_rpc_command()
{
	kablam_command_vip_clear_rpc(&this->result_code);
}

void kablam_command_vip_clear::parse_response(kablam_command* in_command)
{
	kablam_command_vip_clear* command = (kablam_command_vip_clear*)in_command;

	int32 response_string_id = 0;

	switch (command->result_code)
	{
		case vip_clear_result_code_success:
			response_string_id = kablam_string_info_vip_cleared;
			break;
		case vip_clear_result_code_live_only:
			response_string_id = kablam_string_err_command_live_only;
			break;
	}

	kablam_string_quick_wprintf(L"%ws", response_string_id);
}

kablam_command* kablam_command_vip_clear::create_instance(wchar_t** arguments, uint32 argument_count, kablam_string* out_message)
{
	out_message->free();

	if (argument_count > 2)
	{
		out_message->load(kablam_string_err_too_many_args);
		return nullptr;
	}

	kablam_command_vip_clear* instance = new kablam_command_vip_clear();

	instance->type = _kablam_command_vip_clear;
	instance->valid = true;
	instance->result_code = vip_clear_result_code_success;

	return instance;
}
