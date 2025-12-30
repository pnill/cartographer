#include "stdafx.h"
#include "console.h"

#ifdef TERMINAL_ENABLED
#include "debug_keys.h"
#include "main.h"

#include "cseries/threads_windows.h"
#ifdef DEBUG_MENU_ENABLED
#include "debug/menu/debug_menu_main.h"
#endif
#include "hs/hs.h"
#include "interface/terminal.h"
#include "interface/user_interface_guide.h"
#include "shell/shell.h"

/* constants */

enum
{
	k_previous_command_count = 8,
	k_hs_completed_items_per_column = 16,
	k_console_command_length = 255
};

static const real_argb_color k_console_input_color = { 1.f, 1.f, 0.3f, 1.f };	// Pink

/* structures */

struct s_console_globals
{
	bool active;
	bool status_render;
	real32 open_timeout_seconds;
	terminal_gets_state input_state;
	char previous_commands[k_previous_command_count][k_console_command_length];
	int16 previous_command_count;
	int16 newest_previous_command_index;
	int16 selected_previous_command_index;
};

/* prototypes */

// Returns a handle to the text file based on our application type
static _iobuf* console_open_init(void);

static char* console_get_token(void);

static void console_complete(void);

static bool console_process_command(const char* command, bool interactive);

/* globals */

static s_console_globals console_globals;
static bool g_initialize_console = true;
static bool g_dispose_console = true;

bool console_dump_to_file = false;
bool console_dump_to_debug_display = false;

/* public code */

void console_initialize(void)
{
	if (g_initialize_console)
	{
		terminal_initialize();
		console_globals.input_state.color = k_console_input_color;
		console_globals.status_render = true;
		csstrncpy(console_globals.input_state.prompt, "halo( ", NUMBEROF(console_globals.input_state.prompt));
		console_globals.input_state.result[0] = '\0';
		console_globals.newest_previous_command_index = NONE;
		console_globals.previous_command_count = 0;
		console_globals.selected_previous_command_index = NONE;

		g_initialize_console = false;
	}
	return;
}

void console_dispose(void)
{
	if (g_dispose_console)
	{
		if (console_globals.active)
		{
			terminal_gets_end(&console_globals.input_state);
			console_globals.open_timeout_seconds = 0.1f;
			console_globals.active = 0;
		}
		else
		{
#ifdef DEBUG_MENU_ENABLED
			debug_menu_close();
#endif
		}

		debug_keys_dispose();
		terminal_dispose();
		g_dispose_console = false;
	}
	return;
}

bool console_is_active(void)
{
	return 
		console_globals.active
#ifdef DEBUG_MENU_ENABLED
		|| debug_menu_get_active()
#endif
		;
}

bool console_is_empty(void)
{
	return console_globals.active && console_globals.input_state.result[0] == '\0';
}

void console_execute_initial_commands(void)
{
	FILE* init_file = console_open_init();
	if (init_file)
	{
		char string[200] = {};
		char* context = NULL;
		while (fgets(string, NUMBEROF(string) - 1, init_file))
		{
			(void)strtok_s(string, "\r\n\t", &context);
			console_process_command(string, false);
		}
		fclose(init_file);
	}
	return;
}

void console_open(bool open_debug_menu)
{
	if (
		!console_globals.active
#ifdef DEBUG_MENU_ENABLED
		&& !debug_menu_get_active()
#endif
		)
	{
		if (open_debug_menu)
		{
#ifdef DEBUG_MENU_ENABLED
			debug_menu_open();
#endif
		}
		else
		{
			csmemset(console_globals.input_state.result, 0, sizeof(console_globals.input_state.result));
			console_globals.active = terminal_gets_begin(&console_globals.input_state);
		}

		// Prevent game input when console is opened (added code)
		user_interface_guide_state_manager_get()->m_block_game_input = true;
	}
	return;
}

void console_close(void)
{
	if (console_globals.active)
	{
		terminal_gets_end(&console_globals.input_state);
		console_globals.selected_previous_command_index = NONE;
		console_globals.open_timeout_seconds = 0.1f;
		console_globals.active = false;
		
		// Allow game input when closed (added code)
		user_interface_guide_state_manager_get()->m_block_game_input = false;
	}
	else
	{
#ifdef DEBUG_MENU_ENABLED
		debug_menu_close();
#endif
	}
	return;
}

void console_clear(void)
{
	terminal_clear();
	return;
}

void console_printf(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	if (is_main_thread())
	{
		char string[MAX_PATH];
		vsprintf(string, NUMBEROF(string) - 5, format, args);
		terminal_printf(NULL, "%s", string);

		if (console_dump_to_file)
		{
			csstrncat(string, EOL_STRING, NUMBEROF(string) - 2);
			write_to_error_file(_error_category_generic, _error_delayed, string, true);
		}

		if (console_dump_to_debug_display)
		{
			display_debug_string(string);
		}
	}

	va_end(args);
	return;
}

void console_warning(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	
	if (is_main_thread())
	{
		char string[MAX_PATH];
		vsprintf(string, NUMBEROF(string) - 5, format, args);
		terminal_printf(global_real_argb_red, "%s", string);

		if (console_dump_to_file)
		{
			ASSERT(strlen(EOL_STRING) <= 2);
			csstrncat(string, EOL_STRING, NUMBEROF(string) - 2);
			write_to_error_file(_error_category_generic, _error_silent, string, true);
		}
	}
	return;
}

void console_update(real32 dt)
{
	if (!console_is_active() 
#ifdef DEBUG_MENU_ENABLED
		|| debug_menu_get_active()
#endif
		)
	{
		key_stroke key;
		if (input_peek_key(&key) && !key.repeating && key.key_code == _key_tilde)
		{
			input_abstraction_get_key(&key);
			console_open(false);
		}
		else
		{
			debug_keys_update();
		}
	}
	else
	{
		for (int16 i = 0; i < console_globals.input_state.key_count; ++i)
		{
			key_stroke* key = &console_globals.input_state.keys[i];
			ASSERT(key->key_code != NONE);

			bool last_command;
			bool prev_command_valid;
			switch (key->key_code)
			{
			case _key_tab:
				console_dump_to_debug_display = true;
				console_complete();
				console_dump_to_debug_display = false;
				break;
			case _key_return:
			case _keypad_enter:
				if (console_globals.input_state.result[0] == '\0')
				{
					console_close();
					break;
				}
				console_process_command(console_globals.input_state.result, true);
				csmemset(console_globals.input_state.result, 0, sizeof(console_globals.input_state.result));
				edit_text_selection_reset(&console_globals.input_state.edit);
				break;
			case _key_up_arrow:
				console_globals.selected_previous_command_index += 2;
			case _key_down_arrow:
				last_command = --console_globals.selected_previous_command_index > 0;
				console_globals.selected_previous_command_index = last_command ? console_globals.selected_previous_command_index : 0;

				prev_command_valid = console_globals.selected_previous_command_index > console_globals.previous_command_count - 1;
				console_globals.selected_previous_command_index = prev_command_valid ? console_globals.previous_command_count - 1 : console_globals.selected_previous_command_index;

				if (console_globals.selected_previous_command_index != NONE)
				{
					const int16 index = (console_globals.newest_previous_command_index - console_globals.selected_previous_command_index + k_previous_command_count) % k_previous_command_count;
					const char* src = console_globals.previous_commands[index];
					
					// Added code to only copy the length of the string because of the logic within strncpy_s
					const size_t src_len = cstrlen(src) + 1;
					const size_t size = src_len < NUMBEROF(console_globals.input_state.result) ? src_len : NUMBEROF(console_globals.input_state.result);
					csstrncpy(console_globals.input_state.result, src, size);
					
					edit_text_selection_reset(&console_globals.input_state.edit);
				}
				break;
			case _key_tilde:
				console_close();
				break;
			default:
				continue;
			}
		}
	}

	const real32 timeout = console_globals.open_timeout_seconds - dt;
	console_globals.open_timeout_seconds = timeout >= 0.f ? timeout : 0.f;
	return;
}

/* private code */

static _iobuf* console_open_init(void)
{
	_iobuf* result = NULL;
	switch (shell_application_type())
	{
	case _shell_application_game:
		fopen_s(&result, "init.txt", "r");
		break;
	case _shell_application_editor:
		fopen_s(&result, "editor_init.txt", "r");
		break;
	case _shell_application_tool:
		fopen_s(&result, "tool_init.txt", "r");
		break;
	}

	return result;
}

static char* console_get_token(void)
{
	char* string_after_space = strrchr(console_globals.input_state.result, ' ') + 1;
	char* string_after_bracket = strrchr(console_globals.input_state.result, '(') + 1;
	char* string_after_quote = strrchr(console_globals.input_state.result, '"') + 1;

	char* result = console_globals.input_state.result;
	
	result = result <= string_after_space ? string_after_space : result;
	result = result <= string_after_bracket ? string_after_bracket : result;
	result = result <= string_after_quote ? string_after_quote : result;
	return result;
}

static void console_complete(void)
{
	const char* matching_items[256];

	uint32 enumeration_type_flags = (uint32)NONE;

	char* token = console_get_token();
	const int16 count = hs_tokens_enumerate(token, enumeration_type_flags, matching_items, NUMBEROF(matching_items));

	if (count > 0)
	{
		int16 last_similar_character_index = SHORT_MAX;
		const bool print_second_column = count > k_hs_completed_items_per_column;
		
		char print_buffer[1024];
		csstrncpy(print_buffer, "", NUMBEROF(print_buffer));
		console_printf("");

		int32 token_num = 0;
		for (; token_num < count; ++token_num)
		{
			const int16 token_length = (int16)cstrlen(matching_items[token_num]);
			last_similar_character_index = MIN(last_similar_character_index, token_length - 1);

			int16 index = 0;
			for (; ; ++index)
			{
				int32 c = tolower(matching_items[token_num][index]);
				if (c != tolower(matching_items[0][index]) || index > last_similar_character_index)
				{
					break;
				}
			}
			last_similar_character_index = index - 1;

			if (print_second_column)
			{
				csstrncat(print_buffer, matching_items[token_num], NUMBEROF(print_buffer));
				csstrncat(print_buffer, "|t", NUMBEROF(print_buffer));
				if (token_num % 4 == 3)
				{
					console_printf(print_buffer);
					csstrncpy(print_buffer, "", NUMBEROF(print_buffer));
				}
			}
			else
			{
				console_printf(matching_items[token_num]);
			}
		}

		if (print_second_column && (token_num - 1) % 4 != 3)
		{
			console_printf(print_buffer);
		}

		ASSERT((int16)cstrlen(matching_items[0]) >= last_similar_character_index + 1);
		csmemcpy(token, matching_items[0], last_similar_character_index + 1);
		token[last_similar_character_index + 1] = '\0';
		console_globals.input_state.edit.insertion_point_index = (int16)(&token[last_similar_character_index + 1] - console_globals.input_state.result);
	}
	return;
}

static bool console_process_command(const char* command, bool interactive)
{
	bool result = false;
	const size_t length = cstrlen(command);
	if (length < k_console_command_length)
	{
		main_status("console_command", "%s", command);

		const int16 newest_previous_command_index = (console_globals.newest_previous_command_index + 1) % k_previous_command_count;
		console_globals.newest_previous_command_index = newest_previous_command_index;

		// Added code to only copy the length of the string because of the logic within strncpy_s
		// We use the length of the command instead of the size of the buffer
		csstrncpy(console_globals.previous_commands[newest_previous_command_index], command, length + 1);

		console_globals.previous_command_count = MIN(console_globals.previous_command_count + 1, k_previous_command_count);
		console_globals.selected_previous_command_index = NONE;

		result = hs_compile_and_evaluate(command, interactive);

		main_status("console_command", NULL);
	}
	return result;
}

#endif
