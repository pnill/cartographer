#include "stdafx.h"

#include "Players.h"

#include "H2MOD.h"

/*
	- TO NOTE:
	- This functions work only after the game has started (game life cycle is in_game or after map has been loaded).
	- If you need to do something in the pregame lobby, use the functions available in Network Session (H2MOD/Modules/Networking/NetworkSession)
*/

s_data_array* s_player::getArray()
{
	return *Memory::GetAddress<s_data_array**>(0x4A8260, 0x4D64C4);
}

bool s_player::indexValid(int playerIndex)
{
	return playerIndex >= 0 && playerIndex < ENGINE_PLAYER_MAX;
}

s_player* s_player::getPlayer(int playerIndex)
{
	if (!indexValid(playerIndex))
	{
		return nullptr;
	}
	return (s_player*)&getArray()->data[playerIndex * getArray()->datum_element_size];
}

e_object_team s_player::getTeam(int playerIndex)
{
	if (!indexValid(playerIndex))
	{
		return (e_object_team)NONE;
	}
	return getPlayer(playerIndex)->properties[0].player_team;
}

void s_player::setTeam(int playerIndex, e_object_team team)
{
	if (!indexValid(playerIndex))
	{
		return;
	}
	getPlayer(playerIndex)->properties[0].player_team = team;
}

void s_player::setUnitBipedType(int playerIndex, s_player::e_character_type bipedType)
{
	if (!indexValid(playerIndex))
	{
		return;
	}
	getPlayer(playerIndex)->properties[0].profile.player_character_type = bipedType;
}

void s_player::setUnitBipedType(int playerIndex, byte representationIndex)
{
	if (!indexValid(playerIndex))
	{
		return;
	}
	getPlayer(playerIndex)->properties[0].profile.player_character_type = (e_character_type)representationIndex;
}

void s_player::setBipedSpeed(int playerIndex, float speed)
{
	if (!indexValid(playerIndex))
	{
		return;
	}
	getPlayer(playerIndex)->unit_speed = speed;
}

wchar_t* s_player::getName(int playerIndex)
{
	if (!indexValid(playerIndex))
	{
		return L"";
	}
	return getPlayer(playerIndex)->properties[0].player_name;
}

datum s_player::getPlayerUnitDatumIndex(int playerIndex)
{
	if (!indexValid(playerIndex))
		return DATUM_INDEX_NONE;

	if (DATUM_IS_NONE(getPlayer(playerIndex)->unit_index))
		return DATUM_INDEX_NONE;
		
	return getPlayer(playerIndex)->unit_index;
}

XUID s_player::getIdentifier(int playerIndex)
{
	if (!indexValid(playerIndex))
	{
		return XUID(0ll);
	}

	return getPlayer(playerIndex)->identifier;
}

PlayerIterator::PlayerIterator() 
	: s_data_iterator(s_player::getArray())
{

}

bool PlayerIterator::get_next_active_player()
{
	m_current_player = get_next_datum();
	if (m_current_player)
	{
		do
		{
			if (!(m_current_player->flags & FLAG(s_player::flags::player_inactive)))
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

XUID PlayerIterator::get_current_player_id()
{
	return s_player::getIdentifier(this->get_current_player_index());
}
