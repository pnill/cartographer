#include "stdafx.h"
#include "tag_groups.h"

tag_group tag_group_get_name(tag_group group)
{
	// We need to reverse this since little endian reorders the character bytes
	tag_group reversed_group;
	reversed_group.string[0] = group.string[3];
	reversed_group.string[1] = group.string[2];
	reversed_group.string[2] = group.string[1];
	reversed_group.string[3] = group.string[0];
	return reversed_group;
}

uint32 tag_group_get_as_index(tag_group group)
{
	// this is unholy
    switch (group.group)
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
	    case _tag_group_none: return -1;
    }
}

void* __cdecl tag_block_get_element_with_size(const s_tag_block* block, int32 index, int32 block_size)
{
	return INVOKE(0x3C772, 0x32899, tag_block_get_element_with_size, block, index, block_size);
}

void set_tag_group_data_info(uint32 tag_data, uint32 tag_data_size)
{
	uint32* g_tag_group_data = Memory::GetAddress<uint32*>(0x482290, 0);
	uint32* g_tag_group_data_size = Memory::GetAddress<uint32*>(0x482294, 0);

	*g_tag_group_data = tag_data;
	*g_tag_group_data_size = tag_data_size;
}
