#include "stdafx.h"
#include "tag_fixes.h"

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

void main_tag_fixes(void)
{
	tag_fixes_masterchief();
	tag_fixes_grunt();
	tag_fixes_brute();
	tag_fixes_smg();
	tag_fixes_environment();
	tag_fixes_misty_rain();
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
