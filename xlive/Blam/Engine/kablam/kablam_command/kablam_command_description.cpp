#include "stdafx.h"
#include "kablam_command_description.h"

#include "kablam_command_util.h"
#include "kablam_strings.h"

/* public code */

void kablam_command_description::execute_rpc_command(void)
{
	if (type() == _kablam_command_get_description)
	{
		kablam_command_get_description_rpc(&this->result_code, 32, this->description);
	}
	else if (type() == _kablam_command_set_description)
	{
		kablam_command_set_description_rpc(this->description, &this->result_code);
	}

	return;
}

void kablam_command_description::print_help_text(void)
{
	kablam_command_print_help_text(kablam_string_help_description_desc, kablam_string_help_description_usage);
	return;
}

void kablam_command_description::parse_response(
	kablam_command* in_command)
{
	kablam_command_description* command = (kablam_command_description*)in_command;

	if (command->type() == _kablam_command_get_description)
	{
		if (!command->result_code)
		{
			wprintf(L"\"%s\"", command->description);
		}
		else
		{
			kablam_string response_string;

			if (command->result_code == name_result_code_invalid_utf16)
				response_string.load(kablam_string_err_variant_invalid_utf16);
			else
				response_string.load(kablam_string_err_command_live_only);

			wprintf(L"%s", response_string.get());

			response_string.free();
		}
	}

	if (command->type() == _kablam_command_set_description)
	{
		int32 result_string_id;
		switch (command->result_code)
		{
		case name_result_code_success:
			result_string_id = kablam_string_info_session_description_changed;
			break;
		case name_result_code_lan_only:
			result_string_id = kablam_string_err_command_live_only;
			break;
		case name_result_code_invalid_utf16:
			result_string_id = kablam_string_err_variant_invalid_utf16;
			break;
		default:
			result_string_id = 0;
			break;
		}

		kablam_string_quick_wprintf(L"%s", result_string_id);
	}

	return;
}

kablam_command* kablam_command_description::create_instance(
	wchar_t const* const* arguments, 
	uint32 argument_count,
	kablam_string* out_message)
{
	kablam_command_description* result = nullptr;
	out_message->free();

	if (argument_count > 2)
	{
		out_message->load(kablam_string_err_too_many_args);
	}
	else
	{
		result = new kablam_command_description();
		result->set_valid(true);

		if (argument_count == 1)
		{
			result->set_type(_kablam_command_get_description);
			memset(result->description, 0, 32);
		}
		else
		{
			result->set_type(_kablam_command_set_description);
			wcsncpy_s(result->description, 32, arguments[1], _TRUNCATE);
		}
	}

	return result;
}
