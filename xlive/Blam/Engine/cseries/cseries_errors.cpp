#include "stdafx.h"
#include "cseries_errors.h"

#ifdef ERRORS_ENABLED

#include "cseries_windows.h"
#include "profile.h"
#include "stack_walk_windows.h"

#include "interface/terminal.h"
#include "main/console.h"
#include "math/color_math.h"
#include "multithreading/synchronization.h"
#include "networking/network_event.h"
#include "shell/shell.h"
#include "shell/shell_windows.h"
#include "tag_files/files.h"

/* constants */

enum
{
	k_error_message_buffer_maximum_size = 2048,
	k_error_callback_count = 32,

	k_error_category_log_time_slice_msec = 10000,
	k_error_category_maximum_logs_per_timeslice = 5,
	k_error_category_time_slice_per_log = k_error_category_log_time_slice_msec / k_error_category_maximum_logs_per_timeslice,

	k_error_log_entry_max_count= 1024,
	k_error_file_cache_size= SHORT_MAX+1,

	k_error_log_idle_interval = 5000,	// Dump the logs stored in memory onto the disk every # of milliseconds

	k_error_log_max_filesize = 0x200000,
};

static const char k_too_many_errors_msg[] = "[...too many errors to print...]\r\n";

const char *const k_primary_event_log_filename = "debug.txt";

const char *const k_primary_full_event_log_filename = "reports\\debug.txt";

/* structures */

class c_error_file
{
public:
	s_file_reference* get_file_reference(void) const;

	void set_file_reference(s_file_reference* reference);

	void flush(void);

	void append_to_buffer(size_t length, char const* string);
	
	void write_file(void);

private:
	s_file_reference* m_file;
	char m_buffer[k_error_message_buffer_maximum_size];
	int32 m_buffer_length;
};

struct s_error_entry
{
	e_error_category category;
	const char* string;
	const char* time;
};

struct s_error_file_cache
{
	c_static_flags_no_init<k_error_category_count> entry_mask;
	c_static_stack<s_error_entry, k_error_log_entry_max_count> entry_stack;
	char buffer[k_error_file_cache_size];
	uint32 current_size;
	c_static_array<c_error_file, k_error_category_count> file_entries;
};

struct error_global_data
{
	bool delayed;
	bool output_to_debug_file;
	bool display_state;
	bool recursion_lock;
	bool overflow_suppression;
	bool suppress_all;
	// Idles and collects logs before dumping them at a specific interval, or when something immediate needs to be logged
	bool idle_logging_and_caching;
	bool editing_tools;
	int16 message_buffer_size;
	char message_buffer[k_error_message_buffer_maximum_size];
	wchar_t message_buffer_wide[k_error_message_buffer_maximum_size];
	int16 pad;
	int32 callback_count;
	void (*callbacks[k_error_callback_count])(void);
	bool category_disabled[k_error_category_count];
	bool prevent_write_to_primary_log[k_error_category_count];
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
	const char* file_name;
};

/* prototypes */

static e_error_category error_get_category_from_string(const char* category_string);

static void write_to_error_file_internal(e_error_category category, const char* string, const char* time, bool write_immediate);

static void error_write_string_to_file(
	c_static_array<c_error_file, k_error_category_count>* file,
	e_error_category category,
	const char* string,
	const char* time,
	bool write_immediate);

static void error_write_string_to_file_internal(
	c_static_array<c_error_file, k_error_category_count>* files,
	e_error_category category,
	const char* string,
	const char* time,
	bool write_immediate);

static void write_string_to_error_file(
	c_error_file* file,
	e_error_category category,
	const char* string,
	const char* time,
	bool write_immediate);

static void error_write_to_file(void);

static s_file_reference* error_category_file_entry_get(e_error_category category);

static void error_category_file_entry_close(e_error_category category, bool write_immediate);

/* globals */

static const s_error_category k_category_constants[k_error_category_count] =
{
	{ _error_category_generic, "generic", { 1.f, 1.f, 1.f }, ""},									// White
	{ _error_category_internal_full, "generic", { 1.f, 1.f, 1.f }, "" },							// White
	{ _error_category_internal_subfolder, "generic", { 1.f, 1.f, 1.f }, "debug.txt" },				// White
	{ _error_category_animation, "animation", { 1.f, 1.f, 0.f }, "animation_debug.txt"},			// Yellow
	{ _error_category_ai, "ai", { 1.f, 0.63f, 0.48f }, "ai_debug.txt"},								// Salmon
	{ _error_category_shaders, "shaders", { 0.f, 0.f, 1.f }, "shaders_debug.txt"},					// Blue
	{ _error_category_geometry, "geometry", { 0.f, 0.f, 0.8f }, "geometry_debug.txt" },				// Blue
	{ _error_category_environment, "environment", { 0.f, 0.f, 0.6f }, "environment_debug.txt" },	// Dark blue
	{ _error_category_objects, "objects", { 0.4f, 0.1f, 0.4f }, "objects_debug.txt" },				// Light green
	{ _error_category_networking, "networking", { 1.f, 0.4f, 0.7f }, "networking_debug.txt" },		// Pink
	{ _error_category_tags, "tags", { 1.f, 0.f, 1.f }, "tag_debug.txt" },							// Magenta
	{ _error_category_ui, "ui", { 1.f, 0.5f, 0.f }, "ui_debug.txt"},								// Orange
	{ _error_category_sound, "sound", { 0.7f, 0.7f, 0.7f }, "sound_debug.txt" },					// Grey
	{ _error_category_multiplayer, "multiplayer", { 0.8f, 0.1f, 0.6f }, "multiplayer_debug.txt" },	// Violet
	{ _error_category_effects, "effects", { 0.f, 0.f, 0.8f }, "effects_debug.txt" },				// Blue
	{ _error_category_animation_audio_content, "animation_audio_content", { 0.7f, 0.7f, 0.5f }, "animation_audio_content_debug.txt" },	// Yellowish grey
	{ _error_category_environment_materials, "environment_materials", { 0.f, 0.f, 0.6f }, "environment_materials_debug.txt" },			// Dark blue
	{ _error_category_object_materials, "object_materials", { 0.4f, 0.1f, 0.4f }, "object_materials_debug.txt" },						// Dark purple
	{ _error_category_design, "design", { 1.f, 0.63f, 0.48f }, "design_debug.txt" },				// Salmon
	{ _error_category_localization, "localization", { 0.f, 0.f, 0.f }, "localization_debug.txt"}	// Black
};

static c_static_array<s_file_reference, k_error_category_count> g_error_category_file_entries;

static c_static_array<bool, k_error_category_count> g_error_category_file_is_initialized;

static c_static_array<bool, k_error_category_count> g_error_category_written_to_state;

static c_static_array<bool, k_error_category_count> g_error_category_file_currently_open;

static error_global_data error_globals;

static s_error_file_cache g_error_file_cache;

static uint32 g_error_category_last_time[k_error_category_count];

static uint32 g_error_category_current_time[k_error_category_count];

static uint32 g_error_category_log_count_per_time_slice[k_error_category_count];

/* public code */

s_file_reference* c_error_file::get_file_reference(void) const
{ 
	return m_file;
}

void c_error_file::set_file_reference(
	s_file_reference* reference)
{
	ASSERT(reference);
	
	write_file();
	m_file = reference;
	m_buffer_length = 0;

	return;
}

void c_error_file::flush(void)
{
	if (m_file)
	{
		write_file();
		m_file = NULL;
		m_buffer_length = 0;
	}
	return;
}

void c_error_file::append_to_buffer(
	size_t length,
	const char* string)
{
	ASSERT(m_file);

	if (m_file)
	{
		if (length + m_buffer_length > k_error_message_buffer_maximum_size)
		{
			write_file();
		}

		if (length + m_buffer_length > k_error_message_buffer_maximum_size)
		{
			ASSERT(m_buffer_length == 0);
			file_write(m_file, length, string);
		}
		else
		{
			csmemcpy(&m_buffer[m_buffer_length], string, length);
			m_buffer_length += length;
		}
	}

	return;
}

void c_error_file::write_file(void)
{
	if (m_buffer_length > 0)
	{
		ASSERT(m_file);
		if (m_file)
		{
			file_write(m_file, m_buffer_length, m_buffer);
		}
		m_buffer_length = 0;
	}
	return;
}

void errors_output_to_debug_file(
	bool output_to_debug_file)
{
	error_globals.output_to_debug_file = output_to_debug_file;

	return;
}

void errors_set_log_subdirectory(
	char const* subdirectory)
{
	csstrncpy(error_globals.error_subdirectory, subdirectory, NUMBEROF(error_globals.error_subdirectory));

	return;
}

s_file_reference* create_report_file_reference(
	s_file_reference* reference,
	const char* name)
{
	// Construct the path
	char path[256];
	csstrncpy(path, "reports", NUMBEROF(path));
	csstrncat(path, "\\", NUMBEROF(path));

	if (error_globals.error_subdirectory[0]!='\0')
	{
		csstrncat(path, error_globals.error_subdirectory, NUMBEROF(path));
		csstrncat(path, "\\", NUMBEROF(path));
	}

	csstrncat(path, name, NUMBEROF(path));

	// Create file reference
	s_file_reference* result = file_reference_create_from_path(reference, path, false);

	ASSERT(result != NULL);

	// Create files
	if (result)
	{
		file_create_parent_directories_if_not_present(result);
	}

	return result;
}

void errors_initialize(void)
{
	error_globals.output_to_debug_file = true;
	error_globals.suppress_all = false;
	error_globals.idle_logging_and_caching = true;
	error_globals.overflow_suppression = shell_application_type() != _shell_application_tool;
	error_globals.editing_tools = shell_application_type() == _shell_application_tool;

	// Verify our category constants are setup correctly
	for (int32 category = 0; category < k_error_category_count; ++category)
	{
		ASSERT(k_category_constants[category].category == category);
		ASSERT(k_category_constants[category].name != NULL);
		ASSERT(k_category_constants[category].file_name != NULL);
	}

	error_globals.last_time = system_milliseconds();
	error_globals.system_mutex = mutex_create();

	if (!mutex_valid(error_globals.system_mutex))
	{
		ASSERT_EXCEPTION("### WARNING: failed to create error system mutex...", false);
	}

	errors_clear();
	stack_walk_initialize();
	
	atexit(error_write_to_file);
	
	error_globals.delayed = true;

	// Added code to initialize subdirectories for multiple game instances
	{
		uint32 instance_number = shell_get_instance_num();
		if (instance_number > 1)
		{
			char instance_number_string[16];

			if (_itoa_s(instance_number, instance_number_string, 10) == ERROR_SUCCESS)
			{
				errors_set_log_subdirectory(instance_number_string);
			}
		}
	}

	return;
}

void errors_dispose(void)
{
	error_write_to_file();
	stack_walk_dispose();

	error_globals.delayed = false;
	
	return;
}

bool errors_add_fatal_error_callback(
	void (*proc)(void))
{
	bool result = false;

	ASSERT(error_globals.callback_count >= 0);
	
	if (error_globals.callback_count >= k_error_callback_count)
	{
		DISPLAY_ASSERT(NULL);
	}
	else
	{
		error_globals.callbacks[error_globals.callback_count] = proc;
		++error_globals.callback_count;
		result = true;
	}

	return result;
}

void call_fatal_error_callbacks(void)
{
	for (int32 i = 0; i < error_globals.callback_count; ++i)
	{
		void (*proc)(void) = error_globals.callbacks[i];
		if (proc)
		{
			proc();
		}
	}

	return;
}

void error(
	e_error_priority priority,
	const char* format,
	...)
{
	va_list va_args;
	va_start(va_args, format);

	error_va(_error_category_generic, priority, format, va_args);
	
	va_end(va_args);

	return;
}

void error(
	e_error_category category,
	e_error_priority priority,
	const char* format,
	...)
{
	va_list va_args;
	va_start(va_args, format);

	error_va(category, priority, format, va_args);

	va_end(va_args);

	return;
}

void error_category_enable(
	const char* category_string,
	bool enable)
{
	const e_error_category category = error_get_category_from_string(category_string);
	if (category >= k_error_category_count)
	{
#ifdef TERMINAL_ENABLED
		console_warning("category %s was unknown", category_string);
#endif
	}
	else
	{
		error_category_disable(category, enable);
	}

	return;
}

bool error_category_enabled(
	e_error_category category)
{
	ASSERT(VALID_INDEX(category, k_error_category_count));

	return !error_globals.category_disabled[category];
}

real_rgb_color* error_category_color(
	real_rgb_color* color,
	e_error_category category)
{
	ASSERT(category < NUMBEROF(k_category_constants));

	*color = k_category_constants[category].color;

	return color;
}

void error_category_disable(
	e_error_category category,
	bool disable)
{
	ASSERT(VALID_INDEX(category, k_error_category_count));

	error_globals.category_disabled[category] = disable == false;

	return;
}

void error_category_write_to_primary_log(
	e_error_category category,
	bool enable)
{
	ASSERT(VALID_INDEX(category, k_error_category_count));

	error_globals.prevent_write_to_primary_log[category] = enable == false;

	return;
}

void error_va(
	e_error_category category,
	e_error_priority priority,
	const char* format,
	char* ap)
{
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

			if (priority < _error_immediate)
			{
				if (!error_category_enabled(category))
				{
					priority = MAX(priority, _error_silent);
				}


				// If we're profiling we don't want to print errors to the screen
#ifdef PROFILE_ENABLED
				dont_print_error = profile_active();
#endif
				if (error_globals.overflow_suppression)
				{
					const uint32 current_time = system_milliseconds();

					// Set the initial time value for our category
					if (g_error_category_log_count_per_time_slice[category] == 0)
					{
						g_error_category_current_time[category] = current_time;
					}

					// Decrement the count based on the time slice per log
					if (
						++g_error_category_log_count_per_time_slice[category] < k_error_category_maximum_logs_per_timeslice &&
						current_time > g_error_category_current_time[category] + k_error_category_time_slice_per_log)
					{
						const uint32 log_count = (current_time - g_error_category_current_time[category]) / k_error_category_time_slice_per_log;
						const uint32 new_count = g_error_category_log_count_per_time_slice[category] - log_count >= 0 ? g_error_category_log_count_per_time_slice[category] - log_count : 0;

						g_error_category_log_count_per_time_slice[category] = new_count;
						g_error_category_current_time[category] += k_error_category_maximum_logs_per_timeslice * log_count;
					}

					// Reset the log count per time slice count if we've 
					if (current_time > g_error_category_last_time[category] + k_error_category_log_time_slice_msec)
					{
						g_error_category_log_count_per_time_slice[category] = 0;
						g_error_category_last_time[category] = current_time;
					}

					// If we've reached the maximum log count then let the user know we're not printing to the category files anymore
					if (g_error_category_log_count_per_time_slice[category] == k_error_category_maximum_logs_per_timeslice)
					{
						terminal_printf(global_real_argb_white, "too many errors, only printing to debug.txt");
					}

					// Notify that we're suppressing errors
					if (g_error_category_log_count_per_time_slice[category] >= k_error_category_maximum_logs_per_timeslice)
					{
						status_printf("suppressing errors");
						priority = MIN(priority, _error_silent);
					}
				}
			}

			if (!dont_print_error)
			{
				char string[1032];
				vsprintf(string, NUMBEROF(string), format, ap);

				event(_event_message, "error: %s", string);

				display_debug_string(string);
				csstrncat(string, "\r\n", 1027);
				if (priority > _error_silent)
				{
					if (error_globals.editing_tools)
					{
						fprintf(stderr, "%s\n", string);
					}
					else
					{
						real_argb_color color = *global_real_argb_white;
						if (priority < _error_immediate)
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

				if (priority >= _error_delayed)
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

	return;
}

void error_enable_suppression(void)
{
	error_globals.suppress_all = true;

	return;
}

void error_disable_suppression(void)
{
	error_globals.suppress_all = false;

	return;
}

void errors_terminate_logging(void)
{
	errors_terminate_files();
	error_globals.suppress_all = true;

	return;
}

void errors_clear(void)
{
	error_globals.message_buffer_size = 0;
	error_globals.message_buffer[0] = '\0';

	return;
}

void errors_terminate_files(void)
{
	error_write_to_file();
	error_write_string_to_file_internal(NULL, _error_category_generic, "", "", true);
	error_write_string_to_file_internal(NULL, _error_category_internal_full, "", "", true);

	for (e_error_category category = _error_category_animation; category < k_error_category_count; ++category)
	{
		error_write_string_to_file_internal(NULL, category, "", "", true);
	}

	return;
}

void error_output_to_debug_file_enable(
	bool enable)
{
	error_globals.output_to_debug_file = enable;

	return;
}

void errors_overflow_suppression_enable(
	bool enable)
{
	error_globals.overflow_suppression = enable;

	return;
}

const char* error_get(void)
{
	return error_globals.message_buffer;
}

const wchar_t* error_get_wide(void)
{
	utf8_string_to_wchar_string(error_globals.message_buffer, error_globals.message_buffer_wide, NUMBEROF(error_globals.message_buffer));

	return error_globals.message_buffer_wide;
}

void error_message_write_and_flush(void)
{
	error_write_to_file();
	errors_clear();

	return;
}

void error_idle(void)
{
	if (error_globals.idle_logging_and_caching && system_milliseconds() - error_globals.last_time >= k_error_log_idle_interval)
	{
		error_write_to_file();
	}

	return;
}

void write_to_error_file(
	e_error_category category,
	e_error_priority priority,
	const char* string,
	bool append_time)
{
	const bool write_immediate= priority>_error_delayed;

	ASSERT_EXCEPTION(_error_category_internal_full != category, false);
	ASSERT(_error_category_internal_subfolder != category);

	if (append_time)
	{
		char time[256];
		system_get_date_and_time(time, NUMBEROF(time), false);
		write_to_error_file_internal(category, string, time, write_immediate);
	}
	else
	{
		write_to_error_file_internal(category, string, "", write_immediate);
	}
	return;
}

/* private code */

static e_error_category error_get_category_from_string(
	const char* category_string)
{
	e_error_category result = k_error_category_count;
	
	for (e_error_category category = _error_category_generic; category < k_error_category_count; ++category)
	{
		if (!csstricmp(category_string, k_category_constants[category].name))
		{
			result = category;
			break;
		}
	}

	return result;
}

static void write_to_error_file_internal(
	e_error_category category,
	const char* string,
	const char* time,
	bool write_immediate)
{
	ASSERT(string);

	const size_t string_size = cstrlen(string) + 1;
	const size_t time_size = time ? cstrlen(time) + 1 : NULL;

	
	if (g_error_file_cache.current_size >= (0x7FFFFFFF - string_size) ||
		g_error_file_cache.current_size + string_size >= 0x7FFFFFFF - time_size)
	{
		DISPLAY_ASSERT("arithmetic overflow error");
	}

	if (write_immediate
		|| !error_globals.idle_logging_and_caching
		|| g_error_file_cache.entry_stack.full()
		|| string_size + time_size + g_error_file_cache.current_size > NUMBEROF(g_error_file_cache.buffer))
	{
		error_write_to_file();
	}

	ASSERT(!g_error_file_cache.entry_stack.full());

	if (write_immediate
		|| !error_globals.idle_logging_and_caching
		|| g_error_file_cache.entry_stack.full()
		|| string_size + time_size + g_error_file_cache.current_size > NUMBEROF(g_error_file_cache.buffer))
	{
		ASSERT(g_error_file_cache.entry_stack.empty());
		error_write_string_to_file(NULL, category, string, time, write_immediate);
	}
	// Update entry and error state
	else
	{
		const int32 current_size = g_error_file_cache.current_size;
		g_error_file_cache.entry_stack.push();
		
		s_error_entry* entry = g_error_file_cache.entry_stack.get_top();
		entry->category = category;
		g_error_file_cache.entry_mask.set(category, true);
		
		csstrncpy(&g_error_file_cache.buffer[current_size], string, string_size);
		entry->string = &g_error_file_cache.buffer[current_size];

		// Update size
		const size_t new_size = string_size + current_size;
		if (time_size > 0)
		{
			csstrncpy(&g_error_file_cache.buffer[new_size], time, time_size);
			entry->time = &g_error_file_cache.buffer[new_size];
		}

		g_error_file_cache.current_size += time_size + string_size;
		error_globals.last_time = system_milliseconds();
	}

	return;
}

static void error_write_string_to_file(
	c_static_array<c_error_file, k_error_category_count>* files,
	e_error_category category,
	const char* string,
	const char* time,
	bool write_immediate)
{
	if (!category || !error_globals.prevent_write_to_primary_log[category])
	{
		error_write_string_to_file_internal(files, _error_category_generic, string, time, write_immediate);
		error_write_string_to_file_internal(files, _error_category_internal_full, string, time, write_immediate);
	}

	if (category)
	{
		error_write_string_to_file_internal(files, category, string, time, write_immediate);
	}
	else if (error_globals.error_subdirectory[0])
	{
		error_write_string_to_file_internal(files, _error_category_internal_subfolder, string, time, write_immediate);
	}

	return;
}

static void error_write_string_to_file_internal(
	c_static_array<c_error_file, k_error_category_count>* files,
	e_error_category category,
	const char* string,
	const char* time,
	bool write_immediate)
{
	c_critical_section_scope scope(error_globals.system_mutex);
	c_error_file* file = (files != NULL ? &files->operator[](category) : NULL);

	// If we haven't written to this category yet we write out the header first
	if (g_error_category_written_to_state[category] == false)
	{
		g_error_category_written_to_state[category] = true;
		write_string_to_error_file(file, category, "\r\n\r\n", NULL, write_immediate);
		write_string_to_error_file(
			file,
			category,
			"============================================================================================\r\n",
			NULL,
			write_immediate
		);
		write_string_to_error_file(file, category, shell_get_version(), time, write_immediate);
		write_string_to_error_file(file, category, "\r\n", NULL, write_immediate);
		write_string_to_error_file(
			file,
			category,
			"============================================================================================\r\n",
			NULL,
			write_immediate
		);
		write_string_to_error_file(file, category, "\r\n\r\n", NULL, write_immediate);
	}

	// Write the actual error now
	write_string_to_error_file(file, category, string, time, write_immediate);

	return;
}

static void write_string_to_error_file(
	c_error_file* file,
	e_error_category category,
	const char* output_string,
	const char* time,
	bool write_immediate)
{
	if (error_globals.output_to_debug_file)
	{
		char string[1024];

		if (time)
		{
			csstrncpy(string, time, NUMBEROF(string));
			csstrncat(string, "  ", NUMBEROF(string));
			csstrncat(string, output_string, NUMBEROF(string));
			output_string = string;
		}
		ASSERT(output_string);

		if (file)
		{
			const size_t length = cstrlen(output_string);
			file->append_to_buffer(length, output_string);
		}
		else
		{
			s_file_reference* reference = error_category_file_entry_get(category);
			const size_t length = cstrlen(output_string);
			file_write(reference, length, output_string);
			error_category_file_entry_close(category, write_immediate);
		}
	}

	return;
}

static void error_write_to_file(void)
{
	c_critical_section_scope scope(error_globals.system_mutex);

	uint32 category_mask = 0;
	for (int32 i = 0; i < k_error_category_count; ++i)
	{
		if (!g_error_file_cache.entry_mask.test(i))
		{
			int32 new_i;
			if (i >= _error_category_generic)
			{
				new_i = i <= _error_category_internal_subfolder ? i : _error_category_internal_subfolder;
			}
			else
			{
				new_i = _error_category_generic;
			}

			if (new_i != i)
			{
				continue;
			}
		}

		if (i != _error_category_internal_subfolder || error_globals.error_subdirectory[0])
		{
			s_file_reference* reference = error_category_file_entry_get((e_error_category)i);
			c_error_file* file = &g_error_file_cache.file_entries[i];

			file->set_file_reference(reference);

			SET_BIT(category_mask, i, true);
		}
	}

	const int32 count = g_error_file_cache.entry_stack.get_count();

	for (int32 i = 0; i < count; ++i)
	{
		const s_error_entry entry = g_error_file_cache.entry_stack[i];
		error_write_string_to_file(
			&g_error_file_cache.file_entries,
			entry.category,
			entry.string,
			entry.time,
			false
		);
	}

	for (int32 i = 0; i < k_error_category_count; ++i)
	{
		if (TEST_BIT(category_mask, i))
		{
			c_error_file file = g_error_file_cache.file_entries[i];
			file.flush();
		}
	}

	error_globals.last_time = system_milliseconds();
	g_error_file_cache.entry_stack.clear();
	g_error_file_cache.current_size = 0;
	g_error_file_cache.entry_mask.clear();

	return;
}

static s_file_reference* error_category_file_entry_get(e_error_category category)
{
	s_file_reference* result = &g_error_category_file_entries[category];

	const bool not_initialized = g_error_category_file_is_initialized[category] == false;

	if (!g_error_category_file_currently_open[category])
	{
		if (category > _error_category_generic)
		{
			if (category == _error_category_internal_full)
			{
				file_reference_create_from_path(result, k_primary_full_event_log_filename, false);
			}
			else
			{
				create_report_file_reference(result, k_category_constants[category].file_name);
			}
		}
		else
		{
			file_reference_create_from_path(result, k_primary_event_log_filename, false);
		}

		if (!file_exists(result))
		{
			file_create_parent_directories_if_not_present(result);
		}

		if (category != _error_category_internal_full && not_initialized || !file_exists(result))
		{
			file_create(result);
		}

		if (not_initialized)
		{
			file_trim(result, k_error_log_max_filesize);
		}

		e_file_open_flags flags = (e_file_open_flags)(_permission_write_allow_read_bit | _permission_write_append_bit | _permission_write_bit);
		e_file_open_error error_code;
		
		if (file_open(result, flags, &error_code))
		{
			g_error_category_file_currently_open[category] = true;
			g_error_category_file_is_initialized[category] = true;
		}

	}

	return result;
}

static void error_category_file_entry_close(e_error_category category, bool write_immediate)
{
	if (write_immediate || error_globals.suppress_all)
	{
		if (g_error_category_file_currently_open[category])
		{
			s_file_reference* reference = &g_error_category_file_entries[category];
			
			file_close(reference);
			g_error_category_file_currently_open[category] = false;
		}
	}

	return;
}

#endif
