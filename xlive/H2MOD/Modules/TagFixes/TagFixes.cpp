#include "TagFixes.h";
#include "H2MOD/Tags/MetaLoader/tag_loader.h"
#include "H2MOD.h"
#include "Blam/Cache/TagGroups/shader_definition.h"

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

			tags::ilterator shaders(blam_tag::tag_group_type::shader);
			while (!shaders.next().IsNull())
			{
				auto *shader = LOG_CHECK(tags::get_tag<blam_tag::tag_group_type::shader, shader_definition>(shaders.m_datum));
				if (shader && shader->shader_template.TagIndex == borked_template && LOG_CHECK(shader->postprocessDefinition.size > 0))
				{
					LOG_DEBUG_FUNC("shader {} has borked template", tags::get_tag_name(shaders.m_datum));
					auto *post_processing = shader->postprocessDefinition[0];
					if (LOG_CHECK(post_processing->bitmaps.size >= (bitmap_idx + 1)))
					{
						auto *bitmap_block = post_processing->bitmaps[bitmap_idx];
						if (bitmap_block->bitmapGroup == bitmap_to_fix)
						{
							LOG_DEBUG_FUNC("Nulled bitmap {}", bitmap_idx);
							bitmap_block->bitmapGroup = datum::Null;
						}
					}
				}
			}
		}

		void fix_shaders_nvidia()
		{
			if (h2mod->Server) return;

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
						impl_2->TagIndex = cinematic_shadow_datum.data;
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
			if (get_game_life_cycle() == Multiplayer) {
				*(float*)(&tags::get_tag_data()[0xE610B0]) = 14.0f; /*masterchief_mp hlmt max abs acc default value doubled*/
				*(float*)(&tags::get_tag_data()[0xE610B4]) = 20.0f; /*masterchief_mp hlmt max abs acc default value doubled*/
				*(float*)(&tags::get_tag_data()[0xE65D98]) = 14.0f; /*elite_mp hlmt max abs acc default value doubled*/
				*(float*)(&tags::get_tag_data()[0xE65D9C]) = 20.0f; /*elite_mp hlmt max abs acc default value doubled*/
			}
		}


	}


	void OnMapLoad()
	{
		fix_shaders_nvidia();
		ShaderSpecularFix();
		fall_damage_fix();
		fix_dynamic_lights();
	}

	void Initalize()
	{
		tags::on_map_load(OnMapLoad);
	}
}
