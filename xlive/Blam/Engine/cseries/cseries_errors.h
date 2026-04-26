#pragma once

#ifdef ERRORS_ENABLED

/* enums */

enum e_error_priority
{
	_error_log= 0,
	_error_silent,
	_error_delayed,
	_error_immediate,
	k_error_priority_count,
};

enum e_error_category
{
	_error_category_generic= 0,
	_error_category_internal_full,
	_error_category_internal_subfolder,
	_error_category_animation,
	_error_category_ai,
	_error_category_shaders,
	_error_category_geometry,
	_error_category_environment,
	_error_category_objects,
	_error_category_networking,
	_error_category_tags,
	_error_category_ui,
	_error_category_sound,
	_error_category_multiplayer,
	_error_category_effects,
	_error_category_animation_audio_content,
	_error_category_environment_materials,
	_error_category_object_materials,
	_error_category_design,
	_error_category_localization,
	k_error_category_count
};

/* prototypes */

void errors_output_to_debug_file(bool output_to_debug_file);

// Set subdirectory where logs are dumped to within the current report folder
void errors_set_log_subdirectory(char const* subdirectory);

struct s_file_reference* create_report_file_reference(struct s_file_reference* reference, const char* name);

void errors_initialize(void);

void errors_dispose(void);

bool errors_add_fatal_error_callback(void (*proc)(void));

void call_fatal_error_callbacks(void);

void error(e_error_priority priority, const char* format, ...);

void error(e_error_category category, e_error_priority priority, const char* format, ...);

void error_category_enable(const char* category_string, bool disable);

bool error_category_enabled(e_error_category category);

union real_rgb_color* error_category_color(union real_rgb_color* color, e_error_category category);

void error_category_disable(e_error_category category, bool disable);

void error_category_write_to_primary_log(e_error_category category, bool enable);

void error_va(e_error_category category, e_error_priority priority, const char* format, char* ap);

void error_enable_suppression(void);

void error_disable_suppression(void);

void errors_terminate_logging(void);

void errors_clear(void);

void errors_terminate_files(void);

void error_output_to_debug_file_enable(bool enable);

void errors_overflow_suppression_enable(bool enable);

const char* error_get(void);

const wchar_t* error_get_wide(void);

void error_message_write_and_flush(void);

void error_idle(void);

void write_to_error_file(e_error_category category, e_error_priority priority, const char* string, bool append_time);

#else

// If errors are disabled then we just define an error macro that does nothing so we don't have to bring in error functions and logic
#define error(...)	(void)(0)

#endif

/* public code */

#ifdef ERRORS_ENABLED

inline e_error_category operator++(e_error_category& type)
{
	return (e_error_category)++((int32&)type);
};

#endif
