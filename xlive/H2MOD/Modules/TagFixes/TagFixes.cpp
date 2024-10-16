#include "stdafx.h"
#include "TagFixes.h"

#include "Blam/Cache/TagGroups/sound_classes_definition.hpp"

#include "shaders/shader_definitions.h"
#include "units/biped_definitions.h"

#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Tags/TagInterface.h"
#include "objects/light_definitions.h"

// ### TODO Cleanup
extern bool g_xbox_tickrate_enabled;

namespace TagFixes
{
	namespace
	{
		void fix_shader_template_nvidia(const char *template_name, const char* bitmap_name, int32 bitmap_idx)
		{
			datum bitmap_to_fix = tag_loaded(_tag_group_bitmap, bitmap_name);
			datum borked_template = tag_loaded(_tag_group_shader_template, template_name);
			LOG_DEBUG_FUNC("bitmap {0}, borked_template {1}", bitmap_to_fix, borked_template);
			if (bitmap_to_fix == NONE || borked_template == NONE)
				return;
			LOG_DEBUG_FUNC("Fixing: template {}, bitmap {}", template_name, bitmap_name);

			tag_iterator iterator;
			tag_iterator_new(&iterator, _tag_group_shader);

			while (tag_iterator_next(&iterator) != NONE)
			{
				char* shader = (char*)tag_get_fast(iterator.current_tag_index);
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

			datum cinematic_shadow_datum = tag_loaded(_tag_group_vertex_shader, "rasterizer\\vertex_shaders_dx9\\shadow_buffer_generation_cinematic");
			datum shadow_datum = tag_loaded(_tag_group_shader_pass, "shaders\\shader_passes\\shadow\\shadow_generate");
			char* shadow_tag = (char*)tag_get_fast(shadow_datum);
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

		void font_table_fix()
		{
			WriteValue<int>(Memory::GetAddress(0x464940), 0);
			WriteValue<int>(Memory::GetAddress(0x46494C), 0);
			WriteValue<int>(Memory::GetAddress(0x464958), 0);
			WriteValue<int>(Memory::GetAddress(0x464964), 0);
		}

		void shader_lod_max()
		{
			tag_iterator iterator;
			tag_iterator_new(&iterator, _tag_group_shader);

			while (tag_iterator_next(&iterator) != NONE)
			{
				s_shader_definition* shader = (s_shader_definition*)tag_get_fast(iterator.current_tag_index);
				shader->shader_lod_bias = _shader_lod_bias_never;
			}
		}

		void light_framerate_killer()
		{
			tag_iterator iterator;
			tag_iterator_new(&iterator, _tag_group_light);

			while (tag_iterator_next(&iterator) != NONE)
			{
				light_definition* light = (light_definition*)tag_get_fast(iterator.current_tag_index);
				// Disabled since it caused issues where certain lights wouldnt render randomly
				// TODO figure out why it does this at some other point in time
				// light->flags.set(_light_definition_light_framerate_killer, true);

				light->flags.set(_light_definition_multiplayer_override, true);
			}
		}

		void fall_damage_fix()
		{
			tag_iterator iterator;
			tag_iterator_new(&iterator, _tag_group_biped);

			while (tag_iterator_next(&iterator) != NONE)
			{
				_biped_definition* biped = (_biped_definition*)tag_get_fast(iterator.current_tag_index);
				biped->unit.object.abs_acceleration.lower *= 2.0f;
				biped->unit.object.abs_acceleration.upper *= 2.0f;
			}
		}

		void sound_classes_fix()
		{
			// Change sound_classes data to equivalents in original halo 2
			datum sound_classes_datum = tag_loaded(_tag_group_sound_classes, "sound\\sound_classes");
			if (sound_classes_datum != NONE)
			{
				s_sound_classes_block* sound_classes = (s_sound_classes_block*)tag_get_fast(sound_classes_datum);

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
}
