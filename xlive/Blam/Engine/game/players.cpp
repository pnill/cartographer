#include "stdafx.h"
#include "players.h"

#include "Blam/Engine/game/game.h"
#include "Blam/Engine/game/game_engine.h"
#include "Blam/Engine/game/game_engine_util.h"
#include "Blam/Engine/game/game_globals.h"
#include "Blam/Engine/saved_games/game_variant.h"
#include "Blam/Engine/scenario/scenario.h"

#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/SpecialEvents/SpecialEvents.h"

#include "Util/Hooks/Hook.h"

/*
	- NOTES:
    - This gets the player data from the game state, thus it is available only during a match or gameplay (game life cycle is in_game or after map has been loaded)
	- If you need to do something in the pregame lobby, use the functions available in Network Session (Blam/Networking/Session)
*/

s_data_array* s_player::get_data()
{
	return *Memory::GetAddress<s_data_array**>(0x4A8260, 0x4D64C4);
}

bool s_player::is_index_valid(datum player_index)
{
    int32 player_abs_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index);
	return player_abs_index >= 0 && player_abs_index < k_maximum_players;
}

s_player* s_player::get(datum player_index)
{
	if (!is_index_valid(player_index))
	{
		return nullptr;
	}
	return (s_player*)&get_data()->data[DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index) * get_data()->datum_element_size];
}

e_game_team s_player::get_team(datum player_index)
{
	if (!is_index_valid(player_index))
	{
		return (e_game_team)NONE;
	}
	return (e_game_team)get(player_index)->properties[0].team_index;
}

void s_player::set_team(datum player_index, e_game_team team)
{
	if (!is_index_valid(player_index))
	{
		return;
	}
	get(player_index)->properties[0].team_index = (int8)team;
}

void s_player::set_unit_character_type(datum player_index, e_character_type character_type)
{
	if (!is_index_valid(player_index))
	{
		return;
	}
    s_player* player = get(player_index);
    player->properties[0].profile_traits.profile.player_character_type = character_type;
    player->properties[1].profile_traits.profile.player_character_type = character_type;
    return;
}

void s_player::set_unit_speed(datum player_index, float speed)
{
	if (!is_index_valid(player_index))
	{
		return;
	}
	get(player_index)->unit_speed = speed;
}

const wchar_t* s_player::get_name(datum player_index)
{
	if (!is_index_valid(player_index))
	{
		return L"";
	}
	return get(player_index)->properties[0].player_name;
}

datum s_player::get_unit_index(datum player_index)
{
    if (!is_index_valid(player_index)) 
    {
		return DATUM_INDEX_NONE;
    }

	return get(player_index)->unit_index;
}

unit_datum* s_player::get_player_unit_data(datum player_index)
{
    datum unit_index = s_player::get_unit_index(player_index);
    if (DATUM_IS_NONE(unit_index))
        return nullptr;

    return (unit_datum*)object_get_fast_unsafe(unit_index);
}

real_vector3d* s_player::get_unit_coords(datum player_index)
{
    unit_datum* player_unit = get_player_unit_data(player_index);
    if (player_unit != nullptr)
        return &player_unit->object.position;

    return nullptr;
}

uint64 s_player::get_id(datum player_index)
{
	if (!is_index_valid(player_index))
	{
		return 0ull;
	}

	return get(player_index)->identifier;
}

player_iterator::player_iterator() 
	: s_data_iterator(s_player::get_data())
{
}

bool player_iterator::get_next_active_player()
{
	m_current_player = get_next_datum();

	while (m_current_player)
	{
		if (!TEST_BIT(m_current_player->flags, _player_left_game_bit))
			break;

		m_current_player = get_next_datum();
	}

	return m_current_player != nullptr;
}

s_player* player_iterator::get_current_player_data()
{
	return m_current_player;
}

int player_iterator::get_current_player_index()
{
	return get_current_absolute_index();
}

wchar_t* player_iterator::get_current_player_name()
{
	return m_current_player->properties[0].player_name;
}

unsigned long long player_iterator::get_current_player_id()
{
	return s_player::get_id(this->get_current_player_index());
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

void __cdecl player_configuration_validate_character_type(s_player_properties* configuration_data)
{
    // Campaign verification
    if (game_is_campaign())
    {
        scenario* scnr = get_global_scenario();
        uint32 block_size = scnr->player_starting_locations.size;
        if (block_size > 0)
        {
            // Go through every player starting location and check if there's a campaign player type set
            // Set the appropriate data in the player to the values of the tagblock
            bool found = false;
            for (uint32 i = 0; i < block_size; ++i)
            {
                scenario_player* player_starting_location = scnr->player_starting_locations[i];
                if (player_starting_location->campaign_player_type != NONE)
                {
                    configuration_data->team_index = _game_team_player;
                    configuration_data->profile_traits.profile.player_character_type = (e_character_type)player_starting_location->campaign_player_type;
                    found = true;
                    break;
                 }
            }

            // If a campaign_player_type type wasn't found in any of the starting locations set default values
            if (!found)
            {
                configuration_data->team_index = _game_team_player;
                configuration_data->profile_traits.profile.player_character_type = _character_type_masterchief;
            }
        }
        else
        {
            configuration_data->team_index = _game_team_player;
            configuration_data->profile_traits.profile.player_character_type = _character_type_masterchief;
        }
    }
    // Multiplayer verification
    else if (game_is_multiplayer())
    {
        // If the character is mastechief set him to a spartan in multiplayer
        if (configuration_data->profile_traits.profile.player_character_type == _character_type_masterchief)
        {
            configuration_data->profile_traits.profile.player_character_type = _character_type_spartan;
        }

        // Don't allow dervish since he's not loaded properly in shared
        if (configuration_data->profile_traits.profile.player_character_type == _character_type_dervish)
        {
            configuration_data->profile_traits.profile.player_character_type = _character_type_elite;
        }


        // Force skeletons in mp during the halloween event
        // Carto addition
        if (e_character_type character = configuration_data->profile_traits.profile.player_character_type;
            character != _character_type_flood && H2Config_spooky_boy && get_current_special_event() == _special_event_halloween)
        {
            configuration_data->profile_traits.profile.player_character_type = _character_type_skeleton;
            if (!Memory::IsDedicatedServer())
            {
                for (uint32 i = 0; i < k_number_of_users; i++)
                {
                    network_session_interface_set_local_user_character_type(i, _character_type_skeleton);
                }
            }
        }
    }
}

void __cdecl player_validate_configuration(datum player_index, s_player_properties* configuration_data)
{
    player_configuration_validate_character_type(configuration_data);
    
    // General character verification
    e_character_type character = configuration_data->profile_traits.profile.player_character_type;
    if (character != NONE)
    {
        if (character >= _character_type_masterchief)
        {
            s_game_globals* globals = scenario_get_game_globals();
            if (character > (e_character_type)globals->player_representation.size - 1)
            {
                character = (e_character_type)(globals->player_representation.size - 1);
            }
        }
        else
        {
            character = _character_type_masterchief;
        }
        configuration_data->profile_traits.profile.player_character_type = character;
    }

    // Skill verification
    int8 player_displayed_skill = configuration_data->player_displayed_skill;
    if (player_displayed_skill != NONE)
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
        if (TEST_BIT(get_game_variant()->game_engine_flags, _game_engine_teams_bit))
        {
            if (configuration_data->team_index != NONE && !TEST_BIT(game_engine_globals_get()->team_bitmask, configuration_data->team_index))
            {
                configuration_data->team_index = NONE;
            }
        }
    }

    return;
}

// Basic void* type because actually "usercall"
void* p_player_representation_get;

// Calls the original player representation get function
void player_representation_get_orig_fn(int player_index, int* out_variant_index, int* a3)
{
    __asm {
        mov eax, a3
        push eax
        mov ebx, out_variant_index
        mov eax, player_index
        call p_player_representation_get
        add esp, 4
    }
}

void __cdecl player_representation_get(datum player_datum, int* out_variant_index, int* a3)
{
    s_player* player = s_player::get(player_datum);

    player_configuration_validate_character_type(&player->properties[0]);

    // call the original function after validation
    player_representation_get_orig_fn(player_datum, out_variant_index, a3);
}

__declspec(naked) void player_representation_get_to_cdecl()
{
    __asm {
        push eax
        mov eax, [esp + 4h + 4h]
        push eax // a3
        push ebx // out_variant
        mov eax, [esp + 8h]
        push eax // player index
        call player_representation_get
        add esp, 10h // clear 16 bytes to esp == ret addr
        retn
    }
}


void players_apply_patches(void)
{
    // Change the validation for player_appearance_valid to use the updated k_player_character_type_count constant
    WriteValue<BYTE>(Memory::GetAddress(0x54fb3, 0x5D4AB), k_player_character_type_count);

    // Replace the player profile validation function with our own
    PatchCall(Memory::GetAddress(0x5509E, 0x5D596), player_validate_configuration);

    // Validate the player character type pre-spawn
    p_player_representation_get = Memory::GetAddress<void*>(0x53895, 0x5BD8D);
    PatchCall(Memory::GetAddress(0x559F9, 0x5DEF1), player_representation_get_to_cdecl);
    PatchCall(Memory::GetAddress(0x53969, 0x5BE61), player_representation_get_to_cdecl);
}
