#include "stdafx.h"
#include "game_globals.h"

#include "Blam/Engine/units/unit_definitions.h"

#include "Blam/Engine/models/models.h"
#include "Blam/Engine/scenario/scenario.h"
#include "Blam/Engine/tag_files/global_string_ids.h"
#include "Blam/Engine/cache/cache_files.h"

#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/SpecialEvents/SpecialEvents.h"
#include "H2MOD/Tags/MetaExtender.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"
#include "H2MOD/Tags/TagInterface.h"

/**
	 * \brief Adds a new player representation to the globals tag block
	 * \param fp_hands the datum index of the first person hands render model
	 * \param fp_body the datum index of the first person body render model
	 * \param tp_biped the datum index of the third person biped
	 * \param variant the string_id of the variant
	 * \return returns a pointer to the new representation
*/
s_game_globals_player_representation* add_representation(datum fp_hands, datum fp_body, datum tp_biped, string_id variant = NONE);

/**
 * \brief Clones an existing player_representation
 * \param original_type: index of the representation to clone
 * \return returns a pointer to the new representation
 */
s_game_globals_player_representation* clone_representation(e_character_type original_type);

// Set the masterchief representation to the multiplayer version only in multiplayer
// This prevents server owners from forcing masterchief 
void game_globals_remove_singleplayer_representation(void);

void game_globals_add_skeleton_representation(scenario* scenario_definition);
void game_globals_add_flood_representation(scenario* scenario_definition);
void game_globals_add_lmao_representation(void);

// Adds new representations to the globals tag
void game_globals_add_new_player_representations(void);


s_game_globals* scenario_get_game_globals(void)
{
	return *Memory::GetAddress<s_game_globals**>(0x479E70, 0x4A642C);
}

s_ui_levels_definition* game_globals_get_ui_levels(void)
{
	s_game_globals* globals = scenario_get_game_globals();
	if (globals->ui_level_data.count > 0 && globals->ui_level_data.data != NONE)
	{
		return globals->ui_level_data[0];
	}

	return NULL;
}

s_game_globals_player_representation* game_globals_get_representation(e_character_type type)
{
	return scenario_get_game_globals()->player_representation[type];
}


s_game_globals_player_representation* add_representation(datum fp_hands, datum fp_body, datum tp_biped, string_id variant)
{
	s_game_globals* globals = scenario_get_game_globals();

	auto new_rep = MetaExtender::add_tag_block2<s_game_globals_player_representation>((unsigned long)std::addressof(globals->player_representation));
	if (fp_hands != NONE)
	{
		new_rep->first_person_hands.TagGroup.group = _tag_group_render_model;
		new_rep->first_person_hands.TagIndex = fp_hands;
	}
	else
	{
		new_rep->first_person_hands = globals->player_representation[_character_type_spartan]->first_person_hands;
	}

	if (fp_body != NONE)
	{
		new_rep->first_person_body.TagGroup.group = _tag_group_render_model;
		new_rep->first_person_body.TagIndex = fp_body;
	}
	else
		new_rep->first_person_body = globals->player_representation[_character_type_spartan]->first_person_body;

	if (tp_biped != NONE)
	{
		new_rep->third_person_unit.TagGroup.group = _tag_group_biped;
		new_rep->third_person_unit.TagIndex = tp_biped;
	}
	else
	{
		new_rep->third_person_unit = globals->player_representation[_character_type_spartan]->third_person_unit;
	}

	if (variant != NONE)
	{
		new_rep->third_person_variant = variant;
	}

	return new_rep;
}

s_game_globals_player_representation* clone_representation(e_character_type original_type)
{
	s_game_globals* globals = scenario_get_game_globals();
	s_game_globals_player_representation* original_rep = globals->player_representation[original_type];
	s_game_globals_player_representation* new_rep = MetaExtender::add_tag_block2<s_game_globals_player_representation>((unsigned long)std::addressof(globals->player_representation));
	new_rep->first_person_body = original_rep->first_person_body;
	new_rep->first_person_hands = original_rep->first_person_hands;
	new_rep->third_person_unit = original_rep->third_person_unit;
	new_rep->third_person_variant = original_rep->third_person_variant;
	return new_rep;
}

void game_globals_remove_singleplayer_representation(void)
{
	s_game_globals* globals = scenario_get_game_globals();

	s_game_globals_player_representation* masterchief_rep = globals->player_representation[_character_type_masterchief];
	s_game_globals_player_representation* spartan_rep = globals->player_representation[_character_type_spartan];

	masterchief_rep->third_person_unit = spartan_rep->third_person_unit;
	return;
}

void game_globals_add_skeleton_representation(scenario* scenario_definition)
{
	// Add skeleton
	datum skele_datum = tag_loader::Get_tag_datum("objects\\characters\\masterchief_skeleton\\masterchief_skeleton", _tag_group_biped, "carto_shared");
	datum skele_fp_datum = tag_loader::Get_tag_datum("objects\\characters\\masterchief_skeleton\\fp\\fp", _tag_group_render_model, "carto_shared");
	datum skele_body_datum = tag_loader::Get_tag_datum("objects\\characters\\masterchief_skeleton\\fp_body\\fp_body", _tag_group_render_model, "carto_shared");

	if (skele_datum != NONE && skele_fp_datum != NONE && skele_body_datum != NONE && get_current_special_event() == _special_event_halloween && !H2Config_no_events)
	{
		tag_loader::Load_tag(skele_fp_datum, true, "carto_shared");
		tag_loader::Load_tag(skele_body_datum, true, "carto_shared");
		tag_loader::Load_tag(skele_datum, true, "carto_shared");
		tag_loader::Push_Back();
		datum skele_new_datum = tag_loader::ResolveNewDatum(skele_datum);
		add_representation(tag_loader::ResolveNewDatum(skele_fp_datum), tag_loader::ResolveNewDatum(skele_body_datum), skele_new_datum);
		s_scenario_simulation_definition_table_element* new_def = MetaExtender::add_tag_block2<s_scenario_simulation_definition_table_element>((unsigned long)std::addressof(scenario_definition->simulation_definition_table));
		new_def->tag_datum = skele_new_datum;
	}
	else
	{
		clone_representation(_character_type_spartan);
	}
	return;
}

void game_globals_add_flood_representation(scenario* scenario_definition)
{
	datum flood_datum = tag_loader::Get_tag_datum("objects\\characters\\floodcombat_elite\\floodcombat_elite_mp", _tag_group_biped, "carto_shared");
	datum flood_arms_datum = tag_loader::Get_tag_datum("objects\\characters\\flood_mp\\fp_arms\\fp_arms", _tag_group_render_model, "carto_shared");
	datum flood_body_datum = tag_loader::Get_tag_datum("objects\\characters\\flood_mp\\fp_body\\fp_body", _tag_group_render_model, "carto_shared");
	if (flood_datum != NONE && flood_arms_datum != NONE && flood_body_datum != NONE)
	{
		tag_loader::Load_tag(flood_datum, true, "carto_shared");
		tag_loader::Load_tag(flood_arms_datum, true, "carto_shared");
		tag_loader::Load_tag(flood_body_datum, true, "carto_shared");
		tag_loader::Push_Back();
		datum new_flood_index = tag_loader::ResolveNewDatum(flood_datum);
		add_representation(tag_loader::ResolveNewDatum(flood_arms_datum), tag_loader::ResolveNewDatum(flood_body_datum), new_flood_index);
		s_scenario_simulation_definition_table_element* new_def = MetaExtender::add_tag_block2<s_scenario_simulation_definition_table_element>((unsigned long)std::addressof(scenario_definition->simulation_definition_table));
		new_def->tag_datum = new_flood_index;
	}
	else
	{
		clone_representation(_character_type_elite);
	}
	return;
}

void game_globals_add_lmao_representation(void)
{
	// Create copy of default variant for chief and add lmao object to head
	_unit_definition* mp_chief_unit = (_unit_definition*)tag_get_fast(game_globals_get_representation(_character_type_spartan)->third_person_unit.TagIndex);
	datum mode_chief_mp_datum =  mp_chief_unit->object.model.TagIndex;
	if (mode_chief_mp_datum != NONE)
	{
		// Copy the variant
		s_model_definition* mode_chief_mp = tags::get_tag<_tag_group_model, s_model_definition>(mode_chief_mp_datum);
		auto base_variant = mode_chief_mp->variants[0];
		auto new_variant = MetaExtender::add_tag_block2<s_model_variant>((unsigned long)std::addressof(mode_chief_mp->variants));
		new_variant->name = 0xABABABA;
		new_variant->dialogue.TagGroup = base_variant->dialogue.TagGroup;
		new_variant->dialogue.TagIndex = base_variant->dialogue.TagIndex;
		memcpy(new_variant->runtime_model_region_index, base_variant->runtime_model_region_index, sizeof(new_variant->runtime_model_region_index));
		for (auto i = 0; i < base_variant->regions.count; i++)
		{
			auto region = base_variant->regions[i];
			auto new_region = MetaExtender::add_tag_block2<s_model_variant_region>((unsigned long)std::addressof(new_variant->regions));
			new_region->region_name = region->region_name;
			new_region->runtime_model_region_index = region->runtime_model_region_index;
			new_region->runtime_flags = region->runtime_flags;
			new_region->parent_variant = region->parent_variant;
			new_region->sort_order = region->sort_order;
			for (auto k = 0; k < region->permutations.count; k++)
			{
				auto permutation = region->permutations[k];
				auto new_permutation = MetaExtender::add_tag_block2<s_model_variant_permutation>((unsigned long)std::addressof(new_region->permutations));
				new_permutation->permutation_name = permutation->permutation_name;
				new_permutation->runtime_model_permutation_index= permutation->runtime_model_permutation_index;
				new_permutation->flags = permutation->flags;
				new_permutation->probability = permutation->probability;
				memcpy(new_permutation->runtime_state_permutation_index, permutation->runtime_state_permutation_index, sizeof(new_permutation->runtime_state_permutation_index));
				memcpy(new_permutation->pad1, permutation->pad1, sizeof(new_permutation->pad1));
			}
		}

		// Add lmao head as an attachment on the new variant
		datum lmao_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\emoji_head\\emoji_head", _tag_group_scenery, "carto_shared");
		if (lmao_datum != NONE)
		{
			tag_loader::Load_tag(lmao_datum, true, "carto_shared");
			tag_loader::Push_Back();

			lmao_datum = tag_loader::ResolveNewDatum(lmao_datum);
			if (lmao_datum != NONE)
			{
				auto new_object = MetaExtender::add_tag_block2<s_model_variant_object>((unsigned long)std::addressof(new_variant->objects));
				new_object->parent_marker = e_global_string_id::HS_HEAD;
				new_object->child_object.TagGroup.group = _tag_group_scenery;
				new_object->child_object.TagIndex = lmao_datum;
			}
		}
		add_representation(NONE, NONE, NONE, new_variant->name);
	}
	else
	{
		clone_representation(_character_type_spartan);
	}

	return;
}

void game_globals_add_new_player_representations(void)
{
	scenario* scenario_definition = get_global_scenario();

	game_globals_add_skeleton_representation(scenario_definition);
	game_globals_add_flood_representation(scenario_definition);
	game_globals_add_lmao_representation();
	return;
}

void game_globals_apply_tag_patches(s_game_options* options)
{
	if (options->game_mode == _game_mode_multiplayer)
	{
		game_globals_add_new_player_representations();
		game_globals_remove_singleplayer_representation();
	}
	return;
}
