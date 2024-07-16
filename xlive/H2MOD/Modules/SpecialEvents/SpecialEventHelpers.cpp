#include "stdafx.h"
#include "SpecialEventHelpers.h"


#include "items/weapon_definitions.h"
#include "models/models.h"
#include "models/render_model_definitions.h"
#include "tag_files/global_string_ids.h"

#include "H2MOD/Tags/MetaExtender.h"

// Add new marker for elites
void add_special_event_markers()
{
	auto mode_elite_datum = tags::find_tag(_tag_group_render_model, "objects\\characters\\elite\\elite_mp");
	if (!DATUM_IS_NONE(mode_elite_datum))
	{
		auto mode_elite = tags::get_tag<_tag_group_render_model, render_model_definition>(mode_elite_datum);
		auto new_marker_group = MetaExtender::add_tag_block2<render_model_marker_group>((unsigned long)std::addressof(mode_elite->marker_groups));
		new_marker_group->name = new_elite_head_marker;
		render_model_marker* new_marker = MetaExtender::add_tag_block2<render_model_marker>((unsigned long)std::addressof(new_marker_group->markers));
		LOG_INFO_GAME("[{}] {:x}", __FUNCTION__, (unsigned long)std::addressof(new_marker));
		new_marker->node_index = 19;
		new_marker->permutation_index = NONE;
		new_marker->region_index = NONE;
		new_marker->position.x = 0.006f;
		new_marker->position.y = 0.06f;
		new_marker->position.z = 0.0f;
		new_marker->rotation.i = 0.9f;
		new_marker->rotation.j = 0.9f;
		new_marker->rotation.k = 0.9f;
		new_marker->rotation.w = 1.0f;
		new_marker->scale = 1.0f;
	}
}

void replace_fp_and_3p_models_from_weapon(datum weapon_datum, datum fp_model_datum, datum _3p_model_datum)
{
	auto weapon = tags::get_tag<_tag_group_weapon, _weapon_definition>(weapon_datum);
	weapon->item.object.sweetener_size = _sweetener_size_medium;
	weapon->player_interface.first_person[0]->model.index = fp_model_datum;
	weapon->player_interface.first_person[1]->model.index = fp_model_datum;

	datum model_datum = weapon->item.object.model.index;
	tags::get_tag<_tag_group_model, s_model_definition>(model_datum)->render_model.index = _3p_model_datum;

}

void add_hat_to_model(datum player_hlmt_datum, datum hat_scenery_datum, bool is_elite)
{
	auto hlmt = tags::get_tag_fast<s_model_definition>(player_hlmt_datum);
	auto variant = hlmt->variants[0];
	auto hat = MetaExtender::add_tag_block2<s_model_variant_object>((unsigned long)std::addressof(variant->objects));
	hat->parent_marker = (is_elite == false ? string_id(e_global_string_id::HS_HEAD) : new_elite_head_marker);
	hat->child_object.group.group = _tag_group_scenery;
	hat->child_object.index = hat_scenery_datum;
}

void add_hat_and_beard_to_model(datum player_hlmt_datum, datum hat_scenery_datum, datum beard_scenery_datum, bool is_elite)
{
	add_hat_to_model(player_hlmt_datum, hat_scenery_datum, is_elite);

	auto hlmt = tags::get_tag_fast<s_model_definition>(player_hlmt_datum);
	auto variant = hlmt->variants[0];
	auto beard = MetaExtender::add_tag_block2<s_model_variant_object>((unsigned long)std::addressof(variant->objects));
	beard->parent_marker = (is_elite == false ? string_id(e_global_string_id::HS_HEAD) : new_elite_head_marker);
	beard->child_object.group.group = _tag_group_scenery;
	beard->child_object.index = beard_scenery_datum;
}
