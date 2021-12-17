#include "PlayerRepresentation.h"
#include "Blam\Cache\DataTypes\BlamTag.h"
#include "Blam\Cache\TagGroups\biped_definition.hpp"
#include "Blam\Cache\TagGroups\globals_definition.hpp"
#include "Blam\Cache\TagGroups\model_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_definition.hpp"
#include "Blam\Engine\Game\GameEngineGlobals.h"
#include "Blam\Engine\Game\GameGlobals.h"
#include "Blam\Engine\Players\Players.h"
#include "H2MOD.h"
#include "Blam/Engine/Players/Players.h"
#include "Blam/Engine/Players/Players.h"
#include "H2MOD\EngineCalls\EngineCalls.h"
#include "H2MOD/Modules/Config/Config.h"
#include "H2MOD/Modules/SpecialEvents/SpecialEvents.h"
#include "H2MOD\Tags\MetaExtender.h"
#include "H2MOD\Tags\MetaLoader\tag_loader.h"
#include "H2MOD\Tags\TagInterface.h"
#include "Util\Hooks\Hook.h"

namespace player_representation
{
	//Non-zero index based value for the count of valid representation types
	std::map<s_player::e_character_type, byte> type_map
	{
		{s_player::e_character_type::MasterChief, 0},
		{s_player::e_character_type::Dervish, 1},
		{s_player::e_character_type::Spartan, 2},
		{s_player::e_character_type::Elite, 3}
	};
	byte representation_count = 4;
	s_globals_group_definition::s_player_representation_block* add_representation(datum fp_hands, datum fp_body, datum tp_biped, s_player::e_character_type type, string_id variant)
	{
		auto globals_datum = tags::find_tag(blam_tag::tag_group_type::globals, "globals\\globals");
		if (!DATUM_IS_NONE(globals_datum)) 
		{
			auto globals = tags::get_tag_fast<s_globals_group_definition>(globals_datum);
			auto new_rep = MetaExtender::add_tag_block2<s_globals_group_definition::s_player_representation_block>((unsigned long)std::addressof(globals->player_representation));
			if (!DATUM_IS_NONE(fp_hands))
			{
				new_rep->first_person_hands.TagGroup = blam_tag::tag_group_type::rendermodel;
				new_rep->first_person_hands.TagIndex = fp_hands;
			}
			else
				new_rep->first_person_hands = globals->player_representation[2]->first_person_hands;

			if (!DATUM_IS_NONE(fp_body))
			{
				new_rep->first_person_body.TagGroup = blam_tag::tag_group_type::rendermodel;
				new_rep->first_person_body.TagIndex = fp_body;
			}
			else
				new_rep->first_person_body = globals->player_representation[2]->first_person_body;

			if (!DATUM_IS_NONE(tp_biped))
			{
				new_rep->third_person_unit.TagGroup = blam_tag::tag_group_type::biped;
				new_rep->third_person_unit.TagIndex = tp_biped;
			}
			else
				new_rep->third_person_unit = globals->player_representation[2]->third_person_unit;

			if (variant != -1)
				new_rep->third_person_variant = variant;
			type_map.emplace(type, representation_count);
			++representation_count;
			return new_rep;
		}
		return nullptr;
	}


	s_globals_group_definition::s_player_representation_block* clone_representation(int index, s_player::e_character_type newType)
	{
		auto globals_datum = tags::find_tag(blam_tag::tag_group_type::globals, "globals\\globals");
		if (!DATUM_IS_NONE(globals_datum))
		{
			auto globals = tags::get_tag_fast<s_globals_group_definition>(globals_datum);
			auto new_rep = MetaExtender::add_tag_block2<s_globals_group_definition::s_player_representation_block>((unsigned long)std::addressof(globals->player_representation));
			new_rep->first_person_body = globals->player_representation[index]->first_person_body;
			new_rep->first_person_hands = globals->player_representation[index]->first_person_hands;
			new_rep->third_person_unit = globals->player_representation[index]->third_person_unit;
			new_rep->third_person_variant = globals->player_representation[index]->third_person_variant;
			type_map.emplace(newType, representation_count);
			++representation_count;
			return new_rep;
		}
		return nullptr;
	}

	s_globals_group_definition::s_player_representation_block* get_representation(int index)
	{
		auto globals_datum = tags::find_tag(blam_tag::tag_group_type::globals, "globals\\globals");
		if (!DATUM_IS_NONE(globals_datum))
		{
			auto globals = tags::get_tag_fast<s_globals_group_definition>(globals_datum);
			return globals->player_representation[index];
		}
		return nullptr;
	}

	datum get_object_datum_from_representation(s_player::e_character_type representation_index)
	{
		auto globals_datum = tags::find_tag(blam_tag::tag_group_type::globals, "globals\\globals");
		if (!DATUM_IS_NONE(globals_datum))
		{
			auto globals = tags::get_tag_fast<s_globals_group_definition>(globals_datum);
			if(type_map.find(representation_index) != type_map.end())
				return globals->player_representation[type_map[representation_index]]->third_person_unit.TagIndex;
		}
		//Maybe not the best way to do this, but there should be no situation where the spartan datum changes.
		return 0xF28C3826;
	}

	typedef void(__cdecl t_network_session_player_profile_recieve)(int player_index, s_player::s_player_properties* a2);
	t_network_session_player_profile_recieve* p_network_session_player_profile_recieve;

	void __cdecl network_session_player_profile_recieve(int player_index, s_player::s_player_properties* a2)
	{
		LOG_INFO_GAME("[{}] {}", __FUNCTION__, s_game_globals::get()->m_options.m_engine_type);
		if (s_game_globals::game_is_campaign())
		{
			p_network_session_player_profile_recieve(player_index, a2);
			return;
			/*auto scenario = tags::get_tag_fast<s_scenario_group_definition>(tags::get_tags_header()->scenario_datum);
			s_scenario_group_definition::s_player_starting_locations_block::e_campaign_player_type player_type = s_scenario_group_definition::s_player_starting_locations_block::e_campaign_player_type::none;
			int v5 = 0;
			if (scenario->player_starting_locations.size > 0)
			{
				for (auto i = 0; i < scenario->player_starting_locations.size; i++)
				{
					auto starting_location = scenario->player_starting_locations[i];
					if (starting_location->campaign_player_type != s_scenario_group_definition::s_player_starting_locations_block::e_campaign_player_type::none) {
						player_type = starting_location->campaign_player_type;
						break;
					}
				}
				a2->player_team = e_object_team::player;
				if (player_type == s_scenario_group_definition::s_player_starting_locations_block::e_campaign_player_type::none)
					a2->profile.player_character_type = static_cast<Player::Biped>(static_cast<byte>(static_cast<short>(player_type)));
			}*/
		}
		else if (s_game_globals::game_is_multiplayer())
		{
			if (a2->profile.player_character_type == s_player::e_character_type::MasterChief)
				a2->profile.player_character_type = s_player::e_character_type::Spartan;
			if (a2->profile.player_character_type == s_player::e_character_type::Dervish)
				a2->profile.player_character_type = s_player::e_character_type::Elite;

			if(SpecialEvents::getCurrentEvent() != SpecialEvents::e_halloween)
			{
				if (a2->profile.player_character_type == s_player::e_character_type::Skeleton)
					a2->profile.player_character_type = s_player::e_character_type::Spartan;
			}
			else if (H2Config_spooky_boy && !Memory::isDedicatedServer())
				*Memory::GetAddress<s_player::e_character_type*>(0x51A67C) = s_player::e_character_type::Skeleton;

			/*if(s_player::getPlayer(DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index))->identifier == 0x000462d3a1e02a34)
				a2->profile.player_character_type = (s_player::e_character_type)type_map[s_player::e_character_type::Kant];*/
			/*LOG_INFO_GAME("[{}]{:x}", __FUNCTION__, usersSignInInfo[0].xuid);
			if (usersSignInInfo[0].xuid == )
				a2->profile.player_character_type = (s_player::e_character_type)type_map[s_player::e_character_type::Kant];*/

			if ((byte)a2->profile.player_character_type > representation_count)
				a2->profile.player_character_type = s_player::e_character_type::Spartan;
		}

		if ((char)a2->player_displayed_skill != -1)
			if ((char)a2->player_displayed_skill < 0)
				a2->player_displayed_skill = 0;

		if ((char)a2->player_overall_skill != -1)
			if ((char)a2->player_overall_skill < 0)
				a2->player_overall_skill = 0;

		if (a2->player_handicap_level > s_player::e_handicap::Severe)
			a2->player_handicap_level = s_player::e_handicap::Severe;

		if (a2->bungie_user_role <= 7)
			a2->bungie_user_role = 7;

		if (EngineCalls::get_game_mode_engine()
			&& s_game_globals::get()->get_game_variant()->game_engine_flags & FLAG(e_game_engine_flags::_game_engine_teams_bit)
			&& (a2->player_team && !(s_game_engine_globals::get()->Unk1 & FLAG(a2->player_team))))
		{
			a2->player_team = e_object_team::None;
		}
	}
	void on_map_load()
	{
		if (h2mod->GetEngineType() == Multiplayer) 
		{
			representation_count = 4;
			if (H2Config_spooky_boy && SpecialEvents::getCurrentEvent() == SpecialEvents::e_halloween && !Memory::isDedicatedServer())
				*Memory::GetAddress<s_player::e_character_type*>(0x51A67C) = s_player::e_character_type::Skeleton;

			auto scen = tags::get_tag_fast<s_scenario_group_definition>(tags::get_tags_header()->scenario_datum);
			auto skele_datum = tag_loader::Get_tag_datum("objects\\characters\\masterchief_skeleton\\masterchief_skeleton", blam_tag::tag_group_type::biped, "carto_shared");
			auto skele_fp_datum = tag_loader::Get_tag_datum("objects\\characters\\masterchief_skeleton\\fp\\fp", blam_tag::tag_group_type::rendermodel, "carto_shared");
			auto skele_body_datum = tag_loader::Get_tag_datum("objects\\characters\\masterchief_skeleton\\fp_body\\fp_body", blam_tag::tag_group_type::rendermodel, "carto_shared");

			if (!DATUM_IS_NONE(skele_datum) && !DATUM_IS_NONE(skele_fp_datum) && !DATUM_IS_NONE(skele_body_datum) && SpecialEvents::getCurrentEvent() == SpecialEvents::e_halloween && !H2Config_no_events)
			{
				tag_loader::Load_tag(skele_fp_datum, true, "carto_shared");
				tag_loader::Load_tag(skele_body_datum, true, "carto_shared");
				tag_loader::Load_tag(skele_datum, true, "carto_shared");
				tag_loader::Push_Back();
				auto skele_new_datum = tag_loader::ResolveNewDatum(skele_datum);
				player_representation::add_representation(tag_loader::ResolveNewDatum(skele_fp_datum), tag_loader::ResolveNewDatum(skele_body_datum), skele_new_datum, s_player::e_character_type::Skeleton);
				auto new_def = MetaExtender::add_tag_block2<s_scenario_group_definition::s_simulation_definition_table_block>((unsigned long)std::addressof(scen->simulation_definition_table));
				new_def->tag = skele_new_datum;
			}
			else
			{
				clone_representation(2, s_player::e_character_type::Skeleton);
			}
			auto flood_datum = tag_loader::Get_tag_datum("objects\\characters\\floodcombat_elite\\floodcombat_elite_mp", blam_tag::tag_group_type::biped, "carto_shared");
			auto flood_arms_datum = tag_loader::Get_tag_datum("objects\\characters\\flood_mp\\fp_arms\\fp_arms", blam_tag::tag_group_type::rendermodel, "carto_shared");
			auto flood_body_datum = tag_loader::Get_tag_datum("objects\\characters\\flood_mp\\fp_body\\fp_body", blam_tag::tag_group_type::rendermodel, "carto_shared");
			auto dervish_jmad_datum = tags::find_tag(blam_tag::tag_group_type::modelanimationgraph, "objects\\characters\\dervish\\dervish");
			if (!DATUM_IS_NONE(flood_datum) && !DATUM_IS_NONE(flood_arms_datum) && !DATUM_IS_NONE(flood_body_datum))
			{
				tag_loader::Load_tag(flood_datum, true, "carto_shared");
				tag_loader::Load_tag(flood_arms_datum, true, "carto_shared");
				tag_loader::Load_tag(flood_body_datum, true, "carto_shared");
				tag_loader::Push_Back();
				player_representation::add_representation(tag_loader::ResolveNewDatum(flood_arms_datum), tag_loader::ResolveNewDatum(flood_body_datum), tag_loader::ResolveNewDatum(flood_datum), s_player::e_character_type::Flood);
				auto new_def = MetaExtender::add_tag_block2<s_scenario_group_definition::s_simulation_definition_table_block>((unsigned long)std::addressof(scen->simulation_definition_table));
				new_def->tag = tag_loader::ResolveNewDatum(flood_datum);
			}
			else
			{
				clone_representation(3, s_player::e_character_type::Flood);
			}
			/*auto mode_chief_mp_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief_mp");
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
			add_representation(-1, -1, -1, s_player::e_character_type::Kant, new_variant->name);*/
		}
	}
	void apply_hooks()
	{
		p_network_session_player_profile_recieve = Memory::GetAddress<t_network_session_player_profile_recieve*>(0x52F23);
		PatchCall(Memory::GetAddress(0x5509E, 0x5d596), network_session_player_profile_recieve);
		//Change the packet validation for player::properties::profile to just accept anything, we catch it later if it's outside of the acceptable range.
		WriteValue<byte>(Memory::GetAddress(0x54fb3, 0x5D4AB), 25);
	}

	void initialize()
	{
		apply_hooks();
		tags::on_map_load(on_map_load);
	}
}
