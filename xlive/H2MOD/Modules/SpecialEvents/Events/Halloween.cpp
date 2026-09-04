#include "stdafx.h"
#include "Halloween.h"

#include "MapObjectPlacements/Halloween/Coagulation.h"
#include "MapObjectPlacements/Halloween/Lockout.h"


#include "cache/cache_files.h"
#include "models/models.h"
#include "models/model_definitions.h"
#include "objects/objects.h"
#include "objects/scenery.h"
#include "networking/network_game_definitions.h"
#include "scenario/scenario.h"
#include "scenario/scenario_definitions.h"
#include "structures/structure_lightmap.h"
#include "structures/structure_bsp_definitions.h"
#include "tag_files/tag_loader/tag_injection.h"

#include "H2MOD/Modules/EventHandler/EventHandler.hpp"

/* prototypes */

static void halloween_game_life_cycle_update(e_life_cycle_state state);

/* globals */

static datum lbitm_datum = NONE;
static datum sky_datum = NONE;
static datum candle_datum = NONE;
static datum candle_fire_datum = NONE;
static datum large_candle_datum = NONE;
static datum pump_datum = NONE;

/* public code */

void halloween_event_map_load(void)
{
	// Load specific tags from shared and modify placements depending on the map being played

	const cache_file_header* cache_header = cache_files_get_header();

	tag_injection_set_active_map(k_events_map);

	if(!strcmp(cache_header->name, "coagulation"))
	{
		lbitm_datum = tag_injection_load(_tag_group_bitmap, "scenarios\\multi\\halo\\coagulation\\coagulation_coagulation_lightmap_truecolor_bitmaps", true);
		sky_datum = tag_injection_load(_tag_group_sky, "scenarios\\skies\\multi\\halo\\coagulation\\coagulation_night", true);
		candle_fire_datum = tag_injection_load(_tag_group_scenery, "scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\candle\\candle_fire", true);
		candle_datum = tag_injection_load(_tag_group_scenery, "scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\candle\\candle", true);
		pump_datum = tag_injection_load(_tag_group_scenery, "scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\jack_o_lantern", true);
		large_candle_datum = tag_injection_load(_tag_group_scenery, "scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\candle\\candle_big_light", true);

		tag_injection_inject();

		// OG Halo 2 Coag lightmap
		datum ltmp_datum = tag_loaded(_tag_group_scenario_structure_lightmap,
			"scenarios\\multi\\halo\\coagulation\\coagulation_coagulation_lightmap");

		if(tag_injection_is_injected(lbitm_datum) && ltmp_datum != NONE)
		{
			structure_lightmap* ltmp = structure_lightmap_definition_get(ltmp_datum);
			ltmp->lightmap_groups[0]->bitmap_group.index = lbitm_datum;
		}

		scenario const* scenario_definition = global_scenario_get();
		scenario_structure_bsp_reference const* reference = TAG_BLOCK_GET_ELEMENT(&scenario_definition->structure_bsp_references, 0, scenario_structure_bsp_reference);
		structure_bsp* bsp_definition = structure_bsp_definition_get(reference->structure_bsp.index);

		if (tag_injection_is_injected(sky_datum))
		{
			TAG_BLOCK_GET_ELEMENT(&scenario_definition->skies, 0, tag_reference)->index = sky_datum;
		}

		if (ltmp_datum != NONE && lbitm_datum != NONE)
		{
			structure_lightmap* ltmp = structure_lightmap_definition_get(ltmp_datum);
			ltmp->lightmap_groups[0]->bitmap_group.index = lbitm_datum;

			// Null out decorator block since the colour for them is separate from the lightmap colour
			bsp_definition->decorator_placement.count = 0;
			bsp_definition->decorator_placement.data = 0;
		}

		if(tag_injection_is_injected(candle_datum) && tag_injection_is_injected(pump_datum) && tag_injection_is_injected(large_candle_datum))
		{
			EventHandler::register_callback(halloween_game_life_cycle_update, EventType::gamelifecycle_change, EventExecutionType::execute_after, true);
			// We execute this after a bluescreen since our new objects arent recreated automatically
			EventHandler::register_callback(halloween_game_life_cycle_update, EventType::blue_screen, EventExecutionType::execute_after, true);
		}
	}
	if (!strcmp(cache_header->name, "lockout"))
	{
		candle_fire_datum = tag_injection_load(_tag_group_scenery, "scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\candle\\candle_fire", true);
		candle_datum = tag_injection_load(_tag_group_scenery, "scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\candle\\candle", true);
		pump_datum = tag_injection_load(_tag_group_scenery, "scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\jack_o_lantern", true);
		large_candle_datum = tag_injection_load(_tag_group_scenery, "scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\candle\\candle_big_light", true);

		tag_injection_inject();

		if (candle_datum != NONE && pump_datum != NONE)
		{
			EventHandler::register_callback(halloween_game_life_cycle_update, EventType::gamelifecycle_change, EventExecutionType::execute_after, true);
			// We execute this after a bluescreen since our new objects arent recreated automatically
			EventHandler::register_callback(halloween_game_life_cycle_update, EventType::blue_screen, EventExecutionType::execute_after, true);
		}
	}
	return;
}

/* private code */

static void halloween_game_life_cycle_update(e_life_cycle_state state)
{
	if (state == _life_cycle_state_in_game)
	{
		object_placement_data placement;

		scenery_definition* pump = scenery_definition_get(pump_datum);
		s_model_definition* pump_hmlt = model_definition_get(pump->object.model.index);

		const cache_file_header* cache_header = cache_files_get_header();
		if (!strcmp(cache_header->name, "coagulation"))
		{
			for (auto& scen_place : k_coag_scen_places)
			{
				// Set type of object and variant
				switch (scen_place.type)
				{
				case 0:
					object_placement_data_new(&placement, pump_datum, -1, 0);
					placement.variant_name = TAG_BLOCK_GET_ELEMENT(&pump_hmlt->variants, scen_place.variant_id, s_model_variant)->name;
					break;
				case 1:
					object_placement_data_new(&placement, candle_datum, -1, 0);
					placement.variant_name = _string_id_empty_string;
					break;
				case 2:
					object_placement_data_new(&placement, large_candle_datum, -1, 0);
					placement.variant_name = _string_id_empty_string;
					break;
				}
				// Set location orientation and scale
				placement.position = scen_place.position;
				vectors3d_from_euler_angles3d(&placement.forward, &placement.up, &scen_place.rotation);
				placement.scale = scen_place.scale;

				// Create the new object
				object_new(&placement);
			}
		}
		else if (!strcmp(cache_header->name, "lockout"))
		{
			for (auto& scen_place : k_lockout_scen_places)
			{
				// Set type of object and variant
				switch (scen_place.type)
				{
				case 0:
					object_placement_data_new(&placement, pump_datum, NONE, 0);
					placement.variant_name = TAG_BLOCK_GET_ELEMENT(&pump_hmlt->variants, scen_place.variant_id, s_model_variant)->name;
					break;
				case 1:
					object_placement_data_new(&placement, candle_datum, NONE, 0);
					placement.variant_name = _string_id_empty_string;
					break;
				}
				// Set location orientation and scale
				placement.position = scen_place.position;
				vectors3d_from_euler_angles3d(&placement.forward, &placement.up, &scen_place.rotation);
				placement.scale = scen_place.scale;

				// Create the new object
				object_new(&placement);
			}
		}
	}
	return;
}
