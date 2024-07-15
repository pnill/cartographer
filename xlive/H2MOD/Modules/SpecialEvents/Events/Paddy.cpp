#include "stdafx.h"

#include "Paddy.h"
#include "../SpecialEventHelpers.h"

#include "Blam/Cache/TagGroups/scenery_definition.hpp"

#include "items/weapon_definitions.h"
#include "models/models.h"
#include "models/render_model_definitions.h"

#include "H2MOD/Tags/MetaExtender.h"
#include "tag_files/tag_loader/tag_injection.h"

void paddy_event_map_load()
{
	tag_injection_set_active_map(k_events_map);

	datum paddy_hat_datum = tag_injection_load(_tag_group_scenery, "objects\\multi\\stpat_hat\\stpat_hat", true);
	datum paddy_beard_datum = tag_injection_load(_tag_group_scenery, "objects\\multi\\stpat_hat\\beard\\beard", true);
	datum paddy_pot_datum = tag_injection_load(_tag_group_scenery, "scenarios\\objects\\multi\\carto_shared\\pot_of_gold\\pot_of_gold", true);

	if (paddy_hat_datum != NONE && paddy_beard_datum != NONE && paddy_pot_datum != NONE)
	{
		tag_injection_inject();

		auto paddy_pot = tags::get_tag<_tag_group_scenery, s_scenery_group_definition>(paddy_pot_datum, true);
		auto paddy_pot_model = tags::get_tag<_tag_group_model, s_model_definition>(paddy_pot->objectTag.model.index, true);

		// Give Hat and Beard to Masterchief & Friends
		if (datum hlmt_chief_datum = tags::find_tag(_tag_group_model, "objects\\characters\\masterchief\\masterchief");
			hlmt_chief_datum != NONE) 
		{
			add_hat_and_beard_to_model(hlmt_chief_datum, paddy_hat_datum, paddy_beard_datum);
		}
		if (datum hlmt_chief_mp_datum = tags::find_tag(_tag_group_model, "objects\\characters\\masterchief\\masterchief_mp"); hlmt_chief_mp_datum != NONE) 
		{
			add_hat_and_beard_to_model(hlmt_chief_mp_datum, paddy_hat_datum, paddy_beard_datum);
		}
		if (datum hlmt_elite_datum = tags::find_tag(_tag_group_model, "objects\\characters\\elite\\elite_mp"); hlmt_elite_datum != NONE)
		{
			add_hat_and_beard_to_model(hlmt_elite_datum, paddy_hat_datum, paddy_beard_datum, true);
		}

		// Replace ball and bomb with the pot of gold
		if (datum ball_weapon_datum = tags::find_tag(_tag_group_weapon, "objects\\weapons\\multiplayer\\ball\\ball"); 
			ball_weapon_datum != NONE)
		{
			auto ball_weapon = tags::get_tag<_tag_group_weapon, _weapon_definition>(ball_weapon_datum);
			auto ball_model = tags::get_tag<_tag_group_model, s_model_definition>(ball_weapon->item.object.model.index);

			ball_model->render_model.index = paddy_pot_model->render_model.index;

			//Bounding Radius and Sweetener size
			ball_weapon->item.object.bounding_radius = 0.3f;
			ball_weapon->item.object.sweetener_size = _sweetener_size_medium;
		}
		if (datum bomb_weapon_datum = tags::find_tag(_tag_group_weapon, "objects\\weapons\\multiplayer\\assault_bomb\\assault_bomb");
			bomb_weapon_datum != NONE)
		{
			auto bomb_weapon = tags::get_tag<_tag_group_weapon, _weapon_definition>(bomb_weapon_datum);
			auto bomb_model = tags::get_tag<_tag_group_model, s_model_definition>(bomb_weapon->item.object.model.index);

			bomb_model->render_model.index = paddy_pot_model->render_model.index;
			auto paddy_pot_render = tags::get_tag<_tag_group_render_model, render_model_definition>(paddy_pot_model->render_model.index, true);
			auto pot_node = paddy_pot_render->nodes[0];
			pot_node->default_rotation.v.k = -0.75f;
			pot_node->default_translation.y = 0.07f;
			pot_node->default_translation.z = -0.1f;

			// Bounding Radius and Sweetener size
			bomb_weapon->item.object.bounding_radius = 0.3f;
			bomb_weapon->item.object.sweetener_size = _sweetener_size_medium;
		}
	}
}
