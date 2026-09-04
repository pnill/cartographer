#include "stdafx.h"
#include "tag_fixes.h"

#include "cache/cache_files.h"
#include "game/game_globals.h"
#include "interface/new_hud_definitions.h"
#include "models/models.h"
#include "models/model_definitions.h"
#include "models/render_model_definitions.h"
#include "scenario/scenario.h"
#include "scenario/scenario_definitions.h"
#include "shaders/shader_definitions.h"
#include "shaders/shader_postprocess_definitions.h"
#include "sound/sound_classes.h"
#include "sound/sound_definitions.h"
#include "structures/structure_bsp_definitions.h"
#include "tag_files/tag_loader/tag_injection.h"
#include "units/biped_definitions.h"

/* globals */

// Explanation:
// Hud tags are packaged using the data saver option which will cause h2tool to reuse tag blocks
// this will cause the offset fix to apply multiple times to individual blocks here we are just keeping
// a temporary map of all the blocks that have already been adjusted.
static int32 g_already_adjusted_blocks[k_maximum_hud_bitmap_widgets_per_tag + k_maximum_hud_text_widgets_per_tag + k_maximum_hud_screen_effect_widgets_per_tag];
static uint32 g_adjusted_blocks_count = 0;

/* prototypes */

//Fix incorrect values on Masterchief shaders
static void tag_fixes_masterchief(void);

//Fix incorrect values on Grunt shaders
static void tag_fixes_grunt(void);

// Fix brute shaders
static void tag_fixes_brute(void);

// Fix smg shaders
static void tag_fixes_smg(void);

// Fix tags that are related to the environment (the maps themselves)
static void tag_fixes_environment(void);

// Add back misty rain weather effect on delta halo
static void tag_fixes_misty_rain(void);

static void tag_fixes_elite_mp(void);

// Explanation:
// Hud offsets were doubled as a hack by hired gun when they upscaled the hud for H2V by 2
// However, these numbers were only doubled for fullscreen offsets and not the offsets used when in splitscreen
// We double these to fix the hud in splitscreen
static void tag_fixes_split_screen_hud(void);

// Change sound_classes data to equivalents in original halo 2
static void sound_classes_fix_values(void);

// Fix the gravity throne so it uses it's collision model
static void gravity_throne_collision_fix(void);

// Removes a bitmap from every shader that uses a specific template on nvidia cards that breaks
static void shader_template_fix_nvidia(const char* template_name, const char* bitmap_name, int32 bitmap_index);

// Goes through all the shader templates that are broken when nvidia applies it fixes in the driver
static void shader_templates_fix_nvidia(void);

/* public code */

void main_tag_fixes(void)
{
	tag_fixes_masterchief();
	tag_fixes_grunt();
	tag_fixes_brute();
	tag_fixes_smg();
	tag_fixes_environment();
	tag_fixes_misty_rain();
	tag_fixes_split_screen_hud();
	sound_classes_fix_values();
	gravity_throne_collision_fix();
	shader_templates_fix_nvidia();

	// disabled till z-fighting is fixed.
	//tag_fixes_elite_mp();
	return;
}

/* private code */

static void tag_fixes_masterchief(void)
{
	//Fix the Masterchief FP arms shader
	datum fp_shader_datum = tag_loaded(_tag_group_shader, "objects\\characters\\masterchief\\fp\\shaders\\fp_arms");
	if (fp_shader_datum != NONE)
	{
		s_shader_definition* fp_shader = shader_definition_get(fp_shader_datum);
		fp_shader->lightmap_specular_brightness = 1.f;
	}

	//Fix the visor
	datum tex_bump_env_datum = tag_loaded(_tag_group_shader_template, "shaders\\shader_templates\\opaque\\tex_bump_env");
	datum visor_shader_datum = tag_loaded(_tag_group_shader, "objects\\characters\\masterchief\\shaders\\masterchief_visor");
	if (visor_shader_datum != NONE)
	{
		s_shader_definition* visor_shader = shader_definition_get(visor_shader_datum);
		s_shader_postprocess_definition_new* postprocess = TAG_BLOCK_GET_ELEMENT(&visor_shader->postprocess_definition, 0, s_shader_postprocess_definition_new);
		postprocess->shader_template_index = tex_bump_env_datum;
	}
	return;
}

static void tag_fixes_grunt(void)
{
	datum grunt_arm_shader_datum = tag_loaded(_tag_group_shader, "objects\\characters\\grunt\\shaders\\grunt_arms");
	if (grunt_arm_shader_datum != NONE)
	{
		s_shader_definition* grunt_arm_shader = shader_definition_get(grunt_arm_shader_datum);
		grunt_arm_shader->lightmap_specular_brightness = 1.f;
	}

	datum grunt_backpack_shader_datum = tag_loaded(_tag_group_shader, "objects\\characters\\grunt\\shaders\\grunt_backpack");
	if (grunt_backpack_shader_datum != NONE)
	{
		s_shader_definition* grunt_backpack_shader = shader_definition_get(grunt_backpack_shader_datum);
		grunt_backpack_shader->lightmap_specular_brightness = 1.f;
	}

	datum grunt_torso_shader_datum = tag_loaded(_tag_group_shader, "objects\\characters\\grunt\\shaders\\grunt_torso");
	if (grunt_torso_shader_datum != NONE)
	{
		s_shader_definition* grunt_torso_shader = shader_definition_get(grunt_torso_shader_datum);
		grunt_torso_shader->lightmap_specular_brightness = 1.f;
	}
	return;
}

static void tag_fixes_brute(void)
{
	datum brute_shader_index = tag_loaded(_tag_group_shader, "objects\\characters\\brute\\shaders\\brute");
	datum brute_head_shader_index = tag_loaded(_tag_group_shader, "objects\\characters\\brute\\shaders\\brute_head");
	if (brute_shader_index != NONE && brute_head_shader_index != NONE)
	{
		s_shader_definition* shader = shader_definition_get(brute_shader_index);
		s_shader_postprocess_definition_new* postprocess = TAG_BLOCK_GET_ELEMENT(&shader->postprocess_definition, 0, s_shader_postprocess_definition_new);

		*postprocess->pixel_constants[0] = PIXEL32_RGBA(87, 79, 69, 0);
		*postprocess->pixel_constants[1] = PIXEL32_RGBA(180, 179, 189, 0);

		shader = shader_definition_get(brute_head_shader_index);
		*postprocess->pixel_constants[0] = PIXEL32_RGBA(255, 255, 255, 0);
		*postprocess->pixel_constants[1] = PIXEL32_RGBA(180, 179, 189, 0);
	}
	return;
}

static void tag_fixes_smg(void)
{
	datum smg_painted_metal_index = tag_loaded(_tag_group_shader, "objects\\weapons\\rifle\\smg\\shaders\\smg_painted_metal");
	if (smg_painted_metal_index != NONE)
	{
		s_shader_definition* smg_painted_metal = shader_definition_get(smg_painted_metal_index);

		// Original values (Changed for some reason in h2v)
		smg_painted_metal->lightmap_type = _shader_lightmap_type_dull_specular;
		smg_painted_metal->lightmap_specular_brightness = 2.f;

		// Original template (Changed for some reason in h2v)
		datum tex_bump_active_camo_index = tag_loaded(_tag_group_shader_template, "shaders\\shader_templates\\opaque\\tex_bump_active_camo");
		if (tex_bump_active_camo_index != NONE)
		{
			s_shader_postprocess_definition_new* postprocess = TAG_BLOCK_GET_ELEMENT(&smg_painted_metal->postprocess_definition, 0, s_shader_postprocess_definition_new);
			postprocess->shader_template_index = tex_bump_active_camo_index;
		}
	}
	return;
}

static void tag_fixes_environment(void)
{
	// Fix glass shaders
	datum glass_interrior_index = tag_loaded(_tag_group_shader, "scenarios\\shaders\\human\\military\\glass\\glass_interior");
	datum glass_smudged_index = tag_loaded(_tag_group_bitmap, "scenarios\\bitmaps\\human\\military\\glass\\glass_smudged");
	datum forerunner_interiors_index = tag_loaded(_tag_group_bitmap, "scenarios\\bitmaps\\reflection_maps\\forerunner_interiors");
	if (glass_interrior_index != NONE && glass_smudged_index != NONE && forerunner_interiors_index != NONE)
	{
		// Set bitmaps to originals (Changed for some reason in h2v)
		s_shader_definition* glass_interrior = shader_definition_get(glass_interrior_index);

		s_shader_postprocess_definition_new* postprocess = TAG_BLOCK_GET_ELEMENT(&glass_interrior->postprocess_definition, 0, s_shader_postprocess_definition_new);
		
		s_shader_postprocess_bitmap_new* bitmap_0 = TAG_BLOCK_GET_ELEMENT(&postprocess->bitmaps, 0, s_shader_postprocess_bitmap_new);
		bitmap_0->bitmap_index = forerunner_interiors_index;

		s_shader_postprocess_bitmap_new* bitmap_2 = TAG_BLOCK_GET_ELEMENT(&postprocess->bitmaps, 2, s_shader_postprocess_bitmap_new);
		bitmap_2->bitmap_index = glass_smudged_index;
	}

	// Fix forerunner strips shader
	datum panel_thin_strips_index = tag_loaded(_tag_group_shader, "scenarios\\shaders\\forerunner\\industrial\\metals\\panels_thin_strips");
	datum tex_bump_index = tag_loaded(_tag_group_shader_template, "shaders\\shader_templates\\opaque\\tex_bump");
	if (panel_thin_strips_index != NONE && tex_bump_index != NONE)
	{
		const s_shader_definition* shader = shader_definition_get(panel_thin_strips_index);
		s_shader_postprocess_definition_new* postprocess = TAG_BLOCK_GET_ELEMENT(&shader->postprocess_definition, 0, s_shader_postprocess_definition_new);
		postprocess->shader_template_index = tex_bump_index;
	}

	return;
}

static void tag_fixes_misty_rain(void)
{
	const cache_file_header* cache_header = cache_files_get_header();

	if (!strcmp(cache_header->name, "05a_deltaapproach"))
	{
		tag_injection_set_active_map(k_carto_shared_map);
		if (tag_injection_active_map_verified())
		{
			datum misty_rain_datum = tag_injection_load(_tag_group_weather_system, "scenarios\\skies\\solo\\deltatemple\\weather\\misty_rain", true);

			if (misty_rain_datum != NONE)
			{
				tag_injection_inject();

				// Set the field in the scenario
				scenario* scenario_definition = global_scenario_get();
				structure_weather_palette_entry* palette = TAG_BLOCK_GET_ELEMENT(&scenario_definition->weather_palette, 0, structure_weather_palette_entry);
				const char name[] = "misty_cs";
				csstrncpy(palette->name, name, NUMBEROF(name));
				palette->weather_system.group = _tag_group_weather_system;
				palette->weather_system.index = misty_rain_datum;

				// Set the field in every single bsp in the scenario
				for (int32 i = 0; i < scenario_definition->structure_bsp_references.count; ++i)
				{
					scenario_structure_bsp_reference const* reference = TAG_BLOCK_GET_ELEMENT(&scenario_definition->structure_bsp_references, i, scenario_structure_bsp_reference);
					structure_bsp const* bsp_definition = structure_bsp_definition_get(reference->structure_bsp.index);
					structure_weather_palette_entry* bsp_palette = TAG_BLOCK_GET_ELEMENT(&bsp_definition->weather_palette, 0, structure_weather_palette_entry);
					
					csstrncpy(bsp_palette->name, name, NUMBEROF(name));
					bsp_palette->weather_system.group = _tag_group_weather_system;
					bsp_palette->weather_system.index = misty_rain_datum;
				}
			}
		}
	}
	return;
}

static void tag_fixes_elite_mp(void)
{
	const s_game_globals* game_globals = scenario_get_game_globals();
	s_game_globals_player_representation const* player_rep = TAG_BLOCK_GET_ELEMENT(&game_globals->player_representation, _character_type_elite, s_game_globals_player_representation);

	if (player_rep->third_person_unit.index != NONE)
	{
		biped_definition const* elite_biped = biped_definition_get(player_rep->third_person_unit.index);
		
		if (elite_biped->object.model.index != NONE)
		{
			s_model_definition* elite_model = model_definition_get(elite_biped->object.model.index);

			const static char* elite_shader_map[6]
			{
				"objects\\characters\\elite\\shaders\\head_mp",
				"objects\\characters\\elite\\shaders\\helmet_mp",
				"objects\\characters\\elite\\shaders\\arms_mp",
				"objects\\characters\\elite\\shaders\\torso_mp",
				"objects\\characters\\elite\\shaders\\legs_mp",
				"objects\\characters\\elite\\shaders\\inset_lights_mp"
			};

			const static char* sp_elite_shader_map[6]
			{
				"objects\\characters\\elite\\shaders\\head",
				"objects\\characters\\elite\\shaders\\helmet",
				"objects\\characters\\elite\\shaders\\arms",
				"objects\\characters\\elite\\shaders\\torso",
				"objects\\characters\\elite\\shaders\\legs",
				"objects\\characters\\elite\\shaders\\inset_lights"
			};

			render_model_definition* elite_render_model = render_model_definition_get(elite_model->render_model.index);

			tag_injection_set_active_map(L"shared");
			if (tag_injection_active_map_verified())
			{
				for (int32 index = 0; index < elite_render_model->materials.count; ++index)
				{
					geometry_material* material = elite_render_model->materials[index];

					for (int32 k = 0; k < 6; ++k)
					{
						if (!_stricmp(tag_get_name(material->shader.index), elite_shader_map[k]))
						{
							datum new_shader = tag_loaded(_tag_group_shader, sp_elite_shader_map[k]);

							if (new_shader != NONE)
							{
								material->shader.index = new_shader;
								break;
							}
							else
							{
								new_shader = tag_injection_load(_tag_group_shader, sp_elite_shader_map[k], true);

								if(new_shader != NONE)
								{
									material->shader.index = new_shader;
									break;
								}
							}
						}
					}
				}

				tag_injection_inject();
			}
		}
	}

	return;
}

static bool tag_fixes_split_screen_block_adjusted(int32 block_offset)
{
	for(uint32 index = 0; index < g_adjusted_blocks_count; index++)
		if (g_already_adjusted_blocks[index] == block_offset)
			return true;

	return false;
}

static void tag_fixes_split_screen_hud(void)
{
	g_adjusted_blocks_count = 0;

	tag_iterator hud_iterator;
	tag_iterator_new(&hud_iterator, _tag_group_new_hud_definition);

	while (tag_iterator_next(&hud_iterator) != NONE)
	{
		s_new_hud_definition const* hud = new_hud_definition_get(hud_iterator.current_tag_index);

		if (!tag_fixes_split_screen_block_adjusted(hud->bitmap_widgets.data))
		{
			for (int32 i = 0; i < hud->bitmap_widgets.count; ++i)
			{
				s_hud_bitmap_widget_definition* bitmap_widget = TAG_BLOCK_GET_ELEMENT(&hud->bitmap_widgets, i, s_hud_bitmap_widget_definition);

				// this is commented out because ammo clip bitmaps were broken in the port, the bitmaps need to be changed to properly fix them
				// if the bitmaps aren't redone uncomment out the if statement and they'll be generally fixed.
				//if (bitmap_widget->widget_inputs.input_1 != hud_input_type_weapon_clip_ammo)
				//{
					bitmap_widget->screen_offsets[_screen_split_type_half].x *= 2;
					bitmap_widget->screen_offsets[_screen_split_type_half].y *= 2;
					bitmap_widget->screen_offsets[_screen_split_type_quarter].x *= 2;
					bitmap_widget->screen_offsets[_screen_split_type_quarter].y *= 2;
				//}
				//else
				//{
				//	bitmap_widget->halfscreen_offset = bitmap_widget->fullscreen_offset;
				//	bitmap_widget->quarterscreen_offset = bitmap_widget->fullscreen_offset;
				//}
			}

			g_already_adjusted_blocks[g_adjusted_blocks_count++] = hud->bitmap_widgets.data;
		}

		if (!tag_fixes_split_screen_block_adjusted(hud->text_widgets.data))
		{
			for (int32 i = 0; i < hud->text_widgets.count; ++i)
			{
				s_hud_text_widget_definition* text_widget = TAG_BLOCK_GET_ELEMENT(&hud->text_widgets, i, s_hud_text_widget_definition);

				text_widget->screen_offsets[_screen_split_type_half].x *= 2;
				text_widget->screen_offsets[_screen_split_type_half].y *= 2;
				text_widget->screen_offsets[_screen_split_type_quarter].x *= 2;
				text_widget->screen_offsets[_screen_split_type_quarter].y *= 2;
			}

			g_already_adjusted_blocks[g_adjusted_blocks_count++] = hud->text_widgets.data;
		}

		if (!tag_fixes_split_screen_block_adjusted(hud->screen_effect_widgets.data))
		{
			for (int32 i = 0; i < hud->screen_effect_widgets.count; ++i)
			{
				s_hud_screen_effect_widget_definition* effect_widget = TAG_BLOCK_GET_ELEMENT(&hud->screen_effect_widgets, i, s_hud_screen_effect_widget_definition);

				effect_widget->screen_offsets[_screen_split_type_half].x *= 2;
				effect_widget->screen_offsets[_screen_split_type_half].y *= 2;
				effect_widget->screen_offsets[_screen_split_type_quarter].x *= 2;
				effect_widget->screen_offsets[_screen_split_type_quarter].y *= 2;
			}

			g_already_adjusted_blocks[g_adjusted_blocks_count++] = hud->screen_effect_widgets.data;
		}
	}

	return;
}

static void sound_classes_fix_values(void)
{
	s_game_globals const* game_globals = scenario_get_game_globals();
	s_sound_globals_definition const* sound_globals = TAG_BLOCK_GET_ELEMENT(&game_globals->sound_globals, 0, s_sound_globals_definition);

	if (game_globals->sound_globals.count > 0 && sound_globals->sound_classes.index != NONE)
	{
		s_sound_classes_definition* sound_classes = sound_classes_definition_get(sound_globals->sound_classes.index);

		sound_class_get(&sound_classes->sound_classes, _sound_class_projectile_impact)->gain_bounds = { -0.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_projectile_detonation)->gain_bounds = { -0.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_projectile_flyby)->gain_bounds = { -0.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_projectile_unused1)->gain_bounds = { -0.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_weapon_fire)->gain_bounds = { -0.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_weapon_ready)->gain_bounds = { -0.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_weapon_reload)->gain_bounds = { -0.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_weapon_empty)->gain_bounds = { -0.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_weapon_charge)->gain_bounds = { -0.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_weapon_overheat)->gain_bounds = { -0.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_weapon_idle)->gain_bounds = { -0.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_weapon_melee)->gain_bounds = { -0.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_weapon_animation)->gain_bounds = { -0.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_object_impacts)->gain_bounds = { -64.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_particle_impacts)->gain_bounds = { -12.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_footstep)->gain_bounds = { -32.f, -9.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_unit_animation)->gain_bounds = { -0.f, -2.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_vehicle_impact)->gain_bounds = { -0.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_vehicle_engine)->gain_bounds = { -0.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_vehicle_animation)->gain_bounds = { -0.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_device_machinery)->gain_bounds = { -0.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_device_stationary)->gain_bounds = { -0.f, -4.f };
		sound_class_get(&sound_classes->sound_classes, _sound_class_ambient_machinery)->gain_bounds = { -0.f, -4.f };
	}

	return;
}

static void gravity_throne_collision_fix(void)
{
	const char* tag_name = "objects\\vehicles\\gravity_throne\\gravity_throne";
	const datum model_index =  tag_loaded(_tag_group_model, tag_name);
	if (model_index != NONE)
	{
		const datum collision_index = tag_loaded(_tag_group_collision_model, tag_name);
		if (collision_index != NONE)
		{
			s_model_definition* model_definition = model_definition_get(model_index);

			ASSERT(model_definition);

			model_definition->collision_model.index = collision_index;
			model_definition->collision_model.group = { _tag_group_collision_model };
		}
	}
	return;
}


static void shader_template_fix_nvidia(const char* template_name, const char* bitmap_name, int32 bitmap_index)
{
	const datum broken_bitmap_index = tag_loaded(_tag_group_bitmap, bitmap_name);
	const datum broken_template_index = tag_loaded(_tag_group_shader_template, template_name);
	if (broken_bitmap_index != NONE && broken_template_index != NONE)
	{
		tag_iterator iterator;
		tag_iterator_new(&iterator, _tag_group_shader);

		while (tag_iterator_next(&iterator) != NONE)
		{
			s_shader_definition* shader = shader_definition_get(iterator.current_tag_index);
			if (shader->shader_template.index == broken_template_index)
			{
				if (shader->postprocess_definition.count > 0)
				{
					s_shader_postprocess_definition_new* postprocess_data = (s_shader_postprocess_definition_new*)TAG_BLOCK_GET_ELEMENT(
						&shader->postprocess_definition,
						0,
						s_shader_postprocess_definition_new
					);

					if (postprocess_data->bitmaps.count >= bitmap_index + 1)
					{
						s_shader_postprocess_bitmap_new* bitmap = TAG_BLOCK_GET_ELEMENT(
							&postprocess_data->bitmaps,
							bitmap_index,
							s_shader_postprocess_bitmap_new
						);

						if (bitmap->bitmap_group == broken_bitmap_index)
						{
							bitmap->bitmap_group = NONE;
						}
					}
				}
			}
		}
	}
	return;
}

static void shader_templates_fix_nvidia(void)
{
	shader_template_fix_nvidia(
		"shaders\\shader_templates\\opaque\\tex_bump_alpha_test_single_pass",
		"shaders\\default_bitmaps\\bitmaps\\alpha_white",
		4
	);

	shader_template_fix_nvidia(
		"shaders\\shader_templates\\opaque\\tex_bump_alpha_test",
		"shaders\\default_bitmaps\\bitmaps\\gray_50_percent",
		1
	);
	return;
}
