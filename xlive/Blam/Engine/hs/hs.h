#pragma once
#include "tag_files/data_reference.h"
#include "tag_files/tag_groups.h"
#include "tag_files/tag_reference.h"

/* constants */

enum
{
	k_maximum_hs_globals_per_scenario = 256,
	k_maximum_hs_references_per_scenario = 512,
	k_maximum_hs_source_files_per_scenario = 8,
};

/* enums */

enum e_hs_script_type : int16
{
	_hs_script_startup = 0,
	_hs_script_dormant = 1,
	_hs_script_continuous = 2,
	_hs_script_static = 3,
	_hs_script_stub = 4,
	_hs_script_command_script = 5,
	NUMBER_OF_HS_SCRIPT_TYPES
};

enum e_hs_type : int16 
{
	_hs_unparsed = 0,
	_hs_special_form,
	_hs_function_name,
	_hs_passthrough,

	/* types */

	FIRST_HS_TYPE,
	_hs_type_void = FIRST_HS_TYPE,
	_hs_type_boolean,
	_hs_type_real,
	_hs_type_short_integer,
	_hs_type_long_integer,
	_hs_type_string,
	_hs_type_script,
	_hs_type_string_id,
	_hs_type_unit_seat_mapping,
	_hs_type_trigger_volume,
	_hs_type_cutscene_flag,
	_hs_type_cutscene_camera_point,
	_hs_type_cutscene_title,
	_hs_type_cutscene_recording,
	_hs_type_device_group,
	_hs_type_ai,
	_hs_type_ai_command_list,
	_hs_type_ai_command_script,
	_hs_type_ai_behavior,
	_hs_type_ai_orders,
	_hs_type_starting_profile,
	_hs_type_conversation,
	_hs_type_structure_bsp,
	_hs_type_navpoint,
	_hs_type_point_reference,
	_hs_type_style,
	_hs_type_hud_message,
	_hs_type_object_list,

	/* tag references */

	FIRST_HS_TAG_REFERENCE_TYPE,
	_hs_type_sound = FIRST_HS_TAG_REFERENCE_TYPE,
	_hs_type_effect,
	_hs_type_damage,
	_hs_type_looping_sound,
	_hs_type_animation_graph,
	_hs_type_damage_effect,
	_hs_type_object_definition,
	_hs_type_bitmap,
	_hs_type_shader,
	_hs_type_render_model,
	_hs_type_structure_definition,
	_hs_type_lightmap_definition,
	LAST_HS_TAG_REFERENCE_TYPE = _hs_type_lightmap_definition,

	/* enums */

	FIRST_HS_ENUM_TYPE,
	_hs_type_enum_game_difficulty = FIRST_HS_ENUM_TYPE,
	_hs_type_enum_team,
	_hs_type_enum_actor_type,
	_hs_type_enum_hud_corner,
	_hs_type_enum_model_state,
	_hs_type_enum_network_event,
	NOT_LAST_HS_ENUM_TYPE = _hs_type_enum_network_event,

	/* objects */

	FIRST_HS_OBJECT_TYPE,
	_hs_type_object = FIRST_HS_OBJECT_TYPE,
	_hs_type_unit,
	_hs_type_vehicle,
	_hs_type_weapon,
	_hs_type_device,
	_hs_type_scenery,
	LAST_HS_OBJECT_TYPE = _hs_type_scenery,

	/* object names */

	FIRST_HS_OBJECT_NAME_TYPE,
	_hs_type_object_name = FIRST_HS_OBJECT_NAME_TYPE,
	_hs_type_unit_name,
	_hs_type_vehicle_name,
	_hs_type_weapon_name,
	_hs_type_device_name,
	_hs_type_scenery_name,
	LAST_HS_OBJECT_NAME_TYPE = _hs_type_scenery_name,

	NUMBER_OF_HS_NODE_TYPES,

	/* counts */

	NUMBER_OF_HS_OBJECT_TYPES = LAST_HS_OBJECT_TYPE - FIRST_HS_OBJECT_TYPE + 1,
	NUMBER_OF_HS_ENUM_TYPES = NOT_LAST_HS_ENUM_TYPE - FIRST_HS_ENUM_TYPE + 1
};

/* enums */

enum e_hs_function : int16
{
	_hs_function_begin,
	_hs_function_begin_random,
	_hs_function_if,
	_hs_function_cond,
	_hs_function_set,
	_hs_function_and,
	_hs_function_or,
	_hs_function_plus,
	_hs_function_minus,
	_hs_function_times,
	_hs_function_divide,
	_hs_function_min,
	_hs_function_max,
	_hs_function_equal,
	_hs_function_not_equal,
	_hs_function_gt,
	_hs_function_lt,
	_hs_function_gte,
	_hs_function_lte,
	_hs_function_sleep,
	_hs_function_sleep_forever,
	_hs_function_sleep_until,
	_hs_function_wake,
	_hs_function_inspect,
	_hs_function_object_to_unit,

	_hs_function_debug_string__ai_debug_communication_suppress,
	_hs_function_debug_string__ai_debug_communication_ignore,
	_hs_function_debug_string__ai_debug_communication_focus,
	_hs_function_debug_string__first = _hs_function_debug_string__ai_debug_communication_suppress,
	_hs_function_debug_string__last = _hs_function_debug_string__ai_debug_communication_focus,
};

enum
{
	_hs_thread_in_function_call_bit = 0,
	_hs_thread_latent_sleep_bit = 1,
	_hs_thread_display_expression_bit = 2,
	_hs_thread_hide_display_bit = 3,
	_hs_thread_verbose_bit = 4,
	_hs_thread_woken_bit = 5,
	_hs_thread_terminate_bit = 6,
	_hs_thread_abort_bit = 7,
	NUMBER_OF_HS_THREAD_FLAGS = 8,
};

/* structures */

// max count: k_maximum_hs_globals_per_scenario 256
struct hs_global_internal
{
	char name[k_tag_string_length];
	e_hs_script_type script_type;
	e_hs_type return_type;
	datum initialization_expression_index;
};
ASSERT_STRUCT_SIZE(hs_global_internal, 40);

// max count: k_maximum_hs_references_per_scenario 512
struct hs_tag_reference
{
	tag_reference reference;	// any tag allowed
};
ASSERT_STRUCT_SIZE(hs_tag_reference, 8);

// max count: k_maximum_hs_source_files_per_scenario 8
struct hs_source_file
{
	 char name[k_tag_string_length];
	 data_reference source;		// DataSize: 262144
};
ASSERT_STRUCT_SIZE(hs_tag_reference, 8);

struct hs_enum_definition
{
	int16 count;
	const char** identifiers;
};

struct hs_function_definition
{
	e_hs_type return_type;
#ifdef HS_COMPILER_ENABLED
	const char* name;
#endif
	int16 flags;
#ifdef HS_COMPILER_ENABLED
	bool(__cdecl* parse)(int16, int32);
#endif
	void(__cdecl* evaluate)(int16, int32, bool);
#ifdef HS_COMPILER_ENABLED
	const char* documentation;
#endif
	const char* parameters;
	int16 formal_parameter_count;

// Disable warning that comaplins about variable length arrays
#pragma warning(push)
#pragma warning(disable : 4200)
	e_hs_type formal_parameters[];
#pragma warning(pop)
};

/* constants */

#ifdef ERRORS_ENABLED
extern const char* hs_type_names[NUMBER_OF_HS_NODE_TYPES];

extern const char* hs_script_type_names[NUMBER_OF_HS_SCRIPT_TYPES];
#endif

extern const char* const hs_type_string_default;

extern const int16 hs_object_type_masks[NUMBER_OF_HS_OBJECT_TYPES];
extern const tag_group hs_tag_reference_type_group_tags[];
extern const int16 hs_type_sizes[NUMBER_OF_HS_NODE_TYPES];

extern const bool hs_type_boolean_default;
extern const real32 hs_type_real_default;
extern const int16 hs_type_short_integer_default;
extern const int32 hs_type_long_integer_default;
extern const int16 hs_type_script_default;
extern const string_id hs_type_string_id_default;
extern const int32 hs_type_unit_seat_mapping_default;
extern const int16 hs_type_trigger_volume_default;
extern const int16 hs_type_cutscene_flag_default;
extern const int16 hs_type_cutscene_camera_point_default;
extern const int16 hs_type_cutscene_title_default;
extern const int16 hs_type_cutscene_recording_default;
extern const int32 hs_type_device_group_default;
extern const int32 hs_type_ai_default;
extern const int16 hs_type_ai_command_list_default;
extern const int16 hs_type_ai_command_script_default;
extern const int16 hs_type_ai_behavior_default;
extern const int16 hs_type_ai_orders_default;
extern const int16 hs_type_starting_profile_default;
extern const int16 hs_type_conversation_default;
extern const int16 hs_type_structure_bsp_default;
extern const int16 hs_type_navpoint_default;
extern const int32 hs_type_style_default;
extern const int16 hs_type_hud_message_default;
extern const int32 hs_type_object_list_default;
extern const int32 hs_type_sound_default;
extern const int32 hs_type_looping_sound_default;
extern const int32 hs_type_effect_default;
extern const int32 hs_type_damage_default;
extern const int32 hs_type_animation_graph_default;
extern const int32 hs_type_damage_effect_default;
extern const int32 hs_type_object_definition_default;
extern const int32 hs_type_bitmap_default;
extern const int32 hs_type_shader_default;
extern const int32 hs_type_render_model_definition_default;
extern const int32 hs_type_structure_bsp_definition_default;
extern const int32 hs_type_structure_lightmap_definition_default;
extern const int16 hs_type_enum_game_difficulty_default;
extern const int16 hs_type_enum_team_default;
extern const int16 hs_type_enum_actor_type_default;
extern const int16 hs_type_enum_hud_corner_default;
extern const int16 hs_type_enum_model_state_default;
extern const int16 hs_type_enum_network_event_default;
extern const int16 hs_type_object_name_default;
extern const int32 hs_type_object_default;
extern const int32 hs_type_unit_default;
extern const int32 hs_type_vehicle_default;
extern const int32 hs_type_weapon_default;
extern const int32 hs_type_device_default;
extern const int32 hs_type_scenery_default;

extern const hs_enum_definition hs_enum_table[NUMBER_OF_HS_ENUM_TYPES];

/* globals */

#ifdef HS_COMPILER_ENABLED
extern char* g_error_output_buffer;
extern int32 g_error_buffer_length;
#endif

/* macros */

#define HS_GLOBAL_INDEX(index) ((index) & MASK(SHORT_BITS - 1))

#define HS_GLOBAL_EXTERNAL_GET(index) hs_global_external_get((index))
#define HS_GLOBAL_INTERNAL_GET(index) (TAG_BLOCK_GET_ELEMENT((&(global_scenario_get()->hs_globals)), (index), hs_global_internal))

#define hs_type_valid(type) ((type) >= FIRST_HS_TYPE && (type) < NUMBER_OF_HS_NODE_TYPES)

#define HS_TYPE_IS_ENUM(type) IN_RANGE((type), FIRST_HS_ENUM_TYPE, NOT_LAST_HS_ENUM_TYPE)

/* prototypes */

void hs_apply_patches(void);

struct data_array** hs_syntax_data_get(void);

void hs_initialize(void);

void __cdecl hs_initialize_for_new_map(void);

void hs_dispose_from_old_map(void);

void __cdecl hs_update(void);

#ifdef HS_COMPILER_ENABLED

// Takes in a hs expression and compiles the expression into a series of hs syntax nodes
bool hs_compile_and_evaluate(const char* expression, bool interactive);

// Print out an error in the hs statement after attempting to compile it into hs syntax nodes
void hs_compile_source_error(const char* file_name, const char* error_message, char* error_source, const char* source);

// Takes a function name and prints out the respective description to the console
void hs_help(const char* function_name);

// Rebuild hs from source
bool hs_rebuild_and_compile(char* error, int32 length);

#endif

// Performs garbage collection on halo script nodes
void hs_node_gc(void);

// Get a function definition from an index passed
const struct hs_function_definition* hs_function_get(int16 function_index);

// Find the script index given a specific script name
int16 hs_find_script_by_name(const char* name);

// Get a hs global from an index
const struct hs_global_external* hs_global_external_get(int16 global_index);

// Get the hs type of a 
int16 hs_global_get_type(int16 designator);

#ifdef HS_COMPILER_ENABLED

// Given a hs global designator we return the name of a global
const char* hs_global_get_name(int16 designator);

// Given a hs global name we return an index to the global
int16 hs_find_global_by_name(const char* name);

// Given a hs function name and the parameter count for that function we return an index to the function
int16 hs_find_function_by_name(const char* name, int16 count);

// Given a substring, return the number and a list of autocomplete results
int16 hs_tokens_enumerate(const char* substring, uint32 type_flags, const char** results, int16 maximum_count);

#endif
