#include "KantTesting.h"
#include "Blam\Cache\DataTypes\BlamPrimitiveType.h"
#include "Blam\Cache\TagGroups\biped_definition.hpp"
#include "Blam\Cache\TagGroups\globals_definition.hpp"
#include "Blam\Cache\TagGroups\model_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_lightmap_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_structure_bsp_definition.hpp"
#include "Blam\Cache\TagGroups\weapon_definition.hpp"
#include "Blam\Engine\Game\GameEngineGlobals.h"
#include "Blam\Engine\Game\GameGlobals.h"
#include "Blam\Engine\Players\Players.h"
#include "Blam\LazyBlam\LazyBlam.hpp"
#include "H2MOD\EngineCalls\EngineCalls.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\Console\ConsoleCommands.h"
#include "H2MOD\Modules\EventHandler\EventHandler.hpp"
#include "H2MOD\Modules\Networking\CustomPackets\CustomPackets.h"
#include "H2MOD\Modules\Networking\Memory\bitstream.h"
#include "H2MOD\Modules\PlayerRepresentation\PlayerRepresentation.h"
#include "H2MOD\Tags\MetaExtender.h"
#include "H2MOD\Tags\MetaLoader\tag_loader.h"
#include "Util\Hooks\Hook.h"


namespace KantTesting
{
	const std::string weat_tag_path = "scenarios\\multi\\lockout\\lockout_big";
	datum w_datum;
	void MapLoad()
	{
		if (h2mod->GetEngineType() == Multiplayer)
		{
			/*
			auto w_datum_i = tag_loader::Get_tag_datum(weat_tag_path, blam_tag::tag_group_type::weathersystem, "carto_shared");
			if (!DATUM_IS_NONE(w_datum_i))
			{
				tag_loader::Load_tag(w_datum_i, true, "carto_shared");
				tag_loader::Push_Back();
				w_datum = tag_loader::ResolveNewDatum(w_datum_i);
				if (!DATUM_IS_NONE(w_datum))
				{
					auto scen = tags::get_tag_fast<s_scenario_group_definition>(tags::get_tags_header()->scenario_datum);
					auto sbsp = tags::get_tag_fast<s_scenario_structure_bsp_group_definition>(scen->structure_bsps[0]->structure_bsp.TagIndex);

					auto weat_block = MetaExtender::add_tag_block2<s_scenario_structure_bsp_group_definition::s_weather_palette_block>((unsigned long)std::addressof(sbsp->weather_palette));
					weat_block->name = "snow_cs";
					weat_block->weather_system.TagGroup = blam_tag::tag_group_type::weathersystem;
					weat_block->weather_system.TagIndex = w_datum;

					for (auto& cluster : sbsp->clusters)
					{
						cluster.weather = sbsp->weather_palette.size - 1;
					}

				}
			}*/
			auto mode_chief_mp_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief_mp");
			auto mode_chief_mp = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(mode_chief_mp_datum);
			auto base_variant = mode_chief_mp->variants[0];
			auto new_variant = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block>((unsigned long)std::addressof(mode_chief_mp->variants));
			new_variant->name = 0xABABABA;
			new_variant->dialogue.TagGroup = base_variant->dialogue.TagGroup;
			new_variant->dialogue.TagIndex = base_variant->dialogue.TagIndex;
			new_variant->runtime_model_region_0 = base_variant->runtime_model_region_0;
			new_variant->runtime_model_region_1 = base_variant->runtime_model_region_1;
			new_variant->runtime_model_region_2 = base_variant->runtime_model_region_2;
			new_variant->runtime_model_region_3 = base_variant->runtime_model_region_3;
			new_variant->runtime_model_region_4 = base_variant->runtime_model_region_4;
			new_variant->runtime_model_region_5 = base_variant->runtime_model_region_5;
			new_variant->runtime_model_region_6 = base_variant->runtime_model_region_6;
			new_variant->runtime_model_region_7 = base_variant->runtime_model_region_7;
			new_variant->runtime_model_region_8 = base_variant->runtime_model_region_8;
			new_variant->runtime_model_region_9 = base_variant->runtime_model_region_9;
			new_variant->runtime_model_region_10 = base_variant->runtime_model_region_10;
			new_variant->runtime_model_region_11 = base_variant->runtime_model_region_11;
			new_variant->runtime_model_region_12 = base_variant->runtime_model_region_12;
			new_variant->runtime_model_region_13 = base_variant->runtime_model_region_13;
			new_variant->runtime_model_region_14 = base_variant->runtime_model_region_14;
			new_variant->runtime_model_region_15 = base_variant->runtime_model_region_15;
			for (auto i = 0; i < base_variant->regions.size; i++)
			{
				auto region = base_variant->regions[i];
				auto new_region = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_regions_block>((unsigned long)std::addressof(new_variant->regions));
				new_region->region_name = region->region_name;
				new_region->runtime_model_region_index = region->runtime_model_region_index;
				new_region->region_runtime_flags = region->region_runtime_flags;
				new_region->parent_variant = region->parent_variant;
				new_region->sort_order = region->sort_order;
				for (auto k = 0; k < region->permutations.size; k++)
				{
					auto permutation = region->permutations[k];
					auto new_permutation = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_regions_block::s_permutations_block>((unsigned long)std::addressof(new_region->permutations));
					new_permutation->permutation_name = permutation->permutation_name;
					new_permutation->model_permutation_index = permutation->model_permutation_index;
					new_permutation->flags = permutation->flags;
					new_permutation->probability_0 = permutation->probability_0;
					new_permutation->runtime_permutation_index_0 = permutation->runtime_permutation_index_0;
					new_permutation->runtime_permutation_index_1 = permutation->runtime_permutation_index_1;
					new_permutation->runtime_permutation_index_2 = permutation->runtime_permutation_index_2;
					new_permutation->runtime_permutation_index_3 = permutation->runtime_permutation_index_3;
					new_permutation->runtime_permutation_index_4 = permutation->runtime_permutation_index_4;
					new_permutation->unk_1 = permutation->unk_1;
					new_permutation->unk2 = permutation->unk2;
					new_permutation->unk3 = permutation->unk3;
				}
			}

			auto e_datum_i = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\emoji_head\\emoji_head", blam_tag::tag_group_type::scenery, "carto_shared");
			if (!DATUM_IS_NONE(e_datum_i)) 
			{
				tag_loader::Load_tag(e_datum_i, true, "carto_shared");
				tag_loader::Push_Back();
				auto e_datum = tag_loader::ResolveNewDatum(e_datum_i);
				if (!DATUM_IS_NONE(e_datum)) 
				{
					auto new_object = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(new_variant->objects));
					new_object->parent_marker = string_id(184552154);
					new_object->child_object.TagGroup = blam_tag::tag_group_type::scenery;
					new_object->child_object.TagIndex = e_datum;
				}
			}
			auto repb = player_representation::add_representation(-1, -1, -1, s_player::e_character_type::Kant, new_variant->name);
			//auto rep = player_representation::clone_representation(2, s_player::e_character_type::Kant);
			//rep->third_person_variant = new_variant->name;
		}
	}
	
	void Initialize()
	{
		if (ENABLEKANTTEST) {
		//	if (!Memory::isDedicatedServer())
			//{
			//tags::on_map_load(MapLoad);
		//	}
		}
	}
}
