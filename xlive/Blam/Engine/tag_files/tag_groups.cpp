#include "stdafx.h"
#include "tag_groups.h"

#include "tag_group_access.h"

#include "cache/cache_files.h"

/* constants */

enum
{
	k_maximum_string_id_storage = 0x60000
};

static const uint32 k_string_id_length_shift = 24;

static const uint32 k_string_id_index_mask = 0xFFFFFF;	// First 24 bits set to 1

#ifdef HS_COMPILER_ENABLED
const char* global_tag_group_names[]
{
	"model",
	"render_model",
	"collision_model",
	"physics_model",
	"bitmap",
	"color_table",
	"multilingual_unicode_string_list",
	"unit",
	"biped",
	"vehicle",
	"scenery",
	"crate",
	"creature",
	"physics",
	"object",
	"contrail",
	"weapon",
	"light",
	"effect",
	"particle",
	"particle_model",
	"particle_physics",
	"globals",
	"sound",
	"sound_looping",
	"item",
	"equipment",
	"antenna",
	"light_volume",
	"liquid",
	"cellular_automata",
	"cellular_automata2d",
	"stereo_system",
	"camera_track",
	"projectile",
	"device",
	"device_machine",
	"device_control",
	"device_light_fixture",
	"point_physics",
	"scenario_structure_lightmap",
	"scenario_structure_bsp",
	"scenario",
	"shader",
	"shader_template",
	"shader_light_response",
	"shader_pass",
	"vertex_shader",
	"pixel_shader",
	"decorator_set",
	"decorators",
	"sky",
	"wind",
	"sound_environment",
	"lens_flare",
	"planar_fog",
	"patchy_fog",
	"meter",
	"decal",
	"colony",
	"damage_effect",
	"dialogue",
	"item_collection",
	"vehicle_collection",
	"weapon_hud_interface",
	"grenade_hud_interface",
	"unit_hud_interface",
	"new_hud_definition",
	"hud_number",
	"hud_globals",
	"multiplayer_scenario_description",
	"detail_object_collection",
	"sound_scenery",
	"hud_message_text",
	"user_interface_screen_widget_definition",
	"user_interface_list_skin_definition",
	"user_interface_globals_definition",
	"user_interface_shared_globals_definition",
	"text_value_pair_definition",
	"multiplayer_variant_settings_interface_definition",
	"material_effects",
	"garbage",
	"style",
	"character",
	"ai_dialogue_globals",
	"ai_mission_dialogue",
	"scenario_scenery_resource",
	"scenario_bipeds_resource",
	"scenario_vehicles_resource",
	"scenario_equipment_resource",
	"scenario_weapons_resource",
	"scenario_sound_scenery_resource",
	"scenario_lights_resource",
	"scenario_devices_resource",
	"scenario_decals_resource",
	"scenario_cinematics_resource",
	"scenario_trigger_volumes_resource",
	"scenario_cluster_data_resource",
	"scenario_creature_resource",
	"scenario_decorators_resource",
	"scenario_structure_lighting_resource",
	"scenario_hs_source_file",
	"scenario_ai_resource",
	"scenario_comments_resource",
	"breakable_surface",
	"material_physics",
	"sound_classes",
	"multiplayer_globals",
	"sound_effect_template",
	"sound_effect_collection",
	"chocolate_mountain",
	"model_animation_graph",
	"cloth",
	"screen_effect",
	"weather_system",
	"sound_mix",
	"sound_dialogue_constants",
	"sound_cache_file_gestalt",
	"cache_file_sound",
	"mouse_cursor_definition"
};
#endif

/* typedefs */

typedef int32(__cdecl* subtract_function_t)(uint32 a1, uint32 a2, uint32 a3);

/* globals */

static int32 g_string_id_count;

static int32 g_string_id_block_offset;

static int32 g_string_id_index_buffer[0x6000];

static char g_string_id_storage[k_maximum_string_id_storage];

/* public code */

int32 __cdecl tag_group_get_link_set_sort(
	uint32 a1,
	uint32 a2,
	uint32 a3)
{
	return INVOKE(0x30D90, 0x24C40, tag_group_get_link_set_sort, a1, a2, a3);
}

int32 __cdecl tag_group_get_link_set_index(s_tag_group_link* link_set, s_tag_group_link* tag_link_set, uint32 tag_link_set_size, int32 element_size, subtract_function_t sort_function, int32 unk)
{
	return INVOKE(0x8CAC3, 0x7333C, tag_group_get_link_set_index, link_set, tag_link_set, tag_link_set_size, element_size, sort_function, unk);
}

s_tag_group_link* tag_group_get_link_set(uint32 group)
{
	s_cache_file_memory_globals* cache_file_memory = cache_file_memory_globals_get();

	s_tag_group_link* group_link = NULL;

	if (cache_file_memory->tags_header)
	{
		s_tag_group_link link_set;
		link_set.child = group;
		link_set.parent_2 = (uint32)_tag_group_none;
		link_set.parent = (uint32)_tag_group_none;

		int32 link_index = tag_group_get_link_set_index(
			&link_set,
			cache_file_memory->tags_header->tag_group_link_set,
			cache_file_memory->tags_header->tag_group_link_set_count,
			12,
			tag_group_get_link_set_sort,
			0);

		if (link_index != NONE)
		{
			group_link = &cache_file_memory->tags_header->tag_group_link_set[link_index];
		}
	}

	return group_link;
}

char* tag_to_string(
	uint32 t,
	char* s)
{
	// We need to reverse this since little endian reorders the character bytes
	s[0] = (t >> 24) & BYTE_MAX;
	s[1] = (t >> 16) & BYTE_MAX;
	s[2] = (t >> 8) & BYTE_MAX;
	s[3] = t & BYTE_MAX;
	s[4] = '\0';

	return s;
}

// TODO: move this to tag loader code and rename to something like tag_group_to_agent_type and introduce an agent type enum for all tag types
int32 tag_group_get_as_index(tag_group group)
{
	// this is unholy
	switch (group)
	{
		case _tag_group_cache_file_sound: return 0;
		case _tag_group_scenario_scenery_resource: return 1;
		case _tag_group_scenario_weapons_resource: return 2;
		case _tag_group_scenario_vehicles_resource: return 3;
		case _tag_group_scenario_lights_resource: return 4;
		case _tag_group_scenario_bipeds_resource: return 5;
		case _tag_group_scenario_equipment_resource: return 6;
		case _tag_group_scenario_creature_resource: return 7;
		case _tag_group_scenario_sound_scenery_resource: return 8;
		case _tag_group_scenario_comments_resource: return 9;
		case _tag_group_sound_effect_template: return 10;
		case _tag_group_ai_dialogue_globals: return 11;
		case _tag_group_scenario_ai_resource: return 12;
		case _tag_group_antenna: return 13;
		case _tag_group_biped: return 14;
		case _tag_group_bitmap: return 15;
		case _tag_group_crate: return 16;
		case _tag_group_stereo_system: return 17;
		case _tag_group_breakable_surface: return 18;
		case _tag_group_character: return 19;
		case _tag_group_scenario_cinematics_resource: return 20;
		case _tag_group_scenario_cluster_data_resource: return 21;
		case _tag_group_cloth: return 22;
		case _tag_group_collision_model: return 23;
		case _tag_group_colony: return 24;
		case _tag_group_color_table: return 25;
		case _tag_group_contrail: return 26;
		case _tag_group_creature: return 27;
		case _tag_group_device_control: return 28;
		case _tag_group_scenario_decorators_resource: return 29;
		case _tag_group_scenario_decals_resource: return 30;
		case _tag_group_decal: return 31;
		case _tag_group_decorators: return 32;
		case _tag_group_decorator_set: return 33;
		case _tag_group_device: return 34;
		case _tag_group_cellular_automata: return 35;
		case _tag_group_scenario_devices_resource: return 36;
		case _tag_group_detail_object_collection: return 37;
		case _tag_group_effect: return 38;
		case _tag_group_screen_effect: return 39;
		case _tag_group_equipment: return 40;
		case _tag_group_planar_fog: return 41;
		case _tag_group_material_effects: return 42;
		case _tag_group_patchy_fog: return 43;
		case _tag_group_garbage: return 44;
		case _tag_group_chocolate_mountain: return 45;
		case _tag_group_multiplayer_variant_settings_interface_definition: return 46;
		case _tag_group_grenade_hud_interface: return 47;
		case _tag_group_model: return 48;
		case _tag_group_hud_message_text: return 49;
		case _tag_group_scenario_hs_source_file: return 50;
		case _tag_group_hud_number: return 51;
		case _tag_group_hud_globals: return 52;
		case _tag_group_item: return 53;
		case _tag_group_item_collection: return 54;
		case _tag_group_model_animation_graph: return 55;
		case _tag_group_damage_effect: return 56;
		case _tag_group_lens_flare: return 57;
		case _tag_group_device_light_fixture: return 58;
		case _tag_group_light: return 59;
		case _tag_group_sound_looping: return 60;
		case _tag_group_scenario_structure_lightmap: return 61;
		case _tag_group_device_machine: return 62;
		case _tag_group_globals: return 63;
		case _tag_group_mouse_cursor_definition: return 64;
		case _tag_group_ai_mission_dialogue: return 65;
		case _tag_group_meter: return 66;
		case _tag_group_light_volume: return 67;
		case _tag_group_render_model: return 68;
		case _tag_group_material_physics: return 69;
		case _tag_group_multiplayer_scenario_description: return 70;
		case _tag_group_multiplayer_globals: return 71;
		case _tag_group_new_hud_definition: return 72;
		case _tag_group_object: return 73;
		case _tag_group_physics_model: return 74;
		case _tag_group_physics: return 75;
		case _tag_group_pixel_shader: return 76;
		case _tag_group_particle_physics: return 77;
		case _tag_group_point_physics: return 78;
		case _tag_group_projectile: return 79;
		case _tag_group_particle: return 80;
		case _tag_group_particle_model: return 81;
		case _tag_group_scenario_structure_bsp: return 82;
		case _tag_group_scenery: return 83;
		case _tag_group_scenario: return 84;
		case _tag_group_sound_effect_collection: return 85;
		case _tag_group_shader: return 86;
		case _tag_group_text_value_pair_definition: return 87;
		case _tag_group_user_interface_list_skin_definition: return 88;
		case _tag_group_sky: return 89;
		case _tag_group_shader_light_response: return 90;
		case _tag_group_sound_classes: return 91;
		case _tag_group_sound: return 92;
		case _tag_group_sound_environment: return 93;
		case _tag_group_sound_mix: return 94;
		case _tag_group_shader_pass: return 95;
		case _tag_group_sound_dialogue_constants: return 96;
		case _tag_group_sound_scenery: return 97;
		case _tag_group_scenario_structure_lighting_resource: return 98;
		case _tag_group_shader_template: return 99;
		case _tag_group_style: return 100;
		case _tag_group_liquid: return 101;
		case _tag_group_camera_track: return 102;
		case _tag_group_scenario_trigger_volumes_resource: return 103;
		case _tag_group_dialogue: return 104;
		case _tag_group_sound_cache_file_gestalt: return 105;
		case _tag_group_unit_hud_interface: return 106;
		case _tag_group_multilingual_unicode_string_list: return 107;
		case _tag_group_unit: return 108;
		case _tag_group_vehicle_collection: return 109;
		case _tag_group_vehicle: return 110;
		case _tag_group_vertex_shader: return 111;
		case _tag_group_weapon: return 112;
		case _tag_group_weather_system: return 113;
		case _tag_group_user_interface_screen_widget_definition: return 114;
		case _tag_group_user_interface_globals_definition: return 115;
		case _tag_group_cellular_automata2d: return 116;
		case _tag_group_user_interface_shared_globals_definition: return 117;
		case _tag_group_wind: return 118;
		case _tag_group_weapon_hud_interface: return 119;
		default:
		case _tag_group_none: return NONE;
	}
}

void* tag_block_get_element_with_size(
	s_tag_block const* block,
	int32 index,
	int32 block_size)
{
	//return INVOKE(0x3C772, 0x32899, tag_block_get_element_with_size, block, index, block_size);

	void* result = NULL;

	ASSERT(block);
	ASSERT(block->count>=0);

	if (index || block->count)
	{
		vassert(VALID_INDEX(index, block->count), "#%d is not a valid %s index in [#0, #%d)", index, "<unknown>", block->count);

		ASSERT(tag_block_get_address(block));

		result = (void*)(block_size * index + (intptr_t)tag_block_get_address(block));

		ASSERT(result);
	}
	else
	{
		error(_error_delayed, "asked for index=0 with block->count=0");
	}

	return result;
}

void tag_group_set_data_info(
	uint32 tag_data,
	uint32 tag_data_size)
{
	uint32* g_tag_group_data = Memory::GetAddress<uint32*>(0x482290, 0x4A6438);
	uint32* g_tag_group_data_size = Memory::GetAddress<uint32*>(0x482294, 0x4A643C);

	*g_tag_group_data = tag_data;
	*g_tag_group_data_size = tag_data_size;
	return;
}

void string_id_convert_string(
	char* string)
{
	for (size_t i = 0; string[i] != '\0'; ++i)
	{
		// Convert upper to lowercase
		if (IN_RANGE(string[i], 'A', 'Z'))
		{
			string[i] -= 'A';
			string[i] += 'a';
		}

		// Convert spaces and dashes to underscores
		if (string[i] == ' ' || string[i] == '-')
		{
			*string = '_';
		}
	}

	return;
}

bool string_id_load_strings(
	const cache_file_header* header)
{
	const int32 string_idx_offset = header->string_idx_offset;
	
	g_string_id_block_offset = header->string_block_offset;
	g_string_id_count = header->string_table_count;

	uint32 read_count = cache_file_align_read_size_to_cache_page(sizeof(string_id) * g_string_id_count);
	
	ASSERT(read_count < sizeof(g_string_id_index_buffer));

	const bool result = cache_file_blocking_read(NONE, string_idx_offset, read_count, g_string_id_index_buffer);
	
	if (result)
	{
		read_count = cache_file_align_read_size_to_cache_page(header->string_table_size);
		cache_file_blocking_read(NONE, header->string_table_offset, read_count, g_string_id_storage);
	}

	return result;
}

char* string_id_get_string(int32 index, char(& buffer)[128])
{
	ASSERT(index < g_string_id_count);

	csmemset(buffer, 0, sizeof(buffer));
	const int32 storage_index = g_string_id_index_buffer[index];

	ASSERT(VALID_INDEX(storage_index, k_maximum_string_id_storage));
	return csstrncpy(buffer, &g_string_id_storage[storage_index], sizeof(buffer));
}

const char* string_id_get_string_const(string_id id)
{
	const char* result = NULL;
	if ((id & k_string_id_index_mask) < (uint32)g_string_id_count)
	{
		int32 storage_index = g_string_id_index_buffer[id & k_string_id_index_mask];
		
		ASSERT(VALID_INDEX(storage_index, k_maximum_string_id_storage));
		result = &g_string_id_storage[storage_index];
	}

	return result;
}

string_id string_id_exists(
	const char* in_string)
{
	char string[128];
	
	csstrncpy(string, in_string, NUMBEROF(string));
	string_id_convert_string(string);

	string_id result = _string_id_invalid;
	
	for (int32 i = 0; i < g_string_id_count; ++i)
	{
		char string_id_string[128];
		string_id_get_string(i, string_id_string);
		if (csstrcmp(string, string_id_string) == 0)
		{
			const int32 length = (int32)cstrlen(string);
			result = (string_id)(i | (length << k_string_id_length_shift));
		}
	}
	
	return result;
}

/* private code */
