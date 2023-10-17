#include "stdafx.h"
#include "players.h"

#include "Blam/Engine/game/game.h"
#include "Blam/Engine/game/game_engine.h"
#include "Blam/Engine/game/game_engine_util.h"
#include "Blam/Engine/game/game_globals.h"
#include "Blam/Engine/saved_games/game_variant.h"
#include "Blam/Engine/scenario/scenario.h"

#include "H2MOD/Modules/Shell/Config.h"

/*
	- TO NOTE:
	- This functions work only after the game has started (game life cycle is in_game or after map has been loaded).
	- If you need to do something in the pregame lobby, use the functions available in Network Session (Blam/Networking/Session)
*/

s_data_array* s_player::GetArray()
{
	return *Memory::GetAddress<s_data_array**>(0x4A8260, 0x4D64C4);
}

bool s_player::IndexValid(int playerIndex)
{
	return playerIndex >= 0 && playerIndex < k_maximum_players;
}

s_player* s_player::GetPlayer(int playerIndex)
{
	if (!IndexValid(playerIndex))
	{
		return nullptr;
	}
	return (s_player*)&GetArray()->data[playerIndex * GetArray()->datum_element_size];
}

e_game_team s_player::GetTeam(int playerIndex)
{
	if (!IndexValid(playerIndex))
	{
		return (e_game_team)NONE;
	}
	return (e_game_team)GetPlayer(playerIndex)->properties[0].team_index;
}

void s_player::SetTeam(int playerIndex, e_game_team team)
{
	if (!IndexValid(playerIndex))
	{
		return;
	}
	GetPlayer(playerIndex)->properties[0].team_index = (int8)team;
}

void s_player::SetUnitBipedType(int playerIndex, e_character_type bipedType)
{
	if (!IndexValid(playerIndex))
	{
		return;
	}
	GetPlayer(playerIndex)->properties[0].profile_traits.profile.player_character_type = bipedType;
}

void s_player::SetUnitBipedType(int playerIndex, byte representationIndex)
{
	if (!IndexValid(playerIndex))
	{
		return;
	}
	GetPlayer(playerIndex)->properties[0].profile_traits.profile.player_character_type = (e_character_type)representationIndex;
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

PlayerIterator::PlayerIterator() 
	: s_data_iterator(s_player::GetArray())
{
}

bool PlayerIterator::get_next_active_player()
{
	m_current_player = get_next_datum();

	while (m_current_player)
	{
		if (!TEST_FLAG(m_current_player->flags, _player_left_game_bit))
			break;

		m_current_player = get_next_datum();
	}

	return m_current_player != nullptr;
}

s_player* PlayerIterator::get_current_player_data()
{
	return m_current_player;
}

int PlayerIterator::get_current_player_index()
{
	return get_current_absolute_index();
}

wchar_t* PlayerIterator::get_current_player_name()
{
	return m_current_player->properties[0].player_name;
}

unsigned long long PlayerIterator::get_current_player_id()
{
	return s_player::GetId(this->get_current_player_index());
}

s_players_globals* get_players_globals()
{
	return *Memory::GetAddress<s_players_globals**>(0x4A825C, 0x4D64C0);
}

datum __cdecl player_index_from_user_index(int32 user_index)
{
	return INVOKE(0x5141D, 0x5992B, player_index_from_user_index, user_index);
}

bool __cdecl players_user_is_active(int32 user_index)
{
	return INVOKE(0x5139B, 0x598BE, players_user_is_active, user_index);
}

uint32 player_appearance_required_bits()
{
	return 39;
}

void __cdecl player_validate_configuration(datum player_index, s_player_properties* configuration_data)
{
    // Campaign verification
    if (game_is_campaign())
    {
        scenario* scnr = get_global_scenario();
        uint32 block_size = scnr->player_starting_locations.size;
        if (block_size > 0)
        {
            uint32 i = 0;
            while (true)
            {
                scenario_player* player_starting_location = scnr->player_starting_locations[i];
                if (player_starting_location->campaign_player_type != NONE)
                {
                    configuration_data->team_index = 1;
                    configuration_data->profile_traits.profile.player_character_type = (e_character_type)player_starting_location->campaign_player_type;
                    break;
                }

                ++i;
                if (i >= block_size)
                {
                    configuration_data->team_index = 1;
                    configuration_data->profile_traits.profile.player_character_type = character_type_masterchief;
                }
            }
        }
        else
        {
            configuration_data->team_index = 1;
            configuration_data->profile_traits.profile.player_character_type = character_type_masterchief;
        }
    }
    // Multiplayer verification
    else if (game_is_multiplayer())
    {
        // Don't allow dervish since he's not loaded properly in shared
        if (configuration_data->profile_traits.profile.player_character_type == character_type_dervish)
        {
            configuration_data->profile_traits.profile.player_character_type = character_type_elite;
        }


        if (H2Config_spooky_boy && !Memory::IsDedicatedServer())
        {
            configuration_data->profile_traits.profile.player_character_type = character_type_skeleton;
            for (uint32 i = 0; i < k_number_of_users; i++)
            {
                network_session_interface_set_local_user_character_type(i, character_type_skeleton);
            }
            *Memory::GetAddress<e_character_type*>(0x51A67C) = character_type_skeleton;
        }
    }
    
    // General character verification
    e_character_type character = configuration_data->profile_traits.profile.player_character_type;
    if (character != NONE)
    {
        if (character >= character_type_masterchief)
        {
            s_game_globals* globals = scenario_get_game_globals();
            if (character > (e_character_type)globals->player_representation.size - 1)
            {
                character = (e_character_type)(globals->player_representation.size - 1);
            }
        }
        else
        {
            character = character_type_masterchief;
        }
        configuration_data->profile_traits.profile.player_character_type = character;
    }

    // Skill verification
    int8 player_displayed_skill = configuration_data->player_displayed_skill;
    if (player_displayed_skill != -1)
    {
        if (player_displayed_skill < 0)
            player_displayed_skill = 0;
        configuration_data->player_displayed_skill = player_displayed_skill;
    }
    int8 player_overall_skill = configuration_data->player_overall_skill;
    if (player_overall_skill != NONE)
    {
        if (player_overall_skill < 0)
        {
            player_overall_skill = 0;
        }
        configuration_data->player_overall_skill = player_overall_skill;
    }

    // Handicap verification
    e_handicap player_handicap_level = configuration_data->player_handicap_level;
    if (player_handicap_level >= _handicap_none)
    {
        if (player_handicap_level > _handicap_severe)
        {
            player_handicap_level = _handicap_severe;
        }
    }
    else
    {
        player_handicap_level = _handicap_none;
    }
    configuration_data->player_handicap_level = player_handicap_level;
    
    // User role verification
    int8 bungie_user_role = configuration_data->bungie_user_role;
    if (bungie_user_role < 0 || bungie_user_role > 7)
    {
        configuration_data->bungie_user_role = 0;
    }

    if (current_game_engine())
    {
        if (TEST_FLAG(get_game_variant()->game_engine_flags, _game_engine_teams_bit))
        {
            if (configuration_data->team_index != NONE && (FLAG(configuration_data->team_index) & s_game_engine_globals::get()->flags_A) == 0)
            {
                //game_is_authoritative();
                configuration_data->team_index = NONE;
            }
        }
    }
}
