#include "stdafx.h"

#include "PlayerRepresentation.h"
#include "Blam/Cache/TagGroups/biped_definition.hpp"
#include "Blam/Cache/TagGroups/model_definition.hpp"
#include "Blam/Cache/TagGroups/scenario_definition.hpp"
#include "Blam/Engine/game/game_engine.h"
#include "Blam/Engine/game/game_engine_util.h"
#include "Blam/Engine/game/game_globals.h"
#include "Blam/Engine/game/players.h"
#include "Blam/Engine/Networking/Session/NetworkSession.h"
#include "Blam/Engine/tag_files/global_string_ids.h"

#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/SpecialEvents/SpecialEvents.h"
#include "H2MOD/Tags/MetaExtender.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"
#include "H2MOD/Tags/TagInterface.h"

#include "Util/Hooks/Hook.h"

namespace PlayerRepresentation
{
	//Non-zero index based value for the count of valid representation types
	std::map<e_character_type, byte> type_map
	{
		{character_type_masterchief, 0},
		{character_type_dervish, 1},
		{character_type_spartan, 2},
		{character_type_elite, 3},
	};

	BYTE current_representation_count = 4;
	s_globals_group_definition::s_player_representation_block* add_representation(datum fp_hands, datum fp_body, datum tp_biped, e_character_type type, string_id variant)
	{
		s_globals_group_definition* globals = tags::get_matg_globals_ptr();

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
		type_map.emplace(type, current_representation_count);
		++current_representation_count;
		return new_rep;
	}


	s_globals_group_definition::s_player_representation_block* clone_representation(int index, e_character_type newType)
	{
		s_globals_group_definition* globals = tags::get_matg_globals_ptr();
		auto new_rep = MetaExtender::add_tag_block2<s_globals_group_definition::s_player_representation_block>((unsigned long)std::addressof(globals->player_representation));
		new_rep->first_person_body = globals->player_representation[index]->first_person_body;
		new_rep->first_person_hands = globals->player_representation[index]->first_person_hands;
		new_rep->third_person_unit = globals->player_representation[index]->third_person_unit;
		new_rep->third_person_variant = globals->player_representation[index]->third_person_variant;
		type_map.emplace(newType, current_representation_count);
		++current_representation_count;
		return new_rep;
	}

	s_globals_group_definition::s_player_representation_block* get_representation(int index)
	{
		s_globals_group_definition* globals = tags::get_matg_globals_ptr();
		return globals->player_representation[index];
	}

	datum get_object_datum_from_representation(e_character_type representation_index)
	{
		auto game_globals = tags::get_matg_globals_ptr();
		if (game_globals != nullptr)
		{
			if (type_map.find(representation_index) != type_map.end())
				return game_globals->player_representation[type_map[representation_index]]->third_person_unit.TagIndex;
		}
		return DATUM_INDEX_NONE;
	}

	void __cdecl player_validate_extra_characters_type(int player_index, s_player_properties* player_properties)
	{
		if (player_properties->profile_traits.profile.player_character_type == character_type_masterchief)
			player_properties->profile_traits.profile.player_character_type = character_type_spartan;
		if (player_properties->profile_traits.profile.player_character_type == character_type_dervish)
			player_properties->profile_traits.profile.player_character_type = character_type_elite;

		if (get_current_special_event() != e_special_event_type::_halloween)
		{
			if (player_properties->profile_traits.profile.player_character_type == character_type_skeleton)
				player_properties->profile_traits.profile.player_character_type = character_type_spartan;
		}
		else
		{
			if (H2Config_spooky_boy && !Memory::IsDedicatedServer())
			{
				for (byte user_index = 0; user_index < k_number_of_users; user_index++)
				{
					network_session_interface_set_local_user_character_type(user_index, character_type_skeleton);
				}
			}

			player_properties->profile_traits.profile.player_character_type = character_type_skeleton;
		}

		if ((byte)player_properties->profile_traits.profile.player_character_type > current_representation_count)
			player_properties->profile_traits.profile.player_character_type = character_type_spartan;
	}

	typedef void(__cdecl player_properties_validate_configuration_t)(int, s_player_properties*);
	player_properties_validate_configuration_t* p_player_properties_validate_configuration;

	void __cdecl player_properties_validate_configuration_hook(int player_index, s_player_properties* player_properties)
	{
		LOG_INFO_GAME("{} - game engine: {}", __FUNCTION__, s_game_globals::get()->options.engine_type);

		auto player_biped_type = player_properties->profile_traits.profile.player_character_type;
		p_player_properties_validate_configuration(player_index, player_properties);
		
		if (s_game_globals::game_is_campaign())
		{
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
			return;
		}
		else if (s_game_globals::game_is_multiplayer())
		{
			// reset the player biped type to what was previously in the field
			player_properties->profile_traits.profile.player_character_type = player_biped_type;
			// then go through our multiplayer player properties filters
			player_validate_extra_characters_type(player_index, player_properties);
		}
	}

	// no params because actually "usercall"
	void* p_player_representation_get;

	// calls the original player representation get function
	void player_representatio_get_orig_fn(int player_index, int* out_variant_index, int* a3)
	{
		__asm {
			mov eax, a3
			push eax
			mov ebx, out_variant_index
			mov eax, player_index
			call p_player_representation_get
			add esp, 4
		}
	}

	void __cdecl player_representation_get(datum player_datum, int* out_variant_index, int* a3)
	{
		int player_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(player_datum);
		s_player* player = s_player::GetPlayer(player_index);

		if (s_game_globals::game_is_multiplayer())
			player_validate_extra_characters_type(player_index, &player->properties[0]);
		player_representatio_get_orig_fn(player_datum, out_variant_index, a3);
	}

	__declspec(naked) void player_representation_get_to_cdecl()
	{
		__asm {
			push eax
			mov eax, [esp + 4h + 4h]
			push eax // a3
			push ebx // out_variant
			mov eax, [esp + 8h]
			push eax // player index
			call player_representation_get
			add esp, 10h // clear 16 bytes to esp == ret addr
			retn
		}
	}

	void OnMapLoad()
	{
		current_representation_count = 4;

		if (h2mod->GetEngineType() == _multiplayer) 
		{
			if (H2Config_spooky_boy && get_current_special_event() == e_special_event_type::_halloween && !Memory::IsDedicatedServer())
			{
				for (byte user_index = 0; user_index < k_number_of_users; user_index++)
				{
					network_session_interface_set_local_user_character_type(user_index, character_type_skeleton);
				}
			}

			auto scen = tags::get_tag_fast<s_scenario_group_definition>(tags::get_tags_header()->scenario_datum);
			auto skele_datum = tag_loader::Get_tag_datum("objects\\characters\\masterchief_skeleton\\masterchief_skeleton", blam_tag::tag_group_type::biped, "carto_shared");
			auto skele_fp_datum = tag_loader::Get_tag_datum("objects\\characters\\masterchief_skeleton\\fp\\fp", blam_tag::tag_group_type::rendermodel, "carto_shared");
			auto skele_body_datum = tag_loader::Get_tag_datum("objects\\characters\\masterchief_skeleton\\fp_body\\fp_body", blam_tag::tag_group_type::rendermodel, "carto_shared");

			if (!DATUM_IS_NONE(skele_datum) && !DATUM_IS_NONE(skele_fp_datum) && !DATUM_IS_NONE(skele_body_datum) && get_current_special_event() == e_special_event_type::_halloween && !H2Config_no_events)
			{
				tag_loader::Load_tag(skele_fp_datum, true, "carto_shared");
				tag_loader::Load_tag(skele_body_datum, true, "carto_shared");
				tag_loader::Load_tag(skele_datum, true, "carto_shared");
				tag_loader::Push_Back();
				auto skele_new_datum = tag_loader::ResolveNewDatum(skele_datum);
				add_representation(tag_loader::ResolveNewDatum(skele_fp_datum), tag_loader::ResolveNewDatum(skele_body_datum), skele_new_datum, character_type_skeleton);
				auto new_def = MetaExtender::add_tag_block2<s_scenario_group_definition::s_simulation_definition_table_block>((unsigned long)std::addressof(scen->simulation_definition_table));
				new_def->tag = skele_new_datum;
			}
			else
			{
				clone_representation(2, character_type_skeleton);
			}
			auto flood_datum = tag_loader::Get_tag_datum("objects\\characters\\floodcombat_elite\\floodcombat_elite_mp", blam_tag::tag_group_type::biped, "carto_shared");
			auto flood_arms_datum = tag_loader::Get_tag_datum("objects\\characters\\flood_mp\\fp_arms\\fp_arms", blam_tag::tag_group_type::rendermodel, "carto_shared");
			auto flood_body_datum = tag_loader::Get_tag_datum("objects\\characters\\flood_mp\\fp_body\\fp_body", blam_tag::tag_group_type::rendermodel, "carto_shared");
			if (!DATUM_IS_NONE(flood_datum) && !DATUM_IS_NONE(flood_arms_datum) && !DATUM_IS_NONE(flood_body_datum))
			{
				tag_loader::Load_tag(flood_datum, true, "carto_shared");
				tag_loader::Load_tag(flood_arms_datum, true, "carto_shared");
				tag_loader::Load_tag(flood_body_datum, true, "carto_shared");
				tag_loader::Push_Back();
				add_representation(tag_loader::ResolveNewDatum(flood_arms_datum), tag_loader::ResolveNewDatum(flood_body_datum), tag_loader::ResolveNewDatum(flood_datum), character_type_flood);
				auto new_def = MetaExtender::add_tag_block2<s_scenario_group_definition::s_simulation_definition_table_block>((unsigned long)std::addressof(scen->simulation_definition_table));
				new_def->tag = tag_loader::ResolveNewDatum(flood_datum);
			}
			else
			{
				clone_representation(3, character_type_flood);
			}

			// Create copy of default variant for chief and add lmao object to head
			auto mode_chief_mp_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief_mp");
			if (mode_chief_mp_datum != DATUM_INDEX_NONE)
			{
				auto mode_chief_mp = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(mode_chief_mp_datum);
				auto base_variant = mode_chief_mp->variants[0];
				auto new_variant = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block>((unsigned long)std::addressof(mode_chief_mp->variants));
				new_variant->name = 0xABABABA;
				new_variant->dialogue.TagGroup = base_variant->dialogue.TagGroup;
				new_variant->dialogue.TagIndex = base_variant->dialogue.TagIndex;
				memcpy(new_variant->runtime_model_regions, base_variant->runtime_model_regions, sizeof(new_variant->runtime_model_regions));
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
						memcpy(new_permutation->runtime_permutations, permutation->runtime_permutations, sizeof(new_permutation->runtime_permutations));
						new_permutation->unk_1 = permutation->unk_1;
						new_permutation->unk2 = permutation->unk2;
						new_permutation->unk3 = permutation->unk3;
					}
				}

				datum lmao_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\emoji_head\\emoji_head", blam_tag::tag_group_type::scenery, "carto_shared");
				if (!DATUM_IS_NONE(lmao_datum))
				{
					tag_loader::Load_tag(lmao_datum, true, "carto_shared");
					tag_loader::Push_Back();

					lmao_datum = tag_loader::ResolveNewDatum(lmao_datum);
					if (!DATUM_IS_NONE(lmao_datum))
					{
						auto new_object = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(new_variant->objects));
						new_object->parent_marker = e_global_string_id::HS_HEAD;
						new_object->child_object.TagGroup = blam_tag::tag_group_type::scenery;
						new_object->child_object.TagIndex = lmao_datum;
					}
				}
				add_representation(-1, -1, -1, character_type_lmao, new_variant->name);
			}
		}
	}
	void ApplyHooks()
	{
		p_player_properties_validate_configuration = Memory::GetAddress<player_properties_validate_configuration_t*>(0x52F23, 0x5B41B);
		PatchCall(Memory::GetAddress(0x5509E, 0x5D596), player_properties_validate_configuration_hook);
		
		p_player_representation_get = Memory::GetAddress<void*>(0x53895, 0x5BD8D);
		PatchCall(Memory::GetAddress(0x53969, 0x5BE61), player_representation_get_to_cdecl);
		PatchCall(Memory::GetAddress(0x559F9, 0x5DEF1), player_representation_get_to_cdecl);

		// Change the packet validation for player::properties::profile to just accept anything, we catch it later if it's outside of the acceptable range.
		WriteValue<BYTE>(Memory::GetAddress(0x54fb3, 0x5D4AB), 25);
	}

	void Initialize()
	{
		ApplyHooks();
		tags::on_map_load(OnMapLoad);
	}

}
