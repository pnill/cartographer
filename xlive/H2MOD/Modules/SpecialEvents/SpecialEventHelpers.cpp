#include "stdafx.h"
#include "SpecialEventHelpers.h"

#include "cache/cache_files.h"
#include "items/weapon_definitions.h"
#include "models/models.h"
#include "models/model_definitions.h"
#include "models/render_model_definitions.h"

#include "tag_files/tag_loader/tag_injection.h"

// Add new marker for elites
void add_special_event_markers(void)
{
	datum mode_elite_datum = tag_loaded(_tag_group_render_model, "objects\\characters\\elite\\elite_mp");
	if (mode_elite_datum != NONE)
	{
		render_model_definition* mode_elite = render_model_definition_get(mode_elite_datum);
		render_model_marker_group* new_marker_group = (render_model_marker_group*)tag_injection_extend_block(&mode_elite->marker_groups, mode_elite->marker_groups.type_size(), 1);
			
		new_marker_group->name = new_elite_head_marker;
		
		render_model_marker* new_marker = (render_model_marker*)tag_injection_extend_block(&new_marker_group->markers, new_marker_group->markers.type_size(), 1);
		new_marker->node_index = 19;
		new_marker->permutation_index = (uint8)NONE;
		new_marker->region_index = (uint8)NONE;
		new_marker->position.x = 0.006f;
		new_marker->position.y = 0.06f;
		new_marker->position.z = 0.0f;
		new_marker->rotation.v.i = 0.9f;
		new_marker->rotation.v.j = 0.9f;
		new_marker->rotation.v.k = 0.9f;
		new_marker->rotation.w = 1.0f;
		new_marker->scale = 1.0f;
	}
	return;
}

void replace_fp_and_3p_models_from_weapon(datum weapon_datum, datum fp_model_datum, datum _3p_model_datum)
{
	weapon_definition* weapon = weapon_definition_get(weapon_datum);
	weapon->object.sweetener_size = _sweetener_size_medium;
	weapon->weapon.player_interface.first_person[0]->model.index = fp_model_datum;
	weapon->weapon.player_interface.first_person[1]->model.index = fp_model_datum;

	datum model_datum = weapon->object.model.index;
	s_model_definition* model = model_definition_get(model_datum);
	model->render_model.index = _3p_model_datum;
	return;
}

void add_hat_and_beard_to_model(
	datum player_hlmt_datum,
	datum hat_scenery_datum,
	datum beard_scenery_datum,
	bool is_elite)
{
	uint32 block_count = 0;

	if (hat_scenery_datum != NONE)
		++block_count;
	if (beard_scenery_datum != NONE)
		++block_count;

	s_model_definition* model = model_definition_get(player_hlmt_datum);
	s_model_variant* variant = TAG_BLOCK_GET_ELEMENT(&model->variants, 0, s_model_variant);

	s_model_variant_object* blocks = (s_model_variant_object*)tag_injection_extend_block(&variant->objects, variant->objects.type_size(), block_count);

	block_count = 0;

	if (hat_scenery_datum != NONE)
	{
		
		blocks[block_count].parent_marker = (is_elite == false ? string_id(_string_id_head) : new_elite_head_marker);
		blocks[block_count].child_object.group = _tag_group_scenery;
		blocks[block_count].child_object.index = hat_scenery_datum;
		++block_count;
	}

	if (beard_scenery_datum != NONE)
	{
		blocks[block_count].parent_marker = (is_elite == false ? string_id(_string_id_head) : new_elite_head_marker);
		blocks[block_count].child_object.group = _tag_group_scenery;
		blocks[block_count].child_object.index = beard_scenery_datum;
		++block_count;
	}

	return;
}
