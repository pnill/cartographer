#include "stdafx.h"

#include "SpecialEvents.h"

#include "Blam/Engine/Game/GameTimeGlobals.h"
#include "Blam\Cache\TagGroups\model_definition.hpp"
#include "Blam\Cache\TagGroups\render_model_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_lightmap_definition.hpp"
#include "Blam\Cache\TagGroups\scenario_structure_bsp_definition.hpp"
#include "Blam\Cache\TagGroups\scenery_definition.hpp"
#include "Blam\Cache\TagGroups\shader_definition.hpp"
#include "Blam\Cache\TagGroups\weapon_definition.hpp"
#include "Blam\Engine\Game\GameGlobals.h"
#include "Blam\Enums\HaloStrings.h"
#include "H2MOD\Engine\Engine.h"
#include "H2MOD\GUI\imgui_integration\imgui_handler.h"
#include "H2MOD\Modules\Config\Config.h"
#include "H2MOD\Modules\EventHandler\EventHandler.hpp"
#include "H2MOD\Modules\Networking\Networking.h"
#include "H2MOD\Modules\PlayerRepresentation\PlayerRepresentation.h"
#include "H2MOD\Tags\MetaExtender.h"
#include "H2MOD\Tags\MetaLoader\tag_loader.h"
#include <math.h>

#include "Blam/Cache/TagGroups/biped_definition.hpp"
#include "Blam/Cache/TagGroups/unit_definition.hpp"
#define _USE_MATH_DEFINES

namespace SpecialEvents
{
	namespace
	{
		datum mook_ball_datum = DATUM_INDEX_NONE;

		datum paddy_hat_datum = DATUM_INDEX_NONE;
		datum paddy_beard_datum = DATUM_INDEX_NONE;
		datum paddy_pot_datum = DATUM_INDEX_NONE;

		datum santa_hat_datum = DATUM_INDEX_NONE;
		datum santa_beard_datum = DATUM_INDEX_NONE;
		datum snow_datum = DATUM_INDEX_NONE;
		datum candy_cane_datum = DATUM_INDEX_NONE;
		datum deer_datum = DATUM_INDEX_NONE;
		datum ornament_datum = DATUM_INDEX_NONE;
		datum present_datum = DATUM_INDEX_NONE;
		datum fp_present_datum = DATUM_INDEX_NONE;
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
		//This function checks if the current date is in the same week as the event.
		bool CheckIfEventTime(std::wstring date)
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
		bool CheckIfEventTime2(std::wstring date)
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
			return e_none;

		if (CheckIfEventTime(L"3-17"))
			return e_st_paddys;

		if (CheckIfEventTime(L"12-24") || CheckIfEventTime(L"12-30") || CheckIfEventTime(L"1-4"))
			return e_christmas;

		if (CheckIfEventTime(L"4-12"))
			return e_mook_maddness;

		if (CheckIfEventTime(L"10-20") || CheckIfEventTime(L"10-27") || CheckIfEventTime2(L"10-31"))
			return e_halloween;

		return e_none;
	}

	void ChristmasOnMapLoad()
	{
		if(h2mod->GetEngineType() == e_engine_type::_main_menu)
		{
			auto md = tags::find_tag(blam_tag::tag_group_type::soundlooping, "sound\\ui\\main_menu_music\\main_menu_music");
			auto m = tags::get_tag<blam_tag::tag_group_type::soundlooping, char>(md);
			tags::tag_data_block* track_block = reinterpret_cast<tags::tag_data_block*>(m + 0x1c);
			if (track_block->block_count > 0 && track_block->block_data_offset != -1)
			{
				char* track_data = tags::get_tag_data() + track_block->block_data_offset;
				tag_reference* track_in = reinterpret_cast<tag_reference*>(track_data + 0x14);
				tag_reference* track_loop = reinterpret_cast<tag_reference*>(track_data + 0x1C);
				track_in->TagIndex = -1;
				track_loop->TagIndex = -1;
			}
		}
		if (h2mod->GetEngineType() == e_engine_type::_mutliplayer)
		{
			santa_hat_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\christmas_hat_map\\hat\\hat", blam_tag::tag_group_type::scenery, "carto_shared");
			auto w_datum_i = tag_loader::Get_tag_datum("scenarios\\multi\\lockout\\lockout_big", blam_tag::tag_group_type::weathersystem, "carto_shared");
			auto sword_datum_i = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\candy_cane\\candy_cane", blam_tag::tag_group_type::rendermodel, "carto_shared");
			auto beard_datum_i = tag_loader::Get_tag_datum("objects\\multi\\stpat_hat\\beard\\santa_beard", blam_tag::tag_group_type::scenery, "carto_shared");
			auto deer_datum_i = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\reindeer_ghost\\reindeer_ghost", blam_tag::tag_group_type::rendermodel, "carto_shared");
			auto ornament_datum_i = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\ornament\\ornament", blam_tag::tag_group_type::rendermodel, "carto_shared");
			auto present_datum_i = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\present\\present", blam_tag::tag_group_type::rendermodel, "carto_shared");
			auto fp_present_datum_i = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\present\\fp_present", blam_tag::tag_group_type::rendermodel, "carto_shared");
			if (!DATUM_IS_NONE(santa_hat_datum) && !DATUM_IS_NONE(beard_datum_i))
			{
				tag_loader::Load_tag(santa_hat_datum, true, "carto_shared");
				tag_loader::Load_tag(beard_datum_i, true, "carto_shared");
				tag_loader::Push_Back();
				santa_beard_datum = tag_loader::ResolveNewDatum(beard_datum_i);
				//auto scen = tags::get_tag<blam_tag::tag_group_type::scenery, s_scenery_group_definition>(datum(_INJECTED_TAG_START_));
				auto hlmt_chief_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief_mp");
				if (hlmt_chief_datum != DATUM_INDEX_NONE) {
					auto hlmt_chief = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(hlmt_chief_datum);
					auto b = hlmt_chief->variants[0];
					auto a = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(b->objects));
					a->parent_marker = string_id(184552154);
					a->child_object.TagGroup = blam_tag::tag_group_type::scenery;
					a->child_object.TagIndex = tag_loader::ResolveNewDatum(santa_hat_datum);
					auto beard = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(b->objects));
					beard->parent_marker = string_id(HaloString::HS_HEAD);
					beard->child_object.TagGroup = blam_tag::tag_group_type::scenery;
					beard->child_object.TagIndex = santa_beard_datum;
				}
				auto hlmt_elite_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\elite\\elite_mp");
				if (hlmt_elite_datum != DATUM_INDEX_NONE)
				{
					auto hlmt_eliete = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(hlmt_elite_datum);
					auto b = hlmt_eliete->variants[0];
					auto a = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(b->objects));
					a->parent_marker = new_elite_head_marker;
					a->child_object.TagGroup = blam_tag::tag_group_type::scenery;
					a->child_object.TagIndex = tag_loader::ResolveNewDatum(santa_hat_datum);
					auto beard = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(b->objects));
					beard->parent_marker = new_elite_head_marker;
					beard->child_object.TagGroup = blam_tag::tag_group_type::scenery;
					beard->child_object.TagIndex = santa_beard_datum;
				}
				//auto flood_datum = player_representation::get_object_datum_from_representation(s_player::e_character_type::Flood);
				//if(!DATUM_IS_NONE(flood_datum))
				//{
				//	auto flood_biped = tags::get_tag<blam_tag::tag_group_type::biped, s_biped_group_definition>(flood_datum, true);
				//	auto flood_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(flood_biped->unitTag.objectTag.model.TagIndex, true);
				//	auto flood_variant = flood_model->variants[0];
				//	auto hat_object = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(flood_variant->objects));
				//	hat_object->parent_marker = string_id(184552154);
				//	hat_object->child_object.TagGroup = blam_tag::tag_group_type::scenery;
				//	hat_object->child_object.TagIndex = tag_loader::ResolveNewDatum(santa_hat_datum);
				//}
			}
			if (!DATUM_IS_NONE(w_datum_i))
			{
				tag_loader::Load_tag(w_datum_i, true, "carto_shared");
				tag_loader::Push_Back();
				snow_datum = tag_loader::ResolveNewDatum(w_datum_i);
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
			if (!DATUM_IS_NONE(sword_datum_i))
			{
				tag_loader::Load_tag(sword_datum_i, true, "carto_shared");
				tag_loader::Push_Back();
				candy_cane_datum = tag_loader::ResolveNewDatum(sword_datum_i);
				if (!DATUM_IS_NONE(candy_cane_datum))
				{
					auto sword_model_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\weapons\\melee\\energy_blade\\energy_blade");
					auto sword_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(sword_model_datum);
					sword_model->render_model.TagIndex = candy_cane_datum;

					auto sword_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\melee\\energy_blade\\energy_blade");
					auto sword_weapon = tags::get_tag<blam_tag::tag_group_type::weapon, s_weapon_group_definition>(sword_weapon_datum);

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
			}
			if (!DATUM_IS_NONE(deer_datum_i))
			{
				tag_loader::Load_tag(deer_datum_i, true, "carto_shared");
				tag_loader::Push_Back();
				deer_datum = tag_loader::ResolveNewDatum(deer_datum_i);
				if(!DATUM_IS_NONE(deer_datum))
				{
					auto ghost_model_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\vehicles\\ghost\\ghost");
					auto ghost_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(ghost_model_datum);
					ghost_model->render_model.TagIndex = deer_datum;

					auto ghost_datum = tags::find_tag(blam_tag::tag_group_type::vehicle, "objects\\vehicles\\ghost\\ghost");
					auto ghost_vehicle = tags::get_tag<blam_tag::tag_group_type::vehicle, s_unit_group_definition>(ghost_datum);
					ghost_vehicle->objectTag.attachments.data = 0;
					ghost_vehicle->objectTag.attachments.size = 0;

				}
			}
			if(!DATUM_IS_NONE(ornament_datum_i))
			{
				tag_loader::Load_tag(ornament_datum_i, true, "carto_shared");
				tag_loader::Push_Back();
				ornament_datum = tag_loader::ResolveNewDatum(ornament_datum_i);
				if (!DATUM_IS_NONE(ornament_datum))
				{
					auto frag_model_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\weapons\\grenade\\frag_grenade\\frag_grenade_projectile");
					auto frag_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(frag_model_datum);
					frag_model->render_model.TagIndex = ornament_datum;

					auto plasma_model_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\weapons\\grenade\\plasma_grenade\\plasma_grenade");
					auto plasma_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(plasma_model_datum);
					plasma_model->render_model.TagIndex = ornament_datum;
				}
			}
			if(!DATUM_IS_NONE(present_datum_i))
			{
				tag_loader::Load_tag(present_datum_i, true, "carto_shared");
				tag_loader::Push_Back();
				present_datum = tag_loader::ResolveNewDatum(present_datum_i);
				if(!DATUM_IS_NONE(present_datum))
				{
					auto ball_model_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\weapons\\multiplayer\\ball\\ball");
					auto ball_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(ball_model_datum);
					ball_model->render_model.TagIndex = present_datum;

					auto bomb_model_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\weapons\\multiplayer\\assault_bomb\\assault_bomb");
					auto bomb_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(bomb_model_datum);
					bomb_model->render_model.TagIndex = present_datum;

					if (!DATUM_IS_NONE(fp_present_datum_i)) 
					{
						tag_loader::Load_tag(fp_present_datum_i, true, "carto_shared");
						tag_loader::Push_Back();
						fp_present_datum = tag_loader::ResolveNewDatum(fp_present_datum_i);
						if (!DATUM_IS_NONE(fp_present_datum))
						{
							auto ball_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\ball\\ball");
							char* ball_weapon = tags::get_tag<blam_tag::tag_group_type::weapon, char>(ball_weapon_datum);
							if (ball_weapon != nullptr)
							{
								tags::tag_data_block* first_person_block = reinterpret_cast<tags::tag_data_block*>(ball_weapon + 0x2A8);
								if (first_person_block->block_count > 0 && first_person_block->block_data_offset != -1)
								{
									char* first_person_data = tags::get_tag_data() + first_person_block->block_data_offset;
									for (auto i = 0; i < first_person_block->block_count; i++)
									{
										tag_reference* fp_model = reinterpret_cast<tag_reference*>(first_person_data + (0x10 * i));
										tag_reference* fp_anim = reinterpret_cast<tag_reference*>(first_person_data + (0x10 * i) + 8);
										fp_model->TagIndex = fp_present_datum;
									}
								}
							}

							auto bomb_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\assault_bomb\\assault_bomb");
							char* bomb_weapon = tags::get_tag<blam_tag::tag_group_type::weapon, char>(bomb_weapon_datum);
							if (bomb_weapon != nullptr)
							{
								tags::tag_data_block* first_person_block = reinterpret_cast<tags::tag_data_block*>(bomb_weapon + 0x2A8);
								if (first_person_block->block_count > 0 && first_person_block->block_data_offset != -1)
								{
									char* first_person_data = tags::get_tag_data() + first_person_block->block_data_offset;
									for (auto i = 0; i < first_person_block->block_count; i++)
									{
										tag_reference* fp_model = reinterpret_cast<tag_reference*>(first_person_data + (0x10 * i));
										tag_reference* fp_anim = reinterpret_cast<tag_reference*>(first_person_data + (0x10 * i) + 8);
										fp_model->TagIndex = fp_present_datum;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	void PaddysOnMapLoad()
	{
		if (h2mod->GetEngineType() == e_engine_type::_mutliplayer)
		{
			if (tag_loader::Map_exists("carto_shared"))
			{
				paddy_hat_datum = tag_loader::Get_tag_datum("objects\\multi\\stpat_hat\\stpat_hat", blam_tag::tag_group_type::scenery, "carto_shared");
				paddy_beard_datum = tag_loader::Get_tag_datum("objects\\multi\\stpat_hat\\beard\\beard", blam_tag::tag_group_type::scenery, "carto_shared");
				paddy_pot_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\pot_of_gold\\pot_of_gold", blam_tag::tag_group_type::scenery, "carto_shared");
				if (!DATUM_IS_NONE(paddy_hat_datum) && !DATUM_IS_NONE(paddy_beard_datum) && !DATUM_IS_NONE(paddy_pot_datum))
				{
					tag_loader::Load_tag(paddy_hat_datum, true, "carto_shared");
					tag_loader::Load_tag(paddy_beard_datum, true, "carto_shared");
					tag_loader::Load_tag(paddy_pot_datum, true, "carto_shared");
					tag_loader::Push_Back();
					auto hlmt_chief_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief");
					if (hlmt_chief_datum != DATUM_INDEX_NONE) {
						auto hlmt_chief = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(hlmt_chief_datum);
						auto b = hlmt_chief->variants[0];
						auto hat = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(b->objects));
						hat->parent_marker = string_id(HaloString::HS_HEAD);
						hat->child_object.TagGroup = blam_tag::tag_group_type::scenery;
						hat->child_object.TagIndex = tag_loader::ResolveNewDatum(paddy_hat_datum);
						auto beard = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(b->objects));
						beard->parent_marker = string_id(HaloString::HS_HEAD);
						beard->child_object.TagGroup = blam_tag::tag_group_type::scenery;
						beard->child_object.TagIndex = tag_loader::ResolveNewDatum(paddy_beard_datum);

					}
					auto hlmt_chief_mp_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief_mp");
					if (hlmt_chief_mp_datum != DATUM_INDEX_NONE) {
						auto hlmt_chief_mp = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(hlmt_chief_mp_datum);
						auto b = hlmt_chief_mp->variants[0];
						auto hat = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(b->objects));
						hat->parent_marker = string_id(HaloString::HS_HEAD);
						hat->child_object.TagGroup = blam_tag::tag_group_type::scenery;
						hat->child_object.TagIndex = tag_loader::ResolveNewDatum(paddy_hat_datum);
						auto beard = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(b->objects));
						beard->parent_marker = string_id(HaloString::HS_HEAD);
						beard->child_object.TagGroup = blam_tag::tag_group_type::scenery;
						beard->child_object.TagIndex = tag_loader::ResolveNewDatum(paddy_beard_datum);
					}
					auto hlmt_elite_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\elite\\elite_mp");
					if (hlmt_elite_datum != DATUM_INDEX_NONE)
					{
						auto hlmt_eliete = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(hlmt_elite_datum);
						auto b = hlmt_eliete->variants[0];
						auto a = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(b->objects));
						a->parent_marker = new_elite_head_marker;
						a->child_object.TagGroup = blam_tag::tag_group_type::scenery;
						a->child_object.TagIndex = tag_loader::ResolveNewDatum(paddy_hat_datum);
						auto beard = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(b->objects));
						beard->parent_marker = new_elite_head_marker;
						beard->child_object.TagGroup = blam_tag::tag_group_type::scenery;
						beard->child_object.TagIndex = tag_loader::ResolveNewDatum(paddy_beard_datum);
					}
				

					auto paddy_pot = tags::get_tag<blam_tag::tag_group_type::scenery, s_scenery_group_definition>(tag_loader::ResolveNewDatum(paddy_pot_datum), true);
					auto paddy_pot_model_datum = paddy_pot->objectTag.model.TagIndex;
					auto paddy_pot_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(paddy_pot_model_datum, true);

					auto ball_model_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\weapons\\multiplayer\\ball\\ball");
					auto ball_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(ball_model_datum);
					ball_model->render_model.TagIndex = paddy_pot_model->render_model.TagIndex;

					auto bomb_model_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\weapons\\multiplayer\\assault_bomb\\assault_bomb");
					auto bomb_model = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(bomb_model_datum);
					bomb_model->render_model.TagIndex = paddy_pot_model->render_model.TagIndex;

					auto paddy_pot_render = tags::get_tag<blam_tag::tag_group_type::rendermodel, s_render_model_group_definition>(paddy_pot_model->render_model.TagIndex, true);
					auto pot_node = paddy_pot_render->nodes[0];
					pot_node->default_rotation_k = -0.75;
					pot_node->inverse_position_y = 0.07;
					pot_node->inverse_position_z = -0.1;

					auto ball_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\ball\\ball");
					auto ball_weapon = tags::get_tag<blam_tag::tag_group_type::weapon, s_weapon_group_definition>(ball_weapon_datum);

					//Bounding Radius and Sweetener size
					ball_weapon->bounding_radius = 0.3f;
					ball_weapon->sweetener_size = s_weapon_group_definition::e_sweetener_size::medium;

					auto bomb_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\assault_bomb\\assault_bomb");
					auto bomb_weapon = tags::get_tag<blam_tag::tag_group_type::weapon, s_weapon_group_definition>(bomb_weapon_datum);

					//Bounding Radius and Sweetener size
					bomb_weapon->bounding_radius = 0.3f;
					bomb_weapon->sweetener_size = s_weapon_group_definition::e_sweetener_size::medium;
				}
			}
			else
			{
				if (NetworkSession::getCurrentNetworkSession()->local_peer_index != NetworkSession::getCurrentNetworkSession()->session_host_peer_index)
				{
					*Memory::GetAddress<int*>(0x46DCF1) = 1;
					imgui_handler::iMessageBox::SetMessage("Error: Cartographer Shared map content is missing. Try updating your game from the mainmenu.\r\n\r\nBy going to Cartographer > Update.\r\n\r\nIf that doesn't work reach out to us in #help on discord.");
					imgui_handler::ToggleWindow("messagebox");
				}
			}
		}
	}

	void MookMaddnessOnMapLoad()
	{
		if (h2mod->GetEngineType() == e_engine_type::_mutliplayer)
		{
			if (tag_loader::Map_exists("carto_shared"))
			{
				mook_ball_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\basketball\\basketball", blam_tag::tag_group_type::weapon, "carto_shared");
				if (!DATUM_IS_NONE(mook_ball_datum))
				{
					tag_loader::Load_tag(mook_ball_datum, true, "carto_shared");
					tag_loader::Push_Back();

					auto mook_ball = tags::get_tag<blam_tag::tag_group_type::weapon, s_weapon_group_definition>(tag_loader::ResolveNewDatum(mook_ball_datum), true);

					auto ball_model_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\ball\\ball");
					auto ball_model = tags::get_tag<blam_tag::tag_group_type::weapon, s_weapon_group_definition>(ball_model_datum);

					ball_model->model.TagIndex = mook_ball->model.TagIndex;
					ball_model->first_person[0]->first_person_model.TagIndex = mook_ball->first_person[0]->first_person_model.TagIndex;
					ball_model->first_person[1]->first_person_model.TagIndex = mook_ball->first_person[1]->first_person_model.TagIndex;

					auto bomb_model_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\assault_bomb\\assault_bomb");
					auto bomb_model = tags::get_tag<blam_tag::tag_group_type::weapon, s_weapon_group_definition>(bomb_model_datum);

					bomb_model->model.TagIndex = mook_ball->model.TagIndex;
					bomb_model->first_person[0]->first_person_model.TagIndex = mook_ball->first_person[0]->first_person_model.TagIndex;
					bomb_model->first_person[1]->first_person_model.TagIndex = mook_ball->first_person[1]->first_person_model.TagIndex;
				}
			}
			else
			{
				if (NetworkSession::getCurrentNetworkSession()->local_peer_index != NetworkSession::getCurrentNetworkSession()->session_host_peer_index)
				{
					*Memory::GetAddress<int*>(0x46DCF1) = 1;
					imgui_handler::iMessageBox::SetMessage("Error: Cartographer Shared map content is missing. Try updating your game from the mainmenu.\r\n\r\nBy going to Cartographer > Update.\r\n\r\nIf that doesn't work reach out to us in #help on discord.");
					imgui_handler::ToggleWindow("messagebox");
				}
			}
		}
	}
	std::vector<std::tuple<byte, byte, real_point3d, real_vector3d, float>> lockout_scen_places
	{
		{0, 0, {1.755f, -2.273151f, -7.6f}, {-1.5f, 0.f, 0.f}, 1.f},
		{0, 0, {1.755f, -0.529916f, -7.6f}, {-1.5f, 0.f, 0.f}, 1.f},
		{0, 0, {-0.826575f, 2.75f, -7.6f}, {0.f, 0.f, 0.f}, 1.f},
		{0, 0, {-2.17f, 2.75f, -7.6f}, {0.f, 0.f, 0.f}, 1.f},
		{0, 0, {-2.17f, 5.15f, -7.254f}, {0.f, 0.f, 0.f}, 1.f},
		{0, 0, {-0.826575f, 5.15f, -7.254f}, {0.f, 0.f, 0.f}, 1.f},
		{0, 0, {-4.758180f, -0.417534f, -7.6f}, {1.5f, 0.f, 0.f}, 1.f},
		{0, 0, {-4.758180f, -1.785402f, -7.6f}, {1.5f, 0.f, 0.f}, 1.f},
		{0, 0, {-7.65f, -1.785402f, -7.205f}, {1.5f, 0.f, 0.f}, 1.f},
		{0, 0, {-7.65f, -0.417534f, -7.205f}, {1.5f, 0.f, 0.f}, 1.f},
		{0, 2, {-1.51807f, 8.f, -1.489471f}, {0.f, 0.f, 0.65f}, 25.f},
		{1, 0, {-1.472f, 5.767787f, -0.282556f}, {0.f, 0.f, 0.f}, 1.f},
		{1, 0, {-2.072325f, 5.767787f, -0.282556f}, {0.f, 0.f, 0.f}, 1.f},
		{1, 0, {-0.955f, 5.767787f, -0.282556f}, {0.f, 0.f, 0.f}, 1.f},
		{1, 0, {-1.472f, 5.767787f, -0.6f}, {0.f, 0.f, 0.f}, 1.f},
		{1, 0, {-2.072325f, 5.767787f, -0.6f}, {0.f, 0.f, 0.f}, 1.f},
		{1, 0, {-0.955f, 5.767787f, -0.6f}, {0.f, 0.f, 0.f}, 1.f},
		{1, 0, {-1.472f, 5.767787f, -1.f}, {0.f, 0.f, 0.f}, 1.f},
		{1, 0, {-2.072325f, 5.767787f, -1.f}, {0.f, 0.f, 0.f}, 1.f},
		{1, 0, {-0.955f, 5.767787f, -1.f}, {0.f, 0.f, 0.f}, 1.f},
		{1, 0, {-1.472f, 5.767787f, -1.4f}, {0.f, 0.f, 0.f}, 1.f},
		{1, 0, {-2.072325f, 5.767787f, -1.4f}, {0.f, 0.f, 0.f}, 1.f},
		{1, 0, {-0.955f, 5.767787f, -1.4f}, {0.f, 0.f, 0.f}, 1.f}
	};
	std::vector<std::tuple<byte, byte, real_point3d, real_vector3d, float>> coag_scen_places
	{
		{ 0, 1, {85.1084f, -152.117f, 0.363701f}, {0.6927177f, 0.f, 0.f}, 2.f },
		{ 0, 0, {82.5243f, -150.899f, 0.15004f}, {-1.193646f, 0.f, 0.f}, 2.f },
		{ 0, 2, {84.7035f, -149.372f, 0.419855f}, {2.651836f, 0.004750926f, 0.003942908f}, 2.f},
		{ 1, 0, {83.9158f, -150.709f, 0.326849f}, {2.651836f, 0.004750926f, 0.003942908f}, 2.f },
		{ 1, 0, {90.3554f, -159.972f, 0.100233f}, {0.f, 0.f, 0.f}, 2.f },
		{ 1, 0, {90.3697f, -158.978f, 0.100233f}, {0.f, 0.f, 0.f}, 2.f },
		{ 1, 0, {101.57f, -158.997f, 0.092182f}, {0.f, 0.f, 0.f}, 2.f },
		{ 1, 0, {101.531f, -159.965f, 0.06341f}, {0.f, 0.f, 0.f}, 2.f },
		{ 0, 0, {97.9868f, -164.865f, 0.100248f}, {0.5208185f, 0.f, 0.f}, 2.f },
		{ 0, 0, {93.9883f, -164.87f, 0.100248f}, {-0.4845575f, 0.f, 0.f}, 2.f },
		{ 1, 0, {96.7595f, -153.917f, 0.082083f}, {-0.4845575f, 0.f, 0.f}, 2.f },
		{ 1, 0, {95.1653f, -153.948f, 0.0688466f}, {-0.4845575f, 0.f, 0.f}, 2.f },
		{ 0, 0, {38.2269f, -73.6964f, 0.100248f}, {-2.732226f, 0.f, 0.f}, 2.f },
		{ 0, 0, {42.0395f, -73.6596f, 0.100248f}, {2.718891f, 0.f, 0.f}, 2.f },
		{ 1, 0, {34.4627f, -78.5869f, 0.0110598f}, {2.718891f, 0.f, 0.f}, 2.f },
		{ 1, 0, {34.4527f, -79.6557f, 0.0833967f}, {2.718891f, 0.f, 0.f}, 2.f },
		{ 1, 0, {45.6763f, -78.6132f, 0.100235f}, {2.718891f, 0.f, 0.f}, 2.f },
		{ 1, 0, {45.6713f, -79.6315f, 0.100235f}, {2.718891f, 0.f, 0.f}, 2.f },
		{ 1, 0, {39.3133f, -84.671f, 0.0836101f}, {2.718891f, 0.f, 0.f}, 2.f },
		{ 1, 0, {40.8991f, -84.6169f, 0.0781164f}, {2.718891f, 0.f, 0.f}, 2.f },
		{ 1, 0, {57.6936f, -116.389f, 2.80373f}, {2.718891f, 0.f, 0.f}, 2.f },
		{ 1, 0, {54.9177f, -116.234f, 2.69431f}, {2.718891f, 0.f, 0.f}, 2.f },
		{ 1, 0, {54.8636f, -119.463f, 4.5416f}, {2.718891f, 0.f, 0.f}, 2.f },
		{ 1, 0, {67.1824f, -121.246f, 0.0423229f}, {2.718891f, 0.f, 0.f}, 2.f },
		{ 1, 0, {66.1641f, -119.377f, 0.0896063f}, {2.718891f, 0.f, 0.f}, 2.f },
		{ 1, 0, {64.5265f, -121.167f, 0.184917f}, {1.082931f, 0.f, 0.f}, 2.f },
		{ 1, 0, {65.8646f, -122.826f, 0.279448f}, {2.718891f, 0.f, 0.f}, 2.f },
		{ 0, 0, {95.8738f, -164.087f, 1.70024f}, {2.993693f, 0.f, 0.f}, 2.f },
		{ 0, 2, {99.2845f, -156.392f, 1.70024f}, {-0.7429186f, 0.f, 0.f}, 2.f },
		{ 0, 2, {32.8164f, -93.6961f, 0.552448f}, {-2.297098f, 0.f, 0.f}, 2.f },
		{ 0, 1, {34.8741f, -96.73f, 0.651451f}, {0.2612531f, 0.f, 0.f}, 2.f },
		{ 0, 0, {35.6997f, -93.8532f, 0.228822f}, {2.319857f, 0.f, 0.f}, 2.f },
		{ 1, 0, {34.3878f, -94.7817f, 0.431217f}, {2.319857f, 0.f, 0.f}, 2.f },
		{ 1, 0, {39.0936f, -79.1317f, -0.399765f}, {2.319857f, 0.f, 0.f}, 2.f },
		{ 1, 0, {40.0658f, -78.1527f, -0.318855f}, {2.319857f, 0.f, 0.f}, 2.f },
		{ 1, 0, {40.0671f, -79.9843f, -0.326734f}, {2.319857f, 0.f, 0.f}, 2.f },
		{ 1, 0, {40.8466f, -79.1566f, -0.399765f}, {2.319857f, 0.f, 0.f}, 2.f },
		{ 1, 0, {96.0025f, -160.239f, -0.344826f}, {2.314708f, -0.09751521f, 0.1054032f}, 2.f },
		{ 1, 0, {96.9577f, -159.461f, -0.399765f}, {2.314708f, -0.09751521f, 0.1054032f}, 2.f },
		{ 1, 0, {95.2218f, -159.541f, -0.399765f}, {2.314708f, -0.09751521f, 0.1054032f}, 2.f },
		{ 1, 0, {95.9969f, -158.794f, -0.348965f}, {2.314708f, -0.09751521f, 0.1054032f}, 2.f },
		{ 1, 0, {62.8602f, -155.737f, 5.97385f}, {2.314708f, -0.09751521f, 0.1054032f}, 2.f },
		{ 1, 0, {63.0175f, -154.209f, 6.24954f}, {2.317902f, -0.06000267f, 0.06465346f}, 2.f },
		{ 0, 0, {62.9095f, -154.865f, 6.12579f}, {-2.111883f, -0.06000267f, 0.06465363f}, 1.f },
		{ 0, 1, {62.8203f, -155.087f, 6.07893f}, {-2.105914f, -0.06000267f, 0.06465363f}, 2.f },
		{ 0, 2, {62.9789f, -154.936f, 6.22054f}, {-1.883839f, -0.06000215f, 0.06465415f}, 1.f },
		{ 0, 3, {62.9045f, -154.445f, 6.19814f}, {-1.818703f, -0.0600025f, 0.06465363f}, 1.f },
		{ 0, 2, {55.7299f, -117.498f, 1.38182f}, {1.103167f, 0.f, 0.f}, 2.f },
		{ 0, 2, {94.4302f, -116.768f, 3.69325f}, {-0.5194554f, 0.f, 0.f}, 2.f },
		{ 0, 2, {96.2998f, -118.367f, 4.27613f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {101.557f, -109.56f, 4.19486f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {102.425f, -110.716f, 4.17897f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {103.141f, -113.203f, 4.34958f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {102.44f, -114.52f, 4.29428f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {98.0914f, -98.8895f, 4.72495f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {97.0167f, -101.428f, 4.75314f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {69.4706f, -93.942f, 4.79087f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {71.252f, -91.9138f, 4.83399f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {99.2463f, -160.719f, -2.89977f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {99.3683f, -158.38f, -2.89977f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {95.0079f, -157.279f, -2.89977f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {96.9994f, -157.259f, -2.89977f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {92.2714f, -158.183f, -2.89977f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {92.3973f, -160.829f, -2.89977f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {36.7898f, -77.7807f, -2.89977f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {36.6824f, -80.2445f, -2.89977f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {39.1136f, -81.0421f, -2.89977f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {41.137f, -81.0647f, -2.89977f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {43.5285f, -80.2524f, -2.89977f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 1, 0, {43.641f, -77.9079f, -2.89977f}, {-0.9128194f, 0.f, 0.f}, 2.f },
		{ 0, 0, {39.7618f, -74.4427f, 1.70024f}, {0.1758856f, 0.f, 0.f}, 2.f },
		{ 0, 2, {36.7783f, -82.1991f, 1.70023f}, {2.472049f, 0.f, 0.f}, 2.f },
		{ 0, 2, {41.8715f, -124.651f, 2.8754f}, {2.472049f, 0.f, 0.f}, 2.f },
		{ 0, 0, {48.0045f, -136.689f, 0.878168f}, {0.004857932f, 0.f, 0.f}, 2.f },
		{ 0, 0, {36.0797f, -125.138f, 4.20733f}, {2.252522f, 0.f, 0.f}, 2.f },
		{ 0, 2, {35.9463f, -124.841f, 4.25084f}, {2.252522f, 0.f, 0.f}, 2.f },
		{ 0, 1, {35.8915f, -125.053f, 4.43641f}, {2.34958f, -0.1611368f, 0.1611648f}, 1.5f },
		{ 1, 0, {36.4906f, -125.788f, 4.07086f}, {2.252522f, 0.f, 0.f}, 2.f },
		{ 1, 0, {35.6242f, -124.164f, 4.2666f}, {2.252522f, 0.f, 0.f}, 2.f },
		{ 1, 0, {76.4923f, -75.2861f, 5.51429f}, {0.f, 0.f, 0.f}, 2.f },
		{ 1, 0, {73.5377f, -76.6098f, 3.29857f}, {0.f, 0.f, 0.f}, 2.f },
		{ 1, 0, {67.4692f, -83.042f, 1.90231f}, {0.f, 0.f, 0.f}, 2.f },
		{ 1, 0, {63.2808f, -89.5267f, 0.803593f}, {0.f, 0.f, 0.f}, 2.f },
		{ 1, 0, {71.9098f, -82.8371f, 5.83989f}, {0.f, 0.f, 0.f}, 2.f },
		{ 1, 0, {69.5506f, -85.6816f, 5.64538f}, {0.f, 0.f, 0.f}, 2.f },
		{ 1, 0, {88.2196f, -111.649f, 2.24786f}, {0.f, 0.f, 0.f}, 2.f },
		{ 1, 0, {86.0172f, -116.19f, 1.46686f}, {0.f, 0.f, 0.f}, 2.f },
		{ 0, 1, {89.283f, -120.649f, 1.79631f}, {-0.6024335f, 0.f, 0.f}, 2.f },
		{ 1, 0, {63.2927f, -167.23f, 3.87676f}, {-0.6024335f, 0.f, 0.f}, 2.f },
		{ 1, 0, {64.9065f, -167.128f, 3.01219f}, {-0.6024335f, 0.f, 0.f}, 2.f },
		{ 1, 0, {66.7779f, -164.484f, 2.05847f}, {-0.6024335f, 0.f, 0.f}, 2.f },
		{ 1, 0, {66.4975f, -159.395f, 2.04776f}, {-0.6024335f, 0.f, 0.f}, 2.f },
		{ 1, 0, {65.5545f, -154.987f, 2.09995f}, {-0.6024335f, 0.f, 0.f}, 2.f },
		{ 1, 0, {62.5329f, -149.414f, 1.58685f}, {-0.6024335f, 0.f, 0.f}, 2.f },
		{ 1, 0, {56.9924f, -146.674f, 2.66284f}, {-0.6024335f, 0.f, 0.f}, 2.f },
		{ 1, 0, {51.6483f, -147.013f, 2.83751f}, {-0.6024335f, 0.f, 0.f}, 2.f },
		{ 1, 0, {46.587f, -146.142f, 2.869f}, {-0.6024335f, 0.f, 0.f}, 2.f },
		{ 0, 0, {45.0003f, -151.729f, 4.05995f}, {2.608901f, 0.f, 0.f}, 2.f },
		{ 1, 0, {60.9523f, -168.512f, 4.70296f}, {2.608901f, 0.f, 0.f}, 2.f },
		{ 1, 0, {66.6873f, -181.667f, 4.88188f}, {2.608901f, 0.f, 0.f}, 2.f },
		{ 1, 0, {75.7585f, -181.446f, 3.27009f}, {2.608901f, 0.f, 0.f}, 2.f },
		{ 1, 0, {94.9061f, -180.888f, 2.93236f}, {2.608901f, 0.f, 0.f}, 2.f },
		{ 1, 0, {112.149f, -164.165f, 0.886029f}, {2.608901f, 0.f, 0.f}, 2.f },
		{ 1, 0, {114.646f, -148.043f, 0.939377f}, {2.608901f, 0.f, 0.f}, 2.f },
		{ 1, 0, {113.905f, -128.442f, 1.55014f}, {2.608901f, 0.f, 0.f}, 2.f },
		{ 1, 0, {103.795f, -125.301f, 3.76587f}, {2.608901f, 0.f, 0.f}, 2.f },
		{ 1, 0, {121.62f, -184.342f, 6.49842f}, {2.608901f, 0.f, 0.f}, 2.f },
		{ 0, 0, {112.511f, -174.719f, 1.47306f}, {1.080872f, 0.f, 0.f}, 1.f },
		{ 0, 2, {113.147f, -172.872f, 1.70583f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {89.3945f, -94.1537f, 2.56076f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {85.669f, -92.7625f, 1.95624f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {78.2857f, -93.0079f, 1.69695f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {71.306f, -94.7195f, 1.57765f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {62.7931f, -97.4938f, 2.09811f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {79.1431f, -66.4193f, 5.46561f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {70.8462f, -59.6363f, 4.72334f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {59.4823f, -58.7228f, 2.40265f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {46.9515f, -53.0083f, 2.95601f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {35.713f, -52.257f, 2.88171f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {24.0533f, -57.6752f, 3.34266f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {15.9079f, -64.1433f, 4.74662f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {15.9942f, -75.6059f, 3.01752f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {18.9183f, -89.2701f, 2.85182f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {19.9871f, -103.639f, 2.61423f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {30.6646f, -110.885f, 3.05688f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {34.3651f, -117.081f, 3.19603f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {34.0039f, -137.838f, 4.65256f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {39.5719f, -141.598f, 3.17854f}, {1.511316f, 0.f, 0.f}, 1.f },
		{ 1, 0, {69.3464f, -88.4811f, 5.48576f}, {2.718891f, 0.f, 0.f}, 2.f },
		{ 1, 0, {61.1582f, -160.045f, 5.65392f}, {-1.818703f, -0.0600025f, 0.06465363f}, 1.f },
		{ 1, 0, {60.5304f, -149.483f, 6.27032f}, {-1.818703f, -0.0600025f, 0.06465363f}, 1.f },
		{ 1, 0, {54.9746f, -149.118f, 5.32011f}, {-1.818703f, -0.0600025f, 0.06465363f}, 1.f },
		{ 1, 0, {95.053f, -94.8305f, 4.04204f}, {0.6927177f, 0.f, 0.f}, 2.f },
		{ 1, 0, {91.9081f, -103.373f, 1.96487f}, {0.6927177f, 0.f, 0.f}, 2.f },
		{ 1, 0, {80.2784f, -90.0236f, 4.55319f}, {0.6927177f, 0.f, 0.f}, 2.f },
		{ 1, 0, {88.0174f, -90.9017f, 4.81082f}, {0.6927177f, 0.f, 0.f}, 2.f },
		{ 1, 0, {98.6028f, -91.531f, 4.53088f}, {0.6927177f, 0.f, 0.f}, 2.f },
		{ 1, 0, {74.4072f, -79.5794f, 5.82736f}, {0.6927177f, 0.f, 0.f}, 2.f },
		{ 1, 0, {44.822f, -131.738f, 1.32844f}, {2.718891f, 0.f, 0.f}, 2.f },
		{ 0, 3, {19.2999f, -53.6043f, 19.4129f}, {0.523599f, -0.10472f, 0.174533f}, 100.f },
		{ 0, 3, {106.678f, -194.856f, 21.3504f}, {-2.79253f, 0.0872665f, -0.296706f}, 100.f },
		{ 2, 0, {19.3f, -53.6f, 25.36f}, {0.f, 0.f, 0.f}, 1.f },
		{ 2, 0, {106.678f, -194.856f, 26.f}, {0.f, 0.f, 0.f}, 1.f }
	};

	datum lbitm_datum;
	datum sky_datum;
	datum candle_datum;
	datum candle_fire_datum;
	datum large_candle_datum;
	datum pump_datum;
	void halloween_game_life_cycle_update(e_game_life_cycle state)
	{
		if (state == _life_cycle_in_game) {
			if (H2Config_spooky_boy)
				*Memory::GetAddress<s_player::e_character_type*>(0x51A67C) = s_player::e_character_type::Skeleton;

			char* mapName = Memory::GetAddress<char*>(0x47CF0C);
			s_object_placement_data placement;
			datum player_datum = s_player::getPlayerUnitDatumIndex(DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0)));
			typedef void(__cdecl t_set_orientation)(real_vector3d* forward, real_vector3d* up, real_point3d* orient);
			auto set_orientation = Memory::GetAddress<t_set_orientation*>(0x3347B);
			auto pump = tags::get_tag<blam_tag::tag_group_type::scenery, s_scenery_group_definition>(pump_datum, true);
			auto pump_hmlt = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(pump->objectTag.model.TagIndex, true);
			if (strcmp(mapName, "coagulation") == 0)
			{
				for (auto& scen_place : coag_scen_places)
				{
					switch (std::get<0>(scen_place))
					{
					case 0:
						Engine::Objects::create_new_placement_data(&placement, pump_datum, -1, 0);
						placement.variant_name = pump_hmlt->variants[std::get<1>(scen_place)]->name.get_packed();
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
					placement.position = std::get<2>(scen_place);

					set_orientation(&placement.orientation, &placement.up, &std::get<3>(scen_place));
					placement.scale = std::get<4>(scen_place);

					datum object_idx = Engine::Objects::object_new(&placement);
				}
			}
			if (strcmp(mapName, "lockout") == 0)
			{
				for (auto& scen_place : lockout_scen_places)
				{
					switch (std::get<0>(scen_place))
					{
					case 0:
						Engine::Objects::create_new_placement_data(&placement, pump_datum, -1, 0);
						placement.variant_name = pump_hmlt->variants[std::get<1>(scen_place)]->name.get_packed();
						break;
					case 1:
						Engine::Objects::create_new_placement_data(&placement, candle_datum, -1, 0);
						placement.variant_name = 0;
						break;
					}
					placement.position = std::get<2>(scen_place);

					set_orientation(&placement.orientation, &placement.up, &std::get<3>(scen_place));
					placement.scale = std::get<4>(scen_place);

					datum object_idx = Engine::Objects::object_new(&placement);
				}
			}
		}
	}

	void HalloweenOnMapLoad()
	{
		if (h2mod->GetEngineType() == _mutliplayer)
		{
			if (tag_loader::Map_exists("carto_shared"))
			{
				char* mapName = Memory::GetAddress<char*>(0x47CF0C);
				if (strcmp(mapName, "coagulation") == 0)
				{
					lbitm_datum = tag_loader::Get_tag_datum("scenarios\\multi\\halo\\coagulation\\coagulation_coagulation_lightmap_truecolor_bitmaps", blam_tag::tag_group_type::bitmap, "carto_shared");
					sky_datum = tag_loader::Get_tag_datum("scenarios\\skies\\multi\\halo\\coagulation\\coagulation_night", blam_tag::tag_group_type::sky, "carto_shared");
					candle_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\candle\\candle", blam_tag::tag_group_type::scenery, "carto_shared");
					candle_fire_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\candle\\candle_fire", blam_tag::tag_group_type::scenery, "carto_shared");
					pump_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\jack_o_lantern", blam_tag::tag_group_type::scenery, "carto_shared");
					large_candle_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\candle\\candle_big_light", blam_tag::tag_group_type::scenery, "carto_shared");
					tag_loader::Load_tag(pump_datum, true, "carto_shared");
					tag_loader::Load_tag(candle_datum, true, "carto_shared");
					tag_loader::Load_tag(lbitm_datum, true, "carto_shared");
					tag_loader::Load_tag(sky_datum, true, "carto_shared");
					tag_loader::Load_tag(large_candle_datum, true, "carto_shared");
					tag_loader::Push_Back();
				}
				if (strcmp(mapName, "lockout") == 0)
				{
					candle_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\candle\\candle", blam_tag::tag_group_type::scenery, "carto_shared");
					candle_fire_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\candle\\candle_fire", blam_tag::tag_group_type::scenery, "carto_shared");
					pump_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\jack_o_lantern\\jack_o_lantern", blam_tag::tag_group_type::scenery, "carto_shared");
					tag_loader::Load_tag(pump_datum, true, "carto_shared");
					tag_loader::Load_tag(candle_datum, true, "carto_shared");
					tag_loader::Push_Back();
				}

				auto scen = tags::get_tag_fast<s_scenario_group_definition>(tags::get_tags_header()->scenario_datum);
				auto sbps = tags::get_tag_fast< s_scenario_structure_bsp_group_definition>(scen->structure_bsps[0]->structure_bsp.TagIndex);
				if (strcmp(mapName, "coagulation") == 0)
				{
					candle_datum = tag_loader::ResolveNewDatum(candle_datum);
					candle_fire_datum = tag_loader::ResolveNewDatum(candle_fire_datum);
					pump_datum = tag_loader::ResolveNewDatum(pump_datum);
					large_candle_datum = tag_loader::ResolveNewDatum(large_candle_datum);
					LOG_INFO_GAME("{:x}", candle_datum);
					LOG_INFO_GAME("{:x}", candle_fire_datum);
					LOG_INFO_GAME("{:x}", pump_datum);
					LOG_INFO_GAME("{:x}", large_candle_datum);

					if (!DATUM_IS_NONE(sky_datum))
					{
						auto sky = tags::get_tag<blam_tag::tag_group_type::sky, char*>(tag_loader::ResolveNewDatum(sky_datum), true);
						auto render_model_ref = reinterpret_cast<tag_reference*>(sky);
						auto render_model = tags::get_tag<blam_tag::tag_group_type::rendermodel, s_render_model_group_definition>(render_model_ref->TagIndex, true);
						scen->skies[0]->sky.TagIndex = tag_loader::ResolveNewDatum(sky_datum);
					}
					auto ltmp_datum = tags::find_tag(blam_tag::tag_group_type::scenariostructurelightmap, "scenarios\\multi\\halo\\coagulation\\coagulation_coagulation_lightmap");
					if (!DATUM_IS_NONE(ltmp_datum) && !DATUM_IS_NONE(tag_loader::ResolveNewDatum(lbitm_datum))) 
					{
						auto ltmp = tags::get_tag_fast<s_scenario_structure_lightmap_group_definition>(ltmp_datum);
						ltmp->lightmap_groups[0]->bitmap_group.TagIndex = tag_loader::ResolveNewDatum(lbitm_datum);
						sbps->decorators_block.size = 0;
						sbps->decorators_block.data = 0;
					}


					if (!DATUM_IS_NONE(candle_datum) && !DATUM_IS_NONE(pump_datum) && !DATUM_IS_NONE(large_candle_datum))
						EventHandler::register_callback(halloween_game_life_cycle_update, EventType::gamelifecycle_change, EventExecutionType::execute_after, true);
				}
				if (strcmp(mapName, "lockout") == 0)
				{
					candle_datum = tag_loader::ResolveNewDatum(candle_datum);
					candle_fire_datum = tag_loader::ResolveNewDatum(candle_fire_datum);
					pump_datum = tag_loader::ResolveNewDatum(pump_datum);
					LOG_INFO_GAME("{:x}", candle_datum);
					LOG_INFO_GAME("{:x}", candle_fire_datum);
					LOG_INFO_GAME("{:x}", pump_datum);

					if (!DATUM_IS_NONE(candle_datum) && !DATUM_IS_NONE(pump_datum))
						EventHandler::register_callback(halloween_game_life_cycle_update, EventType::gamelifecycle_change, EventExecutionType::execute_after, true);
				}
			}
			else
			{
				if (NetworkSession::getCurrentNetworkSession()->local_peer_index != NetworkSession::getCurrentNetworkSession()->session_host_peer_index)
				{
					*Memory::GetAddress<int*>(0x46DCF1) = 1;
					imgui_handler::iMessageBox::SetMessage("Error: Cartographer Shared map content is missing. Try updating your game from the mainmenu.\r\n\r\nBy going to Cartographer > Update.\r\n\r\nIf that doesn't work reach out to us in #help on discord.");
					imgui_handler::ToggleWindow("messagebox");
				}
			}
		}
	}

	void AddNewMarkers()
	{
		if (h2mod->GetEngineType() == e_engine_type::_mutliplayer) {
			auto mode_elite_datum = tags::find_tag(blam_tag::tag_group_type::rendermodel, "objects\\characters\\elite\\elite_mp");
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
		ApplyHooks();
		tags::on_map_load(AddNewMarkers);
		switch (getCurrentEvent())
		{
		case e_christmas:
			if (H2Config_event_music) {
				auto playSound = [=]()
				{
					using namespace std::chrono_literals;
					static bool flop = false;
					while (true)
					{
						std::this_thread::sleep_for(1000ms);
						if (h2mod->GetEngineType() == _main_menu && H2Config_event_music)
						{
							if (!flop) {
								flop = true;
								PlaySound(L"sounds/feliz_navidad.wav", NULL, SND_FILENAME | SND_LOOP | SND_ASYNC | SND_NODEFAULT);
							}
						}
						else {
							flop = false;
							PlaySound(NULL, 0, 0);
						}
					}

				};
				std::thread(playSound).detach();
			}
			tags::on_map_load(ChristmasOnMapLoad);
			break;
		case e_st_paddys:
			tags::on_map_load(PaddysOnMapLoad);
			break;
		case e_mook_maddness:
			tags::on_map_load(MookMaddnessOnMapLoad);
			break;
		case e_halloween:
			tags::on_map_load(HalloweenOnMapLoad);
			break;
		}
	}
}
