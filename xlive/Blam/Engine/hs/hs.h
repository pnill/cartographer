#pragma once

#define k_maximum_hs_globals_per_scenario 256
#define k_maximum_hs_references_per_scenario 512
#define k_maximum_hs_source_files_per_scenario 8
#include "Blam/Cache/DataTypes/DataRef.h"
#include "Blam/Cache/DataTypes/TagRef.h"
#include "Blam/Engine/cseries/cseries_strings.h"
#include "Blam/Engine/tag_files/string_id.h"

enum e_hs_script_type : short
{
	script_type_startup = 0,
	script_type_dormant = 1,
	script_type_continuous = 2,
	script_type_static = 3,
	script_type_stub = 4,
	script_type_command_script = 5,
};

enum e_hs_type : WORD 
{
	hs_type_unparsed = 0,
	hs_type_special_form,
	hs_type_function_name,
	hs_type_passthrough,
	hs_type_nothing,
	hs_type_boolean,
	hs_type_real,
	hs_type_hs_short,
	hs_type_hs_long,
	hs_type_string,
	hs_type_script,
	hs_type_string_id,
	hs_type_unit_seat_mapping,
	hs_type_trigger_volume,
	hs_type_cutscene_flag,
	hs_type_cutscene_camera_point,
	hs_type_cutscene_title,
	hs_type_cutscene_recording,
	hs_type_device_group,
	hs_type_ai,
	hs_type_ai_command_list,
	hs_type_ai_command_script,
	hs_type_ai_behavior,
	hs_type_ai_orders,
	hs_type_starting_profile,
	hs_type_conversation,
	hs_type_structure_bsp,
	hs_type_navpoint,
	hs_type_point_reference,
	hs_type_style,
	hs_type_hud_message,
	hs_type_object_list,
	hs_type_sound,
	hs_type_effect,
	hs_type_damage,
	hs_type_looping_sound,
	hs_type_animation_graph,
	hs_type_damage_effect,
	hs_type_object_definition,
	hs_type_bitmap,
	hs_type_shader,
	hs_type_render_model,
	hs_type_structure_definition,
	hs_type_lightmap_definition,
	hs_type_game_difficulty,
	hs_type_team,
	hs_type_actor_type,
	hs_type_hud_corner,
	hs_type_model_state,
	hs_type_network_event,
	hs_type_object,
	hs_type_unit,
	hs_type_vehicle,
	hs_type_weapon,
	hs_type_device,
	hs_type_scenery,
	hs_type_object_name,
	hs_type_unit_name,
	hs_type_vehicle_name,
	hs_type_weapon_name,
	hs_type_device_name,
	hs_type_scenery_name
};

// max count: k_maximum_hs_globals_per_scenario 256
struct hs_global_internal
{
	static_string32 name;
	e_hs_type type;
	short pad;
	datum initialization_expression_index;
};
TAG_BLOCK_SIZE_ASSERT(hs_global_internal, 40);

// max count: k_maximum_hs_references_per_scenario 512
struct hs_tag_reference
{
	tag_reference reference;	// any tag allowed
};
TAG_BLOCK_SIZE_ASSERT(hs_tag_reference, 8);

// max count: k_maximum_hs_source_files_per_scenario 8
struct hs_source_file
{
	 static_string32 name;
	 data_block source;		// DataSize: 262144
};
TAG_BLOCK_SIZE_ASSERT(hs_tag_reference, 8);

struct HaloScriptGlobal
{
	e_hs_type type;
	BYTE padding[2];
	void* data;
};

typedef void** (__cdecl* hs_func_impl)(int hs_opcode, void* script_engine, char a3);

struct HaloScriptCommand
{
	e_hs_type type;
	WORD unk;
	hs_func_impl func;
	char* usage;
	WORD arg_count;
	e_hs_type arg_array[1]; // array size is dynamic, but always at least one
};
