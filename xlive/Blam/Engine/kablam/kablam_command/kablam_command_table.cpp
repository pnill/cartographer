#include "stdafx.h"
#include "kablam_command_table.h"

#include "kablam_command_ban.h"
#include "kablam_command_description.h"
#include "kablam_command_exit.h"
#include "kablam_command_kick.h"
#include "kablam_command_live.h"
#include "kablam_command_name.h"
#include "kablam_command_play.h"
#include "kablam_command_players.h"
#include "kablam_command_playing.h"
#include "kablam_command_privacy.h"
#include "kablam_command_skip.h"
#include "kablam_command_status.h"
#include "kablam_command_unban.h"
#include "kablam_command_sendmsg.h"
#include "kablam_command_statsfolder.h"
#include "kablam_command_vip.h"

#include "kablam_strings.h"

/* macros */

#define CREATE_KABLAM_COMMAND_DEFINITION(command_name, class_def) \
	kablam_command_definition \
	{ \
		command_name, \
		class_def::create_instance, \
		class_def::print_help_text, \
		class_def::parse_response \
	}

/* globals */

static const kablam_command_definition g_command_table[k_kablam_command_table_count]
{
	CREATE_KABLAM_COMMAND_DEFINITION(L"ban", kablam_command_ban),
	CREATE_KABLAM_COMMAND_DEFINITION(L"description", kablam_command_description),
	CREATE_KABLAM_COMMAND_DEFINITION(L"exit", kablam_command_exit),
	CREATE_KABLAM_COMMAND_DEFINITION(L"kick", kablam_command_kick),
	CREATE_KABLAM_COMMAND_DEFINITION(L"live", kablam_command_live),
	CREATE_KABLAM_COMMAND_DEFINITION(L"name", kablam_command_name),
	CREATE_KABLAM_COMMAND_DEFINITION(L"play", kablam_command_play),
	CREATE_KABLAM_COMMAND_DEFINITION(L"playing", kablam_command_playing),
	CREATE_KABLAM_COMMAND_DEFINITION(L"players", kablam_command_players),
	CREATE_KABLAM_COMMAND_DEFINITION(L"privacy", kablam_command_privacy),
	CREATE_KABLAM_COMMAND_DEFINITION(L"sendmsg", kablam_command_sendmsg),
	CREATE_KABLAM_COMMAND_DEFINITION(L"skip", kablam_command_skip),
	CREATE_KABLAM_COMMAND_DEFINITION(L"statsfolder", kablam_command_statsfolder),
	CREATE_KABLAM_COMMAND_DEFINITION(L"status", kablam_command_status),
	CREATE_KABLAM_COMMAND_DEFINITION(L"unban", kablam_command_unban),
	CREATE_KABLAM_COMMAND_DEFINITION(L"vip", kablam_command_vip),
};

/* public code */

kablam_command* kablam_command_table_create_command(wchar_t** arguments, uint32 argument_count, bool print_messages, uint32* out_command_table_index)
{
	wchar_t* base_command = *arguments;

	if (!_wcsicmp(base_command, L"?") || !_wcsicmp(base_command, L"help"))
	{
		if (argument_count == 1)
		{
			kablam_string_quick_wprintf(L"%ws\r\n", kablam_string_help_more_info_hint);
		}
		else
		{
			bool command_found = false;

			for (uint32 i = 0; i < k_kablam_command_table_count; ++i)
			{
				if (g_command_table[i].command && !_wcsicmp(g_command_table[i].command, arguments[1]))
				{
					g_command_table[i].help_text_function();

					command_found = true;
				}
			}

			if (!command_found)
			{
				kablam_string_quick_wprintf(L"%ws\r\n", kablam_string_available_commands_label);

				for (uint32 i = 0; i < k_kablam_command_table_count; ++i)
				{
					if (g_command_table[i].command)
						wprintf(L"\r\n%s", g_command_table[i].command);
				}
			}
		}

		return nullptr;
	}

	for (uint32 i = 0; i < k_kablam_command_table_count; ++i)
	{
		if (g_command_table[i].command && !_wcsicmp(g_command_table[i].command, base_command))
		{
			if (g_command_table[i].command_constructor)
			{
				kablam_string message;

				kablam_command* result = g_command_table[i].command_constructor(arguments, argument_count, &message);

				if (!result)
				{
					if (print_messages)
						wprintf(L"%s\r\n\r\n", message.get());

					message.free();

					g_command_table[i].help_text_function();

					return nullptr;
				}

				*out_command_table_index = i;

				return result;
			}
		}
	}

	kablam_string_quick_wprintf(L"%ws\r\n", kablam_string_available_commands_label);

	for (uint32 i = 0; i < k_kablam_command_table_count; ++i)
	{
		if (g_command_table[i].command)
			wprintf(L"\r\n%s", g_command_table[i].command);
	}

	return nullptr;
}

void kablam_command_table_print_help_function(
	e_kablam_command_type command_type)
{
	if (g_command_table[command_type].command && g_command_table[command_type].help_text_function)
	{
		g_command_table[command_type].help_text_function();
	}

	return;
}


void kablam_command_table_parse_input(
	wchar_t* command_buffer,
	wchar_t** command_arguments,
	uint32 max_argument_count,
	uint32* command_argument_count)
{
	wchar_t* quote_write_ptr = NULL;
	uint32 argument_count = 0;
	int32 state = 0;
	wchar_t** arg_slot = command_arguments - 1;

	int done = 0;

	while (!done)
	{
		switch (state)
		{
		case 0: // looking for start of next argument
		{
			wchar_t ch = *command_buffer;
			if (ch == L'\0')
			{
				done = 1;
			}
			else if (ch == L'\t' || ch == L' ')
			{
				++command_buffer;
			}
			else
			{
				if (ch == L'"')
				{
					++command_buffer;
					state = 2; // inside quoted argument
				}
				else
				{
					state = 1; // inside unquoted argument
				}

				++argument_count;
				++arg_slot;
				if (argument_count >= max_argument_count)
				{
					// same as original: stop immediately when limit hit
					done = 1;
				}
				else
				{
					*arg_slot = command_buffer;
				}
			}
			break;
		}

		case 1: // inside unquoted argument
		{
			wchar_t ch = *command_buffer;
			if (ch == L'\0')
			{
				done = 1;
			}
			else if (ch == L'\t' || ch == L' ')
			{
				*command_buffer++ = 0;
				state = 0;
			}
			else
			{
				++command_buffer;
			}
			break;
		}

		case 2: // inside quoted argument, looking for closing quote
		{
			wchar_t ch = *command_buffer;
			if (ch == L'\0')
			{
				// original: jump straight to exit
				done = 1;
			}
			else if (ch == L'"')
			{
				quote_write_ptr = command_buffer;
				++command_buffer;
				state = 3;
			}
			else
			{
				++command_buffer;
			}
			break;
		}

		case 3: // after closing quote; decide what to do with following chars
		{
			wchar_t ch = *command_buffer;
			if (ch == L'\0')
			{
				// LABEL_25 + LABEL_26 in original
				*quote_write_ptr = 0;
				done = 1;
			}
			else if (ch == L'\t' || ch == L' ')
			{
				++command_buffer;
				*quote_write_ptr = 0;
				state = 0;
			}
			else
			{
				if (ch == L'"')
				{
					++command_buffer;
				}
				++quote_write_ptr;
				state = 4;
			}
			break;
		}

		case 4: // copying characters after quote back over the quote
		{
			wchar_t ch = *command_buffer;
			if (ch == L'\0')
			{
				*quote_write_ptr = 0;
				done = 1;
			}
			else if (ch == L'"')
			{
				++command_buffer;
				state = 3;
			}
			else
			{
				*quote_write_ptr++ = ch;
				++command_buffer;
			}
			break;
		}

		default:
			done = 1;
			break;
		}
	}

	*command_argument_count = argument_count;
	return;
}

void kablam_command_table_parse_response(
	kablam_command* command,
	uint32 command_table_index)
{
	g_command_table[command_table_index].parse_response_function(command);
	return;
}

#undef CREATE_KABLAM_COMMAND_DEFINITION