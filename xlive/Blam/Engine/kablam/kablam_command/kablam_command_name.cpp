#include "stdafx.h"
#include "kablam_command_name.h"

#include "kablam_command_util.h"

#include "kablam_strings.h"

void kablam_command_name::execute_rpc_command()
{
	if (this->type == _kablam_command_set_name)
		kablam_command_set_name_rpc(this->name, &this->result_code);

	if (this->type == _kablam_command_get_name)
		kablam_command_get_name_rpc(&this->result_code, 16, this->name);
}

void kablam_command_name::print_help_text()
{
	kablam_command_print_help_text(kablam_string_help_name_desc, kablam_string_help_name_usage);
}

void kablam_command_name::parse_response(kablam_command* in_command)
{
	kablam_command_name* command = (kablam_command_name*)in_command;

	if (command->type == _kablam_command_get_name)
	{
		if (!command->result_code)
		{
			wprintf(L"\"%s\"", command->name);
		}
		else
		{
			kablam_string response_string;

			if (command->result_code == name_result_code_invalid_utf16)
				response_string.load(kablam_string_err_variant_invalid_utf16);
			else
				response_string.load(kablam_string_err_command_lan_only);

			wprintf(L"%s", response_string.get());

			response_string.free();
		}
	}

	if (command->type == _kablam_command_set_name)
	{
		int result_string_id = 0;

		switch (command->result_code)
		{
			case name_result_code_success:
				result_string_id = kablam_string_info_session_name_changed;
				break;
			case name_result_code_lan_only:
				result_string_id = kablam_string_err_command_lan_only;
				break;
			case name_result_code_invalid_utf16:
				result_string_id = kablam_string_err_variant_invalid_utf16;
				break;
		}

		kablam_string_quick_wprintf(L"%s", result_string_id);
	}
}


kablam_command* kablam_command_name::create_instance(wchar_t** arguments, uint32 argument_count, kablam_string* out_message)
{
	out_message->free();

	if (argument_count > 2)
	{
		out_message->load(kablam_string_err_too_many_args);
		return nullptr;
	}

	kablam_command_name* instance = new kablam_command_name();

	instance->valid = true;

	if (argument_count == 1)
	{
		instance->type = _kablam_command_get_name;
		memset(instance->name, 0, 16);
	}
	else
	{
		instance->type = _kablam_command_set_name;
		wcsncpy_s(instance->name, 16, arguments[1], -1);
	}

	return instance;
}
