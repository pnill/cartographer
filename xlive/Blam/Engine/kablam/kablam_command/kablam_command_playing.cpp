#include "stdafx.h"
#include "kablam_command_playing.h"

#include "kablam_command_util.h"

#include "kablam_strings.h"

void kablam_command_playing::execute_rpc_command()
{
	kablam_command_playing_rpc(&this->response);
}

void kablam_command_playing::print_help_text()
{
	kablam_command_print_help_text(kablam_string_help_playing_desc, kablam_string_help_playing_usage);
}

void kablam_command_playing::parse_response(kablam_command* in_command)
{
	kablam_command_playing* command = (kablam_command_playing*)in_command;

	if (!command->response.playlist_load_result)
	{
		kablam_string_quick_wprintf(L"%ws\r\n", kablam_string_hint_playlist_how_to_load);

		return;
	}

	kablam_string playlist_label(kablam_string_label_playlist);

	wprintf(L"%s %ws\r\n", playlist_label.get(), command->response.loaded_playlist_path);

	playlist_label.free();

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
				wprintf(L"\t%ws %lu", line_label.get(), warning->line_number);
			}

			kablam_command_print_playlist_warning(warning);
		}

		line_label.free();
	}
}

kablam_command* kablam_command_playing::create_instance(wchar_t** arguments, uint32 argument_count, kablam_string* out_message)
{
	out_message->free();

	if (argument_count != 1)
	{
		out_message->load(kablam_string_err_too_many_args);
		return nullptr;
	}

	kablam_command_playing* instance = new kablam_command_playing();

	instance->type = _kablam_command_playing;
	instance->valid = true;

	memset(&instance->response, 0, sizeof(kablam_command_play_result));

	return instance;
}
