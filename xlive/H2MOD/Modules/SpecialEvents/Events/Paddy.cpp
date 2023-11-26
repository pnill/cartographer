#include "stdafx.h"

#include "Paddy.h"
#include "../SpecialEventHelpers.h"

#include "Blam/Cache/TagGroups/scenery_definition.hpp"

#include "Blam/Engine/game/game_globals.h"
#include "Blam/Engine/items/weapon_definitions.h"
#include "Blam/Engine/models/models.h"
#include "Blam/Engine/models/render_model_definitions.h"

#include "H2MOD/Tags/MetaExtender.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"

void paddy_event_map_load()
{
	datum paddy_hat_datum = tag_loader::Get_tag_datum("objects\\multi\\stpat_hat\\stpat_hat", blam_tag::tag_group_type::scenery, "carto_shared");
	datum paddy_beard_datum = tag_loader::Get_tag_datum("objects\\multi\\stpat_hat\\beard\\beard", blam_tag::tag_group_type::scenery, "carto_shared");
	datum paddy_pot_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\pot_of_gold\\pot_of_gold", blam_tag::tag_group_type::scenery, "carto_shared");

	if (!DATUM_IS_NONE(paddy_hat_datum) && !DATUM_IS_NONE(paddy_beard_datum) && !DATUM_IS_NONE(paddy_pot_datum))
	{
		tag_loader::Load_tag(paddy_hat_datum, true, "carto_shared");
		tag_loader::Load_tag(paddy_beard_datum, true, "carto_shared");
		tag_loader::Load_tag(paddy_pot_datum, true, "carto_shared");
		tag_loader::Push_Back();

		paddy_hat_datum = tag_loader::ResolveNewDatum(paddy_hat_datum);
		paddy_beard_datum = tag_loader::ResolveNewDatum(paddy_beard_datum);
		paddy_pot_datum = tag_loader::ResolveNewDatum(paddy_pot_datum);

		auto paddy_pot = tags::get_tag<blam_tag::tag_group_type::scenery, s_scenery_group_definition>(paddy_pot_datum, true);
		auto paddy_pot_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_definition>(paddy_pot->objectTag.model.TagIndex, true);

		// Give Hat and Beard to Masterchief & Friends
		if (datum hlmt_chief_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief");
			hlmt_chief_datum != DATUM_INDEX_NONE) 
		{
			add_hat_and_beard_to_model(hlmt_chief_datum, paddy_hat_datum, paddy_beard_datum);
		}
		if (datum hlmt_chief_mp_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief_mp"); hlmt_chief_mp_datum != DATUM_INDEX_NONE) 
		{
			add_hat_and_beard_to_model(hlmt_chief_mp_datum, paddy_hat_datum, paddy_beard_datum);
		}
		if (datum hlmt_elite_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\elite\\elite_mp"); hlmt_elite_datum != DATUM_INDEX_NONE)
		{
			add_hat_and_beard_to_model(hlmt_elite_datum, paddy_hat_datum, paddy_beard_datum, true);
		}

		// Replace ball and bomb with the pot of gold
		if (datum ball_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\ball\\ball"); 
			ball_weapon_datum != DATUM_INDEX_NONE)
		{
			auto ball_weapon = tags::get_tag<blam_tag::tag_group_type::weapon, _weapon_definition>(ball_weapon_datum);
			auto ball_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_definition>(ball_weapon->item.object.model.TagIndex);

			ball_model->render_model.TagIndex = paddy_pot_model->render_model.TagIndex;

			//Bounding Radius and Sweetener size
			ball_weapon->item.object.bounding_radius = 0.3f;
			ball_weapon->item.object.sweetener_size = _sweetener_size_medium;
		}
		if (datum bomb_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\assault_bomb\\assault_bomb");
			bomb_weapon_datum != DATUM_INDEX_NONE)
		{
			auto bomb_weapon = tags::get_tag<blam_tag::tag_group_type::weapon, _weapon_definition>(bomb_weapon_datum);
			auto bomb_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_definition>(bomb_weapon->item.object.model.TagIndex);

			bomb_model->render_model.TagIndex = paddy_pot_model->render_model.TagIndex;
			auto paddy_pot_render = tags::get_tag<blam_tag::tag_group_type::rendermodel, render_model_definition>(paddy_pot_model->render_model.TagIndex, true);
			auto pot_node = paddy_pot_render->nodes[0];
			pot_node->default_rotation.k = -0.75f;
			pot_node->default_translation.y = 0.07f;
			pot_node->default_translation.z = -0.1f;

			// Bounding Radius and Sweetener size
			bomb_weapon->item.object.bounding_radius = 0.3f;
			bomb_weapon->item.object.sweetener_size = _sweetener_size_medium;
		}
	}
}
