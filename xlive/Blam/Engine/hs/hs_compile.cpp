#include "stdafx.h"
#include "hs_compile.h"

#ifdef HS_COMPILER_ENABLED
#include "hs_runtime.h"

#include "cutscene/cinematics_definitions.h"
#include "cutscene/recorded_animation_definitions.h"
#include "devices/devices.h"
#include "devices/device_scenario_definitions.h"
#include "editor/editor_stubs.h"
#include "memory/data.h"
#include "scenario/scenario.h"
#include "scenario/scenario_definitions.h"
#include "scenario/scenario_kill_trigger_volumes.h"

#include "tag_files/tag_group_access.h"
#include "tag_files/tag_group_dependencies.h"

/* constants */

enum
{
	k_begin_random_max_arguments = 32
};

static const char whitespace_characters[2] = { ' ', '\t' };

/* enums */

enum e_skip_whitespace_state
{
	_skip_whitespace_state_no_comment = 0,
	_skip_whitespace_state_comment,
	_skip_whitespace_state_block_comment,
	_skip_whitespace_state_done,
	NUMBER_OF_SKIP_WHITESPACE_STATES,
};

/* structures */

struct s_hs_compile_globals
{
	bool initialized;
	int32 compiled_source_size;
	char* compiled_source;
	char* string_constant_buffer;
	int32 string_constant_buffer_offset;
	int32 string_constant_buffer_size;
	bool error_since_initialize;
	const char* error_message;
	int32 error_offset;
	char error_buffer[k_hs_buffer_size];
	bool malloced;
	bool permanent;
	bool disallow_blocks;
	bool disallow_sets;
	bool variables_predetermined;
	int32 counter;
};

/* prototypes */

static void skip_whitespace(char** c);

static bool character_in_array(char c, int16 count, char* array);

static bool hs_parse_boolean(int32 expression_index);

static bool hs_parse_real(int32 expression_index);

static bool hs_parse_integer(int32 expression_index);

static bool hs_parse_string(int32 expression_index);

static bool hs_parse_script(int32 expression_index);

static bool hs_parse_string_id(int32 expression_index);

static bool hs_parse_seat_mapping(int32 expression_index);

static bool hs_parse_trigger_volume(int32 expression_index);

static bool hs_parse_cutscene_flag(int32 expression_index);

static bool hs_parse_cutscene_camera_point(int32 expression_index);

static bool hs_parse_cutscene_title(int32 expression_index);

static bool hs_parse_cutscene_recording(int32 expression_index);

static bool hs_parse_device_group(int32 expression_index);

static bool hs_parse_ai(int32 expression_index);

static bool hs_parse_ai_command_list(int32 expression_index);

static bool hs_parse_ai_command_script(int32 expression_index);

static bool hs_parse_ai_behavior(int32 expression_index);

static bool hs_parse_ai_orders(int32 expression_index);

static bool hs_parse_starting_profile(int32 expression_index);

static bool hs_parse_conversation(int32 expression_index);

static bool hs_parse_structure_bsp(int32 expression_index);

static bool hs_parse_navpoint(int32 expression_index);

static bool hs_parse_point_ref(int32 expression_index);

static bool hs_parse_style(int32 expression_index);

static bool hs_parse_hud_message(int32 expression_index);

static bool hs_parse_object_list(int32 expression_index);

static bool hs_parse_tag_reference(int32 expression_index);

static bool hs_parse_enum(int32 expression_index);

static bool hs_parse_object(int32 expression_index);

static bool hs_parse_object_name(int32 expression_index);

static int16 hs_count_children(int32 expression_index);

static void hs_compile_resize(void);

static datum hs_tokenize(char** expression);

static void hs_tokenize_primitive(char** expression, int32 expression_index);

static void hs_tokenize_nonprimitive(char** expression, int32 expression_index);

static bool hs_parse_primitive(int32 expression_index);

static bool hs_parse_variable(int32 expression_index);

static void hs_parse_call_predicate(int32 expression_index, bool* is_function);

static bool hs_parse_nonprimitive(int32 expression_index);

/* find string within list of strings */
static int16 string_list_find(const char* string, int16 list_count, const char** string_list);

static bool hs_add_script(int32 expression_index);

static bool hs_add_global(int32 expression_index);

static bool hs_get_parameter_indices(const char* function_name, uint16 count, int32* result_indices, int32 expression_index);

static bool hs_parse_tag_block_element(int32 expression_index, int32 offset, int32 tag_index, s_tag_block* block, int32 element_size);

static bool hs_parse_tag_block_element_string_id(int32 expression_index, int32 offset, int32 tag_index, s_tag_block* block, int32 element_size);

/* globals */

static bool (*hs_type_primitive_parsers[NUMBER_OF_HS_NODE_TYPES])(int32) =
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	hs_parse_boolean,
	hs_parse_real,
	hs_parse_integer,
	hs_parse_integer,
	hs_parse_string,
	hs_parse_script,
	hs_parse_string_id,
	hs_parse_seat_mapping,
	hs_parse_trigger_volume,
	hs_parse_cutscene_flag,
	hs_parse_cutscene_camera_point,
	hs_parse_cutscene_title,
	hs_parse_cutscene_recording,
	hs_parse_device_group,
	hs_parse_ai,
	hs_parse_ai_command_list,
	hs_parse_ai_command_script,
	hs_parse_ai_behavior,
	hs_parse_ai_orders,
	hs_parse_starting_profile,
	hs_parse_conversation,
	hs_parse_structure_bsp,
	hs_parse_navpoint,
	hs_parse_point_ref,
	hs_parse_style,
	hs_parse_hud_message,
	hs_parse_object_list,
	hs_parse_tag_reference,
	hs_parse_tag_reference,
	hs_parse_tag_reference,
	hs_parse_tag_reference,
	hs_parse_tag_reference,
	hs_parse_tag_reference,
	hs_parse_tag_reference,
	hs_parse_tag_reference,
	hs_parse_tag_reference,
	hs_parse_tag_reference,
	hs_parse_tag_reference,
	hs_parse_tag_reference,
	hs_parse_enum,
	hs_parse_enum,
	hs_parse_enum,
	hs_parse_enum,
	hs_parse_enum,
	hs_parse_enum,
	hs_parse_object,
	hs_parse_object,
	hs_parse_object,
	hs_parse_object,
	hs_parse_object,
	hs_parse_object,
	hs_parse_object_name,
	hs_parse_object_name,
	hs_parse_object_name,
	hs_parse_object_name,
	hs_parse_object_name,
	hs_parse_object_name
};

static s_hs_compile_globals hs_compile_globals;

/* public code */

void hs_compile_initialize(bool resize)
{
	ASSERT(!hs_compile_globals.initialized);
	hs_compile_globals.compiled_source = NULL;
	hs_compile_globals.compiled_source_size = 0;
	hs_compile_globals.error_since_initialize = false;
	hs_compile_globals.error_message = NULL;
	hs_compile_globals.counter = 0;
	hs_compile_globals.initialized = true;
	hs_compile_globals.permanent = resize;
	if (resize)
	{
		hs_compile_resize();
		resize_scenario_syntax_data(0);
	}
	return;
}

void hs_compile_dispose(void)
{
	data_array* g_hs_syntax_data = *hs_syntax_data_get();

	ASSERT(hs_compile_globals.initialized);

	if (hs_compile_globals.permanent)
	{
		if (hs_compile_globals.error_since_initialize)
		{
			data_delete_all(g_hs_syntax_data);
		}
		if (hs_compile_globals.compiled_source)
		{
			CSERIES_FREE(hs_compile_globals.compiled_source);
		}
	}
	if (hs_compile_globals.malloced)
	{
		CSERIES_FREE(hs_compile_globals.compiled_source);
		hs_compile_globals.compiled_source = NULL;
		hs_compile_globals.malloced = false;
	}
	hs_compile_globals.initialized = false;
	
	int32 size = g_hs_syntax_data->first_free_absolute_index + 512;
	if (size >= 36864)
	{
		size = 0;
	}
	resize_scenario_syntax_data(size);
	return;
}

int32 hs_compile_expression(int32 source_size, const char* source_data, const char** error_message_pointer, const char** error_source_pointer)
{
	datum result = NONE;
	if (source_size < k_hs_buffer_size)
	{
		int32 new_index;
		if (global_scenario_index_get() == NONE)
		{
			new_index = 0;
			hs_compile_globals.compiled_source = (char*)CSERIES_MALLOC(source_size + 1);
			hs_compile_globals.malloced = true;
			ASSERT(hs_compile_globals.compiled_source);
		}
		else
		{
			ASSERT(global_scenario_get()->hs_string_constants.size >= HS_MAXIMUM_DYNAMIC_SOURCE_DATA_BYTES);
			new_index = global_scenario_get()->hs_string_constants.size - HS_MAXIMUM_DYNAMIC_SOURCE_DATA_BYTES;
			hs_compile_globals.compiled_source = (char*)tag_data_get_address(&global_scenario_get()->hs_string_constants);
		}

		data_array* g_hs_syntax_data = *hs_syntax_data_get();

		ASSERT(g_hs_syntax_data);
		ASSERT(hs_compile_globals.compiled_source);

		csmemcpy(&hs_compile_globals.compiled_source[new_index], source_data, source_size);
		hs_compile_globals.compiled_source_size = new_index + source_size;
		hs_compile_globals.compiled_source[hs_compile_globals.compiled_source_size] = '\0';

		hs_compile_globals.error_message = NULL;
		*error_message_pointer = NULL;
		*error_source_pointer = NULL;
		hs_compile_globals.error_offset = NONE;

		char* source = &hs_compile_globals.compiled_source[new_index];
		skip_whitespace(&source);
		if (source[0] != '\0')
		{
			const datum tokenized_index = hs_tokenize(&source);
			if (hs_compile_globals.error_message == NULL)
			{
				const datum compiled_index = datum_new(g_hs_syntax_data);
				const datum data_index = datum_new(g_hs_syntax_data);
				if (compiled_index != NONE && data_index != NONE)
				{
					hs_syntax_node* compiled_expression = hs_syntax_get(compiled_index);
					hs_syntax_node* data_node = hs_syntax_get(data_index);
					compiled_expression->long_value = data_index;
					compiled_expression->next_node_index = NONE;
					compiled_expression->source_offset = ((hs_syntax_node*)hs_syntax_get(tokenized_index))->source_offset;
					compiled_expression->flags = 0;
					data_node->next_node_index = tokenized_index;
					data_node->source_offset = NONE;
					data_node->function_index = _hs_function_inspect;
					data_node->flags = (e_hs_syntax_node_flags)FLAG(_hs_syntax_node_primitive_bit);
					data_node->type = _hs_function_name;
					if (hs_parse(compiled_index, _hs_type_void))
					{
						result = compiled_index;
					}
				}
			}
			
			if (result == NONE)
			{
				*error_message_pointer = hs_compile_globals.error_message;
				if (hs_compile_globals.error_offset != NONE)
				{
					hs_compile_globals.error_offset -= new_index;
					ASSERT(hs_compile_globals.error_offset >= 0 && hs_compile_globals.error_offset < source_size);
					*error_source_pointer = &source_data[hs_compile_globals.error_offset];
				}
			}
		}
		hs_runtime_require_gc();
	}

	return result;
}

void hs_compile_source(bool fail_on_error)
{
	// TODO: reimplement
	return;
}

void resize_scenario_syntax_data(int32 size)
{
	// Do nothing....
	return;
}

bool hs_parse(int32 expression_index, int16 expected_type)
{
	bool result = true;
	hs_syntax_node* node = hs_syntax_get(expression_index);

	ASSERT(!hs_compile_globals.error_message);
	ASSERT(hs_type_valid(expected_type) || expected_type == _hs_special_form || expected_type == _hs_unparsed);

	if (node->type == _hs_unparsed)
	{
		node->type = (e_hs_type)expected_type;
		if (TEST_BIT(hs_syntax_get(expression_index)->flags, _hs_syntax_node_primitive_bit))
		{
			node->constant_type = expected_type;
			result = hs_parse_primitive(expression_index);
		}
		else
		{
			result = hs_parse_nonprimitive(expression_index);
		}
	}

	return result;
}

bool hs_parse_begin(int16 function_index, int32 expression_index)
{
	bool result = true;
	hs_syntax_node* expression = hs_syntax_get(expression_index);
	int32 current_argument_index = hs_syntax_get(hs_syntax_get(expression_index)->long_value)->next_node_index;

	ASSERT(function_index == _hs_function_begin || function_index == _hs_function_begin_random);

	int16 argument_count = 0;
	while (result && current_argument_index != NONE)
	{
		const int32 next_argument_index = hs_syntax_get(current_argument_index)->next_node_index;
		if (function_index)
		{
			result = hs_parse(current_argument_index, expression->type);
			if (expression->type == _hs_unparsed && result)
			{
				expression->type = hs_syntax_get(current_argument_index)->type;
			}
		}
		else
		{
			result = hs_parse(current_argument_index, next_argument_index == NONE ? expression->type : _hs_type_void);
			if (next_argument_index == NONE && expression->type == _hs_unparsed)
			{
				if (result)
				{
					expression->type = hs_syntax_get(current_argument_index)->type;
				}
			}
		}
		current_argument_index = next_argument_index;
		++argument_count;
	}

	if (result)
	{
		if (argument_count >= 1)
		{
			if (argument_count > k_begin_random_max_arguments && function_index == _hs_function_begin_random)
			{
				hs_compile_globals.error_message = "begin_random can take a maximum of 32 arguments (matt can increase this.)";
				hs_compile_globals.error_offset = hs_syntax_get(expression_index)->source_offset;
				result = false;
			}
		}
		else
		{
			const hs_function_definition* function = hs_function_get(function_index);
			hs_compile_globals.error_message = csprintf(
				hs_compile_globals.error_buffer,
				NUMBEROF(hs_compile_globals.error_buffer),
				"a statement block must contain at least one argument.",
				function->name);
			hs_compile_globals.error_offset = hs_syntax_get(expression_index)->source_offset;
			result = false;
		}
	}
	return result;
}

bool hs_parse_if(int16 function_index, int32 expression_index)
{
	// TODO: implement
	return false;
}

bool hs_parse_cond(int16 function_index, int32 expression_index)
{
	// TODO: implement
	return false;
}

bool hs_parse_set(int16 function_index, int32 expression_index)
{
	bool result = false;

	const int32 variable_index = hs_syntax_get(hs_syntax_get(expression_index)->long_value)->next_node_index;
	hs_syntax_node* expression = hs_syntax_get(expression_index);
	if (variable_index == NONE)
	{
		hs_compile_globals.error_message = "i expected a variable to set and a value.";
		hs_compile_globals.error_offset = hs_syntax_get(expression_index)->source_offset;
	}
	else
	{
		const int32 assignment_index = hs_syntax_get(variable_index)->next_node_index;
		if (assignment_index == NONE)
		{
			hs_compile_globals.error_message = "i expected an assignment value.";
			hs_compile_globals.error_offset = hs_syntax_get(expression_index)->source_offset;
		}
		else if (hs_syntax_get(assignment_index)->next_node_index == NONE)
		{
			hs_syntax_node* variable = hs_syntax_get(variable_index);
			const int16 hs_global_index = hs_find_global_by_name(&hs_compile_globals.compiled_source[variable->source_offset]);
			if (hs_global_index == NONE)
			{
				hs_compile_globals.error_message = "this is not a valid global variable.";
				hs_compile_globals.error_offset = variable->source_offset;
			}
			else
			{
				variable->type = (e_hs_type)hs_global_get_type(hs_global_index);
				if (expression->type && !hs_can_cast(variable->type, expression->type))
				{
					hs_compile_globals.error_message = csprintf(
						hs_compile_globals.error_buffer,
						NUMBEROF(hs_compile_globals.error_buffer),
						"you cannot pass the result of this set (type %s) to a function that expects type %s.",
						hs_type_names[variable->type],
						hs_type_names[hs_syntax_get(expression_index)->type]);
					hs_compile_globals.error_offset = hs_syntax_get(expression_index)->source_offset;
				}
				else
				{
					if (!hs_parse_variable(variable_index))
					{
						DISPLAY_ASSERT("asserted");
					}

					if (expression->type == _hs_unparsed)
					{
						expression->type = variable->type;
					}

					if (hs_parse(assignment_index, variable->type))
					{
						result = true;
					}
				}
			}
		}
		else
		{
			hs_compile_globals.error_message = "i didn't expect this argument.";
			hs_compile_globals.error_offset = hs_syntax_get(hs_syntax_get(assignment_index)->next_node_index)->source_offset;
		}
	}
	return result;
}

bool hs_parse_logical(int16 function_index, int32 expression_index)
{
	// TODO: implement

	ASSERT(function_index == _hs_function_and || function_index == _hs_function_or);
	return false;
}

bool hs_parse_arithmetic(int16 function_index, int32 expression_index)
{
	// TODO: implement
	return false;
}

bool hs_parse_equality(int16 function_index, int32 expression_index)
{
	// TODO: implement
	return false;
}

bool hs_parse_inequality(int16 function_index, int32 expression_index)
{
	// TODO: implement
	return false;
}

bool hs_parse_sleep(int16 function_index, int32 expression_index)
{
	// TODO: implement
	return false;
}

bool hs_parse_sleep_forever(int16 function_index, int32 expression_index)
{
	// TODO: implement
	return false;
}

bool hs_parse_sleep_until(int16 function_index, int32 expression_index)
{
	// TODO: implement
	return false;
}

bool hs_parse_wake(int16 function_index, int32 expression_index)
{
	bool result = false;

	ASSERT(function_index == _hs_function_wake);

	int32 index;
	if (hs_get_parameter_indices(hs_function_get(function_index)->name, 1, &index, expression_index))
	{
		const hs_syntax_node* expression = hs_syntax_get(index);
		if (hs_parse(index, _hs_type_script))
		{
			hs_script* script = scenario_get_hs_script(global_scenario_get(), expression->short_value);
			ASSERT(script);
			if (script->script_type != _hs_script_static && script->script_type != _hs_script_stub && script->script_type != _hs_script_command_script)
			{
				result = true;
			}
			else
			{
				hs_compile_globals.error_message = "this static script cannot be awakened.";
				hs_compile_globals.error_offset = expression->source_offset;
			}
		}
	}

	return result;
}

bool hs_parse_inspect(int16 function_index, int32 expression_index)
{
	bool result = false;

	ASSERT(function_index == _hs_function_inspect);

	const hs_function_definition* function = hs_function_get(function_index);

	int32 index;
	if (hs_get_parameter_indices(function->name, 1, &index, expression_index))
	{
		const hs_syntax_node* expression = hs_syntax_get(index);
		if (hs_parse(index, _hs_unparsed))
		{
			result = true;
		}
		else if (!hs_compile_globals.error_message)
		{
			hs_compile_globals.error_message = "this is not a global variable reference, function call, or script call.";
			hs_compile_globals.error_offset = expression->source_offset;
		}
	}
	return result;
}

bool hs_parse_object_cast_up(int16 function_index, int32 expression_index)
{
	ASSERT(function_index >= _hs_function_object_to_unit && function_index <= _hs_function_object_to_unit);

	const hs_function_definition* function = hs_function_get(function_index);
	int32 index;
	return hs_get_parameter_indices(function->name, 1, &index, expression_index) && hs_parse(index, _hs_type_object);
}

bool hs_macro_function_parse(int16 function_index, int32 expression_index)
{
	bool result = true;
	const hs_function_definition* definition = hs_function_get(function_index);
	int32 string_expression_index = hs_syntax_get(hs_syntax_get(expression_index)->long_value)->next_node_index;

	ASSERT(hs_type_valid(definition->return_type));

	int32 parameter_num;
	for (parameter_num = 0; result && parameter_num < definition->formal_parameter_count && string_expression_index != NONE; ++parameter_num )
	{
		const hs_syntax_node* string = hs_syntax_get(string_expression_index);
		if (hs_parse(string_expression_index, definition->formal_parameters[parameter_num]))
		{
			string_expression_index = string->next_node_index;
		}
		else
		{
			if (string->type == _hs_type_string_id && TEST_BIT(hs_syntax_get(string_expression_index)->flags, _hs_syntax_node_primitive_bit))
			{
				csprintf(
					hs_compile_globals.error_buffer,
					NUMBEROF(hs_compile_globals.error_buffer),
					"this is not a valid string for '%s'",
					definition->name);
				hs_compile_globals.error_message = hs_compile_globals.error_buffer;
				hs_compile_globals.error_offset = string->source_offset;
			}
			result = false;
		}
	}
	
	if (result && (parameter_num != definition->formal_parameter_count || string_expression_index != NONE))
	{
		csprintf(
			hs_compile_globals.error_buffer,
			NUMBEROF(hs_compile_globals.error_buffer),
			"the \"%s\" call requires exactly %d arguments.",
			definition->name,
			definition->formal_parameter_count);
		hs_compile_globals.error_message = hs_compile_globals.error_buffer;
		hs_compile_globals.error_offset = hs_syntax_get(expression_index)->source_offset;
		result = false;
	}
	return result;
}

/* private code */

static void skip_whitespace(char** c)
{
	e_skip_whitespace_state state = _skip_whitespace_state_no_comment;
	while (state != _skip_whitespace_state_done)
	{
		if (state != _skip_whitespace_state_no_comment)
		{
			if (state == _skip_whitespace_state_comment)
			{
				if (**c)
				{
					if (character_in_array(*c[0], 2, "\n\r"))
					{
						state = _skip_whitespace_state_no_comment;
					}
					++*c;
				}
				else
				{
					state = _skip_whitespace_state_done;
				}
			}
			else if (state == _skip_whitespace_state_block_comment)
			{
				if (*c[0] != '\0')
				{
					if (**c == '*' && (*c)[1] == ';')
					{
						state = _skip_whitespace_state_no_comment;
						++*c;
					}
					++*c;
				}
				else
				{
					state = _skip_whitespace_state_done;
					hs_compile_globals.error_message = "unterminated comment.";
				}
			}
			else
			{
				unreachable();
			}
		}
		else if (**c == ';')
		{
			state = _skip_whitespace_state_comment;
			if (*++*c == '*')
			{
				state = _skip_whitespace_state_block_comment;
				++*c;
			}
		}
		else if (character_in_array(**c, 2, " \t") || character_in_array(**c, 2, "\n\r"))
		{
			++*c;
		}
		else
		{
			state = _skip_whitespace_state_done;
		}
	}
	return;
}

static bool character_in_array(char c, int16 count, char* array)
{
	bool result = false;
	for (int16 i = 0; i < count; ++i)
	{
		if (c == array[i])
		{
			result = true;
			break;
		}
	}
	return result;
}

static bool hs_parse_boolean(int32 expression_index)
{
	hs_syntax_node* expression = hs_syntax_get(expression_index);
	const char* source = &hs_compile_globals.compiled_source[expression->source_offset];

	ASSERT(expression->type == _hs_type_boolean);
	ASSERT(expression->constant_type == expression->type);

	bool result = false;
	if (csstrcmp(source, "true") && csstrcmp(source, "on") && csstrcmp(source, "1"))
	{
		if (csstrcmp(source, "false") && csstrcmp(source, "off") && csstrcmp(source, "0"))
		{
			hs_compile_globals.error_message = "i expected \"true\" or \"false\".";
			hs_compile_globals.error_offset = expression->source_offset;
			expression->bool_value = false;
		}
		else
		{
			expression->bool_value = false;
			result = true;
		}
	}
	else
	{
		expression->bool_value = true;
		result = true;
	}
	return result;
}

static bool hs_parse_real(int32 expression_index)
{
	bool first_character_is_decimal = false;
	bool result = true;

	hs_syntax_node* expression = hs_syntax_get(expression_index);
	const char* source = &hs_compile_globals.compiled_source[expression->source_offset];

	ASSERT(expression->type == _hs_type_real);
	ASSERT(expression->constant_type == expression->type);

	if (*source == '-')
	{
		++source;
	}

	while (source[0] != '\0')
	{
		if (!isdigit(source[0]))
		{
			if (first_character_is_decimal || source[0] != '.')
			{
				hs_compile_globals.error_message = "this is not a valid real number.";
				hs_compile_globals.error_offset = expression->source_offset;
				result = false;
				break;
			}
			first_character_is_decimal = true;
		}
		++source;
	}
	expression->real_value = (real32)atof(&hs_compile_globals.compiled_source[expression->source_offset]);
	return result;
}

static bool hs_parse_integer(int32 expression_index)
{
	bool result = true;
	hs_syntax_node* expression = hs_syntax_get(expression_index);
	const char* source = &hs_compile_globals.compiled_source[expression->source_offset];

	ASSERT(expression->type == _hs_type_short_integer || expression->type == _hs_type_long_integer);
	ASSERT(expression->constant_type == expression->type);

	// Ignore negative number before we start parsing for digits
	if (source[0] == '-')
	{
		++source;
	}

	// Loop through every character and make sure they're all digits
	while (source[0] != '\0')
	{
		if (!isdigit(source[0]))
		{
			hs_compile_globals.error_message = "this is not a valid integer.";
			hs_compile_globals.error_offset = expression->source_offset;
			result = false;
			break;
		}
		++source;
	}

	// Grab the value
	const int32 value = atoi(&hs_compile_globals.compiled_source[expression->source_offset]);
	
	// Perform validation on short values
	if (result && expression->type != _hs_type_long_integer && !IN_RANGE(value, -32767, 32768))
	{
		hs_compile_globals.error_message = "shorts must be in the range [-32767, 32768].";
		hs_compile_globals.error_offset = expression->source_offset;
		result = false;
	}

	if (expression->type == _hs_type_long_integer)
	{
		expression->long_value = value;
	}
	else
	{
		expression->short_value = (int16)value;
	}

	return result;
}

static bool hs_parse_string(int32 expression_index)
{
	hs_syntax_node* expression = hs_syntax_get(expression_index);

	ASSERT(expression->type == _hs_type_string);
	ASSERT(expression->constant_type == expression->type);

	expression->string_value = &hs_compile_globals.compiled_source[expression->source_offset];
	return true;
}

static bool hs_parse_script(int32 expression_index)
{
	hs_syntax_node* expression = hs_syntax_get(expression_index);

	ASSERT(expression->type == _hs_type_script);
	ASSERT(expression->constant_type == expression->type);
	
	const int16 script_by_name = hs_find_script_by_name(&hs_compile_globals.compiled_source[expression->source_offset]);
	
	bool result;
	if (script_by_name == NONE)
	{
		hs_compile_globals.error_message = "this is not a valid script name.";
		hs_compile_globals.error_offset = expression->source_offset;
		result = false;
	}
	else
	{
		expression->short_value = script_by_name;
		result = true;
	}
	return result;
}

static bool hs_parse_string_id(int32 expression_index)
{
	hs_syntax_node* expression = hs_syntax_get(expression_index);

	ASSERT(expression->type == _hs_type_string_id);
	ASSERT(expression->constant_type == expression->type);

	const string_id id = string_id_exists(&hs_compile_globals.compiled_source[expression->source_offset]);

	bool result = false;
	if (id == _string_id_invalid)
	{
		hs_compile_globals.error_message = "this is not a valid string parameter.";
		hs_compile_globals.error_offset = expression->source_offset;
	}
	else
	{
		expression->string_id_value = id;
		result = true;
	}
	return result;
}

static bool hs_parse_seat_mapping(int32 expression_index)
{
	hs_syntax_node* expression = hs_syntax_get(expression_index);

	ASSERT(expression->type == _hs_type_unit_seat_mapping);
	ASSERT(expression->constant_type == expression->type);

	// TODO: implement
	return false;
}

static bool hs_parse_trigger_volume(int32 expression_index)
{
	ASSERT(hs_syntax_get(expression_index)->type == _hs_type_trigger_volume);
	return hs_parse_tag_block_element_string_id(
		expression_index,
		offsetof(scenario_trigger_volume, name),
		global_scenario_index_get(),
		&global_scenario_get()->trigger_volumes,
		sizeof(scenario_trigger_volume)
	);
}

static bool hs_parse_cutscene_flag(int32 expression_index)
{
	ASSERT(hs_syntax_get(expression_index)->type == _hs_type_cutscene_flag);
	return hs_parse_tag_block_element(
		expression_index,
		offsetof(scenario_cutscene_flag, name),
		global_scenario_index_get(),
		&global_scenario_get()->cutscene_flags,
		sizeof(scenario_cutscene_flag)
	);
}

static bool hs_parse_cutscene_camera_point(int32 expression_index)
{
	ASSERT(hs_syntax_get(expression_index)->type == _hs_type_cutscene_camera_point);
	return hs_parse_tag_block_element(
		expression_index,
		offsetof(scenario_cutscene_camera_point, name),
		global_scenario_index_get(),
		&global_scenario_get()->cutscene_camera_points,
		sizeof(scenario_cutscene_camera_point)
	);
}

static bool hs_parse_cutscene_title(int32 expression_index)
{
	ASSERT(hs_syntax_get(expression_index)->type == _hs_type_cutscene_title);
	return hs_parse_tag_block_element_string_id(
		expression_index,
		offsetof(s_scenario_cutscene_title, name),
		global_scenario_index_get(),
		&global_scenario_get()->cutscene_chapter_titles,
		sizeof(s_scenario_cutscene_title)
	);
}

static bool hs_parse_cutscene_recording(int32 expression_index)
{
	ASSERT(hs_syntax_get(expression_index)->type == _hs_type_cutscene_recording);
	return hs_parse_tag_block_element(
		expression_index,
		offsetof(recorded_animation_definition, name),
		global_scenario_index_get(),
		&global_scenario_get()->recorded_animations,
		sizeof(recorded_animation_definition)
	);
}

static bool hs_parse_device_group(int32 expression_index)
{
	ASSERT(hs_syntax_get(expression_index)->type == _hs_type_device_group);
	const bool result = hs_parse_tag_block_element(
		expression_index,
		offsetof(scenario_device_group, name),
		global_scenario_index_get(),
		&global_scenario_get()->device_groups,
		sizeof(scenario_device_group)
	);
	if (result)
	{
		hs_syntax_node* expression = hs_syntax_get(expression_index);
		expression->long_value = device_group_get_from_scenario_index(expression->short_value);
	}
	return result;
}

static bool hs_parse_ai(int32 expression_index)
{
	// TODO: implement
	return false;
}

static bool hs_parse_ai_command_list(int32 expression_index)
{
	// TODO: implement
	return false;
}

static bool hs_parse_ai_command_script(int32 expression_index)
{
	// TODO: implement
	return false;
}

static bool hs_parse_ai_behavior(int32 expression_index)
{
	// TODO: implement
	return false;
}

static bool hs_parse_ai_orders(int32 expression_index)
{
	// TODO: implement
	return false;
}

static bool hs_parse_starting_profile(int32 expression_index)
{
	// TODO: implement
	return false;
}

static bool hs_parse_conversation(int32 expression_index)
{
	// TODO: implement
	return false;
}

static bool hs_parse_structure_bsp(int32 expression_index)
{
	// TODO: implement
	return false;
}

static bool hs_parse_navpoint(int32 expression_index)
{
	// TODO: implement
	return false;
}

static bool hs_parse_point_ref(int32 expression_index)
{
	// TODO: implement
	return false;
}

static bool hs_parse_style(int32 expression_index)
{
	// TODO: implement
	return false;
}

static bool hs_parse_hud_message(int32 expression_index)
{
	// TODO: implement
	return false;
}

static bool hs_parse_object_list(int32 expression_index)
{
	// TODO: implement
	return false;
}

static bool hs_parse_tag_reference(int32 expression_index)
{
	// TODO: implement
	return false;
}

static bool hs_parse_enum(int32 expression_index)
{
	bool result = true;
	hs_syntax_node* expression = hs_syntax_get(expression_index);
	if (HS_TYPE_IS_ENUM(expression->type) && expression->constant_type == expression->type)
	{
		const hs_enum_definition* enum_definition = &hs_enum_table[expression->type - FIRST_HS_ENUM_TYPE];
		ASSERT(enum_definition);

		int16 enum_index = 0;
		for (; enum_index < enum_definition->count
			&& csstricmp(&hs_compile_globals.compiled_source[expression->source_offset], enum_definition->identifiers[enum_index]);
			++enum_index)
		{
			;
		}

		if (enum_index == enum_definition->count)
		{
			csprintf(hs_compile_globals.error_buffer, 0x400u, "%s must be ", hs_type_names[expression->type]);// "unparsed"
			int16 i = 0;
			for (; i < enum_definition->count - 1; ++i)
			{
				csstrncat(hs_compile_globals.error_buffer, "\"", NUMBEROF(hs_compile_globals.error_buffer));
				csstrncat(hs_compile_globals.error_buffer, enum_definition->identifiers[i], NUMBEROF(hs_compile_globals.error_buffer));
				csstrncat(hs_compile_globals.error_buffer, "\", ", NUMBEROF(hs_compile_globals.error_buffer));
			}

			if (enum_definition->count > 1)
			{
				csstrncat(hs_compile_globals.error_buffer, "or ", NUMBEROF(hs_compile_globals.error_buffer));
			}

			csstrncat(hs_compile_globals.error_buffer, "\"", NUMBEROF(hs_compile_globals.error_buffer));
			csstrncat(hs_compile_globals.error_buffer, enum_definition->identifiers[i], NUMBEROF(hs_compile_globals.error_buffer));
			csstrncat(hs_compile_globals.error_buffer, "\".", NUMBEROF(hs_compile_globals.error_buffer));
			hs_compile_globals.error_message = hs_compile_globals.error_buffer;
			hs_compile_globals.error_offset = expression->source_offset;
			result = false;
		}
		expression->short_value = enum_index;
	}
	else
	{
		csprintf(
			hs_compile_globals.error_buffer,
			NUMBEROF(hs_compile_globals.error_buffer),
			"corrupt enum expression (type %d constant-type %d)",
			expression->type,
			expression->script_index);
		hs_compile_globals.error_message = hs_compile_globals.error_buffer;
		hs_compile_globals.error_offset = expression->source_offset;
		result = false;
	}

	return result;
}

static bool hs_parse_object(int32 expression_index)
{
	// TODO: implement
	return false;
}

static bool hs_parse_object_name(int32 expression_index)
{
	// TODO: implement
	return false;
}

static int16 hs_count_children(int32 expression_index)
{
	int16 child_count = 0;

	for (int32 next_node_index = hs_syntax_get(expression_index)->long_value;
		next_node_index != NONE;
		next_node_index = hs_syntax_get(next_node_index)->next_node_index)
	{
		child_count++;
	}

	return child_count;
}

static void hs_compile_resize(void)
{
	editor_reset_script_referenced_blocks();
	return;
}

static datum hs_tokenize(char** state)
{
	data_array* g_hs_syntax_data = *hs_syntax_data_get();

	ASSERT(!hs_compile_globals.error_message);
	ASSERT(g_hs_syntax_data);
	
	const datum new_index = datum_new(g_hs_syntax_data);
	if (new_index == NONE)
	{
		hs_compile_globals.error_message = "i couldn't allocate a syntax node.";
	}
	else
	{
		hs_syntax_node* expression = hs_syntax_get(new_index);
		expression->flags = (e_hs_syntax_node_flags)0;
		expression->script_index = NONE;
		expression->type = _hs_unparsed;
		expression->next_node_index = NONE;
		SET_BIT(expression->flags, _hs_syntax_node_primitive_bit, *state[0] != '(');

		if (TEST_BIT((hs_syntax_get(new_index))->flags, _hs_syntax_node_primitive_bit))
		{
			hs_tokenize_primitive(state, new_index);
		}
		else
		{
			hs_tokenize_nonprimitive(state, new_index);
		}
	}

	return new_index;
}

static void hs_tokenize_primitive(char** state, int32 expression_index)
{
	hs_syntax_node* expression = hs_syntax_get(expression_index);
	if (*state[0] == '"')
	{
		expression->source_offset = ++ * state - hs_compile_globals.compiled_source;
		while (*state != '\0' && *state[0] != '"')
		{
			++*state;
		}

		if (!**state)
		{
			hs_compile_globals.error_message = "this quoted constant is unterminated.";
			hs_compile_globals.error_offset = expression->source_offset - 1;
		}
		*(*state)++ = '\0';
	}
	else
	{
		expression->source_offset = *state - hs_compile_globals.compiled_source;
		while (**state
			&& **state != ')'
			&& **state != ';'
			&& !character_in_array(*state[0], 2, " \t")
			&& !character_in_array(*state[0], 2, "\n\r"))
		{
			++*state;
		}
	}
	return;
}

static void hs_tokenize_nonprimitive(char** state, int32 expression_index)
{
	hs_syntax_node* expression = hs_syntax_get(expression_index);
	int32* next_node_index = &expression->long_value;

	expression->source_offset = *state - hs_compile_globals.compiled_source;
	++*state;

	while (!hs_compile_globals.error_message)
	{
		char* old_state = *state;
		skip_whitespace(state);
		if (*state != old_state)
		{
			old_state[0] = '\0';
		}
		
		if (*state[0] != '\0')
		{
			// End tokenization if we reach the ending bracket
			if (*state[0] == ')')
			{
				*(*state)++ = '\0';
				break;
			}

			*next_node_index = hs_tokenize(state);
			if (*next_node_index != NONE)
			{
				next_node_index = &hs_syntax_get(*next_node_index)->next_node_index;
			}
		}
		else
		{
			hs_compile_globals.error_message = "this left parenthesis is unmatched.";
			hs_compile_globals.error_offset = expression->source_offset;
		}
	}

	if (next_node_index == &expression->long_value && !hs_compile_globals.error_message)
	{
		hs_compile_globals.error_message = "this expression is empty.";
		hs_compile_globals.error_offset = expression->source_offset;
	}
	return;
}

static bool hs_parse_primitive(int32 expression_index)
{
	hs_syntax_node* expression = hs_syntax_get(expression_index);

	bool result = false;
	ASSERT(hs_type_valid(expression->type) || expression->type == _hs_special_form || expression->type == _hs_unparsed);

	if (expression->type == _hs_special_form)
	{
		hs_compile_globals.error_message = "i expected a script or variable definition.";
		hs_compile_globals.error_offset = expression->source_offset;
	}
	else if (expression->type == _hs_type_void)
	{
		hs_compile_globals.error_message = "the value of this expression (in a <void> slot) can never be used.";
		hs_compile_globals.error_offset = expression->source_offset;
	}
	else
	{
		if (!hs_compile_globals.variables_predetermined || TEST_BIT(expression->flags, _hs_syntax_node_variable_bit))
		{
			result = hs_parse_variable(expression_index);
		}

		if (!result
			&& expression->type
			&& !hs_compile_globals.error_message
			&& (!hs_compile_globals.variables_predetermined || !TEST_BIT(expression->flags, _hs_syntax_node_variable_bit)))
		{
			if (hs_type_primitive_parsers[expression->type])
			{
				result = hs_type_primitive_parsers[expression->type](expression_index);
			}
			else
			{
				csprintf(
					hs_compile_globals.error_buffer,
					NUMBEROF(hs_compile_globals.error_buffer),
					"expressions of type %s are currently unsupported.",
					hs_type_names[expression->type]);
				hs_compile_globals.error_message = hs_compile_globals.error_buffer;
				hs_compile_globals.error_offset = expression->source_offset;
				result = false;
			}
		}
	}
	return result;
}

static bool hs_parse_variable(int32 expression_index)
{
	bool result = false;
	hs_syntax_node* expression = hs_syntax_get(expression_index);

	ASSERT(hs_type_valid(expression->type) || expression->type == _hs_unparsed);

	expression->short_value = hs_find_global_by_name(&hs_compile_globals.compiled_source[expression->source_offset]);
	if (expression->short_value == NONE)
	{
		if (hs_compile_globals.variables_predetermined)
		{
			hs_compile_globals.error_message = "this is not a valid variable name.";
			hs_compile_globals.error_offset = expression->source_offset;
		}
	}
	else
	{
		const int16 type = hs_global_get_type(expression->short_value);
		if (expression->type && !hs_can_cast(type, expression->type))
		{
			csprintf(
				hs_compile_globals.error_buffer,
				NUMBEROF(hs_compile_globals.error_buffer),
				"i expected a value of type %s, but the variable %s has type %s",
				hs_type_names[expression->type],
				hs_global_get_name(expression->short_value),
				hs_type_names[type]);
		}
		else
		{
			if (expression->type == _hs_unparsed)
			{
				expression->type = (e_hs_type)type;
			}

			SET_BIT(expression->flags, _hs_syntax_node_variable_bit, true);
			result = true;
		}
	}

	return result;
}

static void hs_parse_call_predicate(int32 expression_index, bool* is_function)
{
	hs_syntax_node* expression = hs_syntax_get(expression_index);
	hs_syntax_node* predicate = hs_syntax_get(hs_syntax_get(expression_index)->long_value);
	
	if (predicate->type == _hs_function_name)
	{
		ASSERT(predicate->function_index != NONE);
		expression->function_index = predicate->function_index;
	}
	else
	{
		const int16 count = hs_count_children(expression_index);
		expression->function_index = hs_find_function_by_name(
			&hs_compile_globals.compiled_source[predicate->source_offset],
			count - 1);
		predicate->type = _hs_function_name;
		if (expression->function_index == NONE)
		{
			expression->script_index = hs_find_script_by_name(&hs_compile_globals.compiled_source[predicate->source_offset]);
			if (expression->script_index)
			{
				if (is_function)
				{
					*is_function = hs_find_function_by_name(&hs_compile_globals.compiled_source[predicate->source_offset], NONE) != NONE;
				}
			}
			else
			{
				SET_BIT(expression->flags, _hs_syntax_node_script_bit, true);
			}
		}
		else if (is_function)
		{
			*is_function = true;
		}
		predicate->function_index = expression->function_index;
	}
	return;
}

static bool hs_parse_nonprimitive(int32 expression_index)
{
	bool result = false;

	hs_syntax_node* expression = hs_syntax_get(expression_index);
	const int32 index = hs_syntax_get(expression_index)->long_value;
	const hs_syntax_node* stored_expression = hs_syntax_get(index);
	
	ASSERT(hs_type_valid(expression->type) || expression->type == _hs_special_form || expression->type == _hs_unparsed);

	++hs_compile_globals.counter;
	if (TEST_BIT(hs_syntax_get(index)->flags, _hs_syntax_node_primitive_bit))
	{
		if (expression->type == _hs_special_form)
		{
			if (csstricmp(&hs_compile_globals.compiled_source[stored_expression->source_offset], "global"))
			{
				if (csstricmp(&hs_compile_globals.compiled_source[stored_expression->source_offset], "script"))
				{
					hs_compile_globals.error_message = "i expected \"script\" or \"global\".";
					--hs_compile_globals.counter;
					hs_compile_globals.error_offset = stored_expression->source_offset;
				}
				else
				{
					result = hs_add_script(expression_index);
					--hs_compile_globals.counter;
				}
			}
			else
			{
				result = hs_add_global(expression_index);
				--hs_compile_globals.counter;
			}
		}
		else
		{
			bool is_function = false;
			hs_parse_call_predicate(expression_index, &is_function);
			if (expression->script_index == NONE)
			{
				hs_compile_globals.error_message = is_function
					? "wrong number of arguments for function"
					: "this is not a valid function or script name.";
				hs_compile_globals.error_offset = stored_expression->source_offset;
				--hs_compile_globals.counter;
			}
			else if (TEST_BIT(expression->flags, _hs_syntax_node_script_bit))
			{
				hs_script* script = scenario_get_hs_script(global_scenario_get(), expression->script_index);
				ASSERT(script);
				
				if (script->script_type == _hs_script_static || script->script_type == _hs_script_stub)
				{
					if (expression->type && !hs_can_cast(script->return_type, expression->type))
					{
						csprintf(
							hs_compile_globals.error_buffer,
							NUMBEROF(hs_compile_globals.error_buffer),
							"i expected a %s, but this script returns a %s.",
							hs_type_names[expression->type],
							hs_type_names[script->return_type]);
						hs_compile_globals.error_message = hs_compile_globals.error_buffer;
						hs_compile_globals.error_offset = expression->source_offset;
						--hs_compile_globals.counter;
					}
					else
					{
						if (!expression->type)
						{
							expression->type = (e_hs_type)script->return_type;
						}
						result = true;
					}
				}
				else
				{
					hs_compile_globals.error_message = "this is not a static script.";
					hs_compile_globals.error_offset = expression->source_offset;
				}
			}
			else
			{
				const hs_function_definition* function = hs_function_get(expression->function_index);
				if (expression->type && !hs_can_cast(function->return_type, expression->type))
				{
					csprintf(
						hs_compile_globals.error_buffer,
						NUMBEROF(hs_compile_globals.error_buffer),
						"i expected a %s, but this function returns a %s.",
						hs_type_names[expression->type],
						hs_type_names[function->return_type]);
					hs_compile_globals.error_message = hs_compile_globals.error_buffer;
					hs_compile_globals.error_offset = expression->source_offset;
				}
				else if (hs_compile_globals.disallow_blocks
					&& (expression->constant_type == _hs_type_starting_profile || expression->constant_type == _hs_type_navpoint))
				{
					hs_compile_globals.error_message = "it is illegal to block in this context.";
					hs_compile_globals.error_offset = expression->source_offset;
				}
				else if (hs_compile_globals.disallow_sets && expression->constant_type == _hs_type_void)
				{
					hs_compile_globals.error_message = "it is illegal to set the value of variables in this context.";
					hs_compile_globals.error_offset = expression->source_offset;
				}
				else
				{
					if (!expression->type && function->return_type != _hs_passthrough)
					{
						expression->type = function->return_type;
					}

					ASSERT(function->parse);
					result = function->parse(expression->function_index, expression_index);
				}
			}
		}
	}
	else
	{
		const char* string = expression->type == _hs_special_form ? "\"script\" or \"global\"" : "a function name";
		csprintf(hs_compile_globals.error_buffer, NUMBEROF(hs_compile_globals.error_buffer), "i expected %s, but i got an expression.", string);
		hs_compile_globals.error_message = hs_compile_globals.error_buffer;
		hs_compile_globals.error_offset = stored_expression->source_offset;
	}
	--hs_compile_globals.counter;

	return result;
}

static int16 string_list_find(const char* string, int16 list_count, const char** string_list)
{
	int16 result = NONE;
	for (int16 i = 0; i < list_count; ++i)
	{
		if (!csstricmp(string, string_list[i]))
		{
			result = i;
			break;
		}
	}
	return result;
}

static bool hs_add_script(int32 expression_index)
{
	bool result = false;
	const hs_syntax_node* type_expression = hs_syntax_get(expression_index);
	const int32 name_index = hs_syntax_get(type_expression->long_value)->next_node_index;
	if (name_index == NONE)
	{
		hs_compile_globals.error_message = "i expected (script <type> <name> <expression(s)>)";
		hs_compile_globals.error_offset = hs_syntax_get(expression_index)->source_offset;
	}
	else
	{
		const hs_syntax_node* name_expression = hs_syntax_get(name_index);
		const int16 type = string_list_find(&hs_compile_globals.compiled_source[name_expression->source_offset], 6, hs_script_type_names);
		if (type == NONE)
		{
			hs_compile_globals.error_message = "script type must be \"startup\", \"dormant\", \"continuous\", or \"static\".";
			hs_compile_globals.error_offset = hs_syntax_get(name_index)->source_offset;
		}
		else
		{
			int16 return_type = _hs_type_void;
			int32 next_expression_index = NONE;
			if (type == _hs_passthrough || type == _hs_type_void)
			{
				const int32 current_expression_index = hs_syntax_get(name_index)->next_node_index;
				if (current_expression_index == NONE)
				{
					hs_compile_globals.error_message = "i expected (script local <type> <name> <expression(s)>).";
					hs_compile_globals.error_offset = hs_syntax_get(expression_index)->source_offset;
				}
				else
				{
					const hs_syntax_node* expression = hs_syntax_get(current_expression_index);
					return_type = string_list_find(&hs_compile_globals.compiled_source[expression->source_offset], NUMBER_OF_HS_NODE_TYPES, hs_type_names);
					next_expression_index = hs_syntax_get(current_expression_index)->next_node_index;
					if (hs_type_valid(return_type))
					{
						hs_compile_globals.error_message = "this is not a valid return type.";
						hs_compile_globals.error_offset = hs_syntax_get(current_expression_index)->source_offset;
					}
					else
					{
						result = true;
					}
				}
			}
			else
			{
				next_expression_index = hs_syntax_get(name_index)->next_node_index;
				result = true;
			}

			if (result)
			{
				result = false;
				int16 next_node_index;
				if (next_expression_index == NONE ||
					(next_node_index = (int16)hs_syntax_get(next_expression_index)->next_node_index, next_node_index == NONE))
				{
					switch (type)
					{
					case _hs_passthrough:
						hs_compile_globals.error_message = "i expected (script static <type> <name> <expression(s)>)";
						break;
					case _hs_type_void:
						hs_compile_globals.error_message = "i expected (script stub <type> <name> <expression(s)>)";
						break;
					default:
						hs_compile_globals.error_message = "i expected (script <type> <name> <expression(s)>)";
					}
					hs_compile_globals.error_offset = hs_syntax_get(expression_index)->source_offset;
				}
				else
				{
					const char* name = &hs_compile_globals.compiled_source[hs_syntax_get(next_expression_index)->source_offset];
					if (cstrlen(name) && cstrlen(name) < k_script_name_size)
					{
						int16 script_index = hs_find_script_by_name(name);
						
						// TODO: sapien code 

						if (script_index != NONE)
						{
							data_array* g_hs_syntax_data = *hs_syntax_data_get();

							hs_script* script = scenario_get_hs_script(global_scenario_get(), script_index);

							const datum root_expression_index = datum_new(g_hs_syntax_data);
							const datum initialization_value_index = datum_new(g_hs_syntax_data);
							if (root_expression_index == NONE || initialization_value_index == NONE)
							{
								hs_compile_globals.error_message = "i couldn't allocate a syntax node.";
							}
							else
							{
								hs_syntax_node* initialization_expression = hs_syntax_get(root_expression_index);
								hs_syntax_node* initialization_value = hs_syntax_get(initialization_value_index);
								initialization_expression->long_value = initialization_value_index;
								initialization_expression->next_node_index = NONE;
								initialization_expression->source_offset = hs_syntax_get(expression_index)->source_offset;
								initialization_expression->flags = 0;
								initialization_value->next_node_index = next_node_index;
								initialization_value->source_offset = NONE;
								initialization_value->script_index = 0;
								initialization_value->flags = 1;
								initialization_value->type = _hs_function_name;
								if (hs_parse(root_expression_index, return_type))
								{
									csstrncpy(script->name, name, k_script_name_size);
									script->script_type = (e_hs_script_type)type;
									script->return_type = (e_hs_type)return_type;
									script->root_expression_index = root_expression_index;
									result = true;
								}
							}
						}
					}
					else
					{
						hs_compile_globals.error_message = "i expected a script name less than 32 characters.";
						hs_compile_globals.error_offset = hs_syntax_get(next_expression_index)->source_offset;
					}
				}
			}
		}
	}
	return result;
}

static bool hs_add_global(int32 expression_index)
{
	bool result = false;
	const hs_syntax_node* expression = hs_syntax_get(expression_index);
	const int32 global_index = hs_syntax_get(expression->long_value)->next_node_index;

	int32 name_index;
	int32 value_index;
	if (global_index == NONE
		|| (name_index = hs_syntax_get(global_index)->next_node_index, name_index == NONE)
		|| (value_index = hs_syntax_get(name_index)->next_node_index, value_index == NONE)
		|| hs_syntax_get(value_index)->next_node_index != NONE)
	{
		hs_compile_globals.error_message = "i expected (global<type> <name> <initial value>)";
		hs_compile_globals.error_offset = hs_syntax_get(expression_index)->source_offset;
	}
	else
	{
		const hs_syntax_node* global_expression = hs_syntax_get(global_index);
		const int16 type = string_list_find(&hs_compile_globals.compiled_source[global_expression->source_offset], NUMBER_OF_HS_NODE_TYPES, hs_type_names);
		if (!hs_type_valid(type))
		{
			hs_compile_globals.error_message = "this is not a valid type.";
			hs_compile_globals.error_offset = hs_syntax_get(global_index)->source_offset;
		}
		else
		{
			const char* source = &hs_compile_globals.compiled_source[hs_syntax_get(name_index)->source_offset];
			if (cstrlen(source) && cstrlen(source) < k_script_name_size)
			{
				if (hs_find_global_by_name(source) == NONE)
				{
					hs_compile_globals.disallow_blocks = true;
					hs_compile_globals.disallow_sets = true;
					hs_parse(value_index, type);
					hs_compile_globals.disallow_blocks = false;
					hs_compile_globals.disallow_sets = false;
				}
				else
				{
					hs_compile_globals.error_message = "there is already a variable by this name.";
					hs_compile_globals.error_offset = hs_syntax_get(name_index)->source_offset;
				}
			}
			else
			{
				hs_compile_globals.error_message = "i expected a global variable name less than 32 characters.";
				hs_compile_globals.error_offset = hs_syntax_get(name_index)->source_offset;
			}
		}
	}

	return result;
}

static bool hs_get_parameter_indices(const char* function_name, uint16 count, int32* result_indices, int32 expression_index)
{
	bool result = true;
	int32 next_node_index = hs_syntax_get(hs_syntax_get(expression_index)->long_value)->next_node_index;
	
	int32 node_num;
	for (node_num = 0; next_node_index != NONE && node_num < (int32)count; ++node_num)
	{
		result_indices[node_num] = next_node_index;
		next_node_index = hs_syntax_get(next_node_index)->next_node_index;
	}

	if (node_num != count || next_node_index != NONE)
	{
		hs_compile_globals.error_message = csprintf(
			hs_compile_globals.error_buffer,
			NUMBEROF(hs_compile_globals.error_buffer),
			"the %s call requires %d arguments.",
			function_name,
			count);
		hs_compile_globals.error_offset = hs_syntax_get(expression_index)->source_offset;
		result = false;
	}

	return result;
}

static bool hs_parse_tag_block_element(int32 expression_index, int32 offset, int32 tag_index, s_tag_block* block, int32 element_size)
{
	bool result = false;
	hs_syntax_node* expression = hs_syntax_get(expression_index);

	ASSERT(element_size <= SHORT_MAX);
	ASSERT(offset + (k_tag_string_length - 1) < element_size);

	for (int32 block_num = 0; block_num < block->count; ++block_num)
	{
		const char* name = (const char*)((uintptr_t)tag_block_get_element_with_size(block, block_num, element_size) + offset);
		if (!csstricmp(&name[offset], &hs_compile_globals.compiled_source[expression->source_offset]))
		{
			expression->long_value = block_num;
			result = true;
			break;
		}
	}

	if (result)
	{
		if (hs_compile_globals.permanent)
		{
			const int32 global_scenario_index = global_scenario_index_get();
			if (tag_index == global_scenario_index)
			{
				editor_register_script_referenced_block(block);
			}
			else
			{
				tag_group_dependencies_register_dependency(global_scenario_index, tag_index);
			}
		}
	}
	else
	{
		csprintf(hs_compile_globals.error_buffer, NUMBEROF(hs_compile_globals.error_buffer), "this is not a valid %s name", hs_type_names[expression->type]);
		hs_compile_globals.error_message = hs_compile_globals.error_buffer;
		hs_compile_globals.error_offset = expression->source_offset;
	}
	return result;
}

static bool hs_parse_tag_block_element_string_id(int32 expression_index, int32 offset, int32 tag_index, s_tag_block* block, int32 element_size)
{
	bool result = false;
	hs_syntax_node* expression = hs_syntax_get(expression_index);

	ASSERT(element_size <= SHORT_MAX);
	ASSERT(offset + (k_tag_string_length - 1) < element_size);

	for (int32 block_num = 0; block_num < block->count; ++block_num)
	{
		const string_id id = *(string_id*)((uintptr_t)tag_block_get_element_with_size(block, block_num, element_size) + offset);
		if (id == string_id_exists(&hs_compile_globals.compiled_source[expression->source_offset]))
		{
			expression->long_value = block_num;
			result = true;
			break;
		}
	}

	if (result)
	{
		if (hs_compile_globals.permanent)
		{
			const int32 global_scenario_index = global_scenario_index_get();
			if (tag_index == global_scenario_index)
			{
				editor_register_script_referenced_block(block);
			}
			else
			{
				tag_group_dependencies_register_dependency(global_scenario_index, tag_index);
			}
		}
	}
	else
	{
		csprintf(hs_compile_globals.error_buffer, NUMBEROF(hs_compile_globals.error_buffer), "this is not a valid %s name", hs_type_names[expression->type]);
		hs_compile_globals.error_message = hs_compile_globals.error_buffer;
		hs_compile_globals.error_offset = expression->source_offset;
	}
	return result;
}

#endif
