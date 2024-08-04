#pragma once

#define k_tag_group_count 120

enum e_tag_group : int32
{
	_tag_group_cache_file_sound = '$#!+',
	_tag_group_scenario_scenery_resource = '*cen',
	_tag_group_scenario_weapons_resource = '*eap',
	_tag_group_scenario_vehicles_resource = '*ehi',
	_tag_group_scenario_lights_resource = '*igh',
	_tag_group_scenario_bipeds_resource = '*ipd',
	_tag_group_scenario_equipment_resource = '*qip',
	_tag_group_scenario_creature_resource = '*rea',
	_tag_group_scenario_sound_scenery_resource = '*sce',
	_tag_group_scenario_comments_resource = '/**/',
	_tag_group_sound_effect_template = '<fx>',
	_tag_group_ai_dialogue_globals = 'adlg',
	_tag_group_scenario_ai_resource = 'ai**',
	_tag_group_antenna = 'ant!',
	_tag_group_biped = 'bipd',
	_tag_group_bitmap = 'bitm',
	_tag_group_crate = 'bloc',
	_tag_group_stereo_system = 'BooM',
	_tag_group_breakable_surface = 'bsdt',
	_tag_group_character = 'char',
	_tag_group_scenario_cinematics_resource = 'cin*',
	_tag_group_scenario_cluster_data_resource = 'clu*',
	_tag_group_cloth = 'clwd',
	_tag_group_collision_model = 'coll',
	_tag_group_colony = 'coln',
	_tag_group_color_table = 'colo',
	_tag_group_contrail = 'cont',
	_tag_group_creature = 'crea',
	_tag_group_device_control = 'ctrl',
	_tag_group_scenario_decorators_resource = 'dc*s',
	_tag_group_scenario_decals_resource = 'dec*',
	_tag_group_decal = 'deca',
	_tag_group_decorators = 'DECP',
	_tag_group_decorator_set = 'DECR',
	_tag_group_device = 'devi',
	_tag_group_cellular_automata = 'devo',
	_tag_group_scenario_devices_resource = 'dgr*',
	_tag_group_detail_object_collection = 'dobc',
	_tag_group_effect = 'effe',
	_tag_group_screen_effect = 'egor',
	_tag_group_equipment = 'eqip',
	_tag_group_planar_fog = 'fog ',
	_tag_group_material_effects = 'foot',
	_tag_group_patchy_fog = 'fpch',
	_tag_group_garbage = 'garb',
	_tag_group_chocolate_mountain = 'gldf',
	_tag_group_multiplayer_variant_settings_interface_definition = 'goof',
	_tag_group_grenade_hud_interface = 'grhi',
	_tag_group_model = 'hlmt',
	_tag_group_hud_message_text = 'hmt ',
	_tag_group_scenario_hs_source_file = 'hsc*',
	_tag_group_hud_number = 'hud#',
	_tag_group_hud_globals = 'hudg',
	_tag_group_item = 'item',
	_tag_group_item_collection = 'itmc',
	_tag_group_model_animation_graph = 'jmad',
	_tag_group_damage_effect = 'jpt!',
	_tag_group_lens_flare = 'lens',
	_tag_group_device_light_fixture = 'lifi',
	_tag_group_light = 'ligh',
	_tag_group_sound_looping = 'lsnd',
	_tag_group_scenario_structure_lightmap = 'ltmp',
	_tag_group_device_machine = 'mach',
	_tag_group_globals = 'matg',
	_tag_group_mouse_cursor_definition = 'mcsr',
	_tag_group_ai_mission_dialogue = 'mdlg',
	_tag_group_meter = 'metr',
	_tag_group_light_volume = 'MGS2',
	_tag_group_render_model = 'mode',
	_tag_group_material_physics = 'mpdt',
	_tag_group_multiplayer_scenario_description = 'mply',
	_tag_group_multiplayer_globals = 'mulg',
	_tag_group_new_hud_definition = 'nhdt',
	_tag_group_object = 'obje',
	_tag_group_physics_model = 'phmo',
	_tag_group_physics = 'phys',
	_tag_group_pixel_shader = 'pixl',
	_tag_group_particle_physics = 'pmov',
	_tag_group_point_physics = 'pphy',
	_tag_group_projectile = 'proj',
	_tag_group_particle = 'prt3',
	_tag_group_particle_model = 'PRTM',
	_tag_group_scenario_structure_bsp = 'sbsp',
	_tag_group_scenery = 'scen',
	_tag_group_scenario = 'scnr',
	_tag_group_sound_effect_collection = 'sfx+',
	_tag_group_shader = 'shad',
	_tag_group_text_value_pair_definition = 'sily',
	_tag_group_user_interface_list_skin_definition = 'skin',
	_tag_group_sky = 'sky ',
	_tag_group_shader_light_response = 'slit',
	_tag_group_sound_classes = 'sncl',
	_tag_group_sound = 'snd!',
	_tag_group_sound_environment = 'snde',
	_tag_group_sound_mix = 'snmx',
	_tag_group_shader_pass = 'spas',
	_tag_group_sound_dialogue_constants = 'spk!',
	_tag_group_sound_scenery = 'ssce',
	_tag_group_scenario_structure_lighting_resource = 'sslt',
	_tag_group_shader_template = 'stem',
	_tag_group_style = 'styl',
	_tag_group_liquid = 'tdtl',
	_tag_group_camera_track = 'trak',
	_tag_group_scenario_trigger_volumes_resource = 'trg*',
	_tag_group_dialogue = 'udlg',
	_tag_group_sound_cache_file_gestalt = 'ugh!',
	_tag_group_unit_hud_interface = 'unhi',
	_tag_group_multilingual_unicode_string_list = 'unic',
	_tag_group_unit = 'unit',
	_tag_group_vehicle_collection = 'vehc',
	_tag_group_vehicle = 'vehi',
	_tag_group_vertex_shader = 'vrtx',
	_tag_group_weapon = 'weap',
	_tag_group_weather_system = 'weat',
	_tag_group_user_interface_screen_widget_definition = 'wgit',
	_tag_group_user_interface_globals_definition = 'wgtz',
	_tag_group_cellular_automata2d = 'whip',
	_tag_group_user_interface_shared_globals_definition = 'wigl',
	_tag_group_wind = 'wind',
	_tag_group_weapon_hud_interface = 'wphi',
	_tag_group_none = NONE
};

union tag_group 
{
	e_tag_group group;
	char string[4];
};

struct s_tag_group_link
{
	tag_group child;
	tag_group parent_2;
	tag_group parent;
};


struct s_tag_block
{
	int32 count;
	int32 data;
};

s_tag_group_link* tag_group_get_link_set(tag_group group);

tag_group tag_group_get_name(tag_group group);

uint32 tag_group_get_as_index(tag_group group);

void* __cdecl tag_block_get_element_with_size(const s_tag_block* block, int32 index, int32 block_size);

void __cdecl set_tag_group_data_info(uint32 tag_data, uint32 tag_data_size);