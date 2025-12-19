#include "stdafx.h"
#include "kablam_command_statsfolder.h"

#include "kablam_command_util.h"

#include "kablam_strings.h"

void kablam_command_statsfolder::execute_rpc_command()
{
	if (this->type == _kablam_command_stats_folder_get)
		kablam_command_get_stats_folder_rpc(&this->result_code, MAX_PATH, this->folder_path);

	if (this->type == _kablam_command_stats_folder_set)
		kablam_command_set_stats_folder_rpc(this->folder_path, &this->result_code);
}

void kablam_command_statsfolder::print_help_text()
{
	kablam_command_print_help_text(kablam_string_help_statsfolder_desc, kablam_string_help_statsfolder_usage);
}

void kablam_command_statsfolder::parse_response(kablam_command* in_command)
{
	kablam_command_statsfolder* command = (kablam_command_statsfolder*)in_command;

	if (command->type == _kablam_command_stats_folder_set)
	{
		int32 response_string_id = 0;

		switch (command->result_code)
		{
		case stats_folder_result_code_success:
			response_string_id = kablam_string_info_statsfolder_changed;
			break;
		case stats_folder_result_code_invalid:
			response_string_id = kablam_string_err_statsfolder_invalid;
			break;
		case stats_folder_result_code_not_writable:
			response_string_id = kablam_string_err_statsfolder_not_writeable;
			break;
		case stats_folder_result_code_export_disabled:
			response_string_id = kablam_string_info_stats_export_disabled;
			break;
		}

		kablam_string_quick_wprintf(L"%ws", response_string_id);
	}

	if (command->type == _kablam_command_stats_folder_get)
	{
		if (!command->result_code)
		{
			wprintf(L"\"%ws\"", command->folder_path);
		}
		else
		{
			if (command->result_code == stats_folder_result_code_export_disabled)
			{
				kablam_string_quick_wprintf(L"%ws", kablam_string_info_stats_export_disabled);
			}
			else
			{
				// ASSERT(result==stats_folder_result_code_export_disabled);
			}
		}
	}
}

kablam_command* kablam_command_statsfolder::create_instance(wchar_t** arguments, uint32 argument_count, kablam_string* out_message)
{
	out_message->free();

	if (argument_count > 2)
	{
		out_message->load(kablam_string_err_too_many_args);
		return nullptr;
	}

	kablam_command_statsfolder* instance = new kablam_command_statsfolder();

	instance->valid = true;
	instance->result_code = stats_folder_result_code_success;

	if (argument_count == 2)
	{
		instance->type = _kablam_command_stats_folder_set;
		wcsncpy_s(instance->folder_path, MAX_PATH, arguments[1], -1);
	}

	if (argument_count == 1)
	{
		instance->type = _kablam_command_stats_folder_get;
		instance->folder_path[0] = L'\0';
	}

	return instance;
}
