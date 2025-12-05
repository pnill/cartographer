#include "stdafx.h"
#include "network_event.h"

#ifdef EVENTS_ENABLED
#include "network_globals.h"
#include "network_time.h"

#include "game/game.h"
#include "game/game_time.h"
#include "main/console.h"
#include "memory/data.h"
#include "memory/rockall_heap_manager.h"
#include "render/render.h"

/* structures */

struct s_network_event_globals
{
	bool initialized;
	bool net_event;
	int8 pad[2];
	e_event_level current_display_level;
	e_event_level current_log_level;
	e_event_level current_remote_log_level;
	e_event_level current_minimum_level;
	e_event_level current_minimum_category_level;
	data_array* categories;
};

struct s_network_event_datum
{
	int8 unused[2];
	int16 depth;
	char category_name[64];
	e_event_level current_display_level;
	e_event_level current_log_level;
	int32 parent_index;
	datum current_index;
	datum next_index;
};

/* constants */

enum
{
	k_network_event_category_maximum_depth = 8,
	k_network_event_count = 1024,
	k_event_category_string_length = 64,
};

const char* k_event_level_names[k_network_event_level_count]
{
	"verbose",
	"status",
	"message",
	"warning",
	"error",
	"critical"
};

const char* k_event_level_severity_strings[k_network_event_level_count] =
{
	"verbose",
	"status ",
	"message",
	"WARNING",
	"-ERROR-",
	"-FATAL-"
};

static bool g_network_event_print_critical_errors = false;

/* prototypes */

static void network_event_adjust_current_minimum_level(void);

static uint32 network_event_get_flags(e_event_level event_level, datum index);

static void network_event_dump_categories_recursive(datum category_index);

static void network_event_print(const char* format, e_event_level event_level, int32 category, char* ap);

static int32 network_event_parse_and_get_category_count(const char* event_name, int32 category_count, int32 string_buffer_size, char (*category_names)[k_event_category_string_length]);

static datum network_event_find_category_recursive(datum index, int8 level, int32 category_count, char (*category_names)[k_event_category_string_length]);

static datum network_event_find_category(const char* event_name, int32 level);

static void network_event_log_cateory_recursive(datum network_category_index, e_event_level display_level, e_event_level log_level, char continue_iterating);

static void network_event_format_category_string(datum category_index, char* buffer, int32 buffer_length);

static void network_event_iterate_categories(void);

/* globals */

static s_network_event_globals network_event_globals =
{
	false,			// initialized
	true,			// net_event
	{0, 0},			// pad
	_event_warning,	// current_display_level
	_event_message,	// current_log_level
	_event_message,	// current_remote_log_level
	_event_invalid,	// current_minimum_level
	_event_invalid,	// current_minimum_category_level
	NULL			// categories
};

/* public code */

bool c_event::query(e_event_level event_level)
{
	m_event_level = event_level;
	return event_level >= network_event_globals.current_minimum_level;
}

void c_event::generate(const char* event_name, ...)
{
	va_list args;
	va_start(args, event_name);

	if (network_event_globals.initialized && !m_category_set)
	{
		m_category = network_event_find_category(event_name, 1);
		m_category_set = true;
	}
	network_event_print(event_name, m_event_level, m_category, args);
	return;
}


void network_event_initialize(void)
{
	// TODO: error stuff
	// sub_423170(_error_category_networking, 0);

	network_event_globals.categories = DATA_NEW("network events", k_network_event_count, sizeof(s_network_event_datum), 0, normal_allocation_global_get());
	ASSERT(network_event_globals.categories);

	data_make_valid(network_event_globals.categories);
	const datum category_index = datum_new_at_absolute_index(network_event_globals.categories, 0);
	
	s_network_event_datum* category = (s_network_event_datum*)datum_get(network_event_globals.categories, category_index);
	csstrncpy(category->category_name, "", NUMBEROF(category->category_name));
	category->current_display_level = _event_invalid;
	category->current_log_level = _event_invalid;
	category->depth = 0;
	category->parent_index = NONE;
	category->current_index = NONE;
	category->next_index = NONE;

	network_event_globals.initialized = true;

	event(_event_message, "lifecycle: STARTUP");
	network_event_iterate_categories();
	network_event_adjust_current_minimum_level();
	return;
}

void network_event_dispose(void)
{
	if (network_event_globals.categories)
	{
		data_make_invalid(network_event_globals.categories);
		DATA_DISPOSE(network_event_globals.categories);
		network_event_globals.categories = NULL;
	}
	network_event_globals.initialized = false;
	return;
}

void network_event_dump_categories(const char* event_name)
{
	const datum category_index = network_event_find_category(event_name, 0);
	ASSERT(event_name);

	if (category_index == NONE)
	{
		error(_error_silent, "### ERROR category '%s' not found", event_name);
	}
	else
	{
		error(_error_silent, "dumping categories:");
		network_event_dump_categories_recursive(category_index);
	}
	return;
}

void network_event_display_category(const char* category_string, e_event_level level)
{
	const datum category_index = network_event_find_category(category_string, 1);
	if (category_index == NONE || level > _event_fatal)
	{
		error(_error_silent, "### ERROR invalid network event category '%s'", category_string);
	}
	else
	{
		network_event_log_cateory_recursive(category_index, level, _event_invalid, false);
		network_event_iterate_categories();
		network_event_adjust_current_minimum_level();
	}
	return;
}

void network_event_log_category(const char* category_string, e_event_level level)
{
	const datum category_index = network_event_find_category(category_string, 1);
	if (category_index == NONE || level > _event_fatal)
	{
		error(_error_silent, "### ERROR invalid network event category '%s'", category_string);
	}
	else
	{
		network_event_log_cateory_recursive(category_index, _event_invalid, level, false);
		network_event_iterate_categories();
		network_event_adjust_current_minimum_level();
	}
	return;
}

/* private code */

static void network_event_adjust_current_minimum_level(void)
{
	ASSERT(VALID_INDEX(network_event_globals.current_minimum_category_level, k_network_event_level_count));

	network_event_globals.current_minimum_level = network_event_globals.current_minimum_category_level;

	const bool current_min_greater_than_current_display = network_event_globals.current_minimum_level > network_event_globals.current_display_level;
	network_event_globals.current_minimum_level = (current_min_greater_than_current_display ? network_event_globals.current_display_level : network_event_globals.current_minimum_level);

	const bool current_min_greater_than_current_log = network_event_globals.current_minimum_level > network_event_globals.current_log_level;
	network_event_globals.current_minimum_level = (current_min_greater_than_current_log ? network_event_globals.current_log_level : network_event_globals.current_minimum_level);

	const bool current_min_greater_than_current_remote_log = network_event_globals.current_minimum_level > network_event_globals.current_remote_log_level;
	network_event_globals.current_minimum_level = (current_min_greater_than_current_remote_log ? network_event_globals.current_remote_log_level : network_event_globals.current_minimum_level);
	return;
}

static uint32 network_event_get_flags(e_event_level event_level, datum index)
{
	uint32 result = 0;

	ASSERT(VALID_INDEX(event_level, k_network_event_level_count));

	if (network_event_globals.initialized && network_event_globals.net_event)
	{
		uint32 v3 = event_level < network_event_globals.current_log_level ? event_level >= network_event_globals.current_display_level : (uint32)(event_level >= network_event_globals.current_display_level) | 2;
		uint32 v4 = event_level < network_event_globals.current_remote_log_level ? v3 & ~4 : v3 | 4;
		result = event_level < _event_fatal ? v4 & ~8u : v4 | 8;
		
		if (index != NONE)
		{
			s_network_event_datum* category = (s_network_event_datum*)datum_get(network_event_globals.categories, index);
			if (category->current_display_level != _event_invalid)
			{
				SET_BIT(result, 0, event_level >= category->current_display_level);
			}

			if (category->current_log_level != _event_invalid)
			{
				SET_BIT(result, 1, event_level >= category->current_log_level);
			}
		}
	}
	return result;
}

static void network_event_dump_categories_recursive(datum category_index)
{
	s_network_event_datum* category;
	for (datum current_category_index = category_index; current_category_index != NONE; current_category_index = category->next_index)
	{
		category = (s_network_event_datum*)datum_get(network_event_globals.categories, current_category_index);
		
		char category_string[1024];
		network_event_format_category_string(current_category_index, category_string, NUMBEROF(category_string));
		console_printf("  '%s'", category_string);
		network_event_dump_categories_recursive(category->current_index);
	}
	return;
}

static void network_event_print(const char* format, e_event_level event_level, int32 category, char* ap)
{
	const uint32 flags = network_event_get_flags(event_level, category);
	if (flags)
	{
		const char* severity = k_event_level_severity_strings[event_level];
		ASSERT(format);

		char event_text[2048];
		vsprintf(event_text, NUMBEROF(event_text), format, ap);
		
		char timestamp[256];
		char dst[256];
		dst[0] = '\0';

		for (size_t i = 0; i < 2; ++i)
		{
			// Add one space of padding when the index isn't 0
			const char* padding_string = (i != 1 ? " " : "");

			uint32 time = 0;
			s_transport_secure_identifier id;
			if (network_session_time_get_id_and_time(i, &id, &time))
			{
				const int32 minutes = time / 1000 / 60;
				const int32 hour = minutes / 60;
				const int32 minute = minutes % 60;

				csprintf(timestamp, NUMBEROF(timestamp), "%02d:%02d:%02d.%03d%s", hour, minute, time / 1000 % 60, time % 1000, padding_string);
			}
			else
			{
				csprintf(timestamp, NUMBEROF(timestamp), "            %s", padding_string);
			}
			csstrncat(dst, timestamp, NUMBEROF(dst));
		}

		if (game_in_progress())
		{
			csprintf(timestamp, NUMBEROF(timestamp), "g%06d f%07d", game_time_get(), global_frame_index_get());
		}
		else
		{
			csprintf(timestamp, NUMBEROF(timestamp), "        f%07d", global_frame_index_get());
		}
		csstrncat(dst, timestamp, NUMBEROF(dst));

		if (TEST_BIT(flags, 0))
		{
			if (event_level < _event_warning)
			{
				console_printf("%s", event_text);
			}
			else
			{
				console_warning("%s", event_text);
			}
		}

		if (TEST_BIT(flags, 1))
		{
			error(_error_category_networking, _error_delayed, "NET %s %s %s", severity, dst, event_text);
		}

		if (TEST_BIT(flags, 2))
		{
			// TODO: implement
			//data_mine_add_event(event_level, event_text);
		}

		if (TEST_BIT(flags, 3) && !g_network_event_print_critical_errors)
		{
			char network_event_error[2048];
			csprintf(network_event_error, NUMBEROF(network_event_error), "critical network event encountered: %s", event_text);
			
			ASSERT_EXCEPTION(network_event_error, network_globals_get()->halt_on_critical_events);
		}
	}
	return;
}

static int32 network_event_parse_and_get_category_count(const char* event_name, int32 max_category_count, int32 string_buffer_size, char (*category_names)[k_event_category_string_length])
{
	int32 category_index = 0;
	const char* category_substring = event_name;

	bool failed = false;
	bool category_found = false;
	do
	{
		const char* category_name = category_substring;
		int32 category_name_length = 0;
		do
		{
			if (category_name_length < string_buffer_size)
			{
				const char character = *category_substring;
				if (isalnum(character) || character == '-' || character == '_')
				{
					++category_substring;
					++category_name_length;
				}
				else
				{
					category_found = true;
					if (character == ':')
					{
						++category_substring;
					}
				}
			}
			else
			{
				error(_error_silent, "### ERROR network event category substring #%d '%s' exceeded %d characters", category_index, category_substring, string_buffer_size);
				failed = true;
			}
		}
		while (!failed && !category_found);

		if (category_found)
		{
			if (category_name_length)
			{
				if (category_index < max_category_count)
				{
					csstrncpy(category_names[category_index], category_name, string_buffer_size);
					category_names[category_index][category_name_length] = '\0';
					csstrnlwr(category_names[category_index], category_name_length);
					++category_index;
				}
				else
				{
					error(_error_silent, "### ERROR network event category #%d '%s' exceeded %d category substrings", category_index, category_substring, max_category_count);
					failed = true;
				}
			}
		}
	}
	while (!failed && !category_found);

	vassert(!failed, "failed to parse network event '%s'", event_name);
	return category_index;
}

static datum network_event_find_category_recursive(datum category_index, int8 level, int32 category_count, char (*category_names)[k_event_category_string_length])
{
	s_network_event_datum* category = (s_network_event_datum*)datum_get(network_event_globals.categories, category_index);
	ASSERT(category_count > 0);
	ASSERT(csstrnlen(category_names[0], NUMBEROF(category_names[0])) > 0);

	datum result = NONE;

	s_network_event_datum* current_category;
	for (datum current_index = category->current_index; current_index != NONE; current_index = current_category->next_index)
	{
		current_category = (s_network_event_datum*)datum_get(network_event_globals.categories, current_index);
		if (!csstrncmp(current_category->category_name, category_names[0], NUMBEROF(category_names[0])))
		{
			result = current_index;
			break;
		}
	}

	if (result == NONE && level > 0)
	{
		const datum new_category_index = datum_new(network_event_globals.categories);
		result = new_category_index;
		if (new_category_index == NONE)
		{
			error(_error_silent, "### ERROR %s: ran out of categories creating '%s'", __FUNCTION__, category_names[0]);
		}
		else
		{
			s_network_event_datum* new_category = (s_network_event_datum*)datum_get(network_event_globals.categories, new_category_index);

			csstrncpy(new_category->category_name, category_names[0], NUMBEROF(category_names[0]));
			new_category->current_display_level = category->current_display_level;
			new_category->current_log_level = category->current_log_level;
			new_category->depth = category->depth + 1;
			new_category->parent_index = category_index;
			new_category->current_index = NONE;
			new_category->next_index = category->current_index;
			category->current_index = new_category_index;
		}
	}

	if (result != NONE && category_count > 1)
	{
		result = network_event_find_category_recursive(result, level, category_count - 1, &category_names[1]);
	}
	return result;
}

static datum network_event_find_category(const char* event_name, int32 level)
{
	ASSERT(event_name);
	ASSERT(network_event_globals.initialized);
	char category_description[k_network_event_category_maximum_depth][k_event_category_string_length];
	
	int32 category_count = network_event_parse_and_get_category_count(event_name, k_network_event_category_maximum_depth, k_event_category_string_length, category_description);
	datum result = datum_absolute_index_to_index(network_event_globals.categories, 0);
	if (category_count > 0)
	{
		result = network_event_find_category_recursive(result, (int8)level, category_count, category_description);
	}
	return result;
}

static void network_event_log_cateory_recursive(datum network_category_index, e_event_level display_level, e_event_level log_level, char continue_iterating)
{
	s_network_event_datum* category;
	for (datum current_category_index = network_category_index; current_category_index != NONE; current_category_index = category->next_index)
	{
		category = (s_network_event_datum*)datum_get(network_event_globals.categories, current_category_index);
		if (display_level != _event_invalid)
		{
			ASSERT(VALID_INDEX(display_level, k_network_event_level_count));
			category->current_display_level = display_level;
		}

		if (log_level != _event_invalid)
		{
			ASSERT(VALID_INDEX(log_level, k_network_event_level_count));
			category->current_log_level = log_level;
		}

		char category_name[1024];
		network_event_format_category_string(current_category_index, category_name, NUMBEROF(category_name));
		console_printf("set category: '%s'", category_name);
		network_event_log_cateory_recursive(category->current_index, display_level, log_level, 1);
		if (!continue_iterating)
		{
			break;
		}
	}
	return;
}

static void network_event_format_category_string(datum category_index, char* buffer, int32 buffer_length)
{
	ASSERT(buffer_length > 0);
	ASSERT(buffer);

	datum category_datum_indices[8];

	int32 category_count = 0;
	if (category_index != NONE)
	{
		s_network_event_datum* category;
		bool category_index_is_none = false;
		while (1)
		{
			category = (s_network_event_datum*)datum_get(network_event_globals.categories, category_index);
			if (!category->depth)
			{
				break;
			}

			ASSERT(category_count < k_network_event_category_maximum_depth);
			
			category_datum_indices[category_count] = category_index;
			category_index = category->parent_index;

			++category_count;
			if (category_index == NONE)
			{
				category_index_is_none = true;
				break;
			}
		}

		if (!category_index_is_none)
		{
			ASSERT(category->parent_index == NONE);
		}
	}

	csstrncpy(buffer, "", buffer_length);
	for (int32 i = category_count - 1; i >= 0; --i)
	{
		s_network_event_datum* category = (s_network_event_datum*)datum_get(network_event_globals.categories, category_datum_indices[i]);
		csstrncat(buffer, category->category_name, buffer_length);
		if (i > 0)
		{
			csstrncat(buffer, ":", buffer_length);
		}
	}

	return;
}

static void network_event_iterate_categories(void)
{
	network_event_globals.current_minimum_category_level = _event_fatal;
	
	data_iterator iterator;
	iterator_new(&iterator, network_event_globals.categories);
	s_network_event_datum* category = (s_network_event_datum*)iterator_next(&iterator);
	
	while (category)
	{
		if (category->current_display_level != _event_invalid)
		{
			ASSERT(VALID_INDEX(category->current_display_level, k_network_event_level_count));
			if (network_event_globals.current_minimum_category_level > category->current_display_level)
			{
				network_event_globals.current_minimum_category_level = category->current_display_level;
			}
		}

		if (category->current_log_level != _event_invalid)
		{
			ASSERT(VALID_INDEX(category->current_log_level, k_network_event_level_count));
			if (network_event_globals.current_minimum_category_level > category->current_log_level)
			{
				network_event_globals.current_minimum_category_level = category->current_log_level;
			}
		}

		category = (s_network_event_datum*)iterator_next(&iterator);
	}
	return;
}
#endif
