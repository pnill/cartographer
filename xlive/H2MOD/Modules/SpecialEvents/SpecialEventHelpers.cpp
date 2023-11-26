#include "stdafx.h"
#include "SpecialEventHelpers.h"


#include "Blam/Engine/items/weapon_definitions.h"
#include "Blam/Engine/models/models.h"
#include "Blam/Engine/models/render_model_definitions.h"
#include "Blam/Engine/tag_files/global_string_ids.h"

#include "H2MOD/Tags/MetaExtender.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"

// Add new marker for elites
void add_special_event_markers()
{
	auto mode_elite_datum = tags::find_tag(blam_tag::tag_group_type::rendermodel, "objects\\characters\\elite\\elite_mp");
	if (!DATUM_IS_NONE(mode_elite_datum))
	{
		auto mode_elite = tags::get_tag<blam_tag::tag_group_type::rendermodel, render_model_definition>(mode_elite_datum);
		auto new_marker_group = MetaExtender::add_tag_block2<render_model_marker_group>((unsigned long)std::addressof(mode_elite->marker_groups));
		new_marker_group->name = new_elite_head_marker;
		render_model_marker* new_marker = MetaExtender::add_tag_block2<render_model_marker>((unsigned long)std::addressof(new_marker_group->markers));
		LOG_INFO_GAME("[{}] {:x}", __FUNCTION__, (unsigned long)std::addressof(new_marker));
		new_marker->node_index = 19;
		new_marker->permutation_index = NONE;
		new_marker->region_index = NONE;
		new_marker->orientation.position.x = 0.006f;
		new_marker->orientation.position.y = 0.06f;
		new_marker->orientation.position.z = 0.0f;
		new_marker->orientation.quaternion.i= 0.9f;
		new_marker->orientation.quaternion.j = 0.9f;
		new_marker->orientation.quaternion.k = 0.9f;
		new_marker->orientation.quaternion.w = 1.0f;
		new_marker->orientation.scale = 1.0f;
	}
}

void replace_fp_and_3p_models_from_weapon(datum weapon_datum, datum fp_model_datum, datum _3p_model_datum)
{
	auto weapon = tags::get_tag<blam_tag::tag_group_type::weapon, _weapon_definition>(weapon_datum);
	weapon->item.object.sweetener_size = _sweetener_size_medium;
	weapon->player_interface.first_person[0]->first_person_model.TagIndex = fp_model_datum;
	weapon->player_interface.first_person[1]->first_person_model.TagIndex = fp_model_datum;

	datum model_datum = weapon->item.object.model.TagIndex;
	tags::get_tag<blam_tag::tag_group_type::model, s_model_definition>(model_datum)->render_model.TagIndex = _3p_model_datum;

}

void add_hat_to_model(datum player_hlmt_datum, datum hat_scenery_datum, bool is_elite)
{
	auto hlmt = tags::get_tag_fast<s_model_definition>(player_hlmt_datum);
	auto variant = hlmt->variants[0];
	auto hat = MetaExtender::add_tag_block2<s_model_variant_object>((unsigned long)std::addressof(variant->objects));
	hat->parent_marker = (is_elite == false ? string_id(e_global_string_id::HS_HEAD) : new_elite_head_marker);
	hat->child_object.TagGroup = blam_tag::tag_group_type::scenery;
	hat->child_object.TagIndex = hat_scenery_datum;
}

void add_hat_and_beard_to_model(datum player_hlmt_datum, datum hat_scenery_datum, datum beard_scenery_datum, bool is_elite)
{
	add_hat_to_model(player_hlmt_datum, hat_scenery_datum, is_elite);

	auto hlmt = tags::get_tag_fast<s_model_definition>(player_hlmt_datum);
	auto variant = hlmt->variants[0];
	auto beard = MetaExtender::add_tag_block2<s_model_variant_object>((unsigned long)std::addressof(variant->objects));
	beard->parent_marker = (is_elite == false ? string_id(e_global_string_id::HS_HEAD) : new_elite_head_marker);
	beard->child_object.TagGroup = blam_tag::tag_group_type::scenery;
	beard->child_object.TagIndex = beard_scenery_datum;
}
