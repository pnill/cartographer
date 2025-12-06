#include "stdafx.h"
#include "cseries_errors.h"

#include "cseries_windows.h"
#include "stack_walk_windows.h"

#include "interface/terminal.h"
#include "math/color_math.h"
#include "multithreading/synchronization.h"
#include "networking/network_event.h"
#include "shell/shell.h"
#include "shell/shell_windows.h"

/* constants */

enum
{
	k_error_message_buffer_maximum_size = 2048
};

static const char k_too_many_errors_msg[] = "[...too many errors to print...]\r\n";

/* structures */

#ifdef ERRORS_ENABLED
struct error_global_data
{
	bool delayed;
	bool output_to_debug_file;
	bool display_state;
	bool recursion_lock;
	bool overflow_suppression;
	bool suppress_all;
	bool developer_mode;
	bool editing_tools;
	int16 message_buffer_size;
	char message_buffer[k_error_message_buffer_maximum_size];
	wchar_t message_buffer_wide[k_error_message_buffer_maximum_size];
	int16 pad;
	int32 callback_count;
	void* callbacks[32];
	bool category_enabled[20];
	bool category_disabled[20];
	char error_subdirectory[128];
	LPCRITICAL_SECTION system_mutex;
	int32 unused;
	uint32 last_time;
};

struct s_error_category
{
	e_error_category category;
	const char* name;
	real_rgb_color color;
	const char* path;
};
#endif

/* constants */

#ifdef ERRORS_ENABLED
const s_error_category k_category_constants[k_error_category_count] =
{
	{ _error_category_generic, "generic", *global_real_rgb_white, ""},
	{ _error_category_internal_full, "generic", *global_real_rgb_white, "" },
	{ _error_category_internal_subfolder, "generic", *global_real_rgb_white, "debug.txt" },
	{ _error_category_animation, "animation", *global_real_rgb_yellow, "animation_debug.txt" },
	{ _error_category_ai, "ai", *global_real_rgb_salmon, "ai_debug.txt" },
	{ _error_category_shaders, "shaders", *global_real_rgb_blue, "shaders_debug.txt" },
	{ _error_category_geometry, "geometry", { 0.f, 0.f, 0.8f }, "geometry_debug.txt" },				// Blue
	{ _error_category_environment, "environment", { 0.f, 0.f, 0.6f }, "environment_debug.txt" },	// Dark blue
	{ _error_category_objects, "objects", { 0.4f, 0.1f, 0.4f }, "objects_debug.txt" },				// Light green
	{ _error_category_networking, "networking", { 1.f, 0.4f, 0.7f }, "networking_debug.txt" },		// Pink
	{ _error_category_tags, "tags", *global_real_rgb_magenta, "tag_debug.txt" },
	{ _error_category_ui, "ui", *global_real_rgb_orange, "ui_debug.txt" },
	{ _error_category_sound, "sound", { 0.7f, 0.7f, 0.7f }, "sound_debug.txt" },					// Grey
	{ _error_category_multiplayer, "multiplayer", { 0.8f, 0.1f, 0.6f }, "multiplayer_debug.txt" },	// Violet
	{ _error_category_effects, "effects", { 0.f, 0.f, 0.8f }, "effects_debug.txt" },				// Blue
	{ _error_category_animation_audio_content, "animation_audio_content", { 0.7f, 0.7f, 0.5f }, "animation_audio_content_debug.txt" },	// Yellowish grey
	{ _error_category_environment_materials, "environment_materials", { 0.f, 0.f, 0.6f }, "environment_materials_debug.txt" },			// Dark blue
	{ _error_category_object_materials, "object_materials", { 0.4f, 0.1f, 0.4f }, "object_materials_debug.txt" },						// Dark purple
	{ _error_category_design, "design", *global_real_rgb_salmon, "design_debug.txt" },
	{ _error_category_localization, "localization", *global_real_rgb_black, "localization_debug.txt" }
};
#endif

/* prototypes */

#ifdef ERRORS_ENABLED
static void error_write_to_file(void);

static bool error_category_disabled(e_error_category category);

static real_rgb_color* error_category_color(real_rgb_color* color, e_error_category category);

static void write_to_error_file_internal(e_error_category category, const char* string, const char* time);
#endif

/* globals */

#ifdef ERRORS_ENABLED
static error_global_data error_globals;
#endif

/* public code */

#ifdef ERRORS_ENABLED
void errors_initialize(void)
{
	error_globals.output_to_debug_file = true;
	error_globals.suppress_all = false;
	error_globals.developer_mode = false;
	error_globals.overflow_suppression = shell_application_type() != _shell_application_tool;
	error_globals.editing_tools = shell_application_type() == _shell_application_tool;

	// Verify our category constants are setup correctly
	for (int32 category = 0; category < k_error_category_count; ++category)
	{
		ASSERT(k_category_constants[category].category == category);
		ASSERT(k_category_constants[category].name != NULL);
		ASSERT(k_category_constants[category].path != NULL);
	}

	error_globals.last_time = system_milliseconds();
	error_globals.system_mutex = mutex_create();

	if (!mutex_valid(error_globals.system_mutex))
	{
		ASSERT_EXCEPTION("### WARNING: failed to create error system mutex...", false);
	}

	error_globals.message_buffer_size = 0;
	error_globals.message_buffer[0] = '\0';
	stack_walk_initialize();
	
	atexit(error_write_to_file);
	error_globals.delayed = true;
	return;
}
#endif

void error(e_error_priority priority, const char* format, ...)
{
#ifdef ERRORS_ENABLED
	va_list va_args;
	va_start(va_args, format);

	char string[1024];
	vsnprintf(string, NUMBEROF(string), NUMBEROF(string), format, va_args);
	error_va(_error_category_generic, priority, string, va_args);
	
	va_end(va_args);
#endif
	return;
}

void error(e_error_category category, e_error_priority priority, const char* format, ...)
{
#ifdef ERRORS_ENABLED
	va_list va_args;
	va_start(va_args, format);

	char string[1024];
	vsnprintf(string, NUMBEROF(string), NUMBEROF(string), format, va_args);
	error_va(category, priority, string, va_args);

	va_end(va_args);
#endif
	return;
}


void error_va(e_error_category category, e_error_priority priority, const char* format, char* ap)
{
#ifdef ERRORS_ENABLED
	LOG_INFO_GAME(format/*, va_args*/);

	if (error_globals.delayed)
	{
		if (!error_globals.recursion_lock)
		{
			c_critical_section_scope scope(error_globals.system_mutex);
			
			bool dont_print_error = false;
			error_globals.recursion_lock = true;

			ASSERT(VALID_INDEX(category, k_error_category_count));
			ASSERT(VALID_INDEX(priority, k_error_priority_count));
			ASSERT(format);

			if (priority < _error_log)
			{
				if (!error_category_disabled(category))
				{
					priority = MAX(priority, _error_delayed);
				}

				dont_print_error = true;	// TODO: implement function to set this
				if (error_globals.overflow_suppression)
				{
					// TODO: implement
				}
			}

			if (!dont_print_error)
			{
				char string[1032];
				vsprintf(string, NUMBEROF(string), format, ap);

				event(_event_message, "error: %s", string);

				display_debug_string(string);
				csstrncat(string, "\r\n", 1027);
				if (priority > _error_delayed)
				{
					if (error_globals.editing_tools)
					{
						fprintf(stderr, "%s\n", string);
					}
					else
					{
						real_argb_color color = *global_real_argb_white;
						if (priority < _error_log)
						{
							real_rgb_color category_color;
							color.rgb = *error_category_color(&category_color, category);
						}
						else
						{
							color.rgb = *global_real_rgb_red;
						}
						terminal_printf(&color, "%s", string);
					}
				}

				write_to_error_file(category, priority, string, true);
				if (priority >= _error_silent)
				{
					const int32 copy_size = cstrlen(string);
					if ((copy_size + error_globals.message_buffer_size) >= k_error_message_buffer_maximum_size)
					{
						const char* prefix = k_too_many_errors_msg;
						const int32 prefix_size = cstrlen(k_too_many_errors_msg);

						const int32 index = PIN((prefix_size + copy_size + 1024), 0, error_globals.message_buffer_size - 1);

						const char* full_string = &error_globals.message_buffer[index];
						const char* string_after_prefix = strchr(full_string, '\n');

						const int32 message_size = string_after_prefix ? string_after_prefix - error_globals.message_buffer + 1 : error_globals.message_buffer_size;
						const int32 new_size = error_globals.message_buffer_size - message_size;
						ASSERT(copy_size + new_size + copy_size < k_error_message_buffer_maximum_size);

						csmemcpy(error_globals.message_buffer, prefix, prefix_size);

						if (new_size > 0)
						{
							memmove_guarded(
								&error_globals.message_buffer[prefix_size], 
								string_after_prefix,
								new_size,
								error_globals.message_buffer,
								NUMBEROF(error_globals.message_buffer)
							);
						}

						error_globals.message_buffer[new_size + prefix_size] = '\0';
						error_globals.message_buffer_size = (int16)(new_size + prefix_size);
					}
					if ((copy_size + error_globals.message_buffer_size) < k_error_message_buffer_maximum_size)
					{
						csstrncpy(
							&error_globals.message_buffer[error_globals.message_buffer_size],
							string,
							k_error_message_buffer_maximum_size - error_globals.message_buffer_size
						);
						error_globals.message_buffer_size += (int16)copy_size;
					}
				}
			}
			error_globals.recursion_lock = false;
		}
	}
	else if (is_debugger_present())
	{
		OutputDebugStringA("some kind of error is occurring at initialization time");
		DebugBreak();
	}
#endif

	return;
}

void write_to_error_file(e_error_category category, e_error_priority priority, const char* string, bool append_time)
{
#ifdef ERRORS_ENABLED
	ASSERT_EXCEPTION(_error_category_internal_full != category, false);
	ASSERT(_error_category_internal_subfolder != category);

	if (append_time)
	{
		char time[256];
		system_get_date_and_time(time, NUMBEROF(time), false);
		write_to_error_file_internal(category, string, time);
	}
	else
	{
		write_to_error_file_internal(category, string, '\0');
	}
#endif
	return;
}

/* private code */

#ifdef ERRORS_ENABLED
static void error_write_to_file(void)
{
	// TODO: implement
	return;
}

static bool error_category_disabled(e_error_category category)
{
	ASSERT(VALID_INDEX(category, k_error_category_count));
	return !error_globals.category_enabled[category];
}

static real_rgb_color* error_category_color(real_rgb_color* color, e_error_category category)
{
	ASSERT(category < NUMBEROF(k_category_constants));
	*color = k_category_constants[category].color;
	return color;
}

static void write_to_error_file_internal(e_error_category category, const char* string, const char* time)
{
	ASSERT(string);

	// TODO: implement
	return;
}
#endif
