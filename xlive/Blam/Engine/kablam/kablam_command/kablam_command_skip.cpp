#include "stdafx.h"
#include "kablam_command_skip.h"

#include "kablam_command_util.h"

#include "kablam_strings.h"

void kablam_command_skip::execute_rpc_command()
{
	kablam_command_skip_rpc(&this->result);
}

void kablam_command_skip::print_help_text()
{
	kablam_command_print_help_text(kablam_string_help_skip_desc, kablam_string_help_skip_usage);
}

void kablam_command_skip::parse_response(kablam_command* in_command)
{
	kablam_command_skip* command = (kablam_command_skip*)in_command;

	int response_string_id = 0;

	switch (command->result.result_code)
	{
		case skip_result_code_match_skipped:
			response_string_id = kablam_string_info_playlist_entry_skipped;
			break;
		case skip_result_code_match_ended:
			response_string_id = kablam_string_info_match_ended;
			break;
		case skip_result_code_match_ending_waiting:
			response_string_id = kablam_string_info_match_ending_waiting;
			break;
		case skip_result_code_server_not_active:
			response_string_id = kablam_string_status_server_not_active;
			break;
	}

	kablam_string_quick_wprintf(L"%ws\r\n", response_string_id);

	if (command->result.result_code == skip_result_code_match_skipped)
	{
		if (wcscmp(command->result.playing_variant, L"") && wcscmp(command->result.playing_map, L""))
			wprintf(L"\r\nPlaying: %s on %s.", command->result.playing_variant, command->result.playing_map);

		if (wcscmp(command->result.next_variant, L"") && wcscmp(command->result.next_map, L""))
			wprintf(L"\r\nNext: %s on %s.", command->result.next_variant, command->result.next_map);

		wprintf(L"\r\n");
	}
}

kablam_command* kablam_command_skip::create_instance(wchar_t** arguments, uint32 argument_count, kablam_string* out_message)
{
	out_message->free();

	if (argument_count > 1)
	{
		out_message->load(kablam_string_err_too_many_args);
		return nullptr;
	}

	kablam_command_skip* instance = new kablam_command_skip();

	instance->type = _kablam_command_skip;
	instance->valid = true;

	memset(&instance->result, 0, sizeof(kablam_command_skip_result));

	return instance;
}
