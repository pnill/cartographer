#include "stdafx.h"
#include "TagFixes.h"

#include "Blam/Cache/TagGroups/light_definition.h"
#include "Blam/Cache/TagGroups/sound_classes_definition.hpp"

#include "shaders/shader_definitions.h"
#include "units/biped_definitions.h"

#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Tags/TagInterface.h"

// ### TODO Cleanup
extern bool g_xbox_tickrate_enabled;

namespace TagFixes
{
	namespace
	{
		void fix_shader_template_nvidia(const char *template_name, const char* bitmap_name, int32 bitmap_idx)
		{
			datum bitmap_to_fix = tags::find_tag(_tag_group_bitmap, bitmap_name);
			datum borked_template = tags::find_tag(_tag_group_shader_template, template_name);
			LOG_DEBUG_FUNC("bitmap {0}, borked_template {1}", bitmap_to_fix, borked_template);
			if (DATUM_IS_NONE(bitmap_to_fix) || DATUM_IS_NONE(borked_template))
				return;
			LOG_DEBUG_FUNC("Fixing: template {}, bitmap {}", template_name, bitmap_name);
			auto shaders = tags::find_tags(_tag_group_shader);
			for(auto &shader_item : shaders)
			{
				auto shader = tags::get_tag<_tag_group_shader, byte>(shader_item.first);
				if(shader != nullptr)
				{
					tag_reference* shader_template = reinterpret_cast<tag_reference*>(shader);
					if(shader_template->index == borked_template)
					{
						tag_block<int> *shader_post = reinterpret_cast<tag_block<int>*>(shader + 0x20);
						if(shader_post->count > 0)
						{
							auto shader_post_data = tags::get_tag_data() + shader_post->data;
							auto shader_post_bitmap = reinterpret_cast<tag_block<int>*>(shader_post_data + 0x4);
							if(shader_post_bitmap->count >= bitmap_idx + 1)
							{
								auto bitmap_data = tags::get_tag_data() + (shader_post_bitmap->data + (bitmap_idx * 0xC));
								datum* bitmap = reinterpret_cast<datum*>(bitmap_data);
								if(*bitmap == bitmap_to_fix)
									*bitmap = NONE;
							}
						}
					}
				}
			}
		}
		void fix_shaders_nvidia()
		{
			fix_shader_template_nvidia(
				"shaders\\shader_templates\\opaque\\tex_bump_alpha_test_single_pass",
				"shaders\\default_bitmaps\\bitmaps\\alpha_white",
				4
			);

			fix_shader_template_nvidia(
				"shaders\\shader_templates\\opaque\\tex_bump_alpha_test",
				"shaders\\default_bitmaps\\bitmaps\\gray_50_percent",
				1
			);
		}
		void fix_dynamic_lights()
		{
			// TODO FIXME: this breaks other shadows
			return;

			auto cinematic_shadow_datum = tags::find_tag(_tag_group_vertex_shader, "rasterizer\\vertex_shaders_dx9\\shadow_buffer_generation_cinematic");
			auto shadow_datum = tags::find_tag(_tag_group_shader_pass, "shaders\\shader_passes\\shadow\\shadow_generate");
			byte* shadow_tag = tags::get_tag<_tag_group_shader_pass, BYTE>(shadow_datum);
			if(shadow_tag != nullptr)
			{
				tag_block<int> *shadow_pp = reinterpret_cast<tag_block<int>*>(shadow_tag + 0x1C);
				if(shadow_pp->count > 0 && shadow_pp->data != NONE)
				{
					auto shadow_pp_data = tags::get_tag_data() + shadow_pp->data;
					tag_block<int>*shadow_impl_block = reinterpret_cast<tag_block<int>*>(shadow_pp_data);
					if(shadow_pp->count > 0 && shadow_impl_block->data != NONE)
					{
						auto shadow_impl = tags::get_tag_data() + shadow_impl_block->data;
						tag_reference* impl_1 = reinterpret_cast<tag_reference*>(shadow_impl + (0x14A) + 0xFC);
						tag_reference* impl_2 = reinterpret_cast<tag_reference*>(shadow_impl + (0x14A*2) + 0xFC);

						impl_1->index = cinematic_shadow_datum;
						//TODO: Re-enable this once the vertex shaders for shadows are fixed.
						//impl_2->TagIndex = cinematic_shadow_datum.data;
					}
				}
			}
		}

		void ShaderSpecularFix()
		{
			//Fix the Master Chief FP Arms Shader
			datum fp_shader_datum = tags::find_tag(_tag_group_shader, "objects\\characters\\masterchief\\fp\\shaders\\fp_arms");
			if (fp_shader_datum != NONE)
			{
				s_shader_definition* fp_shader = (s_shader_definition*)tag_get_fast(fp_shader_datum);
				fp_shader->lightmap_specular_brightness = 1.0f;
			}

			//Fix the Visor
			datum tex_bump_env_datum = tags::find_tag(_tag_group_shader_template, "shaders\\shader_templates\\opaque\\tex_bump_env");
			datum visor_shader_datum = tags::find_tag(_tag_group_shader, "objects\\characters\\masterchief\\shaders\\masterchief_visor");
			if (visor_shader_datum != NONE)
			{
				s_shader_definition* visor_shader = (s_shader_definition*)tag_get_fast(visor_shader_datum);
				visor_shader->postprocess_definition[0]->shader_template_index = tex_bump_env_datum;
			}

			//Fix the Grunt Shaders
			datum grunt_arm_shader_datum = tags::find_tag(_tag_group_shader, "objects\\characters\\grunt\\shaders\\grunt_arms");
			if (grunt_arm_shader_datum != NONE)
			{
				s_shader_definition* grunt_arm_shader = (s_shader_definition*)tag_get_fast(grunt_arm_shader_datum);
				grunt_arm_shader->lightmap_specular_brightness = 1.0f;
			}

			datum grunt_backpack_shader_datum = tags::find_tag(_tag_group_shader, "objects\\characters\\grunt\\shaders\\grunt_backpack");
			if (grunt_backpack_shader_datum != NONE)
			{
				s_shader_definition* grunt_backpack_shader = (s_shader_definition*)tag_get_fast(grunt_backpack_shader_datum);
				grunt_backpack_shader->lightmap_specular_brightness = 1.0f;
			}

			datum grunt_torso_shader_datum = tags::find_tag(_tag_group_shader, "objects\\characters\\grunt\\shaders\\grunt_torso");
			if (grunt_torso_shader_datum != NONE)
			{
				s_shader_definition* grunt_torso_shader = (s_shader_definition*)tag_get_fast(grunt_torso_shader_datum);
				grunt_torso_shader->lightmap_specular_brightness = 1.0f;
			}

			// Fix smg shaders
			datum smg_painted_metal_index = tags::find_tag(_tag_group_shader, "objects\\weapons\\rifle\\smg\\shaders\\smg_painted_metal");
			if (smg_painted_metal_index != NONE)
			{
				s_shader_definition* smg_painted_metal = (s_shader_definition*)tag_get_fast(smg_painted_metal_index);

				// Original values (Changed for some reason in h2v)
				smg_painted_metal->lightmap_type = _lightmap_type_dull_specular;
				smg_painted_metal->lightmap_specular_brightness = 2.f;

				// Original template (Changed for some reason in h2v)
				datum tex_bump_active_camo_index = tags::find_tag(_tag_group_shader_template, "shaders\\shader_templates\\opaque\\tex_bump_active_camo");
				if (tex_bump_active_camo_index != NONE)
				{
					smg_painted_metal->postprocess_definition[0]->shader_template_index = tex_bump_active_camo_index;
				}
			}

			// Fix glass shaders
			datum glass_interrior_index =		tags::find_tag(_tag_group_shader, "scenarios\\shaders\\human\\military\\glass\\glass_interior");
			datum glass_smudged_index =			tags::find_tag(_tag_group_bitmap, "scenarios\\bitmaps\\human\\military\\glass\\glass_smudged");
			datum forerunner_interiors_index =	tags::find_tag(_tag_group_bitmap, "scenarios\\bitmaps\\reflection_maps\\forerunner_interiors");
			if (glass_interrior_index != NONE && glass_smudged_index != NONE && forerunner_interiors_index != NONE)
			{
				// Set bitmaps to originals (Changed for some reason in h2v)
				s_shader_definition* glass_interrior = (s_shader_definition*)tag_get_fast(glass_interrior_index);
				glass_interrior->postprocess_definition[0]->bitmaps[0]->bitmap_index = forerunner_interiors_index;
				glass_interrior->postprocess_definition[0]->bitmaps[2]->bitmap_index = glass_smudged_index;
			}

			// Fix brute shaders
			datum brute_shader_index =		tags::find_tag(_tag_group_shader, "objects\\characters\\brute\\shaders\\brute");
			datum brute_head_shader_index = tags::find_tag(_tag_group_shader, "objects\\characters\\brute\\shaders\\brute_head");
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

			// Fix forerunner strips shader
			datum panel_thin_strips_index = tags::find_tag(_tag_group_shader, "scenarios\\shaders\\forerunner\\industrial\\metals\\panels_thin_strips");
			datum tex_bump_index = tags::find_tag(_tag_group_shader_template, "shaders\\shader_templates\\opaque\\tex_bump");
			if (panel_thin_strips_index != NONE && tex_bump_index != NONE)
			{
				s_shader_definition* shader = (s_shader_definition*)tag_get_fast(brute_shader_index);
				shader->postprocess_definition[0]->shader_template_index = tex_bump_index;
			}
		}

		void font_table_fix()
		{
			WriteValue<int>(Memory::GetAddress(0x464940), 0);
			WriteValue<int>(Memory::GetAddress(0x46494C), 0);
			WriteValue<int>(Memory::GetAddress(0x464958), 0);
			WriteValue<int>(Memory::GetAddress(0x464964), 0);
		}

		void shader_lod_max()
		{
			auto shaders = tags::find_tags(_tag_group_shader);
			for (auto& shader_item : shaders)
			{
				auto shader = tags::get_tag<_tag_group_shader, s_shader_definition>(shader_item.first);
				shader->shader_lod_bias = _shader_lod_bias_never;
			}
		}

		//Hacky thingy for flags in the light tagblock
		//Should Prob Remove when a better solution is found
		//Apparently this is the best solution?????
		template<class T> inline T operator~ (T a) { return (T)~(int)a; }
		template<class T> inline T operator| (T a, T b) { return (T)((int)a | (int)b); }
		template<class T> inline T operator& (T a, T b) { return (T)((int)a & (int)b); }
		template<class T> inline T operator^ (T a, T b) { return (T)((int)a ^ (int)b); }
		template<class T> inline T& operator|= (T& a, T b) { return (T&)((int&)a |= (int)b); }
		template<class T> inline T& operator&= (T& a, T b) { return (T&)((int&)a &= (int)b); }
		template<class T> inline T& operator^= (T& a, T b) { return (T&)((int&)a ^= (int)b); }

		void light_framerate_killer()
		{
			auto lights = tags::find_tags(_tag_group_light);
			for (auto& light_item : lights)
			{
				auto light = tags::get_tag_fast<s_light_group_definition>(light_item.first);
				// Disabled since it caused issues where certain lights wouldnt render randomly
				// TODO figure out why it does this at some other point in time
				// light->flags |= s_light_group_definition::e_flags::light_framerate_killer;

				light->flags |= s_light_group_definition::e_flags::multiplayer_override;
			}
		}

		void fall_damage_fix()
		{
			auto bipeds = tags::find_tags(_tag_group_biped);
			for (auto& biped_item : bipeds)
			{
				auto biped = tags::get_tag<_tag_group_biped, _biped_definition>(biped_item.first);
				biped->unit.object.abs_acceleration.lower *= 2.0f;
				biped->unit.object.abs_acceleration.upper *= 2.0f;
			}
		}

		void sound_classes_fix()
		{
			// Change sound_classes data to equivalents in original halo 2
			datum sound_classes_datum = tags::find_tag(_tag_group_sound_classes, "sound\\sound_classes");
			if (sound_classes_datum != NONE)
			{
				s_sound_classes_block* sound_classes = tags::get_tag_fast<s_sound_classes_block>(sound_classes_datum);

				sound_classes->soundClasses[0]->gainBoundsDB = { -0.0f, -4.0f };
				sound_classes->soundClasses[1]->gainBoundsDB = { -0.0f, -4.0f };
				sound_classes->soundClasses[2]->gainBoundsDB = { -0.0f, -4.0f };
				sound_classes->soundClasses[3]->gainBoundsDB = { -0.0f, -4.0f };
				sound_classes->soundClasses[4]->gainBoundsDB = { -0.0f, -4.0f };
				sound_classes->soundClasses[5]->gainBoundsDB = { -0.0f, -4.0f };
				sound_classes->soundClasses[6]->gainBoundsDB = { -0.0f, -4.0f };
				sound_classes->soundClasses[7]->gainBoundsDB = { -0.0f, -4.0f };
				sound_classes->soundClasses[8]->gainBoundsDB = { -0.0f, -4.0f };
				sound_classes->soundClasses[9]->gainBoundsDB = { -0.0f, -4.0f };
				sound_classes->soundClasses[10]->gainBoundsDB = { -0.0f, -4.0f };
				sound_classes->soundClasses[11]->gainBoundsDB = { -0.0f, -4.0f };
				sound_classes->soundClasses[12]->gainBoundsDB = { -0.0f, -4.0f };
				sound_classes->soundClasses[13]->gainBoundsDB = { -64.0f, -4.0f };
				sound_classes->soundClasses[14]->gainBoundsDB = { -12.0f, -4.0f };
				sound_classes->soundClasses[18]->gainBoundsDB = { -32.0f, -9.0f };
				sound_classes->soundClasses[20]->gainBoundsDB = { -0.0f, -2.0f };
				sound_classes->soundClasses[22]->gainBoundsDB = { -0.0f, -4.0f };
				sound_classes->soundClasses[23]->gainBoundsDB = { -0.0f, -4.0f };
				sound_classes->soundClasses[24]->gainBoundsDB = { -0.0f, -4.0f };
				sound_classes->soundClasses[28]->gainBoundsDB = { -0.0f, -4.0f };
				sound_classes->soundClasses[29]->gainBoundsDB = { -0.0f, -4.0f };
				sound_classes->soundClasses[34]->gainBoundsDB = { -0.0f, -4.0f };
			}
		}
	}

	void OnMapLoad()
	{
		if (!Memory::IsDedicatedServer()) 
		{
			fix_shaders_nvidia();
			ShaderSpecularFix();
			fix_dynamic_lights();
			font_table_fix();
			sound_classes_fix();
			if (H2Config_shader_lod_max)
			{
				shader_lod_max();
			}
			if (H2Config_light_suppressor)
			{
				light_framerate_killer();
			}
		}
		if (!g_xbox_tickrate_enabled)
		{
			// both client/server
			fall_damage_fix();
		}
	}

	void Initalize()
	{
		tags::on_map_load(OnMapLoad);
	}
}
