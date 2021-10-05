#include "PlayerRepresentation.h"

#include "H2MOD.h"
#include "Blam/Cache/DataTypes/BlamTag.h"
#include "Blam/Cache/TagGroups/biped_definition.hpp"
#include "Blam/Cache/TagGroups/globals_definition.hpp"
#include "Blam/Cache/TagGroups/model_defenition.hpp"
#include "Blam/Cache/TagGroups/scenario_definition.hpp"
#include "Blam/Engine/Game/GameEngineGlobals.h"
#include "Blam/Engine/Game/GameGlobals.h"
#include "Blam/Engine/Players/Players.h"
#include "H2MOD/EngineCalls/EngineCalls.h"
#include "H2MOD/Tags/MetaExtender.h"
#include "H2MOD/Tags/TagInterface.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"
#include "Util/Hooks/Hook.h"

namespace player_representation
{
	//Non-zero index based value for the count of valid representation types
	byte representation_count = 4;
	s_globals_group_definition::s_player_representation_block* add_representation(datum fp_hands, datum fp_body, datum tp_biped, Player::Biped type, string_id variant)
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
				new_rep->first_person_hands = globals->player_representation[0]->first_person_hands;

			if (!DATUM_IS_NONE(fp_body))
			{
				new_rep->first_person_body.TagGroup = blam_tag::tag_group_type::rendermodel;
				new_rep->first_person_body.TagIndex = fp_body;
			}
			else
				new_rep->first_person_body = globals->player_representation[0]->first_person_body;

			if (!DATUM_IS_NONE(tp_biped))
			{
				new_rep->third_person_unit.TagGroup == blam_tag::tag_group_type::biped;
				new_rep->third_person_unit.TagIndex = tp_biped;
			}
			else
				new_rep->third_person_unit = globals->player_representation[0]->third_person_unit;

			if (variant != -1)
				new_rep->third_person_variant = variant;
			++representation_count;
			return new_rep;
		}
		return nullptr;
	}


	s_globals_group_definition::s_player_representation_block* clone_representation(int index, Player::Biped newType)
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
			++representation_count;
			return new_rep;
		}
		return nullptr;
	}

	datum get_object_datum_from_representation(byte representation_index)
	{
		auto globals_datum = tags::find_tag(blam_tag::tag_group_type::globals, "globals\\globals");
		if (!DATUM_IS_NONE(globals_datum))
		{
			auto globals = tags::get_tag_fast<s_globals_group_definition>(globals_datum);
			if(representation_index < globals->player_representation.size)
				return globals->player_representation[representation_index]->third_person_unit.TagIndex;
		}
		//Maybe not the best way to do this, but there should be no situation where the spartan datum changes.
		return 0xF28C3826;
	}

	typedef void(__cdecl t_network_session_player_profile_recieve)(int player_index, Player::Properties* a2);
	t_network_session_player_profile_recieve* p_network_session_player_profile_recieve;

	void __cdecl network_session_player_profile_recieve(int player_index, Player::Properties* a2)
	{
		auto a = s_game_globals::get()->engine_settings;
		LOG_INFO_GAME("[{}] {}", __FUNCTION__, a.m_engine_type);
		if (EngineCalls::game_is_campaign())
		{
			auto scenario = tags::get_tag_fast<s_scenario_group_definition>(tags::get_tags_header()->scenario_datum);
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
			}
		}
		else if (EngineCalls::game_is_multiplayer())
		{
			auto globals_datum = tags::find_tag(blam_tag::tag_group_type::globals, "globals\\globals");
			auto globals = tags::get_tag_fast<s_globals_group_definition>(globals_datum);

			if (a2->profile.player_character_type == Player::Biped::MasterChief)
				a2->profile.player_character_type = Player::Biped::Spartan;
			if (a2->profile.player_character_type == Player::Biped::Dervish)
				a2->profile.player_character_type = Player::Biped::Elite;

			if ((byte)a2->profile.player_character_type > representation_count)
				a2->profile.player_character_type = Player::Biped::Spartan;
		}

		if ((char)a2->player_displayed_skill != -1)
			if ((char)a2->player_displayed_skill < 0)
				a2->player_displayed_skill = 0;

		if ((char)a2->player_overall_skill != -1)
			if ((char)a2->player_overall_skill < 0)
				a2->player_overall_skill = 0;

		if (a2->player_handicap_level > Player::Handicap::Severe)
			a2->player_handicap_level = Player::Handicap::Severe;

		if (a2->bungie_user_role <= 7)
			a2->bungie_user_role = 7;

		//if (EngineCalls::get_game_mode_engine())
		//	if (EngineCalls::get_game_variant()->game_engine_flags & 1)
		//		if (a2->player_team && !((1 << a2->player_team) & s_game_engine_globals::get()->Unk1))
		//			a2->player_team = e_object_team::None;
	}
	void on_map_load()
	{
		if (h2mod->GetEngineType() == Multiplayer) {
			auto scen = tags::get_tag_fast<s_scenario_group_definition>(tags::get_tags_header()->scenario_datum);
			auto skele_datum = tag_loader::Get_tag_datum("objects\\characters\\masterchief_skeleton\\masterchief_skeleton", blam_tag::tag_group_type::biped, "carto_shared");
			auto skele_fp_datum = tag_loader::Get_tag_datum("objects\\characters\\masterchief_skeleton\\fp\\fp", blam_tag::tag_group_type::rendermodel, "carto_shared");
			auto skele_body_datum = tag_loader::Get_tag_datum("objects\\characters\\masterchief_skeleton\\fp_body\\fp_body", blam_tag::tag_group_type::rendermodel, "carto_shared");

			if (!DATUM_IS_NONE(skele_datum) && !DATUM_IS_NONE(skele_fp_datum) && !DATUM_IS_NONE(skele_body_datum))
			{
				tag_loader::Load_tag(skele_fp_datum, true, "carto_shared");
				tag_loader::Load_tag(skele_body_datum, true, "carto_shared");
				tag_loader::Load_tag(skele_datum, true, "carto_shared");
				tag_loader::Push_Back();
				auto skele_new_datum = tag_loader::ResolveNewDatum(skele_datum);
				player_representation::add_representation(tag_loader::ResolveNewDatum(skele_fp_datum), tag_loader::ResolveNewDatum(skele_body_datum), skele_new_datum, Player::Biped::Skeleton);
				auto new_def = MetaExtender::add_tag_block2<s_scenario_group_definition::s_simulation_definition_table_block>((unsigned long)std::addressof(scen->simulation_definition_table));
				new_def->tag = skele_new_datum;
			}
			else
			{
				clone_representation(2, Player::Biped::Skeleton);
			}
			auto flood_datum = tag_loader::Get_tag_datum("objects\\characters\\floodcombat_elite\\floodcombat_elite_mp", blam_tag::tag_group_type::biped, "carto_shared");
			auto flood_arms_datum = tag_loader::Get_tag_datum("objects\\characters\\flood_mp\\fp_arms\\fp_arms", blam_tag::tag_group_type::rendermodel, "carto_shared");
			auto flood_body_datum = tag_loader::Get_tag_datum("objects\\characters\\flood_mp\\fp_body\\fp_body", blam_tag::tag_group_type::rendermodel, "carto_shared");
			auto dervish_jmad_datum = tags::find_tag(blam_tag::tag_group_type::modelanimationgraph, "objects\\characters\\dervish\\dervish");
			if (!DATUM_IS_NONE(flood_datum) && !DATUM_IS_NONE(flood_arms_datum) && !DATUM_IS_NONE(flood_body_datum) && !DATUM_IS_NONE(dervish_jmad_datum))
			{
				tag_loader::Load_tag(flood_datum, true, "carto_shared");
				tag_loader::Load_tag(flood_arms_datum, true, "carto_shared");
				tag_loader::Load_tag(flood_body_datum, true, "carto_shared");
				tag_loader::Push_Back();
				player_representation::add_representation(tag_loader::ResolveNewDatum(flood_arms_datum), tag_loader::ResolveNewDatum(flood_body_datum), tag_loader::ResolveNewDatum(flood_datum), Player::Biped::Flood);
				auto new_def = MetaExtender::add_tag_block2<s_scenario_group_definition::s_simulation_definition_table_block>((unsigned long)std::addressof(scen->simulation_definition_table));
				new_def->tag = tag_loader::ResolveNewDatum(flood_datum);

				auto biped = tags::get_tag<blam_tag::tag_group_type::biped,s_biped_group_definition>(tag_loader::ResolveNewDatum(flood_datum), true);
				auto model = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(biped->unitTag.objectTag.model.TagIndex, true);
				auto anim = tags::get_tag<blam_tag::tag_group_type::modelanimationgraph, byte>(model->animation.TagIndex, true);
				tag_reference* parent_anim = reinterpret_cast<tag_reference*>(anim);
				parent_anim->TagGroup = blam_tag::tag_group_type::modelanimationgraph;
				parent_anim->TagIndex = dervish_jmad_datum;
			}
			else
			{
				clone_representation(3, Player::Biped::Flood);
			}
		}
	}
	void apply_hooks()
	{
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
