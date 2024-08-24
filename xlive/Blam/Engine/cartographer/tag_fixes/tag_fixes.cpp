#include "stdafx.h"
#include "tag_fixes.h"

#include "interface/new_hud_definitions.h"
#include "scenario/scenario.h"
#include "shaders/shader_definitions.h"
#include "structures/structure_bsp_definitions.h"
#include "tag_files/tag_loader/tag_injection.h"

void tag_fixes_masterchief(void);
void tag_fixes_grunt(void);
void tag_fixes_brute(void);
void tag_fixes_smg(void);
void tag_fixes_environment(void);
void tag_fixes_misty_rain(void);
void tag_fixes_split_screen_hud();

void main_tag_fixes(void)
{
	tag_fixes_masterchief();
	tag_fixes_grunt();
	tag_fixes_brute();
	tag_fixes_smg();
	tag_fixes_environment();
	tag_fixes_misty_rain();
	tag_fixes_split_screen_hud();
	return;
}


//Fix incorrect values on Masterchief shaders
void tag_fixes_masterchief(void)
{
	//Fix the Masterchief FP arms shader
	datum fp_shader_datum = tag_loaded(_tag_group_shader, "objects\\characters\\masterchief\\fp\\shaders\\fp_arms");
	if (fp_shader_datum != NONE)
	{
		s_shader_definition* fp_shader = (s_shader_definition*)tag_get_fast(fp_shader_datum);
		fp_shader->lightmap_specular_brightness = 1.0f;
	}

	//Fix the visor
	datum tex_bump_env_datum = tag_loaded(_tag_group_shader_template, "shaders\\shader_templates\\opaque\\tex_bump_env");
	datum visor_shader_datum = tag_loaded(_tag_group_shader, "objects\\characters\\masterchief\\shaders\\masterchief_visor");
	if (visor_shader_datum != NONE)
	{
		s_shader_definition* visor_shader = (s_shader_definition*)tag_get_fast(visor_shader_datum);
		visor_shader->postprocess_definition[0]->shader_template_index = tex_bump_env_datum;
	}
	return;
}

//Fix incorrect values on Grunt shaders
void tag_fixes_grunt(void)
{
	datum grunt_arm_shader_datum = tag_loaded(_tag_group_shader, "objects\\characters\\grunt\\shaders\\grunt_arms");
	if (grunt_arm_shader_datum != NONE)
	{
		s_shader_definition* grunt_arm_shader = (s_shader_definition*)tag_get_fast(grunt_arm_shader_datum);
		grunt_arm_shader->lightmap_specular_brightness = 1.0f;
	}

	datum grunt_backpack_shader_datum = tag_loaded(_tag_group_shader, "objects\\characters\\grunt\\shaders\\grunt_backpack");
	if (grunt_backpack_shader_datum != NONE)
	{
		s_shader_definition* grunt_backpack_shader = (s_shader_definition*)tag_get_fast(grunt_backpack_shader_datum);
		grunt_backpack_shader->lightmap_specular_brightness = 1.0f;
	}

	datum grunt_torso_shader_datum = tag_loaded(_tag_group_shader, "objects\\characters\\grunt\\shaders\\grunt_torso");
	if (grunt_torso_shader_datum != NONE)
	{
		s_shader_definition* grunt_torso_shader = (s_shader_definition*)tag_get_fast(grunt_torso_shader_datum);
		grunt_torso_shader->lightmap_specular_brightness = 1.0f;
	}
	return;
}

// Fix brute shaders
void tag_fixes_brute(void)
{
	datum brute_shader_index = tag_loaded(_tag_group_shader, "objects\\characters\\brute\\shaders\\brute");
	datum brute_head_shader_index = tag_loaded(_tag_group_shader, "objects\\characters\\brute\\shaders\\brute_head");
	if (brute_shader_index != NONE && brute_head_shader_index != NONE)
	{
		s_shader_definition* shader = (s_shader_definition*)tag_get_fast(brute_shader_index);
		shader->postprocess_definition[0]->pixel_constants[0]->color = D3DCOLOR_RGBA(87, 79, 69, 0);
		shader->postprocess_definition[0]->pixel_constants[1]->color = D3DCOLOR_RGBA(180, 179, 189, 0);

		shader = (s_shader_definition*)tag_get_fast(brute_head_shader_index);
		shader->postprocess_definition[0]->pixel_constants[0]->color = D3DCOLOR_RGBA(255, 255, 255, 0);
		shader->postprocess_definition[0]->pixel_constants[1]->color = D3DCOLOR_RGBA(180, 179, 189, 0);
	}
	return;
}

// Fix smg shaders
void tag_fixes_smg(void)
{
	datum smg_painted_metal_index = tag_loaded(_tag_group_shader, "objects\\weapons\\rifle\\smg\\shaders\\smg_painted_metal");
	if (smg_painted_metal_index != NONE)
	{
		s_shader_definition* smg_painted_metal = (s_shader_definition*)tag_get_fast(smg_painted_metal_index);

		// Original values (Changed for some reason in h2v)
		smg_painted_metal->lightmap_type = _shader_lightmap_type_dull_specular;
		smg_painted_metal->lightmap_specular_brightness = 2.f;

		// Original template (Changed for some reason in h2v)
		datum tex_bump_active_camo_index = tag_loaded(_tag_group_shader_template, "shaders\\shader_templates\\opaque\\tex_bump_active_camo");
		if (tex_bump_active_camo_index != NONE)
		{
			smg_painted_metal->postprocess_definition[0]->shader_template_index = tex_bump_active_camo_index;
		}
	}
	return;
}

// Fix tags that are related to the environment (the maps themselves)
void tag_fixes_environment(void)
{
	// Fix glass shaders
	datum glass_interrior_index = tag_loaded(_tag_group_shader, "scenarios\\shaders\\human\\military\\glass\\glass_interior");
	datum glass_smudged_index = tag_loaded(_tag_group_bitmap, "scenarios\\bitmaps\\human\\military\\glass\\glass_smudged");
	datum forerunner_interiors_index = tag_loaded(_tag_group_bitmap, "scenarios\\bitmaps\\reflection_maps\\forerunner_interiors");
	if (glass_interrior_index != NONE && glass_smudged_index != NONE && forerunner_interiors_index != NONE)
	{
		// Set bitmaps to originals (Changed for some reason in h2v)
		s_shader_definition* glass_interrior = (s_shader_definition*)tag_get_fast(glass_interrior_index);
		glass_interrior->postprocess_definition[0]->bitmaps[0]->bitmap_index = forerunner_interiors_index;
		glass_interrior->postprocess_definition[0]->bitmaps[2]->bitmap_index = glass_smudged_index;
	}

	// Fix forerunner strips shader
	datum panel_thin_strips_index = tag_loaded(_tag_group_shader, "scenarios\\shaders\\forerunner\\industrial\\metals\\panels_thin_strips");
	datum tex_bump_index = tag_loaded(_tag_group_shader_template, "shaders\\shader_templates\\opaque\\tex_bump");
	if (panel_thin_strips_index != NONE && tex_bump_index != NONE)
	{
		s_shader_definition* shader = (s_shader_definition*)tag_get_fast(panel_thin_strips_index);
		shader->postprocess_definition[0]->shader_template_index = tex_bump_index;
	}

	return;
}

void tag_fixes_misty_rain(void)
{
	const s_cache_header* cache_header = cache_files_get_header();

	if (!strcmp(cache_header->name, "05a_deltaapproach"))
	{
		tag_injection_set_active_map(L"carto_shared");
		if (tag_injection_active_map_verified())
		{
			datum misty_rain_datum = tag_injection_load(_tag_group_weather_system, "scenarios\\skies\\solo\\deltatemple\\weather\\misty_rain", true);

			if (misty_rain_datum != NONE)
			{
				tag_injection_inject();

				// Set the field in the scenario
				scenario* scenario_definition = get_global_scenario();
				structure_weather_palette_entry* palette = scenario_definition->weather_palette[0];
				palette->name.set("misty_cs");
				palette->weather_system.group.group = _tag_group_weather_system;
				palette->weather_system.index = misty_rain_datum;

				// Set the field in every single bsp in the scenario
				for(int32 i = 0; i < scenario_definition->structure_bsps.count; ++i)
				{
					structure_bsp* bsp_definition = (structure_bsp*)tag_get_fast(get_global_scenario()->structure_bsps[i]->structure_bsp.index);
					structure_weather_palette_entry* palette = bsp_definition->weather_palette[0];
					palette->name.set("misty_cs");
					palette->weather_system.group.group = _tag_group_weather_system;
					palette->weather_system.index = misty_rain_datum;
				}
			}
		}
	}
	return;
}

void tag_fixes_split_screen_hud()
{
	datum master_chief_hud_index = tag_loaded(_tag_group_new_hud_definition, "ui\\hud\\masterchief");
	datum dervish_hud_index = tag_loaded(_tag_group_new_hud_definition, "ui\\hud\\dervish");
	if(master_chief_hud_index && dervish_hud_index)
	{
		s_new_hud_definition* master_chief_hud = (s_new_hud_definition*)tag_get_fast(master_chief_hud_index);
		s_new_hud_definition* dervish_hud = (s_new_hud_definition*)tag_get_fast(dervish_hud_index);
		if(master_chief_hud && dervish_hud)
		{
			// shield meter widget
			master_chief_hud->bitmap_widgets[3]->halfscreen_offset.y = -155;
			master_chief_hud->bitmap_widgets[3]->quarterscreen_offset.y = -155;
			dervish_hud->bitmap_widgets[3]->halfscreen_offset.y = -155;
			dervish_hud->bitmap_widgets[3]->quarterscreen_offset.y = -155;

			// shield meter mask widget
			master_chief_hud->bitmap_widgets[4]->halfscreen_offset.y = -155;
			master_chief_hud->bitmap_widgets[4]->quarterscreen_offset.y = -155;
			dervish_hud->bitmap_widgets[4]->halfscreen_offset.y = -155;
			dervish_hud->bitmap_widgets[4]->quarterscreen_offset.y = -155;

			// motion tracker background widget
			master_chief_hud->bitmap_widgets[5]->halfscreen_offset.y = -130;
			master_chief_hud->bitmap_widgets[5]->quarterscreen_offset.y = -130;
			dervish_hud->bitmap_widgets[5]->halfscreen_offset.y = -130;
			dervish_hud->bitmap_widgets[5]->quarterscreen_offset.y = -130;

			// second? motion tracker background widget
			master_chief_hud->bitmap_widgets[7]->halfscreen_offset.y = -130;
			master_chief_hud->bitmap_widgets[7]->quarterscreen_offset.y = -130;
			dervish_hud->bitmap_widgets[7]->halfscreen_offset.y = -130;
			dervish_hud->bitmap_widgets[7]->quarterscreen_offset.y = -130;
		}
	}

	datum scoreboard_hud_datum = tag_loaded(_tag_group_new_hud_definition, "ui\\hud\\scoreboard");
	if(scoreboard_hud_datum)
	{
		s_new_hud_definition* scoreboard_hud = (s_new_hud_definition*)tag_get_fast(scoreboard_hud_datum);
		if(scoreboard_hud)
		{
			// player score
			scoreboard_hud->bitmap_widgets[0]->halfscreen_offset = scoreboard_hud->bitmap_widgets[0]->fullscreen_offset;

			// other player score
			scoreboard_hud->bitmap_widgets[1]->halfscreen_offset = scoreboard_hud->bitmap_widgets[1]->fullscreen_offset;

			// player emblem
			scoreboard_hud->bitmap_widgets[2]->quarterscreen_offset.x = -35;
			scoreboard_hud->bitmap_widgets[2]->quarterscreen_offset.y = -68;
			scoreboard_hud->bitmap_widgets[2]->halfscreen_offset = scoreboard_hud->bitmap_widgets[2]->fullscreen_offset;

			// player arrow
			scoreboard_hud->bitmap_widgets[3]->quarterscreen_offset = scoreboard_hud->bitmap_widgets[3]->fullscreen_offset;
			scoreboard_hud->bitmap_widgets[3]->halfscreen_offset = scoreboard_hud->bitmap_widgets[3]->fullscreen_offset;

			// player emblem teams
			scoreboard_hud->bitmap_widgets[4]->quarterscreen_offset.x = -35;
			scoreboard_hud->bitmap_widgets[4]->quarterscreen_offset.y = -32;
			scoreboard_hud->bitmap_widgets[4]->halfscreen_offset = scoreboard_hud->bitmap_widgets[4]->fullscreen_offset;

			// other player emblem
			scoreboard_hud->bitmap_widgets[5]->quarterscreen_offset.x = -35;
			scoreboard_hud->bitmap_widgets[5]->quarterscreen_offset.y = -68;
			scoreboard_hud->bitmap_widgets[5]->halfscreen_offset = scoreboard_hud->bitmap_widgets[5]->fullscreen_offset;

			// other player emblem teams
			scoreboard_hud->bitmap_widgets[6]->quarterscreen_offset.x = -35;
			scoreboard_hud->bitmap_widgets[6]->quarterscreen_offset.y = -68;
			scoreboard_hud->bitmap_widgets[6]->halfscreen_offset = scoreboard_hud->bitmap_widgets[6]->fullscreen_offset;

			//bomb_defusal
			scoreboard_hud->bitmap_widgets[7]->quarterscreen_offset.y = 50;
			scoreboard_hud->bitmap_widgets[7]->halfscreen_offset.y = 50;


		}
	}

	tag_iterator hud_iterator;
	tag_iterator_new(&hud_iterator, _tag_group_new_hud_definition);

	while(tag_iterator_next(&hud_iterator) != NONE)
	{
		const char* tag_name = tag_get_name(hud_iterator.current_tag_index);
		s_new_hud_definition* hud = (s_new_hud_definition*)tag_get_fast(hud_iterator.current_tag_index);

		if (hud->bitmap_widgets.count && hud->bitmap_widgets.data != NONE)
		{
			for (uint32 index = 0; index < hud->bitmap_widgets.count; index++)
			{
				s_hud_bitmap_widget_definition* bitmap_widget = hud->bitmap_widgets[index];
				// these elements on the top of the screen can just copy their fullscreen_offsets

				// weapon_background_right, weapon_background_single, backpack, weapon_background_out, heat_meter_right, heat_background_right
				// heat_background_left, heat_meter_left
				if (bitmap_widget->name == (string_id)0x17001E0D || 
					bitmap_widget->name == (string_id)0x18001F6C || 
					bitmap_widget->name == (string_id)0x8001E11  ||
					bitmap_widget->name == (string_id)0x15001F74 ||
					bitmap_widget->name == (string_id)0x10001F29 ||
					bitmap_widget->name == (string_id)0x15001F27 ||
					bitmap_widget->name == (string_id)0x14001F28 ||
					bitmap_widget->name == (string_id)0xF001F2A)
				{
					bitmap_widget->halfscreen_offset = bitmap_widget->fullscreen_offset;
					bitmap_widget->quarterscreen_offset = bitmap_widget->fullscreen_offset;
				}
			}
		}
	}
}
