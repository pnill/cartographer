#include "stdafx.h"
#include "Players.h"


/*
	- TO NOTE:
	- This functions work only after the game has started (game life cycle is in_game or after map has been loaded).
	- If you need to do something in the pregame lobby, use the functions available in Network Session (H2MOD/Modules/Networking/NetworkSession)
*/

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
}
