#include "stdafx.h"
#include "terminal.h"

#include "cseries/threads_windows.h"
#include "interface/hud.h"
#include "main/console.h"
#include "memory/data.h"
#include "memory/rockall_heap_manager.h"
#include "rasterizer/rasterizer_text.h"
#include "render/render_cameras.h"
#include "render/render.h"
#include "text/draw_string.h"
#include "text/font_group.h"

#ifdef TERMINAL_ENABLED

/* constants */

const real32 k_output_total_seconds = 4.f + 1.f;
const int16 k_terminal_tab_stops[] = { 160, 320, 470 };

/* structures */

struct s_terminal_globals
{
	bool initialized;
	bool console_output;
	bool suppress_output;
	data_array* output_lines;
	int32 newest_output_line_index;
	int32 oldest_output_line_index;
	terminal_gets_state* input_state;
	bool insertion_point_visible;
	real32 last_insertion_point_toggle;
};

struct output_line_datum
{
	int16 identifier;
	int32 newer_line_index;
	int32 older_line_index;
	bool tabstop;
	char buffer[256];
	real_argb_color color;
	real32 fade_progress;
};

/* prototypes */

static void terminal_remove_line(datum line_index);

/* globals */

static s_terminal_globals terminal_globals;

#ifdef _DEBUG 
bool g_terminal_render_enable = true;
#else
bool g_terminal_render_enable = false;
#endif

/* public code */

void terminal_initialize(void)
{
	if (!terminal_globals.initialized)
	{
		terminal_globals.output_lines = DATA_NEW("terminal output", 32, sizeof(output_line_datum), 0, normal_allocation_global_get());
		data_make_valid(terminal_globals.output_lines);
		terminal_globals.input_state = NULL;
		terminal_globals.newest_output_line_index = NONE;
		terminal_globals.oldest_output_line_index = NONE;
		terminal_globals.console_output = false;
		terminal_globals.suppress_output = false;
		terminal_globals.initialized = true;
	}
	return;
}

void terminal_dispose(void)
{
	if (terminal_globals.initialized)
	{
		if (terminal_globals.output_lines)
		{
			if (terminal_globals.output_lines->valid)
			{
				data_make_invalid(terminal_globals.output_lines);
			}

			DATA_DISPOSE(terminal_globals.output_lines);
			terminal_globals.output_lines = NULL;
		}
		terminal_globals.initialized = false;
	}
	return;
}

void terminal_output_to_console(bool enabled)
{
	terminal_globals.console_output = enabled;
	return;
}

void terminal_suppress_output(bool suppress_output)
{
	terminal_globals.suppress_output = suppress_output;
	return;
}

void terminal_clear(void)
{
	if (terminal_globals.initialized && is_main_thread())
	{
		terminal_globals.newest_output_line_index = NONE;
		terminal_globals.oldest_output_line_index = NONE;
		data_delete_all(terminal_globals.output_lines);
	}
	return;
}

bool terminal_gets_begin(terminal_gets_state* state)
{
	bool result = false;
	
	ASSERT(state);

	if (!terminal_globals.input_state)
	{
		terminal_globals.input_state = state;

		state->edit.buffer = state->result;
		state->edit.maximum_length = 255;
		state->horizontal_scroll_amount = 0;
		
		edit_text_new(&state->edit);
		state->key_count = 0;

		result = true;
	}
	return result;
}

void terminal_gets_end(terminal_gets_state* state)
{
	terminal_globals.input_state = terminal_globals.input_state == state ? NULL : terminal_globals.input_state;
	return;
}

bool terminal_update_input(real32 dt)
{
	bool result = false;
	if (terminal_globals.input_state)
	{
		terminal_globals.input_state->key_count = 0;

		key_stroke key;
		while (input_abstraction_get_key(&key))
		{
			if (terminal_globals.input_state->key_count < 32)
			{
				terminal_globals.input_state->keys[terminal_globals.input_state->key_count] = key;
				++terminal_globals.input_state->key_count;
			}
			edit_text_handle_key(&terminal_globals.input_state->edit, &key);
			terminal_globals.insertion_point_visible = true;
			terminal_globals.last_insertion_point_toggle = 0.f;
		}

		input_suppress();

		terminal_globals.last_insertion_point_toggle += dt;
		if (terminal_globals.last_insertion_point_toggle > 1.f)
		{
			terminal_globals.insertion_point_visible = terminal_globals.insertion_point_visible == false;
			terminal_globals.last_insertion_point_toggle = 0.f;
		}

		const int32 new_index = terminal_globals.input_state->edit.insertion_point_index + cstrlen(terminal_globals.input_state->prompt);
		int32 scroll_amount = terminal_globals.input_state->horizontal_scroll_amount;

		if (new_index > scroll_amount + 59)
		{
			scroll_amount = new_index - 59;
		}

		if (new_index < scroll_amount + 20)
		{
			scroll_amount = new_index - 20 >= 0 ? new_index - 20 : 0;
		}

		ASSERT(scroll_amount >= 0);

		terminal_globals.input_state->horizontal_scroll_amount = (int16)scroll_amount;
		result = true;
	}
	return result;
}

void terminal_update_output(real32 dt)
{
	if (!terminal_globals.suppress_output)
	{
		int32 older_line_index;
		for (int32 i = terminal_globals.newest_output_line_index; i != NONE; i = older_line_index)
		{
			output_line_datum* line = (output_line_datum*)datum_get(terminal_globals.output_lines, i);
			older_line_index = line->older_line_index;
			line->fade_progress += dt;
			if (k_output_total_seconds < line->fade_progress)
			{
				terminal_remove_line(i);
			}
		}
	}
	return;
}

datum terminal_new_line(void)
{
	if (terminal_globals.output_lines->first_free_absolute_index == 32)
	{
		terminal_remove_line(terminal_globals.oldest_output_line_index);
	}

	const datum new_line_index = datum_new(terminal_globals.output_lines);
	
	ASSERT(new_line_index != NONE);

	output_line_datum* output_line = (output_line_datum*)datum_get(terminal_globals.output_lines, new_line_index);
	output_line->newer_line_index = NONE;
	output_line->older_line_index = terminal_globals.newest_output_line_index;
	terminal_globals.newest_output_line_index = new_line_index;

	if (output_line->older_line_index == NONE)
	{
		terminal_globals.oldest_output_line_index = new_line_index;
	}
	else
	{
		output_line_datum* line = (output_line_datum*)datum_get(terminal_globals.output_lines, output_line->older_line_index);
		line->newer_line_index = new_line_index;
	}
	return new_line_index;
}

bool terminal_update(real32 dt)
{
	bool updated = false;
	if (terminal_globals.initialized)
	{
		updated = terminal_update_input(dt);
		if (!console_is_active())
		{
			terminal_update_output(dt);
		}
	}
	return updated;
}

void __cdecl terminal_draw(void)
{
	if (terminal_globals.initialized)
	{
		const real32 g_hud_scale_secondary = *get_secondary_hud_scale();
		const int16 line_height = (int16)font_get_line_height(_font_id_fixedsys_9);
		const int16 font_size_scaled = (int16)((real32)line_height * g_hud_scale_secondary);
		const render_camera* global_camera = &render_get()->camera;

		if (terminal_globals.input_state)
		{
			char string[288];
			string[0] = '\0';

			terminal_globals.input_state->prompt[31] = '\0';
			csstrncat(string, terminal_globals.input_state->prompt, NUMBEROF(string));
			terminal_globals.input_state->result[255] = '\0';

			const int16 prompt_length = (int16)cstrlen(string);
			csstrncat(string, terminal_globals.input_state->result, NUMBEROF(string));


			rectangle2d rect;
			rect.top = global_camera->window_bounds.bottom - font_size_scaled;
			rect.left = global_camera->window_bounds.left;
			rect.bottom = global_camera->window_bounds.bottom;
			rect.right = global_camera->window_bounds.right;

			offset_rectangle2d(&rect, -global_camera->viewport_bounds.left, -global_camera->viewport_bounds.top);

			draw_string_set_tab_stops(NULL, 0);
			draw_string_set_draw_mode(0, NONE, 0, 0, &terminal_globals.input_state->color, global_real_argb_black, 0);
			
			if (terminal_globals.insertion_point_visible)
			{
				const int16 index = (prompt_length + terminal_globals.input_state->edit.insertion_point_index);
				if (!string[index])
				{
					string[index + 1] = '\0';
				}
				string[index] = '_';
			}

			const int16 starting_index = PIN(terminal_globals.input_state->horizontal_scroll_amount, 0, (int16)cstrlen(string));
			rasterizer_draw_string(&rect, 0, 0, 0, &string[starting_index]);
		}

		if (g_terminal_render_enable
			// Below condition was added to make sure we can display the autocompleted commands in a release build
			|| console_is_active())
		{
			real_argb_color shadow_color = *global_real_argb_black;
			int16 font_size = global_camera->window_bounds.bottom - font_size_scaled;

			output_line_datum* line;
			for (int32 i = terminal_globals.newest_output_line_index; i != NONE && font_size - font_size_scaled > 0; i = line->older_line_index)
			{
				line = (output_line_datum*)datum_get(terminal_globals.output_lines, i);
				real32 fade_progress = PIN(4.f - line->fade_progress, 0.0f, 1.0f);

				real_argb_color color = line->color;

				color.alpha *= fade_progress;
				shadow_color.alpha = color.alpha;

				rectangle2d bounds;
				
				bounds.left = global_camera->window_bounds.left;
				bounds.right = global_camera->window_bounds.right;
				bounds.bottom = font_size;

				font_size -= font_size_scaled;
				bounds.top = font_size;
				offset_rectangle2d(&bounds, -global_camera->viewport_bounds.left, -global_camera->viewport_bounds.top);

				if (line->tabstop)
				{
					const int16* tab_stops = k_terminal_tab_stops;

					const int16 width = rectangle2d_width(&bounds);
					const int16 width_per_stop = width / 4;
					const int16 scaled_tab_stops[3] =
					{
						(int16)(width_per_stop * (int16)1),
						(int16)(width_per_stop * (int16)2),
						(int16)(width_per_stop * (int16)3)
					};
					tab_stops = scaled_tab_stops;

					draw_string_set_tab_stops(tab_stops, NUMBEROF(k_terminal_tab_stops));
				}

				draw_string_set_draw_mode(0, NONE, 0, 0, &color, &shadow_color, 0);
				rasterizer_draw_string(&bounds, 0, 0, 0, line->buffer);
				draw_string_set_tab_stops(k_terminal_tab_stops, 0);
			}
		}
	}
	return;
}

void terminal_printf(const real_argb_color* color, const char* format, ...)
{
	va_list args;
	va_start(args, format);

	const real_argb_color default_color = { 1.f, 0.7f, 0.7f, 0.7f };
	if (is_main_thread() && terminal_globals.initialized && !terminal_globals.suppress_output)
	{
		const int32 line = terminal_new_line();
		ASSERT(format);
		if (line != NONE)
		{
			output_line_datum* output_line = (output_line_datum*)datum_get(terminal_globals.output_lines, line);
			output_line->fade_progress = 0.f;
			output_line->color = color ? *color : default_color;
			
			vsprintf(output_line->buffer, ARRAYSIZE(output_line->buffer), format, args);
			output_line->tabstop = strstr(output_line->buffer, "|t") != NULL;

			if (terminal_globals.console_output)
			{
				fprintf(stderr, "%s\n", output_line->buffer);
			}
		}
	}

	va_end(args);
	return;
}

/* private code */

static void terminal_remove_line(datum line_index)
{
	const output_line_datum* output_line = (output_line_datum*)datum_get(terminal_globals.output_lines, line_index);

	if (output_line->older_line_index == NONE)
	{
		terminal_globals.oldest_output_line_index = output_line->newer_line_index;
	}
	else
	{
		output_line_datum* line = (output_line_datum*)datum_get(terminal_globals.output_lines, output_line->older_line_index);
		line->newer_line_index = output_line->newer_line_index;
	}

	if (output_line->newer_line_index == NONE)
	{
		terminal_globals.newest_output_line_index = output_line->older_line_index;
	}
	else
	{
		output_line_datum* line = (output_line_datum*)datum_get(terminal_globals.output_lines, output_line->newer_line_index);
		line->older_line_index = output_line->older_line_index;
	}

	datum_delete(terminal_globals.output_lines, line_index);
	return;
}

#endif // #ifdef TERMINAL_ENABLED
