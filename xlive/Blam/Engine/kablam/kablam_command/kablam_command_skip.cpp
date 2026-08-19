#include "stdafx.h"
#include "kablam_command_skip.h"

#include "kablam_command_util.h"

#include "kablam_strings.h"

/* public code */

void kablam_command_skip::execute_rpc_command(void)
{
	kablam_command_skip_rpc(&this->result);
	return;
}

void kablam_command_skip::print_help_text(void)
{
	kablam_command_print_help_text(kablam_string_help_skip_desc, kablam_string_help_skip_usage);
	return;
}

void kablam_command_skip::parse_response(
	kablam_command* in_command)
{
	kablam_command_skip* command = (kablam_command_skip*)in_command;

	int32 response_string_id;
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
	default:
		response_string_id = 0;
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

	return;
}

kablam_command* kablam_command_skip::create_instance(
	wchar_t const* const* arguments,
	uint32 argument_count,
	kablam_string* out_message)
{
	UNREFERENCED_PARAMETER(arguments);

	kablam_command_skip* result = nullptr;

	out_message->free();

	if (argument_count > 1)
	{
		out_message->load(kablam_string_err_too_many_args);
	}
	else
	{
		result = new kablam_command_skip();
		result->set_type(_kablam_command_skip);
		result->set_valid(true);
		memset(&result->result, 0, sizeof(kablam_command_skip_result));
	}

	return result;
}
