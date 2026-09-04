#include "stdafx.h"
#include "hs_function.h"

#include "hs.h"
#ifdef HS_COMPILER_ENABLED
#include "hs_compile.h"
#endif

#include "hs_runtime.h"

#include "effects/player_effects.h"
#include "game/cheats.h"
#ifdef UI_DEBUG
#include "interface/user_interface.h"
#endif

#include "cartographer/cartographer.h"

#include "camera/director.h"
#include "main/console.h"
#include "main/main.h"
#include "main/main_game.h"
#include "main/main_screenshot.h"
#ifdef HS_COMPILER_ENABLED
#include "networking/network_event.h"
#include "objects/objects.h"
#endif

/* macros */

#ifdef HS_COMPILER_ENABLED
#define HS_FUNCTION_DEFINITION_CREATE(global_name, return_type, name, flags, parse, evaluate, documentation, parameters, formal_parameter_count, ...) \
static const hs_function_definition (global_name) = \
{													\
	return_type,									\
	name,											\
	flags,											\
	parse,											\
	evaluate,										\
	documentation,									\
	parameters,										\
	formal_parameter_count,							\
	{__VA_ARGS__}									\
}
#else
#define HS_FUNCTION_DEFINITION_CREATE(global_name, return_type, name, flags, parse, evaluate, documentation, parameters, formal_parameter_count, ...) \
static const hs_function_definition (global_name) = \
{													\
	return_type,									\
	flags,											\
	evaluate,										\
	parameters,										\
	formal_parameter_count,							\
	{__VA_ARGS__}									\
}
#endif

/* prototypes */

static void __cdecl hs_not_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_pin_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_print_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_players_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl scenario_kill_trigger_volume_enable_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl scenario_kill_trigger_volume_disable_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_teleport_players_not_in_trigger_volume_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl scenario_trigger_volume_test_object_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_trigger_volume_test_objects_any_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_trigger_volume_test_objects_all_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl scenario_trigger_volume_return_objects_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl scenario_trigger_volume_return_objects_by_type_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_object_list_get_element_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl object_list_count_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl object_list_count_not_dead_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_effect_new_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_effect_new_from_object_marker_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_damage_new_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_damage_object_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_damage_object_list_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_damage_players_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_object_create_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_object_create_clone_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_object_create_anew_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_object_create_containing_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_object_create_clone_containing_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_object_create_anew_containing_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_object_destroy_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_object_destroy_containing_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_object_destroy_all_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_object_destroy_all_type_mask_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_objects_delete_by_definition_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_object_hide_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl hs_object_set_shadowless_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl scripted_object_function_set_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl object_scripting_set_function_variable_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl object_scripting_clear_function_variable_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl object_scripting_clear_all_function_variables_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl object_scripting_dynamic_simulation_disable_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl object_scripting_set_phantom_power_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl object_scripting_wake_physics_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl object_set_ranged_attack_inhibited_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void __cdecl object_set_melee_attack_inhibited_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void objects_dump_memory_evaluate(int16 function_index, int32 thread_index, bool initialize);


static void hs_help_evaluate(int16 function_index, int32 thread_index, bool initialize);


static void cheat_drop_tag_name_evaluate(int16 function_index, int32 thread_index, bool initialize);


static void main_crash_evaluate(int16 function_index, int32 thread_index, bool initialize);
static void main_print_version_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void screenshot_cubemap_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void main_menu_launch_force_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void main_halt_and_display_errors_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void main_game_launch_legacy_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void main_game_launch_set_multiplayer_engine_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void main_game_launch_set_multiplayer_splitscreen_count_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void main_game_launch_set_difficulty_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void main_game_launch_set_coop_player_count_evaluate(int16 function_index, int32 thread_index, bool initialize);


static void scripted_player_effect_screen_fade_in_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void main_load_core_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void main_load_core_name_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void main_save_core_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void main_save_core_name_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void main_game_load_from_core_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void main_game_load_from_core_name_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void user_interface_enter_game_shell_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void debug_enable_text_bounds_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void debug_render_title_safe_bounds_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void set_debug_frame_element_bounds_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void user_interface_test_screen_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void user_interface_test_transition_out_console_screen_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void user_interface_debug_show_screen_tag_path_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void user_interface_debug_show_current_screen_tag_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void user_interface_debug_set_beta_evaluate(int16 function_index, int32 thread_index, bool initialize);


static void net_event_set_display_level_by_name_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void net_event_set_log_level_by_name_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void net_event_dump_categories_evaluate(int16 function_index, int32 thread_index, bool initialize);


static void user_interface_test_error_ok_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void user_interface_test_error_ok_cancel_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void user_interface_test_confirmation_evaluate(int16 function_index, int32 thread_index, bool initialize);

static void director_debug_camera_evaluate(int16 function_index, int32 thread_index, bool initialize);

/* globals */

extern void __cdecl hs_evaluate_begin(int16 function_index, int32 thread_index, bool initialize);

HS_FUNCTION_DEFINITION_CREATE(
	begin_definition,
	_hs_passthrough,
	"begin",
	2,
	&hs_parse_begin,
	&hs_evaluate_begin,
	"returns the last expression in a sequence after evaluating the sequence in order.",
	"<expression(s)>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	begin_random_definition,
	_hs_passthrough,
	"begin_random",
	2,
	&hs_parse_begin,
	&hs_evaluate_begin_random,
	"evaluates the sequence of expressions in random order and returns the last value evaluated.",
	"<expression(s)>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	if_definition,
	_hs_passthrough,
	"if",
	2,
	&hs_parse_if,
	&hs_evaluate_if,
	"returns one of two values based on the value of a condition.",
	"<boolean> <then> [<else>]",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	cond_definition,
	_hs_passthrough,
	"cond",
	2,
	&hs_parse_cond,
	NULL,
	"returns the value associated with the first true condition.",
	"(<boolean1> <result1>) [(<boolean2> <result2>) [...]]",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	set_definition,
	_hs_passthrough,
	"set",
	2,
	&hs_parse_set,
	&hs_evaluate_set,
	"set the value of a global variable.",
	"<variable name> <expression>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	and_definition,
	_hs_type_boolean,
	"and",
	2,
	&hs_parse_logical,
	&hs_evaluate_logical,
	"returns true if all specified expressions are true.",
	"<boolean(s)>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	or_definition,
	_hs_type_boolean,
	"or",
	2,
	&hs_parse_logical,
	&hs_evaluate_logical,
	"returns true if any specified expressions are true.",
	"<boolean(s)>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	add_definition,
	_hs_type_real,
	"+",
	2,
	&hs_parse_arithmetic,
	&hs_evaluate_arithmetic,
	"returns the sum of all specified expressions.",
	"<number(s)>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	subtract_definition,
	_hs_type_real,
	"-",
	2,
	&hs_parse_arithmetic,
	&hs_evaluate_arithmetic,
	"returns the difference of two expressions.",
	"<number> <number>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	multiply_definition,
	_hs_type_real,
	"*",
	2,
	&hs_parse_arithmetic,
	&hs_evaluate_arithmetic,
	"returns the product of all specified expressions.",
	"<number(s)>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	divide_definition,
	_hs_type_real,
	"/",
	2,
	&hs_parse_arithmetic,
	&hs_evaluate_arithmetic,
	"returns the quotient of two expressions.",
	"<number> <number>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	min_definition,
	_hs_type_real,
	"min",
	2,
	&hs_parse_arithmetic,
	&hs_evaluate_arithmetic,
	"returns the minimum of all specified expressions.",
	"<number(s)>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	max_definition,
	_hs_type_real,
	"max",
	2,
	&hs_parse_arithmetic,
	&hs_evaluate_arithmetic,
	"returns the maximum of all specified expressions.",
	"<number(s)>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	equal_definition,
	_hs_type_boolean,
	"=",
	2,
	&hs_parse_equality,
	&hs_evaluate_equality,
	"returns true if two expressions are equal",
	"<expression> <expression>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	not_equal_definition,
	_hs_type_boolean,
	"!=",
	2,
	&hs_parse_equality,
	&hs_evaluate_equality,
	"returns true if two expressions are not equal",
	"<expression> <expression>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	gt_definition,
	_hs_type_boolean,
	">",
	2,
	&hs_parse_inequality,
	&hs_evaluate_inequality,
	"returns true if the first number is larger than the second.",
	"<number> <number>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	lt_definition,
	_hs_type_boolean,
	"<",
	2,
	&hs_parse_inequality,
	&hs_evaluate_inequality,
	"returns true if the first number is smaller than the second.",
	"<number> <number>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	gte_definition,
	_hs_type_boolean,
	">=",
	2,
	&hs_parse_inequality,
	&hs_evaluate_inequality,
	"returns true if the first number is larger than or equal to the second.",
	"<number> <number>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	lte_definition,
	_hs_type_boolean,
	"<=",
	2,
	&hs_parse_inequality,
	&hs_evaluate_inequality,
	"returns true if the first number is smaller than or equal to the second.",
	"<number> <number>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	sleep_definition,
	_hs_type_void,
	"sleep",
	2,
	&hs_parse_sleep,
	&hs_evaluate_sleep,
	"pauses execution of this script (or, optionally, another script) for the specified number of ticks.",
	"<short> [<script>]",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	sleep_forever_definition,
	_hs_type_void,
	"sleep_forever",
	2,
	&hs_parse_sleep_forever,
	&hs_evaluate_sleep_forever,
	"pauses execution of this script (or, optionally, another script) forever.",
	"[<script>]",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	sleep_until_definition,
	_hs_type_void,
	"sleep_until",
	2,
	&hs_parse_sleep_until,
	&hs_evaluate_sleep_until,
	"pauses execution of this script until the specified condition is true, checking once per second unless a different number of ticks is specified.",
	"<boolean> [<short>]",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	wake_definition,
	_hs_type_void,
	"wake",
	2,
	&hs_parse_wake,
	&hs_evaluate_wake,
	"wakes a sleeping script in the next update.",
	"<script name>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	inspect_definition,
	_hs_type_void,
	"inspect",
	2,
	&hs_parse_inspect,
	&hs_evaluate_inspect,
	"prints the value of an expression to the screen for debugging purposes.",
	"<expression>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	object_to_unit_definition,
	_hs_type_unit,
	"unit",
	2,
	&hs_parse_object_cast_up,
	&hs_evaluate_object_cast_up,
	"converts an object to a unit.",
	"<object>",
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	not_definition,
	_hs_type_boolean,
	"not",
	0,
	&hs_macro_function_parse,
	&hs_not_evaluate,
	"returns the opposite of the expression.",
	NULL,
	1,
	_hs_type_boolean
);

HS_FUNCTION_DEFINITION_CREATE(
	pin_definition,
	_hs_type_real,
	"pin",
	0,
	&hs_macro_function_parse,
	&hs_pin_evaluate,
	"returns the first value pinned between the second two",
	NULL,
	3,
	_hs_type_real, _hs_type_real, _hs_type_real
);

HS_FUNCTION_DEFINITION_CREATE(
	print_definition,
	_hs_type_void,
	"print",
	0,
	&hs_macro_function_parse,
	&hs_print_evaluate,
	"prints a string to the console.",
	NULL,
	1,
	_hs_type_string
);

HS_FUNCTION_DEFINITION_CREATE(
	players_definition,
	_hs_type_object_list,
	"players",
	0,
	&hs_macro_function_parse,
	&hs_players_evaluate,
	"returns a list of the players",
	NULL,
	0,
	_hs_unparsed
);

	
HS_FUNCTION_DEFINITION_CREATE(
	kill_volume_enable_definition,
	_hs_type_void,
	"kill_volume_enable",
	0,
	&hs_macro_function_parse,
	&scenario_kill_trigger_volume_enable_evaluate,
	"enables a kill volume",
	NULL,
	1,
	_hs_type_trigger_volume
);

HS_FUNCTION_DEFINITION_CREATE(
	kill_volume_disable_definition,
	_hs_type_void,
	"kill_volume_disable",
	0,
	&hs_macro_function_parse,
	&scenario_kill_trigger_volume_disable_evaluate,
	"disables a kill volume",
	NULL,
	1,
	_hs_type_trigger_volume
);


HS_FUNCTION_DEFINITION_CREATE(
	volume_teleport_players_not_inside_definition,
	_hs_type_void,
	"volume_teleport_players_not_inside",
	0,
	&hs_macro_function_parse,
	&hs_teleport_players_not_in_trigger_volume_evaluate,
	"moves all players outside a specified trigger volume to a specified flag.",
	NULL,
	2,
	_hs_type_trigger_volume, _hs_type_cutscene_flag
);

HS_FUNCTION_DEFINITION_CREATE(
	volume_test_object_definition,
	_hs_type_boolean,
	"volume_test_object",
	0,
	&hs_macro_function_parse,
	&scenario_trigger_volume_test_object_evaluate,
	"returns true if the specified object is within the specified volume.",
	NULL,
	2,
	_hs_type_trigger_volume, _hs_type_object
);

HS_FUNCTION_DEFINITION_CREATE(
	volume_test_objects_definition,
	_hs_type_boolean,
	"volume_test_objects",
	0,
	&hs_macro_function_parse,
	&hs_trigger_volume_test_objects_any_evaluate,
	"returns true if any of the specified objects are within the specified volume. trigger volume must have been postprocessed",
	NULL,
	2,
	_hs_type_trigger_volume, _hs_type_object_list
);

HS_FUNCTION_DEFINITION_CREATE(
	volume_test_objects_all_definition,
	_hs_type_boolean,
	"volume_test_objects_all",
	0,
	&hs_macro_function_parse,
	&hs_trigger_volume_test_objects_all_evaluate,
	"returns true if any of the specified objects are within the specified volume. trigger volume must have been postprocessed",
	NULL,
	2,
	_hs_type_trigger_volume, _hs_type_object_list
);

HS_FUNCTION_DEFINITION_CREATE(
	volume_return_objects_definition,
	_hs_type_object_list,
	"volume_return_objects",
	0,
	&hs_macro_function_parse,
	&scenario_trigger_volume_return_objects_evaluate,
	"returns list of objects in volume or (max 128).",
	NULL,
	1,
	_hs_type_trigger_volume
);

HS_FUNCTION_DEFINITION_CREATE(
	volume_return_objects_by_type_definition,
	_hs_type_object_list,
	"volume_return_objects_by_type",
	0,
	&hs_macro_function_parse,
	&scenario_trigger_volume_return_objects_by_type_evaluate,
	"returns list of objects in volume or (max 128).",
	NULL,
	2,
	_hs_type_trigger_volume, _hs_type_long_integer
);

HS_FUNCTION_DEFINITION_CREATE(
	list_get_definition,
	_hs_type_object,
	"list_get",
	0,
	&hs_macro_function_parse,
	&hs_object_list_get_element_evaluate,
	"returns an item in an object list.",
	NULL,
	2,
	_hs_type_object_list, _hs_type_short_integer
);

HS_FUNCTION_DEFINITION_CREATE(
	list_count_definition,
	_hs_type_short_integer,
	"list_count",
	0,
	&hs_macro_function_parse,
	&object_list_count_evaluate,
	"returns the number of objects in a list",
	NULL,
	1,
	_hs_type_object_list
);

HS_FUNCTION_DEFINITION_CREATE(
	list_count_not_dead_definition,
	_hs_type_short_integer,
	"list_count_not_dead",
	0,
	&hs_macro_function_parse,
	&object_list_count_not_dead_evaluate,
	"returns the number of objects in a list that aren't dead",
	NULL,
	1,
	_hs_type_object_list
);

HS_FUNCTION_DEFINITION_CREATE(
	effect_new_definition,
	_hs_type_void,
	"effect_new",
	0,
	&hs_macro_function_parse,
	&hs_effect_new_evaluate,
	"starts the specified effect at the specified flag.",
	NULL,
	2,
	_hs_type_effect, _hs_type_cutscene_flag
);

HS_FUNCTION_DEFINITION_CREATE(
	effect_new_on_object_marker_definition,
	_hs_type_void,
	"effect_new_on_object_marker",
	0,
	&hs_macro_function_parse,
	&hs_effect_new_from_object_marker_evaluate,
	"starts the specified effect on the specified object at the specified marker.",
	NULL,
	3,
	_hs_type_effect, _hs_type_object, _hs_type_string_id
);

HS_FUNCTION_DEFINITION_CREATE(
	damage_new_definition,
	_hs_type_void,
	"damage_new",
	0,
	&hs_macro_function_parse,
	&hs_damage_new_evaluate,
	"causes the specified damage at the specified flag.",
	NULL,
	2,
	_hs_type_damage, _hs_type_cutscene_flag
);

HS_FUNCTION_DEFINITION_CREATE(
	damage_object_definition,
	_hs_type_void,
	"damage_object",
	0,
	&hs_macro_function_parse,
	&hs_damage_object_evaluate,
	"causes the specified damage at the specified object.",
	NULL,
	2,
	_hs_type_damage, _hs_type_object
);

HS_FUNCTION_DEFINITION_CREATE(
	damage_objects_definition,
	_hs_type_void,
	"damage_objects",
	0,
	&hs_macro_function_parse,
	&hs_damage_object_list_evaluate,
	"causes the specified damage at the specified object list.",
	NULL,
	2,
	_hs_type_damage, _hs_type_object_list
);

HS_FUNCTION_DEFINITION_CREATE(
	damage_players_definition,
	_hs_type_void,
	"damage_players",
	0,
	&hs_macro_function_parse,
	&hs_damage_players_evaluate,
	"damages all players with the given damage effect",
	NULL,
	1,
	_hs_type_damage
);

HS_FUNCTION_DEFINITION_CREATE(
	object_create_definition,
	_hs_type_void,
	"object_create",
	0,
	&hs_macro_function_parse,
	&hs_object_create_evaluate,
	"creates an object from the scenario.",
	NULL,
	1,
	_hs_type_object_name
);

HS_FUNCTION_DEFINITION_CREATE(
	object_create_clone_definition,
	_hs_type_void,
	"object_create_clone",
	0,
	&hs_macro_function_parse,
	&hs_object_create_clone_evaluate,
	"creates an object, potentially resulting in multiple objects if it already exists.",
	NULL,
	1,
	_hs_type_object_name
);

HS_FUNCTION_DEFINITION_CREATE(
	object_create_anew_definition,
	_hs_type_void,
	"object_create_anew",
	0,
	&hs_macro_function_parse,
	&hs_object_create_anew_evaluate,
	"creates an object, destroying it first if it already exists.",
	NULL,
	1,
	_hs_type_object_name
);

HS_FUNCTION_DEFINITION_CREATE(
	object_create_containing_definition,
	_hs_type_void,
	"object_create_containing",
	0,
	&hs_macro_function_parse,
	&hs_object_create_containing_evaluate,
	"creates all objects from the scenario whose names contain the given substring.",
	NULL,
	1,
	_hs_type_string
);

HS_FUNCTION_DEFINITION_CREATE(
	object_create_clone_containing_definition,
	_hs_type_void,
	"object_create_clone_containing",
	0,
	&hs_macro_function_parse,
	&hs_object_create_clone_containing_evaluate,
	"creates clones for all objects from the scenario whose names contain the given substring.",
	NULL,
	1,
	_hs_type_string
);

HS_FUNCTION_DEFINITION_CREATE(
	object_create_anew_containing_definition,
	_hs_type_void,
	"object_create_anew_containing",
	0,
	&hs_macro_function_parse,
	&hs_object_create_anew_containing_evaluate,
	"creates anew all objects from the scenario whose names contain the given substring.",
	NULL,
	1,
	_hs_type_string
);

HS_FUNCTION_DEFINITION_CREATE(
	object_destroy_definition,
	_hs_type_void,
	"object_destroy",
	0,
	&hs_macro_function_parse,
	&hs_object_destroy_evaluate,
	"destroys an object.",
	NULL,
	1,
	_hs_type_object
);

HS_FUNCTION_DEFINITION_CREATE(
	object_destroy_containing_definition,
	_hs_type_void,
	"object_destroy_containing",
	0,
	&hs_macro_function_parse,
	&hs_object_destroy_containing_evaluate,
	"destroys all objects from the scenario whose names contain the given substring.",
	NULL,
	1,
	_hs_type_string
);

HS_FUNCTION_DEFINITION_CREATE(
	object_destroy_all_definition,
	_hs_type_void,
	"object_destroy_all",
	0,
	&hs_macro_function_parse,
	&hs_object_destroy_all_evaluate,
	"destroys all non player objects.",
	NULL,
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	object_destroy_type_mask_definition,
	_hs_type_void,
	"object_destroy_type_mask",
	0,
	&hs_macro_function_parse,
	&hs_object_destroy_all_type_mask_evaluate,
	"destroys all objects matching the type mask",
	NULL,
	1,
	_hs_type_long_integer
);

HS_FUNCTION_DEFINITION_CREATE(
	objects_delete_by_definition_definition,
	_hs_type_void,
	"objects_delete_by_definition",
	0,
	&hs_macro_function_parse,
	&hs_objects_delete_by_definition_evaluate,
	"deletes all objects of type <definition>",
	NULL,
	1,
	_hs_type_object_definition
);

HS_FUNCTION_DEFINITION_CREATE(
	object_hide_definition,
	_hs_type_void,
	"object_hide",
	0,
	&hs_macro_function_parse,
	&hs_object_hide_evaluate,
	"hides or shows the object passed in",
	NULL,
	2,
	_hs_type_object, _hs_type_boolean
);

HS_FUNCTION_DEFINITION_CREATE(
	object_set_shadowless_definition,
	_hs_type_void,
	"object_set_shadowless",
	0,
	&hs_macro_function_parse,
	&hs_object_set_shadowless_evaluate,
	"set/reset shadow castingness of object",
	NULL,
	2,
	_hs_type_object, _hs_type_boolean
);

HS_FUNCTION_DEFINITION_CREATE(
	object_function_set_definition,
	_hs_type_void,
	"object_function_set",
	0,
	&hs_macro_function_parse,
	&scripted_object_function_set_evaluate,
	"sets a global object function (0-3) to value",
	NULL,
	2,
	_hs_type_long_integer, _hs_type_real
);

HS_FUNCTION_DEFINITION_CREATE(
	object_set_function_variable_definition,
	_hs_type_void,
	"object_set_function_variable",
	0,
	&hs_macro_function_parse,
	&object_scripting_set_function_variable_evaluate,
	"sets funciton variable for sin-o-matic use",
	NULL,
	4,
	_hs_type_object, _hs_type_string_id, _hs_type_real, _hs_type_real
);

HS_FUNCTION_DEFINITION_CREATE(
	object_clear_function_variable_definition,
	_hs_type_void,
	"object_clear_function_variable",
	0,
	&hs_macro_function_parse,
	&object_scripting_clear_function_variable_evaluate,
	"clears one funciton variables for sin-o-matic use",
	NULL,
	2,
	_hs_type_object, _hs_type_string_id
);

HS_FUNCTION_DEFINITION_CREATE(
	object_clear_all_function_variables_definition,
	_hs_type_void,
	"object_clear_all_function_variables",
	0,
	&hs_macro_function_parse,
	&object_scripting_clear_all_function_variables_evaluate,
	"clears all funciton variables for sin-o-matic use",
	NULL,
	1,
	_hs_type_object
);

HS_FUNCTION_DEFINITION_CREATE(
	object_dynamic_simulation_disable_definition,
	_hs_type_void,
	"object_dynamic_simulation_disable",
	0,
	&hs_macro_function_parse,
	&object_scripting_dynamic_simulation_disable_evaluate,
	"disabled dynamic simulation for this object (makes it fixed)",
	NULL,
	2,
	_hs_type_object, _hs_type_boolean
);

HS_FUNCTION_DEFINITION_CREATE(
	object_set_phantom_power_definition,
	_hs_type_void,
	"object_set_phantom_power",
	0,
	&hs_macro_function_parse,
	&object_scripting_set_phantom_power_evaluate,
	"sets phantom power to be latched at 1.0f or 0.0f",
	NULL,
	2,
	_hs_type_object, _hs_type_boolean
);

HS_FUNCTION_DEFINITION_CREATE(
	object_wake_physics_definition,
	_hs_type_void,
	"object_wake_physics",
	0,
	&hs_macro_function_parse,
	&object_scripting_wake_physics_evaluate,
	"wakes physics of an object.  For example it would make an usupported crate fall",
	NULL,
	1,
	_hs_type_object
);

HS_FUNCTION_DEFINITION_CREATE(
	object_set_ranged_attack_inhibited_definition,
	_hs_type_void,
	"object_set_ranged_attack_inhibited",
	0,
	&hs_macro_function_parse,
	&object_set_ranged_attack_inhibited_evaluate,
	"FALSE prevents object from using ranged attack",
	NULL,
	2,
	_hs_type_object, _hs_type_boolean
);

HS_FUNCTION_DEFINITION_CREATE(
	object_set_melee_attack_inhibited_definition,
	_hs_type_void,
	"object_set_melee_attack_inhibited",
	0,
	&hs_macro_function_parse,
	&object_set_melee_attack_inhibited_evaluate,
	"FALSE prevents object from using melee attack",
	NULL,
	2,
	_hs_type_object, _hs_type_boolean
);

HS_FUNCTION_DEFINITION_CREATE(
	objects_dump_memory_definition,
	_hs_type_void,
	"objects_dump_memory",
	0,
	&hs_macro_function_parse,
	&objects_dump_memory_evaluate,
	"debugs object memory usage",
	NULL,
	0,
	_hs_unparsed
);

/*
HS_FUNCTION_DEFINITION_CREATE(
	object_get_health_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_get_shield_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_set_shield_effect_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_set_physics_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_get_parent_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	objects_attach_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_at_marker_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	objects_detach_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_set_scale_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_set_velocity_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_object_set_velocity_definition_79,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_set_deleted_when_deactivated_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_copy_player_appearance_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_model_target_destroyed_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_model_targets_destroyed_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_damage_damage_section_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_cannot_die_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_vitality_pinned_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	garbage_collect_now_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	garbage_collect_unsafe_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	garbage_collect_multiplayer_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_cannot_take_damage_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_can_take_damage_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_cinematic_lod_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_cinematic_collision_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_cinematic_visibility_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_uses_cinematic_lighting_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_lighting_set_primary_light_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_lighting_set_secondary_light_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_lighting_set_ambient_light_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	objects_predict_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	objects_predict_high_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	objects_predict_low_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_type_predict_high_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_type_predict_low_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_type_predict_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_teleport_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_set_facing_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_set_shield_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_set_shield_stun_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_set_shield_stun_infinite_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_set_permutation_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_set_region_state_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	objects_can_see_object_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	objects_can_see_flag_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	objects_distance_to_object_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	objects_distance_to_flag_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	map_info_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	position_predict_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	shader_predict_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	bitmap_predict_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	script_recompile_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	script_doc_definition,
*/

HS_FUNCTION_DEFINITION_CREATE(
	help_definition,
	_hs_type_void,
	"help",
	0,
	&hs_macro_function_parse,
	&hs_help_evaluate,
	"prints a description of the named function.",
	NULL,
	1,
	_hs_type_string
);

/*
HS_FUNCTION_DEFINITION_CREATE(
	game_engine_objects_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	random_range_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	real_random_range_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	physics_constants_reset_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	physics_set_gravity_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	physics_set_velocity_frame_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	physics_disable_character_ground_adhesion_forces_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	havok_debug_start_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	havok_dump_world_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	havok_dump_world_close_movie_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	havok_profile_start_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	havok_profile_end_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	havok_reset_allocated_state_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	breakable_surfaces_enable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	breakable_surfaces_reset_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	recording_play_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	recording_play_and_delete_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	recording_play_and_hover_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	recording_kill_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	recording_time_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	render_lights_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	texture_camera_set_object_marker_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	texture_camera_off_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	render_lights_enable_cinematic_shadow_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_report_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_report_summary_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_report_verbose_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_mode_control_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_off_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_mode_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_mode_off_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_atmospheric_fog_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_atmospheric_fog_off_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_secondary_fog_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_secondary_fog_off_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_sky_fog_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_sky_fog_off_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_planar_fog_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_planar_fog_off_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_planar_fog_plane_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_planar_fog_plane_off_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_planar_max_atmospheric_depth_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_planar_max_atmospheric_depth_off_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_planar_planar_eye_density_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_planar_planar_eye_density_off_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_atmospheric_secondary_blend_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_atmospheric_secondary_blend_off_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_atmospheric_planar_blend_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	fog_override_atmospheric_planar_blend_off_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	render_debug_structure_cluster_visibility_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	render_debug_structure_cluster_fog_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	render_debug_structure_fog_plane_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	render_debug_structure_fog_plane_infinite_extent_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	render_debug_structure_fog_zone_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	render_debug_structure_fog_zone_floodfill_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	render_debug_structure_all_fog_planes_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	render_debug_structure_all_cluster_errors_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	render_debug_structure_line_opacity_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	render_debug_structure_text_opacity_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	render_debug_structure_opacity_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	render_debug_structure_non_occluded_fog_planes_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	scenery_animation_start_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	scenery_animation_start_loop_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	scenery_animation_start_relative_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	scenery_animation_start_relative_loop_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	scenery_animation_start_at_frame_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	scenery_animation_idle_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	scenery_get_animation_time_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	render_effects_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_can_blink_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_set_active_camo_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_open_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_close_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_kill_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_kill_silent_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_is_emitting_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_get_custom_animation_time_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_stop_custom_animation_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	custom_animation_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	custom_animation_loop_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	custom_animation_relative_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	custom_animation_relative_loop_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	custom_animation_list_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_custom_animation_at_frame_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_is_playing_custom_animation_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_set_custom_animations_hold_on_last_frame_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_set_custom_animations_prevent_lipsync_head_movement_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_set_actively_controlled_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_get_team_index_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_aim_without_turning_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_set_enterable_by_player_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_get_enterable_by_player_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_only_takes_damage_from_players_team_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_enter_vehicle_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_board_vehicle_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_set_emotion_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_set_emotion_animation_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_set_emotional_state_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_enable_eye_tracking_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_in_vehicle_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	vehicle_test_seat_list_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	vehicle_test_seat_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_set_prefer_tight_camera_track_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_exit_vehicle_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_unit_exit_vehicle_definition_227,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_set_maximum_vitality_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	units_set_maximum_vitality_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_set_current_vitality_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	units_set_current_vitality_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	vehicle_load_magic_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	vehicle_unload_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_set_animation_mode_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	magic_melee_attack_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	vehicle_riders_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	vehicle_driver_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	vehicle_gunner_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_get_health_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_get_shield_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_get_total_grenade_count_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_has_weapon_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_has_weapon_readied_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_lower_weapon_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_raise_weapon_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_animation_forced_seat_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_doesnt_drop_items_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_impervious_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_suspended_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	unit_add_equipment_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	weapon_hold_trigger_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	weapon_enable_warthog_chaingun_light_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	device_set_never_appears_locked_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	device_set_power_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	device_get_power_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	device_set_position_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	device_get_position_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	device_set_position_immediate_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	device_group_get_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	device_group_set_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	device_group_set_immediate_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	device_one_sided_set_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	device_operates_automatically_set_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	device_closes_automatically_set_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	device_group_change_only_once_more_set_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	device_set_position_track_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	device_set_overlay_track_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	device_animate_position_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	device_animate_overlay_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cheat_all_powerups_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cheat_all_weapons_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cheat_all_vehicles_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cheat_teleport_to_camera_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cheat_active_camouflage_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cheat_active_camouflage_by_player_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cheats_load_definition,
*/

HS_FUNCTION_DEFINITION_CREATE(
	drop_definition,
	_hs_type_void,
	"drop",
	0,
	&hs_macro_function_parse,
	&cheat_drop_tag_name_evaluate,
	"drops the named tag e.g. objects\\vehicles\\banshee\\banshee.vehicle",
	NULL,
	1,
	_hs_type_string
);

/*
HS_FUNCTION_DEFINITION_CREATE(
	ai_enable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_enabled_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_grenades_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_dialogue_enable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_dialogue_log_reset_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_dialogue_log_dump_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_get_object_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_get_unit_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_attach_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_attach_units_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_detach_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_detach_units_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_place_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_ai_place_definition_291,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_place_in_vehicle_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_cannot_die_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_vitality_pinned_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_overcomes_oversteer_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_kill_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_kill_silent_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_erase_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_erase_all_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_disposable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_select_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_deselect_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_set_deaf_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_set_blind_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_magically_see_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_magically_see_object_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_set_active_camo_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_suppress_combat_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_migrate_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_allegiance_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_allegiance_remove_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_braindead_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_braindead_by_unit_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_disregard_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_prefer_target_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_teleport_to_starting_location_if_outside_bsp_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_renew_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_force_active_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_force_active_by_unit_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_playfight_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_reconnect_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_berserk_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_set_team_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_allow_dormant_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_is_attacking_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_fighting_count_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_living_count_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_living_fraction_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_strength_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_swarm_count_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_nonswarm_count_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_actors_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_allegiance_broken_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_set_orders_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_spawn_count_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_get_ai_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_trigger_test_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	generate_pathfinding_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_render_paths_all_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_vehicle_get_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_vehicle_get_from_starting_location_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_vehicle_reserve_seat_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_vehicle_reserve_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_vehicle_enter_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_ai_vehicle_enter_definition_345,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_vehicle_enter_immediate_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_ai_vehicle_enter_immediate_definition_347,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_enter_squad_vehicles_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_vehicle_exit_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_ai_vehicle_exit_definition_350,
);

HS_FUNCTION_DEFINITION_CREATE(
	vehicle_overturned_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	vehicle_flip_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_combat_status_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	flock_start_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	flock_stop_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	flock_create_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	flock_delete_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_verify_tags_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_wall_lean_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_play_line_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_play_line_at_player_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_play_line_on_object_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_scene_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_ai_scene_definition_364,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_ai_scene_definition_365,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_run_command_script_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_queue_command_script_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_stack_command_script_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_run_joint_command_script_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_cs_run_joint_command_script_definition_370,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_command_script_running_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_command_script_queued_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_number_queued_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_switch_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_switch_index_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_transfer_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_transfer_stack_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_transfer_queue_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_fly_to_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_cs_fly_to_definition_380,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_fly_to_and_face_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_cs_fly_to_and_face_definition_382,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_fly_by_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_cs_fly_by_definition_384,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_go_to_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_cs_go_to_definition_386,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_go_by_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_cs_go_by_definition_388,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_go_to_and_face_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_start_to_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_go_to_nearest_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_moving_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_look_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_look_player_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_look_object_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_aim_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_aim_player_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_aim_object_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_face_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_face_player_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_face_object_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_move_in_direction_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_pause_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_shoot_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_cs_shoot_definition_405,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_shoot_point_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_vehicle_speed_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_grenade_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_jump_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_jump_to_point_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_vocalize_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_play_sound_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_cs_play_sound_definition_413,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_cs_play_sound_definition_414,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_stop_sound_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_custom_animation_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_stop_custom_animation_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_play_line_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_die_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_teleport_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_animate_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_movement_mode_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_crouch_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_cs_crouch_definition_424,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_set_pathfinding_radius_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_go_to_vehicle_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_set_behavior_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_formation_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_deploy_turret_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_approach_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_start_approach_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_approach_player_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_start_approach_player_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_approach_stop_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_ignore_obstacles_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_turn_sharpness_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_vehicle_speed_instantaneous_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_vehicle_boost_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_abort_on_alert_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_abort_on_damage_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_abort_on_combat_status_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_enable_targeting_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_enable_looking_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_enable_moving_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_enable_dialogue_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_suppress_dialogue_global_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_set_style_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_force_combat_status_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cs_enable_pathfinding_failsafe_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	camera_control_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	camera_set_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	camera_set_relative_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	camera_set_animation_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	camera_set_animation_relative_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	camera_predict_resources_at_frame_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	camera_predict_resources_at_point_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	camera_set_first_person_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	camera_time_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	camera_set_field_of_view_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	camera_set_pan_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	camera_pan_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	debug_camera_save_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	debug_camera_load_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	debug_camera_save_name_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	debug_camera_load_name_definition,
);
*/
HS_FUNCTION_DEFINITION_CREATE(
	director_debug_camera_definition,
	_hs_type_void,
	"director_debug_camera",
	0,
	&hs_macro_function_parse,
	&director_debug_camera_evaluate,
	"toggles the editor flying camera",
	NULL,
	1,
	_hs_type_boolean
);
/*
HS_FUNCTION_DEFINITION_CREATE(
	game_difficulty_get_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_difficulty_get_real_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	pvs_set_object_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	pvs_set_camera_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	pvs_clear_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	players_unzoom_all_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_enable_input_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_disable_movement_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_flashlight_on_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_active_camouflage_on_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_camera_control_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_reset_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_jump_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_primary_trigger_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_grenade_trigger_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_vision_trigger_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_zoom_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_rotate_weapons_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_rotate_grenades_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_melee_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_action_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_accept_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_cancel_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_look_relative_up_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_look_relative_down_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_look_relative_left_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_look_relative_right_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_look_relative_all_directions_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_move_relative_all_directions_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_start_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_back_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player0_looking_up_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player0_looking_down_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_look_up_begin_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_look_down_begin_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_look_pitch_end_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_lookstick_forward_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_action_test_lookstick_backward_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	debug_teleport_player_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	map_reset_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	switch_bsp_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	switch_bsp_by_name_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	structure_bsp_index_definition,
*/

HS_FUNCTION_DEFINITION_CREATE(
	crash_definition,
	_hs_type_void,
	"crash",
	0,
	&hs_macro_function_parse,
	&main_crash_evaluate,
	"crashes (for debugging).",
	NULL,
	1,
	_hs_type_string
);

HS_FUNCTION_DEFINITION_CREATE(
	version_definition,
	_hs_type_void,
	"version",
	0,
	&hs_macro_function_parse,
	&main_print_version_evaluate,
	"prints the build version.",
	NULL,
	0,
	_hs_unparsed
);

/*
HS_FUNCTION_DEFINITION_CREATE(
	status_definition,
HS_FUNCTION_DEFINITION_CREATE(
	record_movie_definition,
HS_FUNCTION_DEFINITION_CREATE(
	record_movie_distributed_definition,
HS_FUNCTION_DEFINITION_CREATE(
	screenshot_definition,
HS_FUNCTION_DEFINITION_CREATE(
	screenshot_big_definition,
HS_FUNCTION_DEFINITION_CREATE(
	screenshot_big_jittered_definition,
*/

HS_FUNCTION_DEFINITION_CREATE(
	screenshot_cubemap_definition,
	_hs_type_void,
	"screenshot_cubemap",
	0,
	&hs_macro_function_parse,
	&screenshot_cubemap_evaluate,
	"takes a cubemap screenshot and saves as <name>.tif",
	NULL,
	1,
	_hs_type_string
);

HS_FUNCTION_DEFINITION_CREATE(
	main_menu_definition,
	_hs_type_void,
	"main_menu",
	0,
	&hs_macro_function_parse,
	&main_menu_launch_force_evaluate,
	"goes back to the main menu",
	NULL,
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	main_halt_definition,
	_hs_type_void,
	"main_halt",
	0,
	&hs_macro_function_parse,
	&main_halt_and_display_errors_evaluate,
	"goes to a halted pregame state",
	NULL,
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	map_name_definition,
	_hs_type_void,
	"map_name",
	_hs_unparsed,
	&hs_macro_function_parse,
	&main_game_launch_legacy_evaluate,
	"the same as game_start: launches a game for debugging purposes",
	NULL,
	1,
	_hs_type_string
);

HS_FUNCTION_DEFINITION_CREATE(
	game_multiplayer_definition,
	_hs_type_void,
	"game_multiplayer",
	0,
	&hs_macro_function_parse,
	&main_game_launch_set_multiplayer_engine_evaluate,
	"debug map launching: sets the multiplayer variant for the next map.",
	NULL,
	1,
	_hs_type_string
);

HS_FUNCTION_DEFINITION_CREATE(
	game_splitscreen_definition,
	_hs_type_void,
	"game_splitscreen",
	0,
	&hs_macro_function_parse,
	&main_game_launch_set_multiplayer_splitscreen_count_evaluate,
	"debug map launching: sets the number of multiplayer splitscreen players for the next map.",
	NULL,
	1,
	_hs_type_long_integer
);

HS_FUNCTION_DEFINITION_CREATE(
	game_difficulty_definition,
	_hs_type_void,
	"game_difficulty",
	0,
	&hs_macro_function_parse,
	&main_game_launch_set_difficulty_evaluate,
	"debug map launching: sets the difficulty of the next map.",
	NULL,
	1,
	_hs_type_enum_game_difficulty
);

HS_FUNCTION_DEFINITION_CREATE(
	game_coop_players_definition,
	_hs_type_void,
	"game_coop_players",
	0,
	&hs_macro_function_parse,
	&main_game_launch_set_coop_player_count_evaluate,
	"debug map launching: sets the number of coop players for the next map.",
	NULL,
	1,
	_hs_type_long_integer
);
	
/*
HS_FUNCTION_DEFINITION_CREATE(
	game_initial_bsp_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_tick_rate_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_start_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_rate_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	texture_cache_flush_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	geometry_cache_flush_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_cache_flush_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	animation_cache_flush_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	font_cache_flush_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	texture_cache_test_malloc_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	debug_memory_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	debug_memory_by_file_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	debug_memory_for_file_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	debug_tags_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	tags_verify_all_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	profile_reset_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	profile_activate_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	profile_deactivate_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	profile_mode_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	collision_log_enable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	damage_control_get_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	damage_control_set_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_lines_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_debug_sound_point_set_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ai_dialogue_break_on_vocalization_definition,
*/

HS_FUNCTION_DEFINITION_CREATE(
	fade_in_definition,
	_hs_type_void,
	"fade_in",
	0,
	&hs_macro_function_parse,
	&scripted_player_effect_screen_fade_in_evaluate,
	"does a screen fade in from a particular color",
	NULL,
	4,
	_hs_type_real, _hs_type_real, _hs_type_real, _hs_type_short_integer
);
/*
HS_FUNCTION_DEFINITION_CREATE(
	fade_out_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_start_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_stop_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_skip_start_internal_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_skip_stop_internal_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_show_letterbox_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_show_letterbox_immediate_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_set_title_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_set_title_delayed_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_suppress_bsp_object_creation_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_subtitle_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	attract_mode_start_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	attract_mode_set_seconds_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_won_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_lost_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_revert_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_is_cooperative_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_is_playtest_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_can_use_flashlights_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_save_and_quit_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_save_unsafe_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	debug_spawning_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	king_set_hill_definition,
);
*/

HS_FUNCTION_DEFINITION_CREATE(
	core_load_definition,
	_hs_type_void,
	"core_load",
	0,
	&hs_macro_function_parse,
	&main_load_core_evaluate,
	"loads debug game state from core\\core.bin",
	NULL,
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	core_load_name_definition,
	_hs_type_void,
	"core_load_name",
	0,
	&hs_macro_function_parse,
	&main_load_core_name_evaluate,
	"loads debug game state from core\\<path>",
	NULL,
	1,
	_hs_type_string
);

HS_FUNCTION_DEFINITION_CREATE(
	core_save_definition,
	_hs_type_void,
	"core_save",
	0,
	&hs_macro_function_parse,
	&main_save_core_evaluate,
	"saves debug game state to core\\core.bin",
	NULL,
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	core_save_name_definition,
	_hs_type_void,
	"core_save_name",
	0,
	&hs_macro_function_parse,
	&main_save_core_name_evaluate,
	"saves debug game state to core\\<path>",
	NULL,
	1,
	_hs_type_string
);

HS_FUNCTION_DEFINITION_CREATE(
	core_load_game_definition,
	_hs_type_void,
	"core_load_game",
	0,
	&hs_macro_function_parse,
	&main_game_load_from_core_evaluate,
	"loads level and game state from core\\core.bin",
	NULL,
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	core_load_game_name_definition,
	_hs_type_void,
	"core_load_game_name",
	0,
	&hs_macro_function_parse,
	&main_game_load_from_core_name_evaluate,
	"loads level and game state from core\\<path>",
	NULL,
	1,
	_hs_type_string
);

/*
HS_FUNCTION_DEFINITION_CREATE(
	core_regular_upload_to_debug_server_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	core_set_upload_option_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	film_play_hack_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_safe_to_save_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_safe_to_speak_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_all_quiet_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_save_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_save_cancel_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_save_no_timeout_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_save_immediate_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_saving_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_reverted_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_set_tag_parameter_unsafe_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_impulse_predict_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_impulse_trigger_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_impulse_start_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_impulse_start_cinematic_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_impulse_start_effect_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_impulse_time_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_sound_impulse_time_definition_600,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_impulse_language_time_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_impulse_stop_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_impulse_start_3d_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_looping_predict_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_looping_start_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_looping_stop_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_looping_stop_immediately_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_looping_set_scale_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_looping_set_alternate_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_loop_spam_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_class_show_channel_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_class_debug_sound_start_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	debug_sounds_enable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_class_set_gain_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_class_set_gain_db_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_class_enable_ducker_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	debug_sound_environment_parameter_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_set_global_effect_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_set_global_effect_scale_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_set_trace_callback_level_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_dump_miles_debug_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	vehicle_hover_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	vehicle_count_bipeds_killed_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	biped_ragdoll_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_cinematic_fade_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	show_hud_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	show_hud_help_text_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	show_hud_messages_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	enable_hud_help_flash_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_help_flash_restart_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_show_training_text_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_set_training_text_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_enable_training_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_training_activate_flashlight_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_training_activate_crouch_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_training_activate_stealth_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	activate_nav_point_flag_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	activate_nav_point_object_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	activate_team_nav_point_flag_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	activate_team_nav_point_object_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	deactivate_nav_point_flag_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	deactivate_nav_point_object_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	deactivate_team_nav_point_flag_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	deactivate_team_nav_point_object_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cls_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	error_overflow_suppression_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	error_geometry_show_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	error_geometry_hide_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	error_geometry_show_all_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	error_geometry_hide_all_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	error_geometry_list_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_effect_set_max_translation_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_effect_set_max_rotation_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_effect_set_max_vibration_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_effect_start_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_effect_stop_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_show_health_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_blink_health_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_show_shield_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_blink_shield_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_show_motion_sensor_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_blink_motion_sensor_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_show_crosshair_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_show_ammo_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_clear_messages_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_set_help_text_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_set_objective_text_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_set_timer_time_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_set_timer_warning_time_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_set_timer_position_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	show_hud_timer_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	pause_hud_timer_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hud_get_timer_ticks_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	time_code_show_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	time_code_start_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	time_code_reset_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_profile_include_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_profile_include_all_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_profile_include_all_except_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_profile_exclude_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_profile_exclude_all_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_profile_exclude_all_except_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_debug_display_bitmap_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_decals_flush_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_lens_flares_clear_for_frame_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_test_screen_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_test_screen_supablur_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_test_screen_off_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_f2_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_f3_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_f4_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_f5_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_f6_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_decal_depth_bias_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_decal_depth_bias_slope_scale_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_hud_ten_foot_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_blur_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_solid_model_color_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_solid_structure_color_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_solid_structure_instanced_geometry_color_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_solid_sky_color_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_solid_decorator_color_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_solid_decorator_model_color_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_solid_decorator_decal_color_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_solid_decorator_sprite_color_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	decorator_rebuild_all_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	script_screen_effect_set_value_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_screen_effect_start_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_screen_effect_set_depth_of_field_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_screen_effect_set_crossfade_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_screen_effect_set_crossfade2_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_screen_effect_stop_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_set_near_clip_distance_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_set_far_clip_distance_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_set_environment_map_attenuation_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_set_environment_map_bitmap_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_reset_environment_map_bitmap_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_set_environment_map_tint_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_reset_environment_map_tint_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_layer_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_dynamic_reflections_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	lightmaps_expose_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_profile_create_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_profile_create_corrupt_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_has_skills_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	player_has_mad_secret_skills_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	controller_invert_look_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	controller_look_speed_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	controller_set_look_invert_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	controller_get_look_invert_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	controller_unlock_solo_levels_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	user_interface_controller_get_last_level_played_definition,
*/

HS_FUNCTION_DEFINITION_CREATE(
	ui_debug_load_main_menu_definition,
	_hs_type_void,
	"ui_debug_load_main_menu",
	0,
	&hs_macro_function_parse,
	&user_interface_enter_game_shell_evaluate,
	"loads the main menu screen",
	NULL,
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_debug_text_bounds_definition,
	_hs_type_void,
	"ui_debug_text_bounds",
	0,
	&hs_macro_function_parse,
	&debug_enable_text_bounds_evaluate,
	"toggle rendering of ui text boundaries",
	NULL,
	1,
	_hs_type_boolean
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_debug_show_title_safe_bounds_definition,
	_hs_type_void,
	"ui_debug_show_title_safe_bounds",
	0,
	&hs_macro_function_parse,
	&debug_render_title_safe_bounds_evaluate,
	"toggle display of title safe boundary",
	NULL,
	1,
	_hs_type_boolean
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_debug_element_bounds_definition,
	_hs_type_void,
	"ui_debug_element_bounds",
	0,
	&hs_macro_function_parse,
	&set_debug_frame_element_bounds_evaluate,
	"toggle rendering of widget tag block bounds",
	NULL,
	1,
	_hs_type_boolean
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_debug_screen_tag_definition,
	_hs_type_void,
	"ui_debug_screen_tag",
	0,
	&hs_macro_function_parse,
	&user_interface_test_screen_evaluate,
	"test a ui screen",
	NULL,
	1,
	_hs_type_string
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_transition_out_console_window_definition,
	_hs_type_void,
	"ui_transition_out_console_window",
	0,
	&hs_macro_function_parse,
	&user_interface_test_transition_out_console_screen_evaluate,
	"transition out any ui on the console window",
	NULL,
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_debug_show_screen_tag_path_definition,
	_hs_type_void,
	"ui_debug_show_screen_tag_path",
	0,
	&hs_macro_function_parse,
	&user_interface_debug_show_screen_tag_path_evaluate,
	"display tag path of screens as they load",
	NULL,
	1,
	_hs_type_boolean
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_debug_show_current_screen_tag_definition,
	_hs_type_void,
	"ui_debug_show_current_screen_tag",
	0,
	&hs_macro_function_parse,
	&user_interface_debug_show_current_screen_tag_evaluate,
	"displays the current tag path on a specified channel",
	NULL,
	1,
	_hs_type_string
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_set_beta_definition,
	_hs_type_void,
	"ui_set_beta",
	0,
	&hs_macro_function_parse,
	&user_interface_debug_set_beta_evaluate,
	"set ui beta testing on/off",
	NULL,
	1,
	_hs_type_boolean
);

/*
HS_FUNCTION_DEFINITION_CREATE(
	ui_memory_dump_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_set_automation_mode_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_set_automation_hopper_type_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_set_automation_variant_type_and_map_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_set_automation_desired_local_user_count_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_set_automation_desired_desired_network_game_player_count_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_set_automation_desired_network_game_length_seconds_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_set_automation_desired_network_session_name_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_set_automation_desired_controller_team_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_set_automation_desired_controller_player_profile_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	objectives_clear_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	objectives_show_up_to_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	objectives_finish_up_to_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	input_suppress_vibration_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	update_remote_camera_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_status_filter_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_sim_reset_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_sim_spike_now_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_sim_dropspike_now_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_test_ping_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_test_channel_loopback_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_test_channel_delete_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_test_leave_squad_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_test_delegate_host_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_test_delegate_leader_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_test_map_name_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_test_campaign_difficulty_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_test_player_color_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_test_reset_objects_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_test_fatal_error_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_test_matchmaking_hopper_list_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_test_matchmaking_hopper_print_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_test_matchmaking_hopper_set_game_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_set_machine_name_definition,
*/

HS_FUNCTION_DEFINITION_CREATE(
	net_event_display_category_definition,
	_hs_type_void,
	"net_event_display_category",
	0,
	&hs_macro_function_parse,
	&net_event_set_display_level_by_name_evaluate,
	"sets the display level for a named category of network events",
	NULL,
	2,
	_hs_type_string, _hs_type_enum_network_event
);

HS_FUNCTION_DEFINITION_CREATE(
	net_event_log_category_definition,
	_hs_type_void,
	"net_event_log_category",
	0,
	&hs_macro_function_parse,
	&net_event_set_log_level_by_name_evaluate,
	"sets the log level for a named category of network events",
	NULL,
	2,
	_hs_type_string, _hs_type_enum_network_event
);

HS_FUNCTION_DEFINITION_CREATE(
	net_event_list_categories_definition,
	_hs_type_void,
	"net_event_list_categories",
	0,
	&hs_macro_function_parse,
	&net_event_dump_categories_evaluate,
	"lists all categories that exist under a particular category string",
	NULL,
	1,
	_hs_type_string
);

/*
HS_FUNCTION_DEFINITION_CREATE(
	online_task_list_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	play_bink_movie_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	play_credits_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	bink_done_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	set_global_doppler_factor_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	set_global_mixbin_headroom_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	debug_sound_environment_source_parameter_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	data_mine_set_mission_segment_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	data_mine_display_mission_segment_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	data_mine_insert_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	data_mine_upload_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	data_mine_playback_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	data_mine_playback_last_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	data_mine_playback_exit_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	data_mine_enable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	data_mine_track_event_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	data_mine_display_event_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	data_mine_show_all_events_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	data_mine_show_tracked_events_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	data_mine_show_displayed_events_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	data_mine_display_session_data_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	data_mine_display_disk_writes_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	data_mine_debug_menu_setting_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	data_mine_open_debug_menu_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	error_enable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	render_layer_enable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	render_layer_enable_all_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_overdraw_z_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_overdraw_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_layers_overdraw_z_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_layers_overdraw_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	test_memory_allocators_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hs_test_memory_allocators_definition_811,
);

HS_FUNCTION_DEFINITION_CREATE(
	display_video_standard_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	test_xcr_monkey_enable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	test_web_map_snapshot_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	test_telnet_status_enable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	test_telnet_status_interval_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	webstats_disable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	webstats_test_submit_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	netdebug_prefer_internet_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	script_temporary_disable_lightmap_shadows_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	flag_new_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	flag_new_at_look_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	flags_clear_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	flags_save_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	flags_save_filtered_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	flags_save_named_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	flags_save_named_filtered_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	flags_default_name_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	flags_default_comment_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	flags_set_filter_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	flags_export_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	flags_export_filtered_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	flags_export_named_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	flags_export_named_filtered_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	bug_now_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	bug_now_lite_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	bug_now_auto_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	object_list_children_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	voice_set_outgoing_channel_count_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	voice_set_voice_repeater_peer_index_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_leaderboard_clear_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	net_leaderboard_set_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	dump_loaded_tags_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_start_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_start_smooth_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_stop_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_restart_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_is_active_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_is_finished_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_set_current_value_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_get_current_value_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_get_start_value_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_get_final_value_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_get_current_phase_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_get_current_time_fraction_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_get_start_time_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_get_final_time_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_evaluate_at_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_evaluate_at_time_fraction_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_evaluate_at_time_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_evaluate_at_time_delta_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_stop_all_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_restart_all_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	interpolator_flip_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	weather_start_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	weather_stop_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	weather_change_intensity_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	animation_cache_stats_reset_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_clone_players_weapon_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_move_attached_objects_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	vehicle_enable_ghost_effects_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ice_cream_flavor_stock_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	ice_cream_flavor_available_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	set_global_sound_environment_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	reset_dsp_image_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	game_save_cinematic_skip_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_outro_start_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_enable_ambience_details_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	set_rasterizer_gamma_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	predict_model_section_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	predict_structure_section_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	predict_lightmap_bucket_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	predict_bitmap_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	get_prediction_offset_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_bloom_override_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_bloom_override_reset_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_bloom_override_blur_amount_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_bloom_override_threshold_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_bloom_override_brightness_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_bloom_override_box_factor_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_bloom_override_max_factor_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_bloom_override_silver_bullet_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_bloom_override_only_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_bloom_override_high_res_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_bloom_override_brightness_alpha_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_bloom_override_max_factor_alpha_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cache_block_for_one_frame_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	sound_suppress_ambience_update_on_revert_definition,
*/

HS_FUNCTION_DEFINITION_CREATE(
	ui_test_error_ok_definition,
	_hs_type_void,
	"ui_test_error_ok",
	0,
	&hs_macro_function_parse,
	&user_interface_test_error_ok_evaluate,
	"test error code display w/ ok dialog",
	NULL,
	1,
	_hs_type_short_integer
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_test_error_ok_cancel_definition,
	_hs_type_void,
	"ui_test_error_ok_cancel",
	0,
	&hs_macro_function_parse,
	&user_interface_test_error_ok_cancel_evaluate,
	"test error code display w/ ok-cancel dialog",
	NULL,
	1,
	_hs_type_short_integer
);

HS_FUNCTION_DEFINITION_CREATE(
	ui_test_confirmation_definition,
	_hs_type_void,
	"ui_test_confirmation",
	0,
	&hs_macro_function_parse,
	&user_interface_test_confirmation_evaluate,
	"test confirmation dialog display",
	NULL,
	1,
	_hs_type_short_integer
);

/*
HS_FUNCTION_DEFINITION_CREATE(
	cinematic_lightmap_shadow_disable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	cinematic_lightmap_shadow_enable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	predict_animation_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	online_message_send_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	test_web_event_local_cheater_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	test_web_event_remote_cheater_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	test_cheating_detect_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	test_network_storage_simulate_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	scenario_dump_scenario_signature_enable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	scenario_force_signature_failure_enable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	scenario_force_name_failure_enable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	scenario_ignore_failures_enable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	scenario_dump_scenario_signature_disable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	scenario_force_signature_failure_disable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	scenario_force_name_failure_disable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	scenario_ignore_failures_disable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hopper_verify_force_failures_enable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hopper_verify_ignore_failures_enable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hopper_verify_force_failures_disable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	hopper_verify_ignore_failures_disable_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	loading_screen_fade_to_white_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_debug_frame_usage_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	rasterizer_debug_crap_pixel_shader_definition,
);

HS_FUNCTION_DEFINITION_CREATE(
	enable_render_light_suppressor_definition,
	_hs_type_void,
	"enable_render_light_suppressor",
	0,
	&hs_macro_function_parse,
	&hs_evaluate_enable_render_light_suppressor,
	"enables the code that constrains the max # active lights",
	NULL,
	0,
	_hs_unparsed
);

HS_FUNCTION_DEFINITION_CREATE(
	disable_render_light_suppressor_definition,
	_hs_type_void,
	"disable_render_light_suppressor",
	0,
	&hs_macro_function_parse,
	&hs_evaluate_disable_render_light_suppressor,
	"disables the code that constrains the max # active lights",
	NULL,
	0,
	_hs_unparsed
);
*/

// NOTE: if we're ever going to add extra functions to this table read the following:
// Do NOT add function definitions midway through the array,
// ALWAYS ADD THEM AT THE END
// YOU WILL BREAK SCRIPTS IN CAMPAIGN MAPS

const hs_function_definition* hs_function_table[] =
{
	&begin_definition,
	&begin_random_definition,
	&if_definition,
	&cond_definition,
	&set_definition,
	&and_definition,
	&or_definition,
	&add_definition,
	&subtract_definition,
	&multiply_definition,
	&divide_definition,
	&min_definition,
	&max_definition,
	&equal_definition,
	&not_equal_definition,
	&gt_definition,
	&lt_definition,
	&gte_definition,
	&lte_definition,
	&sleep_definition,
	&sleep_forever_definition,
	&sleep_until_definition,
	&wake_definition,
	&inspect_definition,
	&object_to_unit_definition,
	&not_definition,
	&pin_definition,
	&print_definition,
	&players_definition,
	&kill_volume_enable_definition,
	&kill_volume_disable_definition,
	&volume_teleport_players_not_inside_definition,
	&volume_test_object_definition,
	&volume_test_objects_definition,
	&volume_test_objects_all_definition,
	&volume_return_objects_definition,
	&volume_return_objects_by_type_definition,
	&list_get_definition,
	&list_count_definition,
	&list_count_not_dead_definition,
	&effect_new_definition,
	&effect_new_on_object_marker_definition,
	&damage_new_definition,
	&damage_object_definition,
	&damage_objects_definition,
	&damage_players_definition,
	&object_create_definition,
	&object_create_clone_definition,
	&object_create_anew_definition,
	&object_create_containing_definition,
	&object_create_clone_containing_definition,
	&object_create_anew_containing_definition,
	&object_destroy_definition,
	&object_destroy_containing_definition,
	&object_destroy_all_definition,
	&object_destroy_type_mask_definition,
	&objects_delete_by_definition_definition,
	&object_hide_definition,
	&object_set_shadowless_definition,
	&object_function_set_definition,
	&object_set_function_variable_definition,
	&object_clear_function_variable_definition,
	&object_clear_all_function_variables_definition,
	&object_dynamic_simulation_disable_definition,
	&object_set_phantom_power_definition,
	&object_wake_physics_definition,
	&object_set_ranged_attack_inhibited_definition,
	&object_set_melee_attack_inhibited_definition,
	&objects_dump_memory_definition,

/*
	&object_get_health_definition,
	&object_get_shield_definition,
	&object_set_shield_effect_definition,
	&object_set_physics_definition,
	&object_get_parent_definition,
	&objects_attach_definition,
	&object_at_marker_definition,
	&objects_detach_definition,
	&object_set_scale_definition,
	&object_set_velocity_definition,
	&hs_object_set_velocity_definition_79,
	&object_set_deleted_when_deactivated_definition,
	&object_copy_player_appearance_definition,
	&object_model_target_destroyed_definition,
	&object_model_targets_destroyed_definition,
	&object_damage_damage_section_definition,
	&object_cannot_die_definition,
	&object_vitality_pinned_definition,
	&garbage_collect_now_definition,
	&garbage_collect_unsafe_definition,
	&garbage_collect_multiplayer_definition,
	&object_cannot_take_damage_definition,
	&object_can_take_damage_definition,
	&object_cinematic_lod_definition,
	&object_cinematic_collision_definition,
	&object_cinematic_visibility_definition,
	&object_uses_cinematic_lighting_definition,
	&cinematic_lighting_set_primary_light_definition,
	&cinematic_lighting_set_secondary_light_definition,
	&cinematic_lighting_set_ambient_light_definition,
	&objects_predict_definition,
	&objects_predict_high_definition,
	&objects_predict_low_definition,
	&object_type_predict_high_definition,
	&object_type_predict_low_definition,
	&object_type_predict_definition,
	&object_teleport_definition,
	&object_set_facing_definition,
	&object_set_shield_definition,
	&object_set_shield_stun_definition,
	&object_set_shield_stun_infinite_definition,
	&object_set_permutation_definition,
	&object_set_region_state_definition,
	&objects_can_see_object_definition,
	&objects_can_see_flag_definition,
	&objects_distance_to_object_definition,
	&objects_distance_to_flag_definition,
	&map_info_definition,
	&position_predict_definition,
	&shader_predict_definition,
	&bitmap_predict_definition,
	&script_recompile_definition,
	&script_doc_definition,
*/
	&help_definition,
/*
	&game_engine_objects_definition,
	&random_range_definition,
	&real_random_range_definition,
	&physics_constants_reset_definition,
	&physics_set_gravity_definition,
	&physics_set_velocity_frame_definition,
	&physics_disable_character_ground_adhesion_forces_definition,
	&havok_debug_start_definition,
	&havok_dump_world_definition,
	&havok_dump_world_close_movie_definition,
	&havok_profile_start_definition,
	&havok_profile_end_definition,
	&havok_reset_allocated_state_definition,
	&breakable_surfaces_enable_definition,
	&breakable_surfaces_reset_definition,
	&recording_play_definition,
	&recording_play_and_delete_definition,
	&recording_play_and_hover_definition,
	&recording_kill_definition,
	&recording_time_definition,
	&render_lights_definition,
	&texture_camera_set_object_marker_definition,
	&texture_camera_off_definition,
	&render_lights_enable_cinematic_shadow_definition,
	&fog_report_definition,
	&fog_report_summary_definition,
	&fog_report_verbose_definition,
	&fog_mode_control_definition,
	&fog_override_off_definition,
	&fog_override_mode_definition,
	&fog_override_mode_off_definition,
	&fog_override_atmospheric_fog_definition,
	&fog_override_atmospheric_fog_off_definition,
	&fog_override_secondary_fog_definition,
	&fog_override_secondary_fog_off_definition,
	&fog_override_sky_fog_definition,
	&fog_override_sky_fog_off_definition,
	&fog_override_planar_fog_definition,
	&fog_override_planar_fog_off_definition,
	&fog_override_planar_fog_plane_definition,
	&fog_override_planar_fog_plane_off_definition,
	&fog_override_planar_max_atmospheric_depth_definition,
	&fog_override_planar_max_atmospheric_depth_off_definition,
	&fog_override_planar_planar_eye_density_definition,
	&fog_override_planar_planar_eye_density_off_definition,
	&fog_override_atmospheric_secondary_blend_definition,
	&fog_override_atmospheric_secondary_blend_off_definition,
	&fog_override_atmospheric_planar_blend_definition,
	&fog_override_atmospheric_planar_blend_off_definition,
	&render_debug_structure_cluster_visibility_definition,
	&render_debug_structure_cluster_fog_definition,
	&render_debug_structure_fog_plane_definition,
	&render_debug_structure_fog_plane_infinite_extent_definition,
	&render_debug_structure_fog_zone_definition,
	&render_debug_structure_fog_zone_floodfill_definition,
	&render_debug_structure_all_fog_planes_definition,
	&render_debug_structure_all_cluster_errors_definition,
	&render_debug_structure_line_opacity_definition,
	&render_debug_structure_text_opacity_definition,
	&render_debug_structure_opacity_definition,
	&render_debug_structure_non_occluded_fog_planes_definition,
	&scenery_animation_start_definition,
	&scenery_animation_start_loop_definition,
	&scenery_animation_start_relative_definition,
	&scenery_animation_start_relative_loop_definition,
	&scenery_animation_start_at_frame_definition,
	&scenery_animation_idle_definition,
	&scenery_get_animation_time_definition,
	&render_effects_definition,
	&unit_can_blink_definition,
	&unit_set_active_camo_definition,
	&unit_open_definition,
	&unit_close_definition,
	&unit_kill_definition,
	&unit_kill_silent_definition,
	&unit_is_emitting_definition,
	&unit_get_custom_animation_time_definition,
	&unit_stop_custom_animation_definition,
	&custom_animation_definition,
	&custom_animation_loop_definition,
	&custom_animation_relative_definition,
	&custom_animation_relative_loop_definition,
	&custom_animation_list_definition,
	&unit_custom_animation_at_frame_definition,
	&unit_is_playing_custom_animation_definition,
	&object_set_custom_animations_hold_on_last_frame_definition,
	&object_set_custom_animations_prevent_lipsync_head_movement_definition,
	&unit_set_actively_controlled_definition,
	&unit_get_team_index_definition,
	&unit_aim_without_turning_definition,
	&unit_set_enterable_by_player_definition,
	&unit_get_enterable_by_player_definition,
	&unit_only_takes_damage_from_players_team_definition,
	&unit_enter_vehicle_definition,
	&unit_board_vehicle_definition,
	&unit_set_emotion_definition,
	&unit_set_emotion_animation_definition,
	&unit_set_emotional_state_definition,
	&unit_enable_eye_tracking_definition,
	&unit_in_vehicle_definition,
	&vehicle_test_seat_list_definition,
	&vehicle_test_seat_definition,
	&unit_set_prefer_tight_camera_track_definition,
	&unit_exit_vehicle_definition,
	&hs_unit_exit_vehicle_definition_227,
	&unit_set_maximum_vitality_definition,
	&units_set_maximum_vitality_definition,
	&unit_set_current_vitality_definition,
	&units_set_current_vitality_definition,
	&vehicle_load_magic_definition,
	&vehicle_unload_definition,
	&unit_set_animation_mode_definition,
	&magic_melee_attack_definition,
	&vehicle_riders_definition,
	&vehicle_driver_definition,
	&vehicle_gunner_definition,
	&unit_get_health_definition,
	&unit_get_shield_definition,
	&unit_get_total_grenade_count_definition,
	&unit_has_weapon_definition,
	&unit_has_weapon_readied_definition,
	&unit_lower_weapon_definition,
	&unit_raise_weapon_definition,
	&unit_animation_forced_seat_definition,
	&unit_doesnt_drop_items_definition,
	&unit_impervious_definition,
	&unit_suspended_definition,
	&unit_add_equipment_definition,
	&weapon_hold_trigger_definition,
	&weapon_enable_warthog_chaingun_light_definition,
	&device_set_never_appears_locked_definition,
	&device_set_power_definition,
	&device_get_power_definition,
	&device_set_position_definition,
	&device_get_position_definition,
	&device_set_position_immediate_definition,
	&device_group_get_definition,
	&device_group_set_definition,
	&device_group_set_immediate_definition,
	&device_one_sided_set_definition,
	&device_operates_automatically_set_definition,
	&device_closes_automatically_set_definition,
	&device_group_change_only_once_more_set_definition,
	&device_set_position_track_definition,
	&device_set_overlay_track_definition,
	&device_animate_position_definition,
	&device_animate_overlay_definition,
	&cheat_all_powerups_definition,
	&cheat_all_weapons_definition,
	&cheat_all_vehicles_definition,
	&cheat_teleport_to_camera_definition,
	&cheat_active_camouflage_definition,
	&cheat_active_camouflage_by_player_definition,
	&cheats_load_definition,
*/
	&drop_definition,
/*
	&ai_enable_definition,
	&ai_enabled_definition,
	&ai_grenades_definition,
	&ai_dialogue_enable_definition,
	&ai_dialogue_log_reset_definition,
	&ai_dialogue_log_dump_definition,
	&ai_get_object_definition,
	&ai_get_unit_definition,
	&ai_attach_definition,
	&ai_attach_units_definition,
	&ai_detach_definition,
	&ai_detach_units_definition,
	&ai_place_definition,
	&hs_ai_place_definition_291,
	&ai_place_in_vehicle_definition,
	&ai_cannot_die_definition,
	&ai_vitality_pinned_definition,
	&ai_overcomes_oversteer_definition,
	&ai_kill_definition,
	&ai_kill_silent_definition,
	&ai_erase_definition,
	&ai_erase_all_definition,
	&ai_disposable_definition,
	&ai_select_definition,
	&ai_deselect_definition,
	&ai_set_deaf_definition,
	&ai_set_blind_definition,
	&ai_magically_see_definition,
	&ai_magically_see_object_definition,
	&ai_set_active_camo_definition,
	&ai_suppress_combat_definition,
	&ai_migrate_definition,
	&ai_allegiance_definition,
	&ai_allegiance_remove_definition,
	&ai_braindead_definition,
	&ai_braindead_by_unit_definition,
	&ai_disregard_definition,
	&ai_prefer_target_definition,
	&ai_teleport_to_starting_location_if_outside_bsp_definition,
	&ai_renew_definition,
	&ai_force_active_definition,
	&ai_force_active_by_unit_definition,
	&ai_playfight_definition,
	&ai_reconnect_definition,
	&ai_berserk_definition,
	&ai_set_team_definition,
	&ai_allow_dormant_definition,
	&ai_is_attacking_definition,
	&ai_fighting_count_definition,
	&ai_living_count_definition,
	&ai_living_fraction_definition,
	&ai_strength_definition,
	&ai_swarm_count_definition,
	&ai_nonswarm_count_definition,
	&ai_actors_definition,
	&ai_allegiance_broken_definition,
	&ai_set_orders_definition,
	&ai_spawn_count_definition,
	&object_get_ai_definition,
	&ai_trigger_test_definition,
	&generate_pathfinding_definition,
	&ai_render_paths_all_definition,
	&ai_vehicle_get_definition,
	&ai_vehicle_get_from_starting_location_definition,
	&ai_vehicle_reserve_seat_definition,
	&ai_vehicle_reserve_definition,
	&ai_vehicle_enter_definition,
	&hs_ai_vehicle_enter_definition_345,
	&ai_vehicle_enter_immediate_definition,
	&hs_ai_vehicle_enter_immediate_definition_347,
	&ai_enter_squad_vehicles_definition,
	&ai_vehicle_exit_definition,
	&hs_ai_vehicle_exit_definition_350,
	&vehicle_overturned_definition,
	&vehicle_flip_definition,
	&ai_combat_status_definition,
	&flock_start_definition,
	&flock_stop_definition,
	&flock_create_definition,
	&flock_delete_definition,
	&ai_verify_tags_definition,
	&ai_wall_lean_definition,
	&ai_play_line_definition,
	&ai_play_line_at_player_definition,
	&ai_play_line_on_object_definition,
	&ai_scene_definition,
	&hs_ai_scene_definition_364,
	&hs_ai_scene_definition_365,
	&cs_run_command_script_definition,
	&cs_queue_command_script_definition,
	&cs_stack_command_script_definition,
	&cs_run_joint_command_script_definition,
	&hs_cs_run_joint_command_script_definition_370,
	&cs_command_script_running_definition,
	&cs_command_script_queued_definition,
	&cs_number_queued_definition,
	&cs_switch_definition,
	&cs_switch_index_definition,
	&cs_transfer_definition,
	&cs_transfer_stack_definition,
	&cs_transfer_queue_definition,
	&cs_fly_to_definition,
	&hs_cs_fly_to_definition_380,
	&cs_fly_to_and_face_definition,
	&hs_cs_fly_to_and_face_definition_382,
	&cs_fly_by_definition,
	&hs_cs_fly_by_definition_384,
	&cs_go_to_definition,
	&hs_cs_go_to_definition_386,
	&cs_go_by_definition,
	&hs_cs_go_by_definition_388,
	&cs_go_to_and_face_definition,
	&cs_start_to_definition,
	&cs_go_to_nearest_definition,
	&cs_moving_definition,
	&cs_look_definition,
	&cs_look_player_definition,
	&cs_look_object_definition,
	&cs_aim_definition,
	&cs_aim_player_definition,
	&cs_aim_object_definition,
	&cs_face_definition,
	&cs_face_player_definition,
	&cs_face_object_definition,
	&cs_move_in_direction_definition,
	&cs_pause_definition,
	&cs_shoot_definition,
	&hs_cs_shoot_definition_405,
	&cs_shoot_point_definition,
	&cs_vehicle_speed_definition,
	&cs_grenade_definition,
	&cs_jump_definition,
	&cs_jump_to_point_definition,
	&cs_vocalize_definition,
	&cs_play_sound_definition,
	&hs_cs_play_sound_definition_413,
	&hs_cs_play_sound_definition_414,
	&cs_stop_sound_definition,
	&cs_custom_animation_definition,
	&cs_stop_custom_animation_definition,
	&cs_play_line_definition,
	&cs_die_definition,
	&cs_teleport_definition,
	&cs_animate_definition,
	&cs_movement_mode_definition,
	&cs_crouch_definition,
	&hs_cs_crouch_definition_424,
	&cs_set_pathfinding_radius_definition,
	&cs_go_to_vehicle_definition,
	&cs_set_behavior_definition,
	&cs_formation_definition,
	&cs_deploy_turret_definition,
	&cs_approach_definition,
	&cs_start_approach_definition,
	&cs_approach_player_definition,
	&cs_start_approach_player_definition,
	&cs_approach_stop_definition,
	&cs_ignore_obstacles_definition,
	&cs_turn_sharpness_definition,
	&cs_vehicle_speed_instantaneous_definition,
	&cs_vehicle_boost_definition,
	&cs_abort_on_alert_definition,
	&cs_abort_on_damage_definition,
	&cs_abort_on_combat_status_definition,
	&cs_enable_targeting_definition,
	&cs_enable_looking_definition,
	&cs_enable_moving_definition,
	&cs_enable_dialogue_definition,
	&cs_suppress_dialogue_global_definition,
	&cs_set_style_definition,
	&cs_force_combat_status_definition,
	&cs_enable_pathfinding_failsafe_definition,
	&camera_control_definition,
	&camera_set_definition,
	&camera_set_relative_definition,
	&camera_set_animation_definition,
	&camera_set_animation_relative_definition,
	&camera_predict_resources_at_frame_definition,
	&camera_predict_resources_at_point_definition,
	&camera_set_first_person_definition,
	&camera_time_definition,
	&camera_set_field_of_view_definition,
	&camera_set_pan_definition,
	&camera_pan_definition,
	&debug_camera_save_definition,
	&debug_camera_load_definition,
	&debug_camera_save_name_definition,
	&debug_camera_load_name_definition,
*/
	&director_debug_camera_definition,
/*
	&game_difficulty_get_definition,
	&game_difficulty_get_real_definition,
	&pvs_set_object_definition,
	&pvs_set_camera_definition,
	&pvs_clear_definition,
	&players_unzoom_all_definition,
	&player_enable_input_definition,
	&player_disable_movement_definition,
	&player_flashlight_on_definition,
	&player_active_camouflage_on_definition,
	&player_camera_control_definition,
	&player_action_test_reset_definition,
	&player_action_test_jump_definition,
	&player_action_test_primary_trigger_definition,
	&player_action_test_grenade_trigger_definition,
	&player_action_test_vision_trigger_definition,
	&player_action_test_zoom_definition,
	&player_action_test_rotate_weapons_definition,
	&player_action_test_rotate_grenades_definition,
	&player_action_test_melee_definition,
	&player_action_test_action_definition,
	&player_action_test_accept_definition,
	&player_action_test_cancel_definition,
	&player_action_test_look_relative_up_definition,
	&player_action_test_look_relative_down_definition,
	&player_action_test_look_relative_left_definition,
	&player_action_test_look_relative_right_definition,
	&player_action_test_look_relative_all_directions_definition,
	&player_action_test_move_relative_all_directions_definition,
	&player_action_test_start_definition,
	&player_action_test_back_definition,
	&player0_looking_up_definition,
	&player0_looking_down_definition,
	&player_action_test_look_up_begin_definition,
	&player_action_test_look_down_begin_definition,
	&player_action_test_look_pitch_end_definition,
	&player_action_test_lookstick_forward_definition,
	&player_action_test_lookstick_backward_definition,
	&debug_teleport_player_definition,
	&map_reset_definition,
	&switch_bsp_definition,
	&switch_bsp_by_name_definition,
	&structure_bsp_index_definition,
*/
	&crash_definition,
	&version_definition,
/*
	&status_definition,
	&record_movie_definition,
	&record_movie_distributed_definition,
	&screenshot_definition,
	&screenshot_big_definition,
	&screenshot_big_jittered_definition,
*/
	&screenshot_cubemap_definition,
	&main_menu_definition,
	&main_halt_definition,
	&map_name_definition,
	&game_multiplayer_definition,
	&game_splitscreen_definition,
	&game_difficulty_definition,
	&game_coop_players_definition,
/*	
	&game_initial_bsp_definition,
	&game_tick_rate_definition,
	&game_start_definition,
	&game_rate_definition,
	&texture_cache_flush_definition,
	&geometry_cache_flush_definition,
	&sound_cache_flush_definition,
	&animation_cache_flush_definition,
	&font_cache_flush_definition,
	&texture_cache_test_malloc_definition,
	&debug_memory_definition,
	&debug_memory_by_file_definition,
	&debug_memory_for_file_definition,
	&debug_tags_definition,
	&tags_verify_all_definition,
	&profile_reset_definition,
	&profile_activate_definition,
	&profile_deactivate_definition,
	&profile_mode_definition,
	&collision_log_enable_definition,
	&damage_control_get_definition,
	&damage_control_set_definition,
	&ai_lines_definition,
	&ai_debug_sound_point_set_definition,
	&ai_dialogue_break_on_vocalization_definition,
*/
	&fade_in_definition,
/*
	&fade_out_definition,
	&cinematic_start_definition,
	&cinematic_stop_definition,
	&cinematic_skip_start_internal_definition,
	&cinematic_skip_stop_internal_definition,
	&cinematic_show_letterbox_definition,
	&cinematic_show_letterbox_immediate_definition,
	&cinematic_set_title_definition,
	&cinematic_set_title_delayed_definition,
	&cinematic_suppress_bsp_object_creation_definition,
	&cinematic_subtitle_definition,
	&attract_mode_start_definition,
	&attract_mode_set_seconds_definition,
	&game_won_definition,
	&game_lost_definition,
	&game_revert_definition,
	&game_is_cooperative_definition,
	&game_is_playtest_definition,
	&game_can_use_flashlights_definition,
	&game_save_and_quit_definition,
	&game_save_unsafe_definition,
	&debug_spawning_definition,
	&king_set_hill_definition,
*/
	&core_load_definition,
	&core_load_name_definition,
	&core_save_definition,
	&core_save_name_definition,
	&core_load_game_definition,
	&core_load_game_name_definition,
/*
	&core_regular_upload_to_debug_server_definition,
	&core_set_upload_option_definition,
	&film_play_hack_definition,
	&game_safe_to_save_definition,
	&game_safe_to_speak_definition,
	&game_all_quiet_definition,
	&game_save_definition,
	&game_save_cancel_definition,
	&game_save_no_timeout_definition,
	&game_save_immediate_definition,
	&game_saving_definition,
	&game_reverted_definition,
	&sound_set_tag_parameter_unsafe_definition,
	&sound_impulse_predict_definition,
	&sound_impulse_trigger_definition,
	&sound_impulse_start_definition,
	&sound_impulse_start_cinematic_definition,
	&sound_impulse_start_effect_definition,
	&sound_impulse_time_definition,
	&hs_sound_impulse_time_definition_600,
	&sound_impulse_language_time_definition,
	&sound_impulse_stop_definition,
	&sound_impulse_start_3d_definition,
	&sound_looping_predict_definition,
	&sound_looping_start_definition,
	&sound_looping_stop_definition,
	&sound_looping_stop_immediately_definition,
	&sound_looping_set_scale_definition,
	&sound_looping_set_alternate_definition,
	&sound_loop_spam_definition,
	&sound_class_show_channel_definition,
	&sound_class_debug_sound_start_definition,
	&debug_sounds_enable_definition,
	&sound_class_set_gain_definition,
	&sound_class_set_gain_db_definition,
	&sound_class_enable_ducker_definition,
	&debug_sound_environment_parameter_definition,
	&sound_set_global_effect_definition,
	&sound_set_global_effect_scale_definition,
	&sound_set_trace_callback_level_definition,
	&sound_dump_miles_debug_definition,
	&vehicle_hover_definition,
	&vehicle_count_bipeds_killed_definition,
	&biped_ragdoll_definition,
	&hud_cinematic_fade_definition,
	&show_hud_definition,
	&show_hud_help_text_definition,
	&show_hud_messages_definition,
	&enable_hud_help_flash_definition,
	&hud_help_flash_restart_definition,
	&hud_show_training_text_definition,
	&hud_set_training_text_definition,
	&hud_enable_training_definition,
	&player_training_activate_flashlight_definition,
	&player_training_activate_crouch_definition,
	&player_training_activate_stealth_definition,
	&activate_nav_point_flag_definition,
	&activate_nav_point_object_definition,
	&activate_team_nav_point_flag_definition,
	&activate_team_nav_point_object_definition,
	&deactivate_nav_point_flag_definition,
	&deactivate_nav_point_object_definition,
	&deactivate_team_nav_point_flag_definition,
	&deactivate_team_nav_point_object_definition,
	&cls_definition,
	&error_overflow_suppression_definition,
	&error_geometry_show_definition,
	&error_geometry_hide_definition,
	&error_geometry_show_all_definition,
	&error_geometry_hide_all_definition,
	&error_geometry_list_definition,
	&player_effect_set_max_translation_definition,
	&player_effect_set_max_rotation_definition,
	&player_effect_set_max_vibration_definition,
	&player_effect_start_definition,
	&player_effect_stop_definition,
	&hud_show_health_definition,
	&hud_blink_health_definition,
	&hud_show_shield_definition,
	&hud_blink_shield_definition,
	&hud_show_motion_sensor_definition,
	&hud_blink_motion_sensor_definition,
	&hud_show_crosshair_definition,
	&hud_show_ammo_definition,
	&hud_clear_messages_definition,
	&hud_set_help_text_definition,
	&hud_set_objective_text_definition,
	&hud_set_timer_time_definition,
	&hud_set_timer_warning_time_definition,
	&hud_set_timer_position_definition,
	&show_hud_timer_definition,
	&pause_hud_timer_definition,
	&hud_get_timer_ticks_definition,
	&time_code_show_definition,
	&time_code_start_definition,
	&time_code_reset_definition,
	&rasterizer_profile_include_definition,
	&rasterizer_profile_include_all_definition,
	&rasterizer_profile_include_all_except_definition,
	&rasterizer_profile_exclude_definition,
	&rasterizer_profile_exclude_all_definition,
	&rasterizer_profile_exclude_all_except_definition,
	&rasterizer_debug_display_bitmap_definition,
	&rasterizer_decals_flush_definition,
	&rasterizer_lens_flares_clear_for_frame_definition,
	&rasterizer_test_screen_definition,
	&rasterizer_test_screen_supablur_definition,
	&rasterizer_test_screen_off_definition,
	&rasterizer_f2_definition,
	&rasterizer_f3_definition,
	&rasterizer_f4_definition,
	&rasterizer_f5_definition,
	&rasterizer_f6_definition,
	&rasterizer_decal_depth_bias_definition,
	&rasterizer_decal_depth_bias_slope_scale_definition,
	&rasterizer_hud_ten_foot_definition,
	&rasterizer_blur_definition,
	&rasterizer_solid_model_color_definition,
	&rasterizer_solid_structure_color_definition,
	&rasterizer_solid_structure_instanced_geometry_color_definition,
	&rasterizer_solid_sky_color_definition,
	&rasterizer_solid_decorator_color_definition,
	&rasterizer_solid_decorator_model_color_definition,
	&rasterizer_solid_decorator_decal_color_definition,
	&rasterizer_solid_decorator_sprite_color_definition,
	&decorator_rebuild_all_definition,
	&script_screen_effect_set_value_definition,
	&cinematic_screen_effect_start_definition,
	&cinematic_screen_effect_set_depth_of_field_definition,
	&cinematic_screen_effect_set_crossfade_definition,
	&cinematic_screen_effect_set_crossfade2_definition,
	&cinematic_screen_effect_stop_definition,
	&cinematic_set_near_clip_distance_definition,
	&cinematic_set_far_clip_distance_definition,
	&cinematic_set_environment_map_attenuation_definition,
	&cinematic_set_environment_map_bitmap_definition,
	&cinematic_reset_environment_map_bitmap_definition,
	&cinematic_set_environment_map_tint_definition,
	&cinematic_reset_environment_map_tint_definition,
	&cinematic_layer_definition,
	&cinematic_dynamic_reflections_definition,
	&lightmaps_expose_definition,
	&player_profile_create_definition,
	&player_profile_create_corrupt_definition,
	&player_has_skills_definition,
	&player_has_mad_secret_skills_definition,
	&controller_invert_look_definition,
	&controller_look_speed_definition,
	&controller_set_look_invert_definition,
	&controller_get_look_invert_definition,
	&controller_unlock_solo_levels_definition,
	&user_interface_controller_get_last_level_played_definition,
*/
	&ui_debug_load_main_menu_definition,
	&ui_debug_text_bounds_definition,
	&ui_debug_show_title_safe_bounds_definition,
	&ui_debug_element_bounds_definition,
	&ui_debug_screen_tag_definition,
	&ui_transition_out_console_window_definition,
	&ui_debug_show_screen_tag_path_definition,
	&ui_debug_show_current_screen_tag_definition,
	&ui_set_beta_definition,
/*
	&ui_memory_dump_definition,
	&ui_set_automation_mode_definition,
	&ui_set_automation_hopper_type_definition,
	&ui_set_automation_variant_type_and_map_definition,
	&ui_set_automation_desired_local_user_count_definition,
	&ui_set_automation_desired_desired_network_game_player_count_definition,
	&ui_set_automation_desired_network_game_length_seconds_definition,
	&ui_set_automation_desired_network_session_name_definition,
	&ui_set_automation_desired_controller_team_definition,
	&ui_set_automation_desired_controller_player_profile_definition,
	&objectives_clear_definition,
	&objectives_show_up_to_definition,
	&objectives_finish_up_to_definition,
	&input_suppress_vibration_definition,
	&update_remote_camera_definition,
	&net_status_filter_definition,
	&net_sim_reset_definition,
	&net_sim_spike_now_definition,
	&net_sim_dropspike_now_definition,
	&net_test_ping_definition,
	&net_test_channel_loopback_definition,
	&net_test_channel_delete_definition,
	&net_test_leave_squad_definition,
	&net_test_delegate_host_definition,
	&net_test_delegate_leader_definition,
	&net_test_map_name_definition,
	&net_test_campaign_difficulty_definition,
	&net_test_player_color_definition,
	&net_test_reset_objects_definition,
	&net_test_fatal_error_definition,
	&net_test_matchmaking_hopper_list_definition,
	&net_test_matchmaking_hopper_print_definition,
	&net_test_matchmaking_hopper_set_game_definition,
	&net_set_machine_name_definition,
*/
	&net_event_display_category_definition,
	&net_event_log_category_definition,
	&net_event_list_categories_definition,
/*
	&online_task_list_definition,
	&play_bink_movie_definition,
	&play_credits_definition,
	&bink_done_definition,
	&set_global_doppler_factor_definition,
	&set_global_mixbin_headroom_definition,
	&debug_sound_environment_source_parameter_definition,
	&data_mine_set_mission_segment_definition,
	&data_mine_display_mission_segment_definition,
	&data_mine_insert_definition,
	&data_mine_upload_definition,
	&data_mine_playback_definition,
	&data_mine_playback_last_definition,
	&data_mine_playback_exit_definition,
	&data_mine_enable_definition,
	&data_mine_track_event_definition,
	&data_mine_display_event_definition,
	&data_mine_show_all_events_definition,
	&data_mine_show_tracked_events_definition,
	&data_mine_show_displayed_events_definition,
	&data_mine_display_session_data_definition,
	&data_mine_display_disk_writes_definition,
	&data_mine_debug_menu_setting_definition,
	&data_mine_open_debug_menu_definition,
	&error_enable_definition,
	&render_layer_enable_definition,
	&render_layer_enable_all_definition,
	&rasterizer_overdraw_z_definition,
	&rasterizer_overdraw_definition,
	&rasterizer_layers_overdraw_z_definition,
	&rasterizer_layers_overdraw_definition,
	&test_memory_allocators_definition,
	&hs_test_memory_allocators_definition_811,
	&display_video_standard_definition,
	&test_xcr_monkey_enable_definition,
	&test_web_map_snapshot_definition,
	&test_telnet_status_enable_definition,
	&test_telnet_status_interval_definition,
	&webstats_disable_definition,
	&webstats_test_submit_definition,
	&netdebug_prefer_internet_definition,
	&script_temporary_disable_lightmap_shadows_definition,
	&flag_new_definition,
	&flag_new_at_look_definition,
	&flags_clear_definition,
	&flags_save_definition,
	&flags_save_filtered_definition,
	&flags_save_named_definition,
	&flags_save_named_filtered_definition,
	&flags_default_name_definition,
	&flags_default_comment_definition,
	&flags_set_filter_definition,
	&flags_export_definition,
	&flags_export_filtered_definition,
	&flags_export_named_definition,
	&flags_export_named_filtered_definition,
	&bug_now_definition,
	&bug_now_lite_definition,
	&bug_now_auto_definition,
	&object_list_children_definition,
	&voice_set_outgoing_channel_count_definition,
	&voice_set_voice_repeater_peer_index_definition,
	&net_leaderboard_clear_definition,
	&net_leaderboard_set_definition,
	&dump_loaded_tags_definition,
	&interpolator_start_definition,
	&interpolator_start_smooth_definition,
	&interpolator_stop_definition,
	&interpolator_restart_definition,
	&interpolator_is_active_definition,
	&interpolator_is_finished_definition,
	&interpolator_set_current_value_definition,
	&interpolator_get_current_value_definition,
	&interpolator_get_start_value_definition,
	&interpolator_get_final_value_definition,
	&interpolator_get_current_phase_definition,
	&interpolator_get_current_time_fraction_definition,
	&interpolator_get_start_time_definition,
	&interpolator_get_final_time_definition,
	&interpolator_evaluate_at_definition,
	&interpolator_evaluate_at_time_fraction_definition,
	&interpolator_evaluate_at_time_definition,
	&interpolator_evaluate_at_time_delta_definition,
	&interpolator_stop_all_definition,
	&interpolator_restart_all_definition,
	&interpolator_flip_definition,
	&weather_start_definition,
	&weather_stop_definition,
	&weather_change_intensity_definition,
	&animation_cache_stats_reset_definition,
	&cinematic_clone_players_weapon_definition,
	&cinematic_move_attached_objects_definition,
	&vehicle_enable_ghost_effects_definition,
	&ice_cream_flavor_stock_definition,
	&ice_cream_flavor_available_definition,
	&set_global_sound_environment_definition,
	&reset_dsp_image_definition,
	&game_save_cinematic_skip_definition,
	&cinematic_outro_start_definition,
	&cinematic_enable_ambience_details_definition,
	&set_rasterizer_gamma_definition,
	&predict_model_section_definition,
	&predict_structure_section_definition,
	&predict_lightmap_bucket_definition,
	&predict_bitmap_definition,
	&get_prediction_offset_definition,
	&rasterizer_bloom_override_definition,
	&rasterizer_bloom_override_reset_definition,
	&rasterizer_bloom_override_blur_amount_definition,
	&rasterizer_bloom_override_threshold_definition,
	&rasterizer_bloom_override_brightness_definition,
	&rasterizer_bloom_override_box_factor_definition,
	&rasterizer_bloom_override_max_factor_definition,
	&rasterizer_bloom_override_silver_bullet_definition,
	&rasterizer_bloom_override_only_definition,
	&rasterizer_bloom_override_high_res_definition,
	&rasterizer_bloom_override_brightness_alpha_definition,
	&rasterizer_bloom_override_max_factor_alpha_definition,
	&cache_block_for_one_frame_definition,
	&sound_suppress_ambience_update_on_revert_definition,
*/
	&ui_test_error_ok_definition,
	&ui_test_error_ok_cancel_definition,
	&ui_test_confirmation_definition,
/*
	&cinematic_lightmap_shadow_disable_definition,
	&cinematic_lightmap_shadow_enable_definition,
	&predict_animation_definition,
	&online_message_send_definition,
	&test_web_event_local_cheater_definition,
	&test_web_event_remote_cheater_definition,
	&test_cheating_detect_definition,
	&test_network_storage_simulate_definition,
	&scenario_dump_scenario_signature_enable_definition,
	&scenario_force_signature_failure_enable_definition,
	&scenario_force_name_failure_enable_definition,
	&scenario_ignore_failures_enable_definition,
	&scenario_dump_scenario_signature_disable_definition,
	&scenario_force_signature_failure_disable_definition,
	&scenario_force_name_failure_disable_definition,
	&scenario_ignore_failures_disable_definition,
	&hopper_verify_force_failures_enable_definition,
	&hopper_verify_ignore_failures_enable_definition,
	&hopper_verify_force_failures_disable_definition,
	&hopper_verify_ignore_failures_disable_definition,
	&loading_screen_fade_to_white_definition,
	&rasterizer_debug_frame_usage_definition,
	&rasterizer_debug_crap_pixel_shader_definition,
	&enable_render_light_suppressor_definition,
	&disable_render_light_suppressor_definition
*/
};

const int32 hs_function_table_count = NUMBEROF(hs_function_table);	// Originally 927

/* private code */

static void __cdecl hs_not_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xE9DA1, 0xE8FAB, hs_not_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_pin_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xE9DE4, 0xE8FEE, hs_pin_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_print_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xE9E31, 0xE903B, hs_print_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_players_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xE9E62, 0xE906C, hs_players_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl scenario_kill_trigger_volume_enable_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xE9E76, 0x0, scenario_kill_trigger_volume_enable_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl scenario_kill_trigger_volume_disable_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xE9EA8, 0x0, scenario_kill_trigger_volume_disable_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_teleport_players_not_in_trigger_volume_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xE9EDA, 0x0, hs_teleport_players_not_in_trigger_volume_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl scenario_trigger_volume_test_object_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xE9F11, 0x0, scenario_trigger_volume_test_object_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_trigger_volume_test_objects_any_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xE9F58, 0x0, hs_trigger_volume_test_objects_any_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_trigger_volume_test_objects_all_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xE9F9F, 0x0, hs_trigger_volume_test_objects_all_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl scenario_trigger_volume_return_objects_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xE9FE6, 0x0, scenario_trigger_volume_return_objects_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl scenario_trigger_volume_return_objects_by_type_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA017, 0x0, scenario_trigger_volume_return_objects_by_type_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_object_list_get_element_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA04C, 0x0, hs_object_list_get_element_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl object_list_count_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA081, 0x0, object_list_count_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl object_list_count_not_dead_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA0C4, 0x0, object_list_count_not_dead_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_effect_new_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA107, 0x0, hs_effect_new_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_effect_new_from_object_marker_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA13D, 0x0, hs_effect_new_from_object_marker_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_damage_new_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA176, 0x0, hs_damage_new_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_damage_object_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA1AC, 0x0, hs_damage_object_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_damage_object_list_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA1E1, 0x0, hs_damage_object_list_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_damage_players_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA216, 0x0, hs_damage_players_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_object_create_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA247, 0x0, hs_object_create_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_object_create_clone_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA279, 0x0, hs_object_create_clone_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_object_create_anew_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA2AB, 0x0, hs_object_create_anew_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_object_create_containing_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA2DD, 0x0, hs_object_create_containing_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_object_create_clone_containing_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA30E, 0x0, hs_object_create_clone_containing_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_object_create_anew_containing_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA33F, 0x0, hs_object_create_anew_containing_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_object_destroy_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA370, 0x0, hs_object_destroy_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_object_destroy_containing_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA3A1, 0x0, hs_object_destroy_containing_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_object_destroy_all_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA3D2, 0x0, hs_object_destroy_all_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_object_destroy_all_type_mask_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA3E7, 0x0, hs_object_destroy_all_type_mask_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_objects_delete_by_definition_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA418, 0x0, hs_objects_delete_by_definition_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_object_hide_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA449, 0x0, hs_object_hide_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl hs_object_set_shadowless_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA47F, 0x0, hs_object_set_shadowless_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl scripted_object_function_set_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA4B5, 0x0, scripted_object_function_set_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl object_scripting_set_function_variable_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA4ED, 0x0, object_scripting_set_function_variable_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl object_scripting_clear_function_variable_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA532, 0x0, object_scripting_clear_function_variable_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl object_scripting_clear_all_function_variables_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA567, 0x0, object_scripting_clear_all_function_variables_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl object_scripting_dynamic_simulation_disable_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA598, 0x0, object_scripting_dynamic_simulation_disable_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl object_scripting_set_phantom_power_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA5CE, 0x0, object_scripting_set_phantom_power_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl object_scripting_wake_physics_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA604, 0x0, object_scripting_wake_physics_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl object_set_ranged_attack_inhibited_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA635, 0x0, object_set_ranged_attack_inhibited_evaluate, function_index, thread_index, initialize);
	return;
}

static void __cdecl object_set_melee_attack_inhibited_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	INVOKE(0xEA66B, 0x0, object_set_melee_attack_inhibited_evaluate, function_index, thread_index, initialize);
	return;
}

static void objects_dump_memory_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
#ifdef OBJECT_DEBUG
	objects_dump_memory();
#endif
	hs_return(thread_index, 0);
	return;
}


static void hs_help_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
#ifdef HS_COMPILER_ENABLED
		hs_help((const char*)arguments[0]);
#endif
		hs_return(thread_index, 0);
	}
	return;
}


static void cheat_drop_tag_name_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
		cheat_drop_tag_name((const char*)arguments[0]);
		hs_return(thread_index, 0);
	}
	return;
}


static void main_crash_evaluate(
	int16 function_index,
	int32 thread_index, 
	bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
		main_crash((const char*)arguments[0]);
		hs_return(thread_index, 0);
	}
	return;
}

static void main_print_version_evaluate(
	int16 function_index,
	int32 thread_index,
	bool initialize)
{
	main_print_version();
	hs_return(thread_index, 0);
	return;
}

static void screenshot_cubemap_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
		screenshot_cubemap((const char*)arguments[0]);
		hs_return(thread_index, 0);
	}
	return;
}

static void main_menu_launch_force_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	main_menu_launch_force();
	hs_return(thread_index, 0);
	return;
}

static void main_halt_and_display_errors_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	main_halt_and_display_errors();
	hs_return(thread_index, 0);
	return;
}

static void main_game_launch_legacy_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
		main_game_launch_legacy((const char*)arguments[0]);
		hs_return(thread_index, 0);
	}
	return;
}

static void main_game_launch_set_multiplayer_engine_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
		main_game_launch_set_multiplayer_variant((const char*)arguments[0]);
		hs_return(thread_index, 0);
	}
	return;
}

static void main_game_launch_set_multiplayer_splitscreen_count_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
		main_game_launch_set_multiplayer_splitscreen_count(*(int32*)&arguments[0]);
		hs_return(thread_index, 0);
	}
	return;
}

static void main_game_launch_set_difficulty_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
		main_game_launch_set_difficulty(*(int16*)&arguments[0]);
		hs_return(thread_index, 0);
	}
	return;
}

static void main_game_launch_set_coop_player_count_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
		main_game_launch_set_coop_player_count(*(int32*)&arguments[0]);
		hs_return(thread_index, 0);
	}
	return;
}


static void scripted_player_effect_screen_fade_in_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
		scripted_player_effect_screen_fade_in(*(real32*)&arguments[0], *(real32*)&arguments[1], *(real32*)&arguments[2], *(int16*)&arguments[3]);
		hs_return(thread_index, 0);
	}
	return;
}

static void main_load_core_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	if (cartographer_allow_action_during_networked_game())
	{
		main_load_core();
	}
	else
	{
		console_printf(k_cartographer_function_evaluate_error);
	}
	hs_return(thread_index, 0);
	return;
}

static void main_load_core_name_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	int32* arguments = (int32*)hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
		main_load_core_name((const char*)arguments[0]);
		hs_return(thread_index, 0);
	}
	return;
}

static void main_save_core_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	if (cartographer_allow_action_during_networked_game())
	{
		main_save_core();
	}
	else
	{
		console_printf(k_cartographer_function_evaluate_error);
	}

	hs_return(thread_index, 0);
	return;
}

static void main_save_core_name_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	int32* arguments = (int32*)hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
		if (cartographer_allow_action_during_networked_game())
		{
			main_save_core_name((const char*)arguments[0]);
		}
		else
		{
			console_printf(k_cartographer_function_evaluate_error);
		}
		hs_return(thread_index, 0);
	}
	return;
}

static void main_game_load_from_core_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	if (cartographer_allow_action_during_networked_game())
	{
		main_game_load_from_core();
	}
	else
	{
		console_printf(k_cartographer_function_evaluate_error);
	}
	hs_return(thread_index, 0);
	return;
}

static void main_game_load_from_core_name_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	int32* arguments = (int32*)hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
		if (cartographer_allow_action_during_networked_game())
		{
			main_game_load_from_core_name((const char*)arguments[0]);
		}
		else
		{
			console_printf(k_cartographer_function_evaluate_error);
		}
		hs_return(thread_index, 0);
	}
	return;
}

static void user_interface_enter_game_shell_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
#ifdef UI_DEBUG
	user_interface_debug_load_main_menu();
#endif
	hs_return(thread_index, 0);
	return;
}

static void debug_enable_text_bounds_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
#ifdef UI_DEBUG
		user_interface_debug_text_bounds(*(bool*)&arguments[0]);
#endif
		hs_return(thread_index, 0);
	}
	return;
}

static void debug_render_title_safe_bounds_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
#ifdef UI_DEBUG
		debug_render_title_safe_bounds(*(bool*)&arguments[0]);
#endif
		hs_return(thread_index, 0);
	}
	return;
}

static void set_debug_frame_element_bounds_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
#ifdef UI_DEBUG
		set_debug_frame_element_bounds(*(bool*)&arguments[0]);
#endif
		hs_return(thread_index, 0);
	}
	return;
}

static void user_interface_test_screen_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
#ifdef UI_DEBUG
		user_interface_test_screen((const char*)arguments[0]);
#endif
		hs_return(thread_index, 0);
	}
	return;
}

static void user_interface_test_transition_out_console_screen_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
#ifdef UI_DEBUG
	user_interface_test_transition_out_console_screen();
#endif
	hs_return(thread_index, 0);
	return;
}

static void user_interface_debug_show_screen_tag_path_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
#ifdef UI_DEBUG
		set_debug_show_screen_tag_path(*(bool*)&arguments[0]);
#endif
		hs_return(thread_index, 0);
	}
	return;
}

static void user_interface_debug_show_current_screen_tag_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
#ifdef UI_DEBUG
		user_interface_show_current_screen_tag((const char*)arguments[0]);
#endif
		hs_return(thread_index, 0);
	}
	return;
}

static void user_interface_debug_set_beta_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
#ifdef UI_DEBUG
		debug_set_ui_beta(*(bool*)&arguments[0]);
#endif
		hs_return(thread_index, 0);
	}
	return;
}


static void net_event_set_display_level_by_name_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
#ifdef EVENTS_ENABLED
		network_event_display_category((const char*)arguments[0], *(e_event_level*)&arguments[1]);
#endif
		hs_return(thread_index, 0);
	}
	return;
}

static void net_event_set_log_level_by_name_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
#ifdef EVENTS_ENABLED
		network_event_log_category((const char*)arguments[0], *(e_event_level*)&arguments[1]);
#endif
		hs_return(thread_index, 0);
	}
	return;
}

static void net_event_dump_categories_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
#ifdef EVENTS_ENABLED
		network_event_dump_categories((const char*)arguments[0]);
#endif
		hs_return(thread_index, 0);
	}
	return;
}


static void user_interface_test_error_ok_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
#ifdef UI_DEBUG
		user_interface_test_error_ok(*(int16*)&arguments[0]);
#endif
		hs_return(thread_index, 0);
	}

	return;
}

static void user_interface_test_error_ok_cancel_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
#ifdef UI_DEBUG
		user_interface_test_error_ok_cancel(*(int16*)&arguments[0]);
#endif
		hs_return(thread_index, 0);
	}
	return;
}

static void user_interface_test_confirmation_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
#ifdef UI_DEBUG
		user_interface_test_confirmation(*(int16*)&arguments[0]);
#endif
		hs_return(thread_index, 0);
	}
	return;
}

static void director_debug_camera_evaluate(int16 function_index, int32 thread_index, bool initialize)
{
	const int32* arguments = hs_macro_function_evaluate(function_index, thread_index, initialize);
	if (arguments)
	{
		director_debug_camera(*(const uint8*)&arguments[0] != 0);
		hs_return(thread_index, 0);
	}
	return;
}
