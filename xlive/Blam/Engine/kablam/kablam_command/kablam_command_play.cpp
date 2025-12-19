#include "stdafx.h"
#include "kablam_command_play.h"

#include "kablam_command_util.h"

#include "kablam_strings.h"

void kablam_command_play::execute_rpc_command()
{
	kablam_command_play_rpc(this->file_path, &this->response);
	return;
}

void kablam_command_play::print_help_text()
{
	kablam_command_print_help_text(kablam_string_help_play_desc, kablam_string_help_play_usage);
}

void kablam_command_play::parse_response(kablam_command* in_command)
{
	kablam_command_play* command = (kablam_command_play*)in_command;

	int32 response_string_id = kablam_string_err_playlist_not_loaded;

	if (command->response.playlist_load_result == 1 && command->response.error_code)
	{
		switch (command->response.error_code)
		{

			case play_error_file_not_found:
				response_string_id = kablam_string_err_file_not_found;
				break;
			case play_error_file_open_access_denied:
				response_string_id = kablam_string_err_file_open_access_denied;
				break;
			case play_error_playlist_no_valid_matches:
				response_string_id = kablam_string_err_playlist_no_valid_matches;
				break;
			default:
			case play_error_playlist_read_failed:
				response_string_id = kablam_string_err_playlist_read_failed;
				break;
		}
	}
	else
	{
		response_string_id = kablam_string_info_playlist_loaded;
	}

	kablam_string_quick_wprintf(L"%ws\r\n", response_string_id);

	if (command->response.warning_count)
	{
		kablam_string_quick_wprintf(L"\r\n%ws\r\n", kablam_string_warnings_label);

		kablam_string line_label;

		line_label.load(kablam_string_line_label);

		for (uint32 i = 0; i < command->response.warning_count; ++i)
		{
			kablam_command_playlist_warning* warning = &command->response.warnings[i];

			if (!warning->line_number)
			{
				wprintf(L"\t");
			}
			else
			{
				wprintf(L"\t%ws %lu: ", line_label.get(), warning->line_number);
			}

			kablam_command_print_playlist_warning(warning);
		}

		line_label.free();
	}
}

kablam_command* kablam_command_play::create_instance(wchar_t** arguments, uint32 argument_count, kablam_string* out_message)
{
	out_message->free();

	if (argument_count > 2)
	{
		out_message->load(kablam_string_err_too_many_args);
		return nullptr;
	}

	if (argument_count == 1)
	{
		out_message->load(kablam_string_err_missing_argument);
		return nullptr;
	}

	kablam_command_play* instance = new kablam_command_play();

	instance->type = _kablam_command_play;
	instance->valid = true;

	wcsncpy_s(instance->file_path, 256, arguments[1], -1);
	memset(&instance->response, 0, sizeof(kablam_command_play_result));

	return instance;
}
