#include "stdafx.h"
#include "players.h"

#include "Blam/Cache/TagGroups/scenario_definition.hpp"
#include "Blam/Engine/Game/game/game_engine.h"
#include "Blam/Engine/Game/networking/logic/network_life_cycle.h"
#include "Blam/Engine/Game/networking/session/network_session.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/SpecialEvents/SpecialEvents.h"
#include "H2MOD/Modules/PlayerRepresentation/PlayerRepresentation.h"
#include "H2MOD/Tags/TagInterface.h"
#include "Util/Hooks/Hook.h"

/*
	- TO NOTE:
	- This functions work only after the game has started (game life cycle is in_game or after map has been loaded).
	- If you need to do something in the pregame lobby, use the functions available in Network Session (H2MOD/Modules/Networking/NetworkSession)
*/
typedef void(__cdecl* change_team_t)(int a1, int a2);
change_team_t p_change_local_team;

s_data_array* s_player::GetArray()
{
	return *Memory::GetAddress<s_data_array**>(0x4A8260, 0x4D64C4);
}

#pragma region s_player functions
bool s_player::IndexValid(int playerIndex)
{
	return playerIndex >= 0 && playerIndex < ENGINE_MAX_PLAYERS;
}

s_player* s_player::GetPlayer(int playerIndex)
{
	if (!IndexValid(playerIndex))
	{
		return nullptr;
	}
	return (s_player*)&GetArray()->data[playerIndex * GetArray()->datum_element_size];
}

e_object_team s_player::GetTeam(int playerIndex)
{
	if (!IndexValid(playerIndex))
	{
		return (e_object_team)NONE;
	}
	return GetPlayer(playerIndex)->properties[0].player_team;
}

void s_player::SetTeam(int playerIndex, e_object_team team)
{
	if (!IndexValid(playerIndex))
	{
		return;
	}
	GetPlayer(playerIndex)->properties[0].player_team = team;
}

void s_player::SetUnitBipedType(int playerIndex, s_player::e_character_type bipedType)
{
	if (!IndexValid(playerIndex))
	{
		return;
	}
	GetPlayer(playerIndex)->properties[0].profile.player_character_type = bipedType;
}

void s_player::SetUnitBipedType(int playerIndex, byte representationIndex)
{
	if (!IndexValid(playerIndex))
	{
		return;
	}
	GetPlayer(playerIndex)->properties[0].profile.player_character_type = (e_character_type)representationIndex;
}

void s_player::SetBipedSpeed(int playerIndex, float speed)
{
	if (!IndexValid(playerIndex))
	{
		return;
	}
	GetPlayer(playerIndex)->unit_speed = speed;
}

const wchar_t* s_player::GetName(int playerIndex)
{
	if (!IndexValid(playerIndex))
	{
		return L"";
	}
	return GetPlayer(playerIndex)->properties[0].player_name;
}

datum s_player::GetPlayerUnitDatumIndex(int playerIndex)
{
	if (!IndexValid(playerIndex))
		return DATUM_INDEX_NONE;

	return GetPlayer(playerIndex)->unit_index;
}

unsigned long long s_player::GetId(int playerIndex)
{
	if (!IndexValid(playerIndex))
	{
		return 0ull;
	}

	return GetPlayer(playerIndex)->identifier;
}
#pragma endregion

PlayerIterator::PlayerIterator() 
	: s_data_iterator(s_player::GetArray())
{
}

bool PlayerIterator::get_next_active_player()
{
	m_current_player = get_next_datum();
	if (m_current_player)
	{
		do
		{
			if (!TEST_FLAG(m_current_player->flags, s_player::flags::player_inactive))
				break;

			m_current_player = get_next_datum();
		} while (m_current_player);
	}

	return m_current_player != nullptr;
}

s_player* PlayerIterator::get_current_player_data()
{
	return m_current_player;
}

int PlayerIterator::get_current_player_index() const
{
	return get_current_absolute_index();
}

wchar_t* PlayerIterator::get_current_player_name()
{
	return m_current_player->properties[0].player_name;
}

unsigned long long PlayerIterator::get_current_player_id() const
{
	return s_player::GetId(this->get_current_player_index());
}

namespace players
{
	byte get_local_team_index()
	{
		return *Memory::GetAddress<byte*>(0x51A6B4);
	}

	/* controller index aka local player index -> player index */
	datum get_player_datum_index_from_controller_index(int controller_index)
	{
		typedef int(__cdecl* get_local_player_index_t)(int controller_index);
		auto p_get_local_player_index = Memory::GetAddress<get_local_player_index_t>(0x5141D);
		return p_get_local_player_index(controller_index);
	}

	bool local_user_has_player(int user_index)
	{
		typedef bool(__cdecl* local_user_has_player_t)(int user_index);
		auto p_local_user_has_player = Memory::GetAddress<local_user_has_player_t>(0x5139B);
		return p_local_user_has_player(user_index);
	}

	datum local_user_get_player_idx(int user_index)
	{
		typedef datum(__cdecl* local_user_get_player_idx_t)(int user_index);
		auto p_local_user_has_player = Memory::GetAddress<local_user_get_player_idx_t>(0x5141D);
		return p_local_user_has_player(user_index);
	}

	void set_local_team_index(int local_player_index, int team_index)
	{
		// we only use player index 0 due to no splitscreen support but whatever
		typedef void(__cdecl* update_player_profile_t)(int local_player_index);
		auto p_update_player_profile = Memory::GetAddress<update_player_profile_t>(0x206A97);

		p_change_local_team(local_player_index, team_index);
		p_update_player_profile(local_player_index); // fixes infection handicap glitch
	}

	void set_local_clan_tag(int local_player_index, unsigned long long tag)
	{
		typedef void(__cdecl* update_player_profile_t)(int local_player_index);
		auto p_update_player_profile = Memory::GetAddress<update_player_profile_t>(0x206A97);
		unsigned long low = tag & 0xFFFFFFFF;
		*(unsigned long*)Memory::GetAddress(0x51A6A8 + (0xB8 * local_player_index)) = low;
		p_update_player_profile(local_player_index);
	}

	void __cdecl carto_player_validate_configuration(int player_index, s_player::s_player_properties* player_properties)
	{
		LOG_INFO_GAME("{} - game engine: {}", __FUNCTION__, s_game_globals::get()->options.m_engine_type);
		if (s_game_globals::game_is_campaign())
		{
			auto scenario = tags::get_tag_fast<s_scenario_group_definition>(tags::get_tags_header()->scenario_datum);
			auto player_type = s_scenario_group_definition::s_player_starting_locations_block::e_campaign_player_type::masterchief;
			if (scenario->player_starting_locations.size > 0)
			{
				for (auto i = 0; i < scenario->player_starting_locations.size; i++)
				{
					auto starting_location = scenario->player_starting_locations[i];
					if (starting_location->campaign_player_type != s_scenario_group_definition::s_player_starting_locations_block::e_campaign_player_type::none) {
						player_type = starting_location->campaign_player_type;
						break;
					}
				}
				player_properties->player_team = e_object_team::player;
				if (player_type == s_scenario_group_definition::s_player_starting_locations_block::e_campaign_player_type::none)
					player_properties->profile.player_character_type = (s_player::e_character_type)player_type;
			}
			player_properties->player_team = e_object_team::player;
			player_properties->profile.player_character_type = (s_player::e_character_type)player_type;
		}
		else if (s_game_globals::game_is_multiplayer())
		{
			if (player_properties->profile.player_character_type == s_player::e_character_type::MasterChief)
				player_properties->profile.player_character_type = s_player::e_character_type::Spartan;
			if (player_properties->profile.player_character_type == s_player::e_character_type::Dervish)
				player_properties->profile.player_character_type = s_player::e_character_type::Elite;

			if (SpecialEvents::getCurrentEvent() != SpecialEvents::_halloween)
			{
				if (player_properties->profile.player_character_type == s_player::e_character_type::Skeleton)
					player_properties->profile.player_character_type = s_player::e_character_type::Spartan;
			}
			else if (H2Config_spooky_boy && !Memory::IsDedicatedServer())
				*Memory::GetAddress<s_player::e_character_type*>(0x51A67C) = s_player::e_character_type::Skeleton;

			if ((__int8)player_properties->profile.player_character_type > PlayerRepresentation::current_representation_count)
				player_properties->profile.player_character_type = s_player::e_character_type::Spartan;
		}

		if (player_properties->player_displayed_skill != -1 && player_properties->player_displayed_skill < 0)
			player_properties->player_displayed_skill = 0;

		if (player_properties->player_overall_skill != -1 && player_properties->player_overall_skill < 0)
			player_properties->player_overall_skill = 0;

		if (player_properties->player_handicap_level >= s_player::e_handicap::None)
		{
			if (player_properties->player_handicap_level > s_player::e_handicap::Severe)
				player_properties->player_handicap_level = s_player::e_handicap::Severe;
		}
		else
		{
			player_properties->player_handicap_level = s_player::e_handicap::None;
		}

		if (player_properties->bungie_user_role >= 0)
		{
			if (player_properties->bungie_user_role <= 7)
				player_properties->bungie_user_role = 7;
		}
		else
		{
			player_properties->bungie_user_role = 0;
		}

		if (game::get_game_mode_engine()
			&& s_game_globals::get_game_variant()->game_engine_flags & FLAG(e_game_engine_flags::_game_engine_teams_bit)
			&& (player_properties->player_team && !(game::s_game_engine_globals::get()->Unk1 & FLAG(player_properties->player_team))))
		{
			player_properties->player_team = e_object_team::None;
		}
	}

	void carto_set_local_rank(BYTE rank)
	{
		if (Memory::IsDedicatedServer())
			return;

		static bool initialized = false;

		if (!initialized)
		{
			NopFill(Memory::GetAddress(0x1b2c29), 7);
			initialized = true;
		}

		s_player::s_player_properties* local_player_properties = Memory::GetAddress<s_player::s_player_properties*>(0x51A638);

		local_player_properties->player_overall_skill = rank;
		local_player_properties->player_displayed_skill = rank;
	}

	void __cdecl carto_changeTeam(int localPlayerIndex, int teamIndex)
	{
		s_network_session* session = NetworkSession::GetCurrentNetworkSession();

		if ((session->parameters[0].session_mode == 4 && network_life_cycle::get_game_life_cycle() == _life_cycle_pre_game)
			|| (StrStrIW(NetworkSession::GetGameVariantName(), L"rvb") != NULL && teamIndex > 1)) {
			//rvb mode enabled, don't change teams
			return;
		}
		p_change_local_team(localPlayerIndex, teamIndex);
	}

	void ApplyPatches()
	{
		PatchCall(Memory::GetAddress(0x5509E, 0x5d596), carto_player_validate_configuration);
		DETOUR_ATTACH(p_change_local_team, Memory::GetAddress<change_team_t>(0x2068F2), carto_changeTeam);
	}
}
