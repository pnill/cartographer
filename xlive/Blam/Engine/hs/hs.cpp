#include "stdafx.h"
#include "hs.h"

#ifdef HS_COMPILER_ENABLED
#include "hs_compile.h"
#endif
#include "hs_function.h"
#include "hs_globals_external.h"
#include "hs_runtime.h"
#include "object_lists.h"

#ifdef HS_COMPILER_ENABLED
#include "ai/actor_types.h"
#include "ai/ai_script.h"
#include "ai/behavior.h"
#include "ai/cl_engine.h"
#include "ai/ai_mission_dialogue.h"
#include "ai/ai_scenario_definitions.h"
#include "ai/ai_orders.h"
#include "cache/cache_files.h"
#include "cseries/sort.h"
#include "cutscene/cinematics_definitions.h"
#include "cutscene/recorded_animation_definitions.h"
#endif

#include "game/game.h"
#include "game/game_globals.h"

#include "interface/hud_definitions.h"
#include "interface/hud_messaging.h"
#include "interface/interface.h"
#include "interface/terminal.h"
#include "main/console.h"
#include "math/random_math.h"
#include "memory/data.h"

#ifdef HS_COMPILER_ENABLED
#include "models/model_definitions.h"
#include "networking/network_event.h"
#endif

#include "objects/object_types.h"
#include "scenario/scenario.h"
#include "scenario/scenario_definitions.h"

#include "scenario/scenario_kill_trigger_volumes.h"

/* constants */

#ifdef ERRORS_ENABLED
const char *hs_type_names[NUMBER_OF_HS_NODE_TYPES] =
{
	"unparsed",
	"special form",
	"function name",
	"passthrough",
	"void",
	"boolean",
	"real",
	"short",
	"long",
	"string",
	"script",
	"string_id",
	"unit_seat_mapping",
	"trigger_volume",
	"cutscene_flag",
	"cutscene_camera_point",
	"cutscene_title",
	"cutscene_recording",
	"device_group",
	"ai",
	"ai_command_list",
	"ai_command_script",
	"ai_behavior",
	"ai_orders",
	"starting_profile",
	"conversation",
	"structure_bsp",
	"navpoint",
	"point reference",
	"style",
	"hud_message",
	"object_list",
	"sound",
	"effect",
	"damage",
	"looping_sound",
	"animation_graph",
	"damage_effect",
	"object_definition",
	"bitmap",
	"shader",
	"render model",
	"structure definition",
	"lightmap definition",
	"game_difficulty",
	"team",
	"actor_type",
	"hud_corner",
	"model_state",
	"network_event",
	"object",
	"unit",
	"vehicle",
	"weapon",
	"device",
	"scenery",
	"object_name",
	"unit_name",
	"vehicle_name",
	"weapon_name",
	"device_name",
	"scenery_name"
};

const char* hs_script_type_names[NUMBER_OF_HS_SCRIPT_TYPES] =
{
	"startup",
	"dormant",
	"continuous",
	"static",
	"stub",
	"command_script"
};
#endif

const char* const hs_type_string_default = "";

const int16 hs_object_type_masks[NUMBER_OF_HS_OBJECT_TYPES] =
{
	_object_mask_all,		// objects
	_object_mask_unit,		// unit
	_object_mask_vehicle,	// vehicle
	_object_mask_weapon,	// weapon
	_object_mask_device,	// device
	_object_mask_scenery	// scenery
};

// This is unused in h2?
const tag_group hs_tag_reference_type_group_tags[] =
{
	_tag_group_sound,
	_tag_group_effect,
	_tag_group_damage_effect,
	_tag_group_sound_looping,
	_tag_group_model_animation_graph,
	_tag_group_damage_effect,
	_tag_group_object,
	_tag_group_bitmap,
	_tag_group_shader,
	_tag_group_render_model,
	_tag_group_scenario_structure_bsp,
	_tag_group_scenario_structure_lightmap
};

const int16 hs_type_sizes[NUMBER_OF_HS_NODE_TYPES] =
{
	0,
	0,
	0,
	0,
	0,
	1,
	4,
	2,
	4,
	4,
	2,
	4,
	4,
	2,
	2,
	2,
	2,
	2,
	4,
	4,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	4,
	4,
	2,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	4,
	2,
	2,
	2,
	2,
	2,
	2,
	4,
	4,
	4,
	4,
	4,
	4,
	2,
	2,
	2,
	2,
	2,
	2
};

const bool hs_type_boolean_default = false;
const real32 hs_type_real_default = 0.f;
const int16 hs_type_short_integer_default = 0;
const int32 hs_type_long_integer_default = 0;
const int16 hs_type_script_default = NONE;
const string_id hs_type_string_id_default = _string_id_empty_string;
const int32 hs_type_unit_seat_mapping_default = NONE;
const int16 hs_type_trigger_volume_default = NONE;
const int16 hs_type_cutscene_flag_default = NONE;
const int16 hs_type_cutscene_camera_point_default = NONE;
const int16 hs_type_cutscene_title_default = NONE;
const int16 hs_type_cutscene_recording_default = NONE;
const int32 hs_type_device_group_default = NONE;
const int32 hs_type_ai_default = NONE;
const int16 hs_type_ai_command_list_default = NONE;
const int16 hs_type_ai_command_script_default = NONE;
const int16 hs_type_ai_behavior_default = NONE;
const int16 hs_type_ai_orders_default = NONE;
const int16 hs_type_starting_profile_default = NONE;
const int16 hs_type_conversation_default = NONE;
const int16 hs_type_structure_bsp_default = NONE;
const int16 hs_type_navpoint_default = NONE;
const int32 hs_type_style_default = NONE;
const int16 hs_type_hud_message_default = NONE;
const int32 hs_type_object_list_default = NONE;
const int32 hs_type_sound_default = NONE;
const int32 hs_type_looping_sound_default = NONE;
const int32 hs_type_effect_default = NONE;
const int32 hs_type_damage_default = NONE;
const int32 hs_type_animation_graph_default = NONE;
const int32 hs_type_damage_effect_default = NONE;
const int32 hs_type_object_definition_default = NONE;
const int32 hs_type_bitmap_default = NONE;
const int32 hs_type_shader_default = NONE;
const int32 hs_type_render_model_definition_default = NONE;
const int32 hs_type_structure_bsp_definition_default = NONE;
const int32 hs_type_structure_lightmap_definition_default = NONE;
const int16 hs_type_enum_game_difficulty_default = NONE;
const int16 hs_type_enum_team_default = NONE;
const int16 hs_type_enum_actor_type_default = NONE;
const int16 hs_type_enum_hud_corner_default = NONE;
const int16 hs_type_enum_model_state_default = NONE;
const int16 hs_type_enum_network_event_default = NONE;
const int16 hs_type_object_name_default = NONE;
const int32 hs_type_object_default = NONE;
const int32 hs_type_unit_default = NONE;
const int32 hs_type_vehicle_default = NONE;
const int32 hs_type_weapon_default = NONE;
const int32 hs_type_device_default = NONE;
const int32 hs_type_scenery_default = NONE;

#ifdef HS_COMPILER_ENABLED
const hs_enum_definition hs_enum_table[NUMBER_OF_HS_ENUM_TYPES] =
{
	{ NUMBEROF(global_campaign_difficulty_level_names), global_campaign_difficulty_level_names },
	{ NUMBEROF(global_campaign_team_names), global_campaign_team_names },
	{ NUMBEROF(global_actor_type_names), global_actor_type_names },
	{ NUMBEROF(global_hud_anchor_names), global_hud_anchor_names },
	{ NUMBEROF(global_model_state_names), global_model_state_names },
	{ NUMBEROF(k_event_level_names), k_event_level_names }
};
#endif

/* prototypes */

static void __cdecl hs_print(const char* text);

#ifdef HS_COMPILER_ENABLED

static bool sort_by_found_index(const void* s1, const void* s2, const void* compare_data);

static void hs_tokens_enumerate_add_string(const char* string);

static void hs_enumerate_block_data(const s_tag_block* block, int16 element_offset, int32 size);

static void hs_enumerate_scenario_data(int16 block_offset, int16 element_offset, int32 size);

static void hs_enumerate_special_form_names(void);

static void hs_enumerate_script_type_names(void);

static void hs_enumerate_type_names(void);

static void hs_enumerate_function_names(void);

static void hs_enumerate_script_names(void);

static void hs_enumerate_variable_names(void);

static void hs_enumerate_ai_names(void);

static void hs_enumerate_ai_command_list_names(void);

static void hs_enumerate_ai_command_script_names(void);

static void hs_enumerate_ai_behavior_names(void);

static void hs_enumerate_starting_profile_names(void);

static void hs_enumerate_conversation_names(void);

static void hs_enumerate_object_name(void);

static void hs_enumerate_trigger_volume_names(void);

static void hs_enumerate_cutscene_flag_names(void);

static void hs_enumerate_cutscene_camera_point_names(void);

static void hs_enumerate_cutscene_title_names(void);

static void hs_enumerate_cutscene_recording_names(void);

static void hs_enumerate_navpoints(void);

static void hs_enumerate_hud_messages(void);

// Construct a string with the function parameters for the function passed
static void hs_get_function_parameters_string(int16 function_index, char* buffer, int32 buffer_size);

#endif

/* globals */

#ifdef HS_COMPILER_ENABLED

// Array of functions that enumerates through all instances of a specific type and adds autocomplete results to the terminal
static void(*hs_token_enumerators[])(void) =
{
	&hs_enumerate_special_form_names,
	&hs_enumerate_script_type_names,
	&hs_enumerate_type_names,
	&hs_enumerate_function_names,
	&hs_enumerate_script_names,
	&hs_enumerate_variable_names,
	&hs_enumerate_ai_names,
	&hs_enumerate_ai_command_list_names,
	&hs_enumerate_ai_command_script_names,
	&hs_enumerate_ai_behavior_names,
	&hs_enumerate_starting_profile_names,
	&hs_enumerate_conversation_names,
	&hs_enumerate_object_name,
	&hs_enumerate_trigger_volume_names,
	&hs_enumerate_cutscene_flag_names,
	&hs_enumerate_cutscene_camera_point_names,
	&hs_enumerate_cutscene_title_names,
	&hs_enumerate_cutscene_recording_names,
	&hs_enumerate_navpoints,
	&hs_enumerate_hud_messages
};

static bool g_recompile = false;

static const char* enumeration_substring = NULL;
static const char** enumeration_results = NULL;
static int16 enumeration_maximum_count = 0;
static int16 enumeration_count = 0;

char* g_error_output_buffer = NULL;
int32 g_error_buffer_length = 0;

#endif

/* public code */

void hs_apply_patches(void)
{
	// hook the print command to redirect the text to our console
	PatchCall(Memory::GetAddress(0xE9E50, 0xE49AC), hs_print);

	WritePointer((uintptr_t)&get_game_systems()[55].initialize_proc, hs_initialize);
	WritePointer((uintptr_t)&get_game_systems()[55].dispose_from_old_map_proc, hs_dispose_from_old_map);
	return;
}

data_array** hs_syntax_data_get(void)
{
	return Memory::GetAddress<data_array**>(0x4CE850, 0x4F503C);
}

void hs_initialize(void)
{
#ifdef HS_COMPILER_ENABLED
	if (!hs_type_names[NUMBER_OF_HS_NODE_TYPES - 1])
	{
		DISPLAY_ASSERT("you can't add an hs type without defining its name.");
	}
#endif

	// TODO: rewrite this and replace all g_hs_syntax_data references in the game with our own
	INVOKE(0xA3971, 0x95BD1, hs_initialize);

	return;
}

void __cdecl hs_initialize_for_new_map(void)
{
	INVOKE(0xA38E2, 0x95B42, hs_initialize_for_new_map);
	return;
}

void hs_dispose_from_old_map(void)
{
	data_array* g_hs_syntax_data = *hs_syntax_data_get();

	hs_node_gc();
	if (!TEST_BIT(g_hs_syntax_data->flags, _data_array_disconnected_bit))
	{
		data_disconnect(g_hs_syntax_data);
	}

	hs_runtime_dispose_from_old_map();
	object_lists_dispose_from_old_map();
	return;
}

void __cdecl hs_update(void)
{
	//INVOKE(0xA37A9, 0x95A09, hs_update);
	hs_runtime_update();
	return;
}

#ifdef HS_COMPILER_ENABLED

bool hs_compile_and_evaluate(const char* expression, bool interactive)
{
	bool result = false;

	random_seed_allow_use();
	
	char string[1024];
	csstrncpy(string, expression, NUMBEROF(string));
	csstrnlwr(string, NUMBEROF(string));
	if (strchr(string, ';'))
	{
		string[0] = '\0';
	}

	bool failed = true;
	for (const char* curr_char = string; curr_char[0]; ++curr_char)
	{
		if (!isspace(curr_char[0]))
		{
			failed = false;
			break;
		}
	}

	if (!failed)
	{
		data_array* g_hs_syntax_data = *hs_syntax_data_get();

		int16 type = 0;
		if (g_hs_syntax_data && g_hs_syntax_data->valid && hs_runtime_safe_to_gc())
		{
			hs_node_gc();
		}

		hs_compile_initialize(false);
		ASSERT(g_hs_syntax_data);

		hs_syntax_node data[128];
		if (TEST_BIT(g_hs_syntax_data->flags, _data_array_disconnected_bit))
		{
			csmemset(data, 0, sizeof(data));
			data_connect(g_hs_syntax_data, NUMBEROF(data), data);
		}

		if (string[0] != '(')
		{
			char* parameters = strchr(string, ' ');
			if (parameters)
			{
				parameters[0] = '\0';
			}

			if (hs_find_global_by_name(string) == NONE)
			{
				type = 1;
			}
			else if (parameters)
			{
				type = 2;
			}

			if (parameters)
			{
				parameters[0] = ' ';
			}
		}

		char source_data[1024];
		switch (type)
		{
		case 0:
			csstrncpy(source_data, string, NUMBEROF(source_data));
			break;
		case 1:
			csprintf(source_data, NUMBEROF(source_data), "(%s)", string);
			break;
		case 2:
			csprintf(source_data, NUMBEROF(source_data), "(set %s)", string);
			break;
		default:
			halt();
		}

		const size_t length = cstrlen(source_data);
		const char* error_message;
		char* error_source;

		const datum expression_index = hs_compile_expression(length, source_data, &error_message, (const char**)&error_source);
		if (expression_index == NONE)
		{
			if (error_message[0] != '\0')
			{
				hs_compile_source_error(NULL, error_message, error_source, source_data);
			}
		}
		else
		{
			result = true;
			hs_runtime_evaluate(expression_index, interactive);
		}

		if (g_hs_syntax_data->data == data)
		{
			data_disconnect(g_hs_syntax_data);
		}
		hs_compile_dispose();
	}

	if (g_recompile)
	{
		hs_rebuild_and_compile(NULL, 0);
		g_recompile = false;
	}

	random_seed_disallow_use();
	return result;
}

void hs_compile_source_error(const char* file_name, const char* error_message, char* error_source, const char* source)
{
	char* string_after_newline = NULL;
	if (error_source)
	{
		string_after_newline = strchr(error_source, '\n');
		if (string_after_newline)
		{
			string_after_newline[0] = '\0';
		}
		else
		{
			string_after_newline = &error_source[cstrlen(error_source)];
		}
	}

	if (file_name && string_after_newline)
	{
		int16 line = 1;
		for (; source < string_after_newline; --string_after_newline)
		{
			if (string_after_newline[0] == '\n')
			{
				++line;
			}
		}

		const char* string = error_source ? error_source : "";
		error(_error_category_design, _error_immediate, "[%s line %d] %s: %s", file_name, line, error_message, string);
		if (g_error_output_buffer)
		{
			csnappendf(
				g_error_output_buffer,
				g_error_buffer_length,
				"[%s line %d] %s: %s\n",
				file_name,
				line,
				error_message,
				string
			);
		}
	}
	else
	{
		const char* string = error_source ? error_source : "";
		error(_error_category_design, _error_immediate, "%s: %s", error_message, string);
		if (g_error_output_buffer)
		{
			csnappendf(
				g_error_output_buffer,
				g_error_buffer_length,
				"%s: %s\n",
				error_message,
				string
			);
		}
	}
	return;
}

void hs_help(const char* function_name)
{
	for (int16 function_index = 0; function_index < hs_function_table_count; ++function_index)
	{
		if (!csstricmp(hs_function_table[function_index]->name, function_name))
		{
			// Print params

			char string[2048];
			hs_get_function_parameters_string(function_index, string, NUMBEROF(string));
			console_printf(string);
			
			// Print documentation

			const hs_function_definition* function = hs_function_get(function_index);
			csstrncpy(string, function->documentation, NUMBEROF(string));
			console_printf(string);
		}
	}
	return;
}

bool hs_rebuild_and_compile(char* error, int32 length)
{
	hs_runtime_dirty();
	g_error_output_buffer = error;
	g_error_buffer_length = length;
	hs_compile_source(false);
	g_error_output_buffer = NULL;
	g_error_buffer_length = 0;
	hs_dispose_from_old_map();
	hs_initialize_for_new_map();
	return ai_handle_script_verification(false);
}
#endif

void hs_node_gc(void)
{
	data_array* g_hs_syntax_data = *hs_syntax_data_get();
	data_array* hs_thread_data = hs_thread_data_get();

	// Delete all threads that are runtime evaluations
	if (hs_runtime_initialized())
	{
		for (
			int32 thread_index = data_next_index(hs_thread_data, NONE);
			thread_index != NONE;
			thread_index = data_next_index(hs_thread_data, thread_index)
		)
		{
			if (hs_thread_get(thread_index)->type == _hs_thread_type_runtime_evaluate)
			{
				hs_thread_delete(thread_index);
			}
		}
	}

	// Delete all nodes/expressions that aren't considered "permanent"
	for (
		int32 expression_index = data_next_index(g_hs_syntax_data, NONE);
		expression_index != NONE;
		expression_index = data_next_index(g_hs_syntax_data, expression_index)
	) 
	{
		if (!TEST_BIT(hs_syntax_get(expression_index)->flags, _hs_syntax_node_permanent_bit))
		{
			datum_delete(g_hs_syntax_data, expression_index);
		}
	}

	// Make sure that none of our current threads are pointing to invalid expressions
	if (hs_runtime_initialized())
	{
		for (
			int32 thread_index = data_next_index(hs_thread_data, NONE);
			thread_index != NONE;
			thread_index = data_next_index(hs_thread_data, thread_index)
		)
		{
			hs_thread* thread = hs_thread_get(thread_index);

			ASSERT(thread->stack);

			if (thread->stack->expression_index != NONE)
			{
				ASSERT(hs_syntax_get(thread->stack->expression_index) != NULL);
			}
		}
	}
	return;
}

const hs_function_definition* hs_function_get(int16 function_index)
{
	ASSERT(VALID_INDEX(function_index, hs_function_table_count));
	return hs_function_table[function_index];
}

int16 hs_find_script_by_name(const char* name)
{
	int16 result = NONE;
	if (global_scenario_index_get() != NONE)
	{
		struct scenario* scenario = global_scenario_get();
		for (int16 i = 0; i < scenario->hs_scripts.count; ++i)
		{
			const hs_script* script = TAG_BLOCK_GET_ELEMENT(&scenario->hs_scripts, i, hs_script);
			if (!csstricmp(name, script->name))
			{
				result = i;
				break;
			}
		}
	}
	return result;
}

const hs_global_external* hs_global_external_get(int16 global_index)
{
	ASSERT(VALID_INDEX(global_index, k_hs_external_global_count));
	return hs_external_globals[global_index];
}

int16 hs_global_get_type(int16 designator)
{
	int16 result;
	if (designator < 0)
	{
		
		result = assert_return(HS_GLOBAL_EXTERNAL_GET(HS_GLOBAL_INDEX(designator)))->type;
	}
	else
	{
		result = (int16)assert_return(HS_GLOBAL_INTERNAL_GET(HS_GLOBAL_INDEX((designator))))->script_type;
	}
	return result;
}

#ifdef HS_COMPILER_ENABLED

const char* hs_global_get_name(int16 designator)
{
	const char* result;
	if (designator < 0)
	{
		result = assert_return(HS_GLOBAL_EXTERNAL_GET(HS_GLOBAL_INDEX(designator)))->name;
	}
	else
	{
		result = assert_return(HS_GLOBAL_INTERNAL_GET(HS_GLOBAL_INDEX((designator))))->name;
	}
	return result;
}


int16 hs_find_global_by_name(const char* name)
{
	int16 result = NONE;
	bool global_found = false;

	for (int16 global_index = 0; global_index < k_hs_external_global_count; ++global_index)
	{
		const hs_global_external* global = assert_return(HS_GLOBAL_EXTERNAL_GET(HS_GLOBAL_INDEX(global_index)));
		if (!csstricmp(name, global->name))
		{
			result = HS_GLOBAL_INDEX(global_index) | FLAG(SHORT_BITS - 1);
			global_found = true;
			break;
		}
	}

	// If we don't find an external global start looking through the internal globals
	if (!global_found)
	{
		if (global_scenario_index_get() != NONE)
		{
			const scenario* scnr = global_scenario_get();
			for (int16 global_index = 0; global_index < scnr->hs_globals.count; ++global_index)
			{
				const hs_global_internal* global = assert_return(HS_GLOBAL_INTERNAL_GET(global_index));
				if (!csstricmp(name, global->name))
				{
					result = HS_GLOBAL_INDEX(global_index);
					global_found = true;
					break;
				}
			}
		}
	}
	return result;
}

int16 hs_find_function_by_name(const char* name, int16 count)
{
	int16 result = NONE;
	for (int16 i = 0; i < hs_function_table_count; ++i)
	{
		const hs_function_definition* function = hs_function_table[i];
		if (!csstricmp(function->name, name) &&
			(TEST_BIT(function->flags, 1)
			|| count == NONE
			|| hs_function_table[i]->formal_parameter_count == count)
		)
		{
			result = i;
			;
		}
	}
	return result;
}

int16 hs_tokens_enumerate(const char* substring, uint32 type_flags, const char** results, int16 maximum_count)
{
	ASSERT(!enumeration_results);

	enumeration_count = 0;
	enumeration_maximum_count = maximum_count;
	enumeration_results = results;
	enumeration_substring = substring ? substring : "";
	
	// Go through every token enumerator and get our autocomplete results
	for (int32 type_index = 0; type_index < NUMBEROF(hs_token_enumerators); ++type_index)
	{
		ASSERT(hs_token_enumerators[type_index]);
		if (TEST_BIT(type_flags, type_index))
		{
			hs_token_enumerators[type_index]();
		}
	}

	// Sort the enumeration results
	qsort_4byte(results, enumeration_count, sort_by_found_index, 0);
	enumeration_results = NULL;
	return enumeration_count;
}

#endif

/* private code */

static void __cdecl hs_print(const char* text)
{
	terminal_printf(global_real_argb_green, text);
	return;
}

#ifdef HS_COMPILER_ENABLED

static bool sort_by_found_index(const void* s1, const void* s2, const void* compare_data)
{
	return csstricmp((const char*)s1, (const char*)s2) > 0;
}

static void hs_tokens_enumerate_add_string(const char* string)
{
	ASSERT(enumeration_results);

	if (enumeration_count < enumeration_maximum_count)
	{
		const size_t size = cstrlen(enumeration_substring);
		if (!csstrnicmp(string, enumeration_substring, size))
		{
			enumeration_results[enumeration_count++] = string;
		}
	}
	return;
}

static void hs_enumerate_block_data(const s_tag_block* block, int16 element_offset, int32 size)
{
	for (int32 i = 0; i < block->count; ++i)
	{
		const void* element = tag_block_get_element_with_size(block, i, size);
		hs_tokens_enumerate_add_string(((const char*)element) + element_offset);
	}
	return;
}

static void hs_enumerate_block_data_string_id(const s_tag_block* block, int16 element_offset, int32 size)
{
	for (int32 i = 0; i < block->count; ++i)
	{
		const void* element = tag_block_get_element_with_size(block, i, size);
		const char* string = (const char*)string_id_get_string_const(*(string_id*)((char*)element + element_offset));
		hs_tokens_enumerate_add_string(string);
	}
	return;
}

static void hs_enumerate_scenario_data(int16 block_offset, int16 element_offset, int32 size)
{
	if (global_scenario_index_get() != NONE)
	{
		const s_tag_block* block = (s_tag_block*)(block_offset + (uint8*)global_scenario_get());
		hs_enumerate_block_data(block, element_offset, size);
	}
	return;
}

static void hs_enumerate_scenario_data_string_id(int16 block_offset, int16 element_offset, int32 size)
{
	if (global_scenario_index_get() != NONE)
	{
		const s_tag_block* block = (s_tag_block*)(block_offset + (uint8*)global_scenario_get());
		hs_enumerate_block_data_string_id(block, element_offset, size);
	}
	return;
}

static void hs_enumerate_from_string_list(const char** strings, int16 start_index, int16 count)
{
	while (start_index < count)
	{
		hs_tokens_enumerate_add_string(strings[start_index++]);
	}
	return;
}

static void hs_enumerate_special_form_names(void)
{
	hs_tokens_enumerate_add_string("script");
	hs_tokens_enumerate_add_string("global");
	return;
}

static void hs_enumerate_script_type_names(void)
{
	hs_enumerate_from_string_list(hs_script_type_names, 0, NUMBEROF(hs_script_type_names));
	return;
}

static void hs_enumerate_type_names(void)
{
	hs_enumerate_from_string_list(hs_type_names, FIRST_HS_TYPE, NUMBER_OF_HS_NODE_TYPES);
	return;
}

static void hs_enumerate_function_names(void)
{
	for (int16 i = 0; i < hs_function_table_count; ++i)
	{
		const hs_function_definition* function = hs_function_get(i);
		hs_tokens_enumerate_add_string(function->name);
	}
	return;
}

static void hs_enumerate_script_names(void)
{
	hs_enumerate_scenario_data(offsetof(scenario, hs_scripts), 0, sizeof(hs_script));
	return;
}

static void hs_enumerate_variable_names(void)
{
	for (int16 i = 0; i < k_hs_external_global_count; ++i)
	{
		const hs_global_external* global = hs_global_external_get(i);
		hs_tokens_enumerate_add_string(global->name);
	}
	hs_enumerate_scenario_data(offsetof(scenario, hs_globals), 0, sizeof(hs_global_internal));
	return;
}

static void hs_enumerate_ai_names(void)
{
	if (global_scenario_index_get() != NONE)
	{
		scenario* scenario = global_scenario_get();
		
		// Print all squad names
		for (int32 i = 0; i < scenario->squads.count; ++i)
		{
			const squad_definition* squad = TAG_BLOCK_GET_ELEMENT(&scenario->squads, i, squad_definition);
			hs_tokens_enumerate_add_string(squad->name);
		}

		// Print all squad group names
		for (int32 i = 0; i < scenario->squad_groups.count; ++i)
		{
			const squad_group_definition* squad_group = TAG_BLOCK_GET_ELEMENT(&scenario->squad_groups, i, squad_group_definition);
			hs_tokens_enumerate_add_string(squad_group->name);
		}

		// Print all zone names
		for (int32 i = 0; i < scenario->zones.count; ++i)
		{
			const zone_definition* zone = TAG_BLOCK_GET_ELEMENT(&scenario->zones, i, zone_definition);
			hs_tokens_enumerate_add_string(zone->name);
		}

		// Print all order names
		for (int32 i = 0; i < scenario->orders.count; ++i)
		{
			const orders_definition* order = TAG_BLOCK_GET_ELEMENT(&scenario->orders, i, orders_definition);
			hs_tokens_enumerate_add_string(order->name);
		}

		// Print every command script point name
		if (scenario->cs_scripting_data.count > 0)
		{
			cs_script_data* script_data = cs_scenario_get_script_data(scenario);
			for (int32 i = 0; i < script_data->point_sets.count; ++i)
			{
				const cs_point_set* point = cs_get_point_set(i);
				hs_tokens_enumerate_add_string(point->name);
			}
		}
	}
	return;
}

static void hs_enumerate_ai_command_list_names(void)
{
	// This is empty in the original game?
	return;
}

static void hs_enumerate_ai_command_script_names(void)
{
	if (global_scenario_index_get() != NONE)
	{
		scenario* scenario = global_scenario_get();
		for (int32 i = 0; i < scenario->hs_scripts.count; ++i)
		{
			const hs_script* script = TAG_BLOCK_GET_ELEMENT(&scenario->hs_scripts, i, hs_script);
			if (script->script_type == _hs_script_command_script)
			{
				hs_tokens_enumerate_add_string(script->name);
			}
		}
	}
	return;
}

static void hs_enumerate_ai_behavior_names(void)
{
	for (size_t i = 0; i < NUMBEROF(behavior_names); ++i)
	{
		hs_tokens_enumerate_add_string(behavior_names[i]);
	}
	return;
}

static void hs_enumerate_starting_profile_names(void)
{
	hs_enumerate_scenario_data(
		offsetof(scenario, player_starting_profile),
		offsetof(scenario_starting_profile, name),
		sizeof(scenario_starting_profile)
	);
	return;
}

static void hs_enumerate_conversation_names(void)
{
	hs_enumerate_scenario_data(
		offsetof(scenario, ai_conversations),
		offsetof(ai_conversation, name),
		sizeof(ai_conversation)
	);
	return;
}

static void hs_enumerate_object_name(void)
{
	hs_enumerate_scenario_data(
		offsetof(scenario, object_names),
		offsetof(scenario_object_name, name),
		sizeof(scenario_object_name)
	);
	return;
}

static void hs_enumerate_trigger_volume_names(void)
{
	hs_enumerate_scenario_data_string_id(
		offsetof(scenario, trigger_volumes),
		offsetof(scenario_trigger_volume, name),
		sizeof(scenario_trigger_volume)
	);
	return;
}

static void hs_enumerate_cutscene_flag_names(void)
{
	hs_enumerate_scenario_data(
		offsetof(scenario, cutscene_flags),
		offsetof(scenario_cutscene_flag, name),
		sizeof(scenario_cutscene_flag)
	);
	return;
}

static void hs_enumerate_cutscene_camera_point_names(void)
{
	hs_enumerate_scenario_data(
		offsetof(scenario, cutscene_camera_points),
		offsetof(scenario_cutscene_camera_point, name),
		sizeof(scenario_cutscene_camera_point)
	);
	return;
}

static void hs_enumerate_cutscene_title_names(void)
{
	hs_enumerate_scenario_data(
		offsetof(scenario, cutscene_chapter_titles),
		offsetof(s_scenario_cutscene_title, name),
		sizeof(s_scenario_cutscene_title)
	);
	return;
}

static void hs_enumerate_cutscene_recording_names(void)
{
	hs_enumerate_scenario_data(
		offsetof(scenario, recorded_animations),
		offsetof(recorded_animation_definition, name),
		sizeof(recorded_animation_definition)
	);
	return;
}

static void hs_enumerate_navpoints(void)
{
	if (global_scenario_index_get() != NONE && interface_get_tag_index(_interface_hud_globals) != NONE)
	{
		datum const hud_globals_index = interface_get_tag_index(_interface_hud_globals);
		hud_globals_definition const* hud_globals = hud_globals_definition_get(hud_globals_index);
		
		ASSERT(hud_globals);

		hs_enumerate_block_data(&hud_globals->waypoint_arrows, 0, sizeof(s_waypoint_arrows_block));
	}
	return;
}

static void hs_enumerate_hud_messages(void)
{
	if (global_scenario_index_get() != NONE)
	{
		int32 index = global_scenario_get()->hud_messages.index;
		if (index != NONE)
		{
			hud_state_messages const* hud_message = hud_state_messages_get(index);
			
			hs_enumerate_block_data(&hud_message->messages, 0, sizeof(hud_state_message_definition));
		}
	}
	return;
}

static void hs_get_function_parameters_string(int16 function_index, char* buffer, int32 buffer_size)
{
	const hs_function_definition* function = hs_function_get(function_index);
	csprintf(buffer, buffer_size, "(%s", function->name);
	if (function->parameters)
	{
		csnappendf(buffer, buffer_size, " %s", function->parameters);
		csstrncat(buffer, ")", buffer_size);
	}
	else
	{
		for (int16 i = 0; i < function->formal_parameter_count; ++i)
		{
			csstrncat(buffer, " <", buffer_size);
			csstrncat(buffer, hs_type_names[function->formal_parameters[i]], buffer_size);
			csstrncat(buffer, ">", buffer_size);
		}
		csstrncat(buffer, ")", buffer_size);
	}
	return;
}

#endif
