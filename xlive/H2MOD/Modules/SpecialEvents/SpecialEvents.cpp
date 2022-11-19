#include "stdafx.h"

#include "SpecialEvents.h"
#include "MapObjectPlacements\Halloween\Coagulation.h"
#include "MapObjectPlacements\Halloween\Lockout.h"

#include "Blam\Cache\TagGroups\biped_definition.hpp"
#include "Blam\Cache\TagGroups\model_definition.hpp"
#include "Blam\Cache\TagGroups\render_model_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_lightmap_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_structure_bsp_definition.hpp"
#include "Blam\Cache\TagGroups\scenery_definition.hpp"
#include "Blam\Cache\TagGroups\weapon_definition.hpp"
#include "Blam\Engine\Networking\NetworkMessageTypeCollection.h"
#include "Blam/Engine/Game/tag_files/string_ids.h"
#include "H2MOD\Engine\Engine.h"
#include "H2MOD\GUI\ImGui_Integration\ImGui_Handler.h"
#include "H2MOD\Modules\EventHandler\EventHandler.hpp"
#include "H2MOD\Modules\PlayerRepresentation\PlayerRepresentation.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD\Tags\MetaExtender.h"
#include "H2MOD\Tags\MetaLoader\tag_loader.h"

namespace SpecialEvents
{
	namespace
	{
		datum lbitm_datum;
		datum sky_datum;
		datum candle_datum;
		datum candle_fire_datum;
		datum large_candle_datum;
		datum pump_datum;
		string_id new_elite_head_marker(0xFEE01234);

		std::time_t getEpochTime(int year, const std::wstring& dateTime)
		{
			// Let's consider we are getting all the input in
			// this format: '2014-07-25T20:17:22Z' (T denotes
			// start of Time part, Z denotes UTC zone).
			// A better approach would be to pass in the format as well.
			static const std::wstring dateTimeFormat{ L"%Y-%m-%dT%H:%M:%SZ" };

			// Create a stream which we will use to parse the string,
			// which we provide to constructor of stream to fill the buffer.
			std::wstring tmp = std::to_wstring(year + 1900);
			tmp.append(L"-");
			tmp.append(dateTime);
			std::wistringstream ss{ tmp };

			// Create a tm object to store the parsed date and time.
			std::tm dt;

			// Now we read from buffer using get_time manipulator
			// and formatting the input appropriately.
			ss >> std::get_time(&dt, dateTimeFormat.c_str());

			// Convert the tm structure to time_t value and return.
			return std::mktime(&dt);
		}
		//This function checks if the current date passed is in the current week.
		bool CheckEventWeek(std::wstring date)
		{
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			time_t tt = std::chrono::system_clock::to_time_t(now);
			tm utc_tm = *gmtime(&tt);

			time_t pat = getEpochTime(utc_tm.tm_year, date.append(L"T00:00:00Z"));
			tm utc_pat = *gmtime(&pat);

			int a, b = 0;
			a = utc_tm.tm_yday - utc_tm.tm_wday;
			b = utc_pat.tm_yday - utc_pat.tm_wday;
			return a == b;
		}
		//This function checks if the current date matches the one passed.
		bool CheckEventDate(std::wstring date)
		{
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			time_t tt = std::chrono::system_clock::to_time_t(now);
			tm utc_tm = *gmtime(&tt);

			time_t pat = getEpochTime(utc_tm.tm_year, date.append(L"T00:00:00Z"));
			tm utc_pat = *gmtime(&pat);

			return utc_tm.tm_yday == utc_pat.tm_yday;
		}
	}

	e_event_type getCurrentEvent()
	{
		if (H2Config_no_events)
			return _no_event;

#ifndef NDEBUG
		if (H2Config_forced_event != _no_event)
			return (e_event_type)H2Config_forced_event;
#endif
		if (CheckEventWeek(L"3-17"))
			return _st_paddys;

		if (CheckEventWeek(L"12-24") || CheckEventWeek(L"12-30") || CheckEventWeek(L"1-4"))
			return _christmas;

		// One time event
		/*if (CheckIfEventTime(L"4-12"))
			return _mook_maddness;*/

		if (CheckEventWeek(L"10-20") || CheckEventWeek(L"10-27") || CheckEventDate(L"10-31"))
			return _halloween;

		if (CheckEventDate(L"11-08") || CheckEventDate(L"11-09") || CheckEventDate(L"11-10"))
			return _birthday;

		return _no_event;
	}

	void ReplaceFirstAndThirdPersonModelFromWeapon(datum weapon_datum, datum fp_model_datum, datum _3p_model_datum)
	{
		auto weapon = tags::get_tag<blam_tag::tag_group_type::weapon, s_weapon_group_definition>(weapon_datum);
		weapon->first_person[0]->first_person_model.TagIndex = fp_model_datum;
		weapon->first_person[1]->first_person_model.TagIndex = fp_model_datum;

		datum model_datum = weapon->model.TagIndex;
		auto model = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(model_datum);
		model->render_model.TagIndex = _3p_model_datum;

	}

	void AddHat(datum player_hlmt_datum, datum hat_scenery_datum, bool is_elite = false)
	{
		auto hlmt = tags::get_tag_fast<s_model_group_definition>(player_hlmt_datum);
		auto variant = hlmt->variants[0];
		auto hat = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(variant->objects));
		hat->parent_marker = (is_elite == false ? string_id(e_global_string_ids::HS_HEAD) : new_elite_head_marker);
		hat->child_object.TagGroup = blam_tag::tag_group_type::scenery;
		hat->child_object.TagIndex = hat_scenery_datum;
	}

	void AddHatAndBeard(datum player_hlmt_datum, datum hat_scenery_datum, datum beard_scenery_datum, bool is_elite = false)
	{
		AddHat(player_hlmt_datum, hat_scenery_datum, is_elite);

		auto hlmt = tags::get_tag_fast<s_model_group_definition>(player_hlmt_datum);
		auto variant = hlmt->variants[0];
		auto beard = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(variant->objects));
		beard->parent_marker = (is_elite == false ? string_id(e_global_string_ids::HS_HEAD) : new_elite_head_marker);
		beard->child_object.TagGroup = blam_tag::tag_group_type::scenery;
		beard->child_object.TagIndex = beard_scenery_datum;
	}

	void ChristmasOnMapLoad()
	{
		if (s_game_globals::game_is_multiplayer())
		{
			// Halo 2 tags
			datum sword_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\melee\\energy_blade\\energy_blade");
			datum ghost_datum = tags::find_tag(blam_tag::tag_group_type::vehicle, "objects\\vehicles\\ghost\\ghost");
			datum frag_model_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\weapons\\grenade\\frag_grenade\\frag_grenade_projectile");
			datum plasma_model_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\weapons\\grenade\\plasma_grenade\\plasma_grenade");
			datum ball_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\ball\\ball");
			datum bomb_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\assault_bomb\\assault_bomb");

			// Carto Shared tags
			datum santa_hat_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\christmas_hat_map\\hat\\hat", blam_tag::tag_group_type::scenery, "carto_shared");
			datum beard_datum = tag_loader::Get_tag_datum("objects\\multi\\stpat_hat\\beard\\santa_beard", blam_tag::tag_group_type::scenery, "carto_shared");
			datum snow_datum = tag_loader::Get_tag_datum("scenarios\\multi\\lockout\\lockout_big", blam_tag::tag_group_type::weathersystem, "carto_shared");
			datum candy_cane_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\candy_cane\\candy_cane", blam_tag::tag_group_type::rendermodel, "carto_shared");
			datum deer_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\reindeer_ghost\\reindeer_ghost", blam_tag::tag_group_type::rendermodel, "carto_shared");
			datum ornament_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\ornament\\ornament", blam_tag::tag_group_type::rendermodel, "carto_shared");
			datum present_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\present\\present", blam_tag::tag_group_type::rendermodel, "carto_shared");
			datum fp_present_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\present\\fp_present", blam_tag::tag_group_type::rendermodel, "carto_shared");

			if (!DATUM_IS_NONE(santa_hat_datum) && !DATUM_IS_NONE(beard_datum))
			{
				tag_loader::Load_tag(santa_hat_datum, true, "carto_shared");
				tag_loader::Load_tag(beard_datum, true, "carto_shared");
				tag_loader::Push_Back();

				santa_hat_datum = tag_loader::ResolveNewDatum(santa_hat_datum);
				beard_datum = tag_loader::ResolveNewDatum(beard_datum);

				datum hlmt_chief_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief");
				if (hlmt_chief_datum != DATUM_INDEX_NONE) {
					AddHatAndBeard(hlmt_chief_datum, santa_hat_datum, beard_datum);
				}
				datum hlmt_chief_mp_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief_mp");
				if (hlmt_chief_mp_datum != DATUM_INDEX_NONE) {
					AddHatAndBeard(hlmt_chief_mp_datum, santa_hat_datum, beard_datum);
				}
				datum hlmt_elite_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\elite\\elite_mp");
				if (hlmt_elite_datum != DATUM_INDEX_NONE)
				{
					AddHatAndBeard(hlmt_elite_datum, santa_hat_datum, beard_datum, true);
				}

				datum flood_datum = PlayerRepresentation::get_object_datum_from_representation(s_player::e_character_type::Flood);
				if (!DATUM_IS_NONE(flood_datum))
				{
					auto flood_biped = tags::get_tag<blam_tag::tag_group_type::biped, s_biped_group_definition>(flood_datum, true);
					AddHatAndBeard(flood_biped->unitTag.objectTag.model.TagIndex, santa_hat_datum, beard_datum, false);
				}
			}
			if (!DATUM_IS_NONE(snow_datum))
			{
				tag_loader::Load_tag(snow_datum, true, "carto_shared");
				tag_loader::Push_Back();

				snow_datum = tag_loader::ResolveNewDatum(snow_datum);

				if (!DATUM_IS_NONE(snow_datum))
				{
					auto scen = tags::get_tag_fast<s_scenario_group_definition>(tags::get_tags_header()->scenario_datum);
					auto sbsp = tags::get_tag_fast<s_scenario_structure_bsp_group_definition>(scen->structure_bsps[0]->structure_bsp.TagIndex);

					auto weat_block = MetaExtender::add_tag_block2<s_scenario_structure_bsp_group_definition::s_weather_palette_block>((unsigned long)std::addressof(sbsp->weather_palette));
					weat_block->name = "snow_cs";
					weat_block->weather_system.TagGroup = blam_tag::tag_group_type::weathersystem;
					weat_block->weather_system.TagIndex = snow_datum;

					for (auto& cluster : sbsp->clusters)
						cluster.weather = sbsp->weather_palette.size - 1;
				}
			}
			if (!DATUM_IS_NONE(candy_cane_datum) && !DATUM_IS_NONE(sword_weapon_datum))
			{
				tag_loader::Load_tag(candy_cane_datum, true, "carto_shared");
				tag_loader::Push_Back();

				candy_cane_datum = tag_loader::ResolveNewDatum(candy_cane_datum);

				auto sword_weapon = tags::get_tag_fast<s_weapon_group_definition>(sword_weapon_datum);

				datum sword_model_datum = sword_weapon->model.TagIndex;
				auto sword_model = tags::get_tag_fast<s_model_group_definition>(sword_model_datum);

				sword_model->render_model.TagIndex = candy_cane_datum;

				for (auto& first_person : sword_weapon->first_person)
					first_person.first_person_model.TagIndex = candy_cane_datum;

				for (auto& attachment : sword_weapon->attachments)
				{
					attachment.type.TagIndex = -1;
					attachment.type.TagGroup = blam_tag::tag_group_type::none;
					attachment.marker_old_string_id = 0;
					attachment.primary_scale = 0;
				}
			}
			if (!DATUM_IS_NONE(deer_datum) && !DATUM_IS_NONE(ghost_datum))
			{
				tag_loader::Load_tag(deer_datum, true, "carto_shared");
				tag_loader::Push_Back();

				deer_datum = tag_loader::ResolveNewDatum(deer_datum);

				auto ghost_vehicle = tags::get_tag<blam_tag::tag_group_type::vehicle, s_unit_group_definition>(ghost_datum);
				ghost_vehicle->objectTag.attachments.data = 0;
				ghost_vehicle->objectTag.attachments.size = 0;

				datum ghost_model_datum = ghost_vehicle->objectTag.model.TagIndex;
				auto ghost_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(ghost_model_datum);
				ghost_model->render_model.TagIndex = deer_datum;
			}
			if (!DATUM_IS_NONE(ornament_datum) && !DATUM_IS_NONE(frag_model_datum) && !DATUM_IS_NONE(plasma_model_datum))
			{
				tag_loader::Load_tag(ornament_datum, true, "carto_shared");
				tag_loader::Push_Back();

				ornament_datum = tag_loader::ResolveNewDatum(ornament_datum);

				auto frag_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(frag_model_datum);
				frag_model->render_model.TagIndex = ornament_datum;

				auto plasma_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(plasma_model_datum);
				plasma_model->render_model.TagIndex = ornament_datum;
			}
			if (!DATUM_IS_NONE(present_datum) && !DATUM_IS_NONE(fp_present_datum) && !DATUM_IS_NONE(ball_weapon_datum) && !DATUM_IS_NONE(bomb_weapon_datum))
			{
				tag_loader::Load_tag(present_datum, true, "carto_shared");
				tag_loader::Load_tag(fp_present_datum, true, "carto_shared");
				tag_loader::Push_Back();

				present_datum = tag_loader::ResolveNewDatum(present_datum);
				fp_present_datum = tag_loader::ResolveNewDatum(fp_present_datum);

				ReplaceFirstAndThirdPersonModelFromWeapon(ball_weapon_datum, fp_present_datum, present_datum);
				ReplaceFirstAndThirdPersonModelFromWeapon(bomb_weapon_datum, fp_present_datum, present_datum);
			}
		}
	}

	void PaddysOnMapLoad()
	{
		if (s_game_globals::game_is_multiplayer())
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
				auto paddy_pot_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(paddy_pot->objectTag.model.TagIndex, true);

				// Give Hat and Beard to Masterchief & Friends
				datum hlmt_chief_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief");
				if (hlmt_chief_datum != DATUM_INDEX_NONE) {
					AddHatAndBeard(hlmt_chief_datum, paddy_hat_datum, paddy_beard_datum);
				}
				datum hlmt_chief_mp_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief_mp");
				if (hlmt_chief_mp_datum != DATUM_INDEX_NONE) {
					AddHatAndBeard(hlmt_chief_mp_datum, paddy_hat_datum, paddy_beard_datum);
				}
				datum hlmt_elite_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\elite\\elite_mp");
				if (hlmt_elite_datum != DATUM_INDEX_NONE)
				{
					AddHatAndBeard(hlmt_elite_datum, paddy_hat_datum, paddy_beard_datum, true);
				}

				// Replace ball and bomb with the pot of gold
				datum ball_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\ball\\head_sp");
				if (ball_weapon_datum != DATUM_INDEX_NONE)
				{
					auto ball_weapon = tags::get_tag<blam_tag::tag_group_type::weapon, s_weapon_group_definition>(ball_weapon_datum);
					auto ball_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(ball_weapon->model.TagIndex);

					ball_model->render_model.TagIndex = paddy_pot_model->render_model.TagIndex;

					//Bounding Radius and Sweetener size
					ball_weapon->bounding_radius = 0.3f;
					ball_weapon->sweetener_size = s_weapon_group_definition::e_sweetener_size::medium;
				}
				datum bomb_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\assault_bomb\\assault_bomb");
				if (bomb_weapon_datum != DATUM_INDEX_NONE)
				{
					auto bomb_weapon = tags::get_tag<blam_tag::tag_group_type::weapon, s_weapon_group_definition>(bomb_weapon_datum);
					auto bomb_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(bomb_weapon->model.TagIndex);

					bomb_model->render_model.TagIndex = paddy_pot_model->render_model.TagIndex;
					auto paddy_pot_render = tags::get_tag<blam_tag::tag_group_type::rendermodel, s_render_model_group_definition>(paddy_pot_model->render_model.TagIndex, true);
					auto pot_node = paddy_pot_render->nodes[0];
					pot_node->default_rotation_k = -0.75;
					pot_node->inverse_position_y = 0.07;
					pot_node->inverse_position_z = -0.1;

					// Bounding Radius and Sweetener size
					bomb_weapon->bounding_radius = 0.3f;
					bomb_weapon->sweetener_size = s_weapon_group_definition::e_sweetener_size::medium;
				}
			}
		}
	}

	void MookMaddnessOnMapLoad()
	{
		if (s_game_globals::game_is_multiplayer())
		{
			datum ball_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\ball\\ball");
			datum bomb_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\assault_bomb\\assault_bomb");
			datum mook_ball_weapon_datum = tag_loader::Get_tag_datum
			("scenarios\\objects\\multi\\carto_shared\\basketball\\basketball", blam_tag::tag_group_type::weapon, "carto_shared");
			if (!DATUM_IS_NONE(mook_ball_weapon_datum) && !DATUM_IS_NONE(ball_weapon_datum) && !DATUM_IS_NONE(bomb_weapon_datum))
			{
				tag_loader::Load_tag(mook_ball_weapon_datum, true, "carto_shared");
				tag_loader::Push_Back();

				mook_ball_weapon_datum = tag_loader::ResolveNewDatum(mook_ball_weapon_datum);

				auto mook_ball_weapon = tags::get_tag<blam_tag::tag_group_type::weapon, s_weapon_group_definition>(mook_ball_weapon_datum, true);

				ReplaceFirstAndThirdPersonModelFromWeapon(ball_weapon_datum, mook_ball_weapon->model.TagIndex, mook_ball_weapon->model.TagIndex);
				ReplaceFirstAndThirdPersonModelFromWeapon(bomb_weapon_datum, mook_ball_weapon->model.TagIndex, mook_ball_weapon->model.TagIndex);
			}
		}
	}

	void halloween_game_life_cycle_update(e_game_life_cycle state)
	{
		if (state == _life_cycle_in_game)
		{
			if (H2Config_spooky_boy) {
				*Memory::GetAddress<s_player::e_character_type*>(0x51A67C) = s_player::e_character_type::Skeleton; 
			}

			typedef void(__cdecl t_set_orientation)(real_vector3d* forward, real_vector3d* up, const real_vector3d* orient);
			auto set_orientation = Memory::GetAddress<t_set_orientation*>(0x3347B);

			s_object_placement_data placement;

			auto pump = tags::get_tag<blam_tag::tag_group_type::scenery, s_scenery_group_definition>(pump_datum, true);
			auto pump_hmlt = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(pump->objectTag.model.TagIndex, true);

			s_cache_header* cache_header = tags::get_cache_header();
			if (!strcmp(cache_header->name, "coagulation"))
			{
				for (auto& scen_place : coag_scen_places)
				{
					// Set type of object and variant
					switch (scen_place.type)
					{
					case 0:
						Engine::Objects::create_new_placement_data(&placement, pump_datum, -1, 0);
						placement.variant_name = pump_hmlt->variants[scen_place.variant_id]->name.get_packed();
						break;
					case 1:
						Engine::Objects::create_new_placement_data(&placement, candle_datum, -1, 0);
						placement.variant_name = 0;
						break;
					case 2:
						Engine::Objects::create_new_placement_data(&placement, large_candle_datum, -1, 0);
						placement.variant_name = 0;
						break;
					}
					// Set location orientation and scale
					placement.position = scen_place.position;
					set_orientation(&placement.orientation, &placement.up, &scen_place.rotation);
					placement.scale = scen_place.scale;

					// Create the new object
					datum object_idx = Engine::Objects::object_new(&placement);
				}
			}
			else if (!strcmp(cache_header->name, "lockout"))
			{
				for (auto& scen_place : lockout_scen_places)
				{
					// Set type of object and variant
					switch (scen_place.type)
					{
					case 0:
						Engine::Objects::create_new_placement_data(&placement, pump_datum, -1, 0);
						placement.variant_name = pump_hmlt->variants[scen_place.variant_id]->name.get_packed();
						break;
					case 1:
						Engine::Objects::create_new_placement_data(&placement, candle_datum, -1, 0);
						placement.variant_name = 0;
						break;
					}
					// Set location orientation and scale
					placement.position = scen_place.position;
					set_orientation(&placement.orientation, &placement.up, &scen_place.rotation);
					placement.scale = scen_place.scale;

					// Create the new object
					datum object_idx = Engine::Objects::object_new(&placement);
				}
			}
		}
	}

	void HalloweenOnMapLoad()
	{
		if (s_game_globals::game_is_multiplayer())
		{
			// Load specific tags from shared and modify placements depending on the map being played
			s_cache_header* cache_header = tags::get_cache_header();
			auto scnr = tags::get_tag_fast<s_scenario_group_definition>(tags::get_tags_header()->scenario_datum);
			auto sbps = tags::get_tag_fast< s_scenario_structure_bsp_group_definition>(scnr->structure_bsps[0]->structure_bsp.TagIndex);
			if (!strcmp(cache_header->name, "coagulation"))
			{
				lbitm_datum = tag_loader::Get_tag_datum("scenarios\\multi\\halo\\coagulation\\coagulation_coagulation_lightmap_truecolor_bitmaps", blam_tag::tag_group_type::bitmap, "carto_shared");
				sky_datum = tag_loader::Get_tag_datum("scenarios\\skies\\multi\\halo\\coagulation\\coagulation_night", blam_tag::tag_group_type::sky, "carto_shared");
				candle_fire_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\candle\\candle_fire", blam_tag::tag_group_type::scenery, "carto_shared");
				candle_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\candle\\candle", blam_tag::tag_group_type::scenery, "carto_shared");
				pump_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\jack_o_lantern", blam_tag::tag_group_type::scenery, "carto_shared");
				large_candle_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\candle\\candle_big_light", blam_tag::tag_group_type::scenery, "carto_shared");

				tag_loader::Load_tag(pump_datum, true, "carto_shared");
				tag_loader::Load_tag(candle_datum, true, "carto_shared");
				tag_loader::Load_tag(lbitm_datum, true, "carto_shared");
				tag_loader::Load_tag(sky_datum, true, "carto_shared");
				tag_loader::Load_tag(large_candle_datum, true, "carto_shared");
				tag_loader::Push_Back();

				// OG Halo 2 Coag lightmap
				datum ltmp_datum = tags::find_tag(blam_tag::tag_group_type::scenariostructurelightmap,
					"scenarios\\multi\\halo\\coagulation\\coagulation_coagulation_lightmap");

				candle_datum = tag_loader::ResolveNewDatum(candle_datum);
				candle_fire_datum = tag_loader::ResolveNewDatum(candle_fire_datum);
				pump_datum = tag_loader::ResolveNewDatum(pump_datum);
				large_candle_datum = tag_loader::ResolveNewDatum(large_candle_datum);
				lbitm_datum = tag_loader::ResolveNewDatum(lbitm_datum);
				sky_datum = tag_loader::ResolveNewDatum(sky_datum);

				LOG_INFO_GAME("{:x}", candle_datum);
				LOG_INFO_GAME("{:x}", candle_fire_datum);
				LOG_INFO_GAME("{:x}", pump_datum);
				LOG_INFO_GAME("{:x}", lbitm_datum);
				LOG_INFO_GAME("{:x}", sky_datum);

				if (!DATUM_IS_NONE(sky_datum))
				{
					auto sky = tags::get_tag<blam_tag::tag_group_type::sky, char*>(sky_datum, true);
					auto render_model_ref = reinterpret_cast<tag_reference*>(sky);
					auto render_model = tags::get_tag<blam_tag::tag_group_type::rendermodel, s_render_model_group_definition>(render_model_ref->TagIndex, true);
					scnr->skies[0]->sky.TagIndex = sky_datum;
				}
				if (!DATUM_IS_NONE(ltmp_datum) && !DATUM_IS_NONE(lbitm_datum))
				{
					auto ltmp = tags::get_tag_fast<s_scenario_structure_lightmap_group_definition>(ltmp_datum);
					ltmp->lightmap_groups[0]->bitmap_group.TagIndex = lbitm_datum;
					sbps->decorators_block.size = 0;
					sbps->decorators_block.data = 0;
				}

				// Add items to scenario
				if (!DATUM_IS_NONE(candle_datum) && !DATUM_IS_NONE(pump_datum) && !DATUM_IS_NONE(large_candle_datum))
				{
					EventHandler::register_callback(halloween_game_life_cycle_update, EventType::gamelifecycle_change, EventExecutionType::execute_after, true);
					// We execute this after a bluescreen since our new objects arent recreated automatically
					EventHandler::register_callback(halloween_game_life_cycle_update, EventType::blue_screen, EventExecutionType::execute_after, true);
				}
			}
			else if (!strcmp(cache_header->name, "lockout"))
			{
				candle_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\candle\\candle", blam_tag::tag_group_type::scenery, "carto_shared");
				candle_fire_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\candle\\candle_fire", blam_tag::tag_group_type::scenery, "carto_shared");
				pump_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\jack_o_lantern", blam_tag::tag_group_type::scenery, "carto_shared");

				tag_loader::Load_tag(pump_datum, true, "carto_shared");
				tag_loader::Load_tag(candle_datum, true, "carto_shared");
				tag_loader::Push_Back();

				candle_datum = tag_loader::ResolveNewDatum(candle_datum);
				candle_fire_datum = tag_loader::ResolveNewDatum(candle_fire_datum);
				pump_datum = tag_loader::ResolveNewDatum(pump_datum);

				LOG_INFO_GAME("{:x}", candle_datum);
				LOG_INFO_GAME("{:x}", candle_fire_datum);
				LOG_INFO_GAME("{:x}", pump_datum);

				if (!DATUM_IS_NONE(candle_datum) && !DATUM_IS_NONE(pump_datum))
				{
					EventHandler::register_callback(halloween_game_life_cycle_update, EventType::gamelifecycle_change, EventExecutionType::execute_after, true);
					// We execute this after a bluescreen since our new objects arent recreated automatically
					EventHandler::register_callback(halloween_game_life_cycle_update, EventType::blue_screen, EventExecutionType::execute_after, true);
				}
			}
		}
	}

	void BirthdayOnMapLoad()
	{
		if (s_game_globals::game_is_multiplayer())
		{
			// Carto Shared Tags
			datum bday_hat_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\birthday_hat\\birthday_hat", blam_tag::tag_group_type::scenery, "carto_shared");
			datum bday_cake_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\birthday_cake\\birthday_cake", blam_tag::tag_group_type::rendermodel, "carto_shared");
			datum fp_bday_cake_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\birthday_cake\\fp\\fp", blam_tag::tag_group_type::rendermodel, "carto_shared");

			// Halo 2 Tags
			datum ball_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\ball\\ball");
			datum bomb_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\assault_bomb\\assault_bomb");

			if (!DATUM_IS_NONE(bday_hat_datum))
			{
				tag_loader::Load_tag(bday_hat_datum, true, "carto_shared");
				tag_loader::Push_Back();

				bday_hat_datum = tag_loader::ResolveNewDatum(bday_hat_datum);


				datum hlmt_chief_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief");
				if (hlmt_chief_datum != DATUM_INDEX_NONE) {
					AddHat(hlmt_chief_datum, bday_hat_datum);
				}
				datum hlmt_chief_mp_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief_mp");
				if (hlmt_chief_mp_datum != DATUM_INDEX_NONE) {
					AddHat(hlmt_chief_mp_datum, bday_hat_datum);
				}
				datum hlmt_elite_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\elite\\elite_mp");
				if (hlmt_elite_datum != DATUM_INDEX_NONE)
				{
					AddHat(hlmt_elite_datum, bday_hat_datum, true);
				}
			}
			
			if (!DATUM_IS_NONE(bday_cake_datum) && !DATUM_IS_NONE(fp_bday_cake_datum) && !DATUM_IS_NONE(ball_weapon_datum) && !DATUM_IS_NONE(bomb_weapon_datum))
			{
				tag_loader::Load_tag(bday_cake_datum, true, "carto_shared");
				tag_loader::Load_tag(fp_bday_cake_datum, true, "carto_shared");
				tag_loader::Push_Back();

				bday_cake_datum = tag_loader::ResolveNewDatum(bday_cake_datum);
				fp_bday_cake_datum = tag_loader::ResolveNewDatum(fp_bday_cake_datum);

				ReplaceFirstAndThirdPersonModelFromWeapon(ball_weapon_datum, fp_bday_cake_datum, bday_cake_datum);
				ReplaceFirstAndThirdPersonModelFromWeapon(bomb_weapon_datum, fp_bday_cake_datum, bday_cake_datum);
			}

		}
	}

	// Add new marker for elites
	void AddNewMarkers()
	{
		auto mode_elite_datum = tags::find_tag(blam_tag::tag_group_type::rendermodel, "objects\\characters\\elite\\elite_mp");
		if (!DATUM_IS_NONE(mode_elite_datum))
		{
			auto mode_elite = tags::get_tag<blam_tag::tag_group_type::rendermodel, s_render_model_group_definition>(mode_elite_datum);
			auto new_marker_group = MetaExtender::add_tag_block2<s_render_model_group_definition::s_marker_groups_block>((unsigned long)std::addressof(mode_elite->marker_groups));
			new_marker_group->name_old_string_id = new_elite_head_marker;
			auto new_marker = MetaExtender::add_tag_block2<s_render_model_group_definition::s_marker_groups_block::s_markers_block>((unsigned long)std::addressof(new_marker_group->markers));
			LOG_INFO_GAME("[{}] {:x}", __FUNCTION__, (unsigned long)std::addressof(new_marker));
			new_marker->node_index = 19;
			new_marker->permutation_index = -1;
			new_marker->region_index = -1;
			new_marker->translation_x = 0.006;
			new_marker->translation_y = 0.06;
			new_marker->translation_z = 0;
			new_marker->rotation_i = 0.9;
			new_marker->rotation_j = 0.9;
			new_marker->roatation_k = 0.9;
			new_marker->rotation_w = 1;
			new_marker->scale = 1;
		}
	}

	void ApplyHooks()
	{

	}

	void Initialize()
	{
		if (tag_loader::Map_exists("carto_shared"))
		{
			ApplyHooks();
			tags::on_map_load(AddNewMarkers);
			switch (getCurrentEvent())
			{
			case _christmas:
				tags::on_map_load(ChristmasOnMapLoad);
				break;
			case _st_paddys:
				tags::on_map_load(PaddysOnMapLoad);
				break;
			case _mook_maddness:
				tags::on_map_load(MookMaddnessOnMapLoad);
				break;
			case _halloween:
				tags::on_map_load(HalloweenOnMapLoad);
				break;
			case _birthday:
				tags::on_map_load(BirthdayOnMapLoad);
				break;
			}
		}
		else
		{
			if (!NetworkSession::LocalPeerIsSessionHost())
			{
				*Memory::GetAddress<int*>(0x46DCF1) = 1;
				ImGuiHandler::ImMessageBox::SetMessage("Error: Cartographer Shared map content is missing. Try updating your game from the mainmenu.\r\n\r\nBy going to Cartographer > Update.\r\n\r\nIf that doesn't work reach out to us in #help on discord.");
				ImGuiHandler::ToggleWindow(ImGuiHandler::ImMessageBox::windowName);
			}
		}
	}
}
