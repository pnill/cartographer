#include "KantTesting.h"
#include "Blam/Engine/Players/Players.h"
#include "H2MOD/Modules/Networking/Memory/bitstream.h"
#include "H2MOD/EngineCalls/EngineCalls.h"
#include "Blam/Cache/TagGroups/model_defenition.hpp"
#include "H2MOD/Tags/MetaExtender.h"
#include "H2MOD/Tags/MetaLoader/tag_loader.h"
#include "H2MOD/Modules/Config/Config.h"
#include "Util/Hooks/hook.h"
#include "H2MOD/Modules/Networking/CustomPackets/CustomPackets.h"
#include "H2MOD/Modules/EventHandler/EventHandler.hpp"
#include "Blam/Cache/TagGroups/biped_definition.hpp"
#include "Blam/Cache/TagGroups/globals_definition.hpp"
#include "Blam/Cache/TagGroups/scenario_definition.hpp"
#include "Blam/LazyBlam/LazyBlam.hpp"
#include "Blam/Cache/TagGroups/weapon_definition.hpp"
#include "Blam/Engine/Objects/GameStateObjects.h"
#include "H2MOD/Modules/Console/ConsoleCommands.h"


namespace KantTesting
{
	std::map<std::wstring, byte> temp;
	std::map<int, string_id> variants;
	string_id variant_temp;

	void testApply()
	{
		switch (EngineCalls::get_game_life_cycle())
		{
		case life_cycle_pre_game:
		case life_cycle_in_game:
		case life_cycle_joining:
		{
			PlayerIterator it;
			while (it.get_next_active_player())
			{
				if (temp.find(it.get_current_player_name()) != temp.end())
				{
					it.get_current_player_data()->properties[0].unk2 = temp[it.get_current_player_name()];
					it.get_current_player_data()->properties[1].unk2 = temp[it.get_current_player_name()];
				}
			}
			break;
		}
		case life_cycle_none:
		case life_cycle_start_game:
		case life_cycle_post_game:
		case life_cycle_matchmaking:
		default:
			temp.clear();
			break;
		}

	}

	typedef void(__cdecl t_encode_player_add)(bitstream* stream, int unk, char* data);
	t_encode_player_add* p_encode_player_add;

	void __cdecl encode_player_add_impl(bitstream* stream, int unk, char* data)
	{
		byte val = rand() % variants.size();
		stream->data_encode_integer("test", val, 8);
		Player::Properties* prop = (Player::Properties*)(data + 28);
		if (temp.find(prop->player_name) == temp.end())
		{
			temp[prop->player_name] = val;
		}
		p_encode_player_add(stream, unk, data);
	}

	typedef bool(__cdecl t_decode_player_add)(bitstream* stream, int unk, char* data);
	t_decode_player_add* p_decode_player_add;

	bool __cdecl decode_player_add_impl(bitstream* stream, int unk, char* data)
	{
		byte val = stream->data_decode_integer("test", 8);
		Player::Properties* prop = (Player::Properties*)(data + 28);
		auto res = p_decode_player_add(stream, unk, data);
		if (temp.find(prop->player_name) == temp.end())
		{
			temp[prop->player_name] = val;
		}
		prop->unk2 = val;
		LOG_ERROR_GAME(L"[DECODE PACKET] {} {}", prop->player_name, prop->unk2);
		return res;
	}


	typedef void(__cdecl t_encode_player_properties)(bitstream* stream, int unk, char* data);
	t_encode_player_properties* p_encode_player_properties;
	void __cdecl encode_player_properties_impl(bitstream* stream, int unk, char* data)
	{
		byte val = rand() % variants.size();
		stream->data_encode_integer("test", val, 8);
		Player::Properties* prop = (Player::Properties*)(data + 16);
		if (temp.find(prop->player_name) == temp.end())
		{
			temp[prop->player_name] = val;
		}
		p_encode_player_properties(stream, unk, data);
	}

	typedef bool(__cdecl t_decode_player_properties)(bitstream* stream, int unk, char* data);
	t_decode_player_properties* p_decode_player_properties;
	bool __cdecl decode_player_properties_impl(bitstream* stream, int unk, char* data)
	{
		int val = stream->data_decode_integer("test", 8);
		Player::Properties* prop = (Player::Properties*)(data + 16);
		auto res = p_decode_player_properties(stream, unk, data);
		if (temp.find(prop->player_name) == temp.end())
		{
			temp[prop->player_name] = val;
		}
		prop->unk2 = val;
		LOG_ERROR_GAME(L"[DECODE PACKET] {} {}", prop->player_name, prop->unk2);
		return res;
	}


	void fix_elite_model_variant()
	{
		if (h2mod->GetEngineType() == e_engine_type::Multiplayer)
		{
			datum mode_elite_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\elite\\elite");
			if (mode_elite_datum.IsNull())
			{
				auto e_datum = tag_loader::Get_tag_datum("objects\\characters\\elite\\elite", blam_tag::tag_group_type::model, "00a_introduction");
				tag_loader::Load_tag(e_datum.ToInt(), false, "00a_introduction");
				tag_loader::Push_Back();
				mode_elite_datum = tag_loader::ResolveNewDatum(e_datum.ToInt());
			}
			//
			auto mode_elite = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(mode_elite_datum, true);
			auto mode_elite_mp_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\elite\\elite_mp");
			auto mode_elite_mp = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(mode_elite_mp_datum);

			variant_temp = mode_elite->variants[17]->name.get_id();
			for (auto i = 0; i < mode_elite->variants.size; i++) {
				auto variant = mode_elite->variants[i];
				variants[i] = variant->name;
				auto new_variant_group = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block>((unsigned long)std::addressof(mode_elite_mp->variants));
				new_variant_group->name = variant->name;
				new_variant_group->dialogue.TagGroup = variant->dialogue.TagGroup;
				new_variant_group->dialogue.TagIndex = variant->dialogue.TagIndex;
				new_variant_group->dialogue_sound_effect = variant->dialogue_sound_effect;
				new_variant_group->runtime_model_region_0 = -1;
				new_variant_group->runtime_model_region_1 = 3;
				new_variant_group->runtime_model_region_2 = 0;
				new_variant_group->runtime_model_region_3 = 1;
				new_variant_group->runtime_model_region_4 = 2;
				new_variant_group->runtime_model_region_5 = -1;
				new_variant_group->runtime_model_region_6 = -1;
				new_variant_group->runtime_model_region_7 = -1;
				new_variant_group->runtime_model_region_8 = -1;
				new_variant_group->runtime_model_region_9 = -1;
				new_variant_group->runtime_model_region_10 = -1;
				new_variant_group->runtime_model_region_11 = -1;
				new_variant_group->runtime_model_region_12 = -1;
				new_variant_group->runtime_model_region_13 = -1;
				new_variant_group->runtime_model_region_14 = -1;
				new_variant_group->runtime_model_region_15 = -1;

				for (auto j = 0; j < variant->regions.size; j++)
				{
					auto region = variant->regions[j];
					auto new_region = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_regions_block>((unsigned long)std::addressof(new_variant_group->regions));
					new_region->region_name = region->region_name;
					new_region->runtime_model_region_index = region->runtime_model_region_index;
					new_region->region_runtime_flags = region->region_runtime_flags;
					new_region->parent_variant = region->parent_variant;
					new_region->sort_order = region->sort_order;

					for (auto k = 0; k < region->permutations.size; k++)
					{
						auto permutation = region->permutations[k];
						auto new_permutation = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_regions_block::s_permutations_block>((unsigned long)std::addressof(new_region->permutations));
						new_permutation->permutation_name = permutation->permutation_name;
						new_permutation->model_permutation_index = permutation->model_permutation_index;
						new_permutation->flags = permutation->flags;
						new_permutation->probability_0 = permutation->probability_0;
						new_permutation->runtime_permutation_index_0 = permutation->runtime_permutation_index_0;
						new_permutation->runtime_permutation_index_1 = permutation->runtime_permutation_index_1;
						new_permutation->runtime_permutation_index_2 = permutation->runtime_permutation_index_2;
						new_permutation->runtime_permutation_index_3 = permutation->runtime_permutation_index_3;
						new_permutation->runtime_permutation_index_4 = permutation->runtime_permutation_index_4;
						new_permutation->unk_1 = permutation->unk_1;
						new_permutation->unk2 = permutation->unk2;
						new_permutation->unk3 = permutation->unk3;
					}
				}
			}

			auto bipd_d = tags::find_tag(blam_tag::tag_group_type::biped, "objects\\characters\\elite\\elite_mp");
			auto bipd = tags::get_tag<blam_tag::tag_group_type::biped, s_biped_group_definition>(bipd_d);
			LOG_INFO_GAME("[{}] {:x}", __FUNCTION__, (unsigned long)std::addressof(*bipd));
			auto nbipd = calloc(1, sizeof(s_biped_group_definition));
			memcpy(nbipd, &bipd, sizeof(s_biped_group_definition));
			reinterpret_cast<s_biped_group_definition*>(nbipd)->unitTag.objectTag.default_model_variant = variant_temp;
			tags::tag_instance a = tag_loader::AddNewtagInstance(blam_tag::tag_group_type::biped, *(int*)nbipd, sizeof(s_biped_group_definition));
			auto globals_datum = tags::find_tag(blam_tag::tag_group_type::globals, "globals\\globals");
			tag_loader::Fix_global_objects_ref(a.datum_index);
			LOG_INFO_GAME("[{}] {:X}", __FUNCTION__, a.datum_index.ToInt());
			if (globals_datum != -1) {
				auto globals = tags::get_tag<blam_tag::tag_group_type::globals, s_globals_group_definition>(globals_datum);
				auto new_rep = MetaExtender::add_tag_block2<s_globals_group_definition::s_player_representation_block>((unsigned long)std::addressof(globals->player_representation));
				globals->player_representation[3]->third_person_variant = variant_temp;
				new_rep->first_person_body = globals->player_representation[3]->first_person_body;
				new_rep->first_person_hands = globals->player_representation[3]->first_person_hands;
				new_rep->third_person_unit.TagGroup = blam_tag::tag_group_type::biped;
				new_rep->third_person_unit.TagIndex = a.datum_index;
				new_rep->third_person_variant = variant_temp;
				LOG_INFO_GAME("[{}] {:x}", __FUNCTION__, (unsigned long)std::addressof(globals->player_representation[4]->third_person_variant));
			}
			//auto scen_datum = tags::find_tag(blam_tag::tag_group_type::scenario, "scenarios\\shared\\shared");
			//if(scen_datum != -1)
			//{
				//auto b = datum(0xE1782713);
				auto scen = Memory::GetAddress<s_scenario_group_definition*>(0x479e74);
				auto new_def = MetaExtender::add_tag_block2<s_scenario_group_definition::s_simulation_definition_table_block>((unsigned long)std::addressof(scen->simulation_definition_table));
				new_def->tag = 0xF26B3805;
			//}
		}
	}

	void add_elite_variants()
	{
		if (variants.size() > 0) {
			auto globals_datum = tags::find_tag(blam_tag::tag_group_type::globals, "globals\\globals");
			if (globals_datum != -1) {
				auto globals = tags::get_tag<blam_tag::tag_group_type::globals, s_globals_group_definition>(globals_datum);
				auto new_rep = MetaExtender::add_tag_block2<s_globals_group_definition::s_player_representation_block>((unsigned long)std::addressof(globals->player_representation));
				new_rep->first_person_body = globals->player_representation[3]->first_person_body;
				new_rep->first_person_hands = globals->player_representation[3]->first_person_hands;
				new_rep->third_person_unit = globals->player_representation[3]->third_person_unit;
				new_rep->third_person_variant = variants.at(10).get_id();
			}
		}
	}

	//typedef int(__cdecl object_placement_new_def)(s_object_placement_data* s_object_placement_data, datum a2, datum a3, int a4);
	//object_placement_new_def* imp_object_placement_data;
	//int __cdecl object_placement_new(s_object_placement_data* object_placement_data, datum a2, datum a3, int a4)
	//{
	//	auto res = imp_object_placement_data(object_placement_data, a2, a3, a4);
	//	if (a2 == 0xF29A3834) {
	//		if (object_placement_data->player_index != -1) {
	//			auto globals_datum = tags::find_tag(blam_tag::tag_group_type::globals, "globals\\globals");
	//			if (globals_datum != -1) {
	//				auto globals = tags::get_tag<blam_tag::tag_group_type::globals, s_globals_group_definition>(globals_datum);
	//				object_placement_data->variant_index = globals->player_representation[Player::getPlayer(object_placement_data->player_index.ToAbsoluteIndex())->properties->profile.player_character_type]->third_person_variant.get_id();
	//			}
	//		}
	//	}
	//	return res;
	//}

	typedef signed int(__cdecl t_get_model_variant)(unsigned __int16 object_index, string_id model_variant);
	t_get_model_variant* p_get_model_variant;
	signed int __cdecl get_model_variant(unsigned __int16 object_index, string_id model_variant)
	{
		//auto tag_instance = tags::get_tag_instances()[(*(datum*)s_game_state_objects::getObject(object_index)).ToAbsoluteIndex()];
		//auto tag = reinterpret_cast<s_biped_group_definition*>(tags::get_tag_data() + tag_instance.data_offset);
		auto res = p_get_model_variant(object_index, model_variant);
		LOG_INFO_GAME("[{}] {:x} {:x} {} ", __FUNCTION__, object_index, res, model_variant.get_id());
		if (model_variant.get_id() == 4214)
			return 15;
		//LOG_INFO_GAME("[{}] {:x} {:x} {:x} {} {}", __FUNCTION__, tag_instance.datum_index.ToInt(), object_index, res, model_variant.get_id(), tags::get_tag_name(tag_instance.datum_index));
		return res;
	}

	typedef int(__cdecl t_object_new)(s_object_placement_data* data);
	t_object_new* p_object_new;

	int __cdecl object_new_impl(s_object_placement_data* data)
	{
		//data->variant_index = variants[Player::getPlayer(data->player_index.Index)->properties[0].unk2].get_id();
		return p_object_new(data);
	}

	typedef void(__cdecl *register_player_packets)(void* a1);
	register_player_packets register_player_packets_method;

	void __cdecl registerPlayerPackets(void* packetObject) {

		register_packet_impl(packetObject, 25, "membership-update", 0, 15784, 15784,
			Memory::GetAddress<void*>(0x1EF6B9, 0x1D0072),
			Memory::GetAddress<void*>(0x1EFADD, 0x1D0496), 0);

		register_packet_impl(packetObject, 37, "virtual-couch-update", 0, 216, 216,
			Memory::GetAddress<void*>(0x1F0042, 0x1D09FB),
			Memory::GetAddress<void*>(0x1F0143, 0x1D0AFC), 0);

		register_packet_impl(packetObject, 38, "virtual-couch-request", 0, 208, 208,
			Memory::GetAddress<void*>(0x1F0264, 0x1D0C1D),
			Memory::GetAddress<void*>(0x1F031D, 0x1D0CD6), 0);

		register_packet_impl(packetObject, 26, "peer-properties", 0, 208, 208,
			Memory::GetAddress<void*>(0x1F03F5, 0x1D0DAE),
			Memory::GetAddress<void*>(0x1F04E0, 0x1D0E99), 0);

		register_packet_impl(packetObject, 27, "delegate-leadership", 0, 44, 44,
			Memory::GetAddress<void*>(0x1F05EE, 0x1D0FA7),
			Memory::GetAddress<void*>(0x1F061A, 0x1D0FD3), 0);

		register_packet_impl(packetObject, 28, "boot-machine", 0, 44, 44,
			Memory::GetAddress<void*>(0x1F0652, 0x1D100B),
			Memory::GetAddress<void*>(0x1F067E, 0x1D1037), 0);

		register_packet_impl(packetObject, 29, "player-add", 0, 168, 168,
			(void*)encode_player_add_impl, (void*)decode_player_add_impl, 0);
		//Memory::GetAddress<void*>(0x1F06B6, 0x1D106F),
		//Memory::GetAddress<void*>(0x1F0752, 0x1D110B), 0);
		//(void*)deserializePlayerAdd, 0);

		register_packet_impl(packetObject, 30, "player-refuse", 0, 20, 20,
			Memory::GetAddress<void*>(0x1F081F, 0x1D11D8),
			Memory::GetAddress<void*>(0x1F085F, 0x1D1218), 0);

		register_packet_impl(packetObject, 31, "player-remove", 0, 12, 12,
			Memory::GetAddress<void*>(0x1F08BC, 0x1D1275),
			Memory::GetAddress<void*>(0x1F08EA, 0x1D12A3), 0);

		register_packet_impl(packetObject, 32, "player-properties", 0, 156, 156,
			(void*)encode_player_properties_impl, (void*)decode_player_properties_impl, 0);
		//Memory::GetAddress<void*>(0x1F0935, 0x1D12EE),
		//Memory::GetAddress<void*>(0x1F09AC, 0x1D1365), 0);
	}

	void mapLoad()
	{
		auto lb = new lazy_blam::lazy_blam("shared.map");
		auto t = lb->get_tag_data<s_weapon_group_definition>(0xE90C2EA7);

		LOG_INFO_GAME("[{}] {} {:x} {:x} {:x} {}", __FUNCTION__, 
			t->barrels[0]->acceleration_time, 
			t->barrels[0]->firing_effects[0]->firing_effect.TagIndex.ToInt(),
			t->new_triggers[0]->charging_effect.TagIndex.ToInt(),
			t->predicted_resources_1[5]->tag_index.ToInt(),
			t->attachments[0]->marker_old_string_id.get_id());
		auto a = 0;
		//lazy_blam::init_cache_file("shared.map");
		////auto elite_d = lazy_blam::get_datum_from_name("objects\\characters\\dervish\\dervish", blam_tag::tag_group_type::biped);
		////auto elite_data = lazy_blam::get_tag_data<s_biped_group_definition>(elite_d);
		////LOG_INFO_GAME("{}", elite_data->contact_points.data);
		//auto chief_d = lazy_blam::get_datum_from_name("objects\\characters\\masterchief\\masterchief", blam_tag::tag_group_type::biped);
		//auto chief_data = lazy_blam::get_tag_data<s_biped_group_definition>(chief_d);

		//LOG_INFO_GAME("[{}] {}", __FUNCTION__, chief_data->unitTag.objectTag.change_colors[0]->initial_permutations[0]->weight);
		//lazy_blam::clear_loaded_tags();
	}

	void player_representation_testing()
	{

		if (h2mod->GetEngineType() == e_engine_type::Multiplayer)
		{
			datum mode_elite_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\elite\\elite");
			if (mode_elite_datum.IsNull())
			{
				auto e_datum = tag_loader::Get_tag_datum("objects\\characters\\elite\\elite", blam_tag::tag_group_type::model, "00a_introduction");
				tag_loader::Load_tag(e_datum.ToInt(), false, "00a_introduction");
				tag_loader::Push_Back();
				mode_elite_datum = tag_loader::ResolveNewDatum(e_datum.ToInt());
			}
			//
			auto mode_elite = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(mode_elite_datum, true);
			auto mode_elite_mp_datum = tags::find_tag(blam_tag::tag_group_type::model, "objects\\characters\\elite\\elite_mp");
			auto mode_elite_mp = tags::get_tag<blam_tag::tag_group_type::model, s_model_group_definition>(mode_elite_mp_datum);

			for (auto i = 0; i < mode_elite->variants.size; i++) {
				auto variant = mode_elite->variants[i];
				auto new_variant_group = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block>((unsigned long)std::addressof(mode_elite_mp->variants));
				new_variant_group->name = variant->name;
				new_variant_group->dialogue.TagGroup = variant->dialogue.TagGroup;
				new_variant_group->dialogue.TagIndex = variant->dialogue.TagIndex;
				new_variant_group->dialogue_sound_effect = variant->dialogue_sound_effect;
				new_variant_group->runtime_model_region_0 = -1;
				new_variant_group->runtime_model_region_1 = 3;
				new_variant_group->runtime_model_region_2 = 0;
				new_variant_group->runtime_model_region_3 = 1;
				new_variant_group->runtime_model_region_4 = 2;
				new_variant_group->runtime_model_region_5 = -1;
				new_variant_group->runtime_model_region_6 = -1;
				new_variant_group->runtime_model_region_7 = -1;
				new_variant_group->runtime_model_region_8 = -1;
				new_variant_group->runtime_model_region_9 = -1;
				new_variant_group->runtime_model_region_10 = -1;
				new_variant_group->runtime_model_region_11 = -1;
				new_variant_group->runtime_model_region_12 = -1;
				new_variant_group->runtime_model_region_13 = -1;
				new_variant_group->runtime_model_region_14 = -1;
				new_variant_group->runtime_model_region_15 = -1;

				for (auto j = 0; j < variant->regions.size; j++)
				{
					auto region = variant->regions[j];
					auto new_region = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_regions_block>((unsigned long)std::addressof(new_variant_group->regions));
					new_region->region_name = region->region_name;
					new_region->runtime_model_region_index = region->runtime_model_region_index;
					new_region->region_runtime_flags = region->region_runtime_flags;
					new_region->parent_variant = region->parent_variant;
					new_region->sort_order = region->sort_order;

					for (auto k = 0; k < region->permutations.size; k++)
					{
						auto permutation = region->permutations[k];
						auto new_permutation = MetaExtender::add_tag_block2<s_model_group_definition::s_variants_block::s_regions_block::s_permutations_block>((unsigned long)std::addressof(new_region->permutations));
						new_permutation->permutation_name = permutation->permutation_name;
						new_permutation->model_permutation_index = permutation->model_permutation_index;
						new_permutation->flags = permutation->flags;
						new_permutation->probability_0 = permutation->probability_0;
						new_permutation->runtime_permutation_index_0 = permutation->runtime_permutation_index_0;
						new_permutation->runtime_permutation_index_1 = permutation->runtime_permutation_index_1;
						new_permutation->runtime_permutation_index_2 = permutation->runtime_permutation_index_2;
						new_permutation->runtime_permutation_index_3 = permutation->runtime_permutation_index_3;
						new_permutation->runtime_permutation_index_4 = permutation->runtime_permutation_index_4;
						new_permutation->unk_1 = permutation->unk_1;
						new_permutation->unk2 = permutation->unk2;
						new_permutation->unk3 = permutation->unk3;
					}
				}
			}
			auto globals_datum = tags::find_tag(blam_tag::tag_group_type::globals, "globals\\globals");
			if (globals_datum != -1) 
			{
				auto globals = tags::get_tag<blam_tag::tag_group_type::globals, s_globals_group_definition>(globals_datum);
				if (globals_datum != -1) 
				{
					auto globals = tags::get_tag<blam_tag::tag_group_type::globals, s_globals_group_definition>(globals_datum);
					globals->player_representation[3]->third_person_variant = string_id(0x0b001076);
				}
			}
		}
		if (h2mod->GetEngineType() == e_engine_type::Multiplayer) {
			auto tag_path = "objects\\characters\\masterchief_skeleton\\masterchief_skeleton";
			auto skele_datum = tag_loader::Get_tag_datum(tag_path, blam_tag::tag_group_type::biped, "carto_shared");
			if (!skele_datum.IsNull())
			{
				tag_loader::Load_tag(skele_datum.ToInt(), true, "carto_shared");
				tag_loader::Push_Back();
				auto skele_new_datum = tag_loader::ResolveNewDatum(skele_datum.ToInt());
				LOG_INFO_GAME("[{}] {:x}", __FUNCTION__, skele_new_datum.ToInt());
				auto globals_datum = tags::find_tag(blam_tag::tag_group_type::globals, "globals\\globals");
				if (globals_datum != -1) {
					auto globals = tags::get_tag<blam_tag::tag_group_type::globals, s_globals_group_definition>(globals_datum);
					auto new_rep = MetaExtender::add_tag_block2<s_globals_group_definition::s_player_representation_block>((unsigned long)std::addressof(globals->player_representation));
					new_rep->first_person_body = globals->player_representation[0]->first_person_body;
					new_rep->first_person_hands = globals->player_representation[0]->first_person_hands;
					new_rep->third_person_unit.TagIndex = skele_new_datum;
					new_rep->third_person_unit.TagGroup = blam_tag::tag_group_type::biped;
					globals->player_representation[3]->third_person_variant = string_id(0x0b001076);
					//globals->player_representation[0]->third_person_unit.TagIndex = skele_new_datum;
					
					auto scen = tags::get_tag_fast<s_scenario_group_definition>(tags::get_tags_header()->scenario_datum);
					auto new_def = MetaExtender::add_tag_block2<s_scenario_group_definition::s_simulation_definition_table_block>((unsigned long)std::addressof(scen->simulation_definition_table));
					new_def->tag = skele_new_datum;
					new_def = MetaExtender::add_tag_block2<s_scenario_group_definition::s_simulation_definition_table_block>((unsigned long)std::addressof(scen->simulation_definition_table));
					auto dervish = tags::find_tag(blam_tag::tag_group_type::biped, "objects\\characters\\dervish\\dervish");
					new_def->tag = dervish;
				}
			}
		}
	}

	//typedef void(__cdecl t_network_session_player_profile_recieve)(int a1, Player::Properties* a2);
	//t_network_session_player_profile_recieve* p_network_session_player_profile_recieve;

	//void __cdecl network_ession_player_profile_recieve(int a1, Player::Properties* a2)
	//{
	//	
	//}

	void Initialize()
	{
		if (ENABLEKANTTEST) {
			if (!Memory::isDedicatedServer()) {
				//tags::on_map_load(mapLoad);
			/*	register_player_packets_method = (register_player_packets)DetourFunc(Memory::GetAddress<BYTE*>(0x1F0A55, 0x1D140E), (BYTE*)registerPlayerPackets, 5);

				p_encode_player_add = Memory::GetAddress<t_encode_player_add*>(0x1F06B6);
				p_decode_player_add = Memory::GetAddress<t_decode_player_add*>(0x1F0752);
				p_encode_player_properties = Memory::GetAddress<t_encode_player_properties*>(0x1F0935);
				p_decode_player_properties = Memory::GetAddress<t_decode_player_properties*>(0x1F09AC);

				EventHandler::register_callback<EventHandler::GameLoopEvent>(testApply);
				p_object_new = Memory::GetAddress<t_object_new*>(0x136CA7);
				PatchCall(Memory::GetAddress(0x55C06), object_new_impl);*/

				//imp_object_placement_data = Memory::GetAddress<object_placement_new_def*>(0x132163);
				//PatchCall(Memory::GetAddress(0x55B43), object_placement_new);
				//NopFill(Memory::GetAddress(0x55B5A), 7);



				//p_get_model_variant = Memory::GetAddress<t_get_model_variant*>(0x12FE84);
				//PatchCall(Memory::GetAddress(0x12FEF1), get_model_variant);
				//tags::on_map_load(fix_elite_model_variant);
				//tags::on_map_load(add_elite_variants);

				//network_session_player_profile_recieve
				//increase MaxValue of encode_integer
				WriteValue<byte>(Memory::GetAddress(0x54fb3), 5);
				//player_properties_encode
				//Increase MaxValue of encode_integer
				WriteValue<byte>(Memory::GetAddress(0x1e369a), 5);
				//player_properties_decode
				//Increase MaxValue of Decode_Integer
				WriteValue<byte>(Memory::GetAddress(0x1e3745), 5);
				//Stop the game from overriding the player biped
				NopFill(Memory::GetAddress(0x52fc5), 3);
				//Stop  the game from overriding the player biped
				//NopFill(Memory::GetAddress(0x52fF5), 3);
				tags::on_map_load(player_representation_testing);
			}
		}
	}
}
