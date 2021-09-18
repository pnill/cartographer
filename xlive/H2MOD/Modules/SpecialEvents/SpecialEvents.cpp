
#include "SpecialEvents.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"
#include "H2MOD/Tags/MetaExtender.h"
#include "Blam/Enums/Game/HaloStrings.h"
#include "H2MOD/Modules/Networking/Networking.h"
#include "H2MOD/GUI/imgui_integration/imgui_handler.h"
#include "H2MOD/Modules/Config/Config.h"

#include "Blam/Cache/TagGroups/model_defenition.hpp"
#include "Blam/Cache/TagGroups/render_model_definition.hpp"
#include "Blam/Cache/TagGroups/weapon_definition.hpp"
#include "Blam/Cache/TagGroups/scenery_definition.hpp"

namespace SpecialEvents
{
	namespace
	{
		datum mook_ball_datum = DATUM_NONE;

		datum paddy_hat_datum = DATUM_NONE;
		datum paddy_beard_datum = DATUM_NONE;
		datum paddy_pot_datum = DATUM_NONE;

		datum santa_hat_datum = DATUM_NONE;

		string_id new_elite_head_marker(0xFFEE01234);

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
	}

	void ChristmasOnMapLoad()
	{
		if (h2mod->GetEngineType() == e_engine_type::Multiplayer)
		{
			santa_hat_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\christmas_hat_map\\hat\\hat", blam_tag::tag_group_type::scenario, "carto_shared");
			if (!DATUM_IS_NONE(santa_hat_datum)) {
				tag_loader::Load_tag(santa_hat_datum, true, "carto_shared");
				tag_loader::Push_Back();
				//auto scen = tags::get_tag<blam_tag::tag_group_type::scenery, s_scenery_group_definition>(datum(_INJECTED_TAG_START_));
				auto hlmt_chief_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief_mp");
				if (hlmt_chief_datum != DATUM_NONE) {
					auto hlmt_chief = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(hlmt_chief_datum);
					auto b = hlmt_chief->variants[0];
					auto a = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(b->objects));
					a->parent_marker = string_id(184552154);
					a->child_object.TagGroup = blam_tag::tag_group_type::scenery;
					a->child_object.TagIndex = tag_loader::ResolveNewDatum(santa_hat_datum);
				}
				auto hlmt_elite_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\elite\\elite_mp");
				if (hlmt_elite_datum != DATUM_NONE)
				{
					auto hlmt_eliete = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(hlmt_elite_datum);
					auto b = hlmt_eliete->variants[0];
					auto a = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(b->objects));
					a->parent_marker = new_elite_head_marker;
					a->child_object.TagGroup = blam_tag::tag_group_type::scenery;
					a->child_object.TagIndex = tag_loader::ResolveNewDatum(santa_hat_datum);
				}
			}
		}
	}

	void PaddysOnMapLoad()
	{
		if (h2mod->GetEngineType() == e_engine_type::Multiplayer)
		{
			if (tag_loader::Map_exists("carto_shared"))
			{
				paddy_hat_datum = tag_loader::Get_tag_datum("objects\\multi\\stpat_hat\\stpat_hat", blam_tag::tag_group_type::scenario, "carto_shared");
				paddy_beard_datum = tag_loader::Get_tag_datum("objects\\multi\\stpat_hat\\beard\\beard", blam_tag::tag_group_type::scenario, "carto_shared");
				paddy_pot_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\pot_of_gold\\pot_of_gold", blam_tag::tag_group_type::scenario, "carto_shared");
				if (!DATUM_IS_NONE(paddy_hat_datum) && !DATUM_IS_NONE(paddy_beard_datum) && !DATUM_IS_NONE(paddy_pot_datum))
				{
					tag_loader::Load_tag(paddy_hat_datum, true, "carto_shared");
					tag_loader::Load_tag(paddy_beard_datum, true, "carto_shared");
					tag_loader::Load_tag(paddy_pot_datum, true, "carto_shared");
					tag_loader::Push_Back();
					auto hlmt_chief_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief");
					if (hlmt_chief_datum != DATUM_NONE) {
						auto hlmt_chief = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(hlmt_chief_datum);
						auto b = hlmt_chief->variants[0];
						auto hat = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(b->objects));
						hat->parent_marker = string_id(Blam::Enums::Game::HS_HEAD);
						hat->child_object.TagGroup = blam_tag::tag_group_type::scenery;
						hat->child_object.TagIndex = tag_loader::ResolveNewDatum(paddy_hat_datum);
						auto beard = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(b->objects));
						beard->parent_marker = string_id(Blam::Enums::Game::HS_HEAD);
						beard->child_object.TagGroup = blam_tag::tag_group_type::scenery;
						beard->child_object.TagIndex = tag_loader::ResolveNewDatum(paddy_beard_datum);

					}
					auto hlmt_chief_mp_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\masterchief\\masterchief_mp");
					if (hlmt_chief_mp_datum != DATUM_NONE) {
						auto hlmt_chief_mp = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(hlmt_chief_mp_datum);
						auto b = hlmt_chief_mp->variants[0];
						auto hat = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(b->objects));
						hat->parent_marker = string_id(Blam::Enums::Game::HS_HEAD);
						hat->child_object.TagGroup = blam_tag::tag_group_type::scenery;
						hat->child_object.TagIndex = tag_loader::ResolveNewDatum(paddy_hat_datum);
						auto beard = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_objects_block>((unsigned long)std::addressof(b->objects));
						beard->parent_marker = string_id(Blam::Enums::Game::HS_HEAD);
						beard->child_object.TagGroup = blam_tag::tag_group_type::scenery;
						beard->child_object.TagIndex = tag_loader::ResolveNewDatum(paddy_beard_datum);
					}
					auto hlmt_elite_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\elite\\elite_mp");
					if (hlmt_elite_datum != DATUM_NONE)
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
				char* ball_weapon = tags::get_tag<blam_tag::tag_group_type::weapon, char>(ball_weapon_datum);

				//Bounding Radius and Sweetener size
				//*((float*)ball_weapon + 0x4) = 0.3f;
				//*((byte*)ball_weapon + 0x1A) = 1;

				auto bomb_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\assault_bomb\\assault_bomb");
				char* bomb_weapon = tags::get_tag<blam_tag::tag_group_type::weapon, char>(bomb_weapon_datum);

				//Bounding Radius and Sweetener size
				//*((float*)bomb_weapon + 0x4) = 0.3f;
				//*((byte*)bomb_weapon + 0x1A) = 1;

				/*auto ball_weapon_datum = tags::find_tag(blam_tag::tag_group_type::weapon, "objects\\weapons\\multiplayer\\ball\\ball");
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
							fp_model->TagIndex = paddy_pot_model->render_model.TagIndex;
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
							fp_model->TagIndex = paddy_pot_model->render_model.TagIndex;
						}
					}
				}*/

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
		if (h2mod->GetEngineType() == e_engine_type::Multiplayer)
		{
			if (tag_loader::Map_exists("carto_shared"))
			{
				mook_ball_datum = tag_loader::Get_tag_datum("scenarios\\objects\\multi\\carto_shared\\basketball\\basketball", blam_tag::tag_group_type::weapon, "carto_shared");
				if(!DATUM_IS_NONE(mook_ball_datum))
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

	void AddNewMarkers()
	{
		if (h2mod->GetEngineType() == e_engine_type::Multiplayer) {
			auto mode_elite_datum = tags::find_tag(blam_tag::tag_group_type::rendermodel, "objects\\characters\\elite\\elite_mp");
			auto mode_elite = tags::get_tag<blam_tag::tag_group_type::rendermodel, s_render_model_group_definition>(mode_elite_datum);
			auto new_marker_group = MetaExtender::add_tag_block2<s_render_model_group_definition::s_marker_groups_block>((unsigned long)std::addressof(mode_elite->marker_groups));
			new_marker_group->name_old_string_id = new_elite_head_marker;
			auto new_marker = MetaExtender::add_tag_block2<s_render_model_group_definition::s_marker_groups_block::s_markers_block>((unsigned long)std::addressof(new_marker_group->markers));
			new_marker->node_index = 19;
			new_marker->permutation_index = -1;
			new_marker->region_index = -1;
			new_marker->translation_x = 0.005;
			new_marker->translation_y = 0.05;
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
		if (!H2Config_no_events) {
			tags::on_map_load(AddNewMarkers);
			if (CheckIfEventTime(L"3-17"))
				tags::on_map_load(PaddysOnMapLoad);

			if (CheckIfEventTime(L"12-25"))
				tags::on_map_load(ChristmasOnMapLoad);

			if(CheckIfEventTime(L"4-12"))
				tags::on_map_load(MookMaddnessOnMapLoad);
		}
	}
}