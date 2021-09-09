#include "TagFixes.h"

#include "H2MOD.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD/Tags/TagInterface.h"
#include "Blam/Cache/TagGroups/shader_definition.hpp"

namespace TagFixes
{
	namespace
	{
		void fix_shader_template_nvidia(const std::string &template_name, const std::string &bitmap_name, size_t bitmap_idx)
		{
			datum bitmap_to_fix = tags::find_tag(blam_tag::tag_group_type::bitmap, bitmap_name);
			datum borked_template = tags::find_tag(blam_tag::tag_group_type::shadertemplate, template_name);
			LOG_DEBUG_FUNC("bitmap {0}, borked_template {1}", bitmap_to_fix.data, borked_template.data);
			if (bitmap_to_fix.IsNull() || borked_template.IsNull())
				return;
			LOG_DEBUG_FUNC("Fixing: template {}, bitmap {}", template_name, bitmap_name);
			auto shaders = tags::find_tags(blam_tag::tag_group_type::shader);
			for(auto &shader_item : shaders)
			{
				auto shader = tags::get_tag<blam_tag::tag_group_type::shader, byte>(shader_item.first);
				if(shader != nullptr)
				{
					tag_reference* shader_template = reinterpret_cast<tag_reference*>(shader);
					if(shader_template->TagIndex == borked_template)
					{
						auto *shader_post = reinterpret_cast<tags::tag_data_block*>(shader + 0x20);
						if(shader_post->block_count > 0)
						{
							auto shader_post_data = tags::get_tag_data() + shader_post->block_data_offset;
							auto shader_post_bitmap = reinterpret_cast<tags::tag_data_block*>(shader_post_data + 0x4);
							if(shader_post_bitmap->block_count >= bitmap_idx + 1)
							{
								auto bitmap_data = tags::get_tag_data() + (shader_post_bitmap->block_data_offset + (bitmap_idx * 0xC));
								unsigned long* bitmap = reinterpret_cast<unsigned long*>(bitmap_data);
								if(*bitmap == bitmap_to_fix.data)
									*bitmap = NONE;
							}
						}
					}
				}
			}
		}
		void fix_shaders_nvidia()
		{
			if (Memory::isDedicatedServer()) return;

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
			return;
			auto cinematic_shadow_datum = tags::find_tag(blam_tag::tag_group_type::vertexshader, "rasterizer\\vertex_shaders_dx9\\shadow_buffer_generation_cinematic");
			auto shadow_datum = tags::find_tag(blam_tag::tag_group_type::shaderpass, "shaders\\shader_passes\\shadow\\shadow_generate");
			byte* shadow_tag = tags::get_tag<blam_tag::tag_group_type::shaderpass, BYTE>(shadow_datum);
			if(shadow_tag != nullptr)
			{
				auto *shadow_pp = reinterpret_cast<tags::tag_data_block*>(shadow_tag + 0x1C);
				if(shadow_pp->block_count > 0 && shadow_pp->block_data_offset != -1)
				{
					auto shadow_pp_data = tags::get_tag_data() + shadow_pp->block_data_offset;
					auto *shadow_impl_block = reinterpret_cast<tags::tag_data_block*>(shadow_pp_data);
					if(shadow_impl_block->block_count > 0 && shadow_impl_block->block_data_offset != -1)
					{
						auto *shadow_impl = tags::get_tag_data() + shadow_impl_block->block_data_offset;
						tag_reference* impl_1 = reinterpret_cast<tag_reference*>(shadow_impl + (0x14A) + 0xFC);
						tag_reference* impl_2 = reinterpret_cast<tag_reference*>(shadow_impl + (0x14A*2) + 0xFC);

						impl_1->TagIndex = cinematic_shadow_datum.data;
						//TODO: Re-enable this once the vertex shaders for shadows are fixed.
						//impl_2->TagIndex = cinematic_shadow_datum.data;
					}
				}
			}
		}
		void ShaderSpecularFix()
		{
			//Fix the Master Chief FP Arms Shader
			auto fp_shader_datum = tags::find_tag(blam_tag::tag_group_type::shader, "objects\\characters\\masterchief\\fp\\shaders\\fp_arms");
			BYTE* fp_shader_tag_data = tags::get_tag<blam_tag::tag_group_type::shader, BYTE>(fp_shader_datum);
			if (fp_shader_tag_data != nullptr)
				*(float*)(fp_shader_tag_data + 0x44) = 1;

			//Fix the Visor(s)
			auto tex_bump_env_datum = tags::find_tag(blam_tag::tag_group_type::shadertemplate, "shaders\\shader_templates\\opaque\\tex_bump_env");

			//Fix the Visor
			auto visor_shader_datum = tags::find_tag(blam_tag::tag_group_type::shader, "objects\\characters\\masterchief\\shaders\\masterchief_visor");
			BYTE* visor_shader_tag_data = tags::get_tag<blam_tag::tag_group_type::shader, BYTE>(visor_shader_datum);

			if (visor_shader_tag_data != nullptr)
				*(unsigned long*)(visor_shader_tag_data + 0x4) = tex_bump_env_datum.data;
			if (visor_shader_tag_data != nullptr)
			{
				auto *visor_pp = reinterpret_cast<tags::tag_data_block*>(visor_shader_tag_data + 0x20);
				if (visor_pp->block_count > 0 && visor_pp->block_data_offset != -1)
				{
					auto visor_pp_data = tags::get_tag_data() + visor_pp->block_data_offset;
					*(unsigned long*)(visor_pp_data) = tex_bump_env_datum.data;
				}
			}

			auto pilot_visor_shader_datum = tags::find_tag(blam_tag::tag_group_type::shader, "objects\\characters\\marine\\shaders\\helmet_pilot_visor");
			auto pilot_visor_tag = tags::get_tag < blam_tag::tag_group_type::shader, shader_definition>(pilot_visor_shader_datum);
			if(pilot_visor_tag != nullptr)
			{
				pilot_visor_tag->postprocessDefinition[0]->shaderTemplateIndex.TagIndex = tex_bump_env_datum;
			}

			//Fix the Grunt Shaders
			auto grunt_arm_shader_datum = tags::find_tag(blam_tag::tag_group_type::shader, "objects\\characters\\grunt\\shaders\\grunt_arms");
			BYTE* grunt_arm_shader_tag_data = tags::get_tag<blam_tag::tag_group_type::shader, BYTE>(grunt_arm_shader_datum);
			if (grunt_arm_shader_tag_data != nullptr)
				*(float*)(grunt_arm_shader_tag_data + 0x44) = 1;

			auto grunt_backpack_shader_datum = tags::find_tag(blam_tag::tag_group_type::shader, "objects\\characters\\grunt\\shaders\\grunt_backpack");
			BYTE* grunt_backpack_shader_tag_data = tags::get_tag<blam_tag::tag_group_type::shader, BYTE>(grunt_backpack_shader_datum);
			if (grunt_backpack_shader_tag_data != nullptr)
				*(float*)(grunt_backpack_shader_tag_data + 0x44) = 1;

			auto grunt_torso_shader_datum = tags::find_tag(blam_tag::tag_group_type::shader, "objects\\characters\\grunt\\shaders\\grunt_torso");
			BYTE* grunt_torso_shader_tag_data = tags::get_tag<blam_tag::tag_group_type::shader, BYTE>(grunt_torso_shader_datum);
			if (grunt_torso_shader_tag_data != nullptr)
				*(float*)(grunt_torso_shader_tag_data + 0x44) = 1;

		}
		void fall_damage_fix()
		{
			if (h2mod->GetEngineType() == Multiplayer) {
				*(float*)(&tags::get_tag_data()[0xE610B0]) = 14.0f; /*masterchief_mp hlmt max abs acc default value doubled*/
				*(float*)(&tags::get_tag_data()[0xE610B4]) = 20.0f; /*masterchief_mp hlmt max abs acc default value doubled*/
				*(float*)(&tags::get_tag_data()[0xE65D98]) = 14.0f; /*elite_mp hlmt max abs acc default value doubled*/
				*(float*)(&tags::get_tag_data()[0xE65D9C]) = 20.0f; /*elite_mp hlmt max abs acc default value doubled*/
			}
		}
		void font_table_fix()
		{
			WriteValue<int>(Memory::GetAddress(0x464940), 0);
			WriteValue<int>(Memory::GetAddress(0x46494C), 0);
			WriteValue<int>(Memory::GetAddress(0x464958), 0);
			WriteValue<int>(Memory::GetAddress(0x464964), 0);
		}
	}


	void OnMapLoad()
	{
		if (!Memory::isDedicatedServer()) {
			fix_shaders_nvidia();
			ShaderSpecularFix();
			fix_dynamic_lights();
			font_table_fix();
		}

		// both server and client
		fall_damage_fix();
	}

	void Initalize()
	{
		tags::on_map_load(OnMapLoad);
	}
}
