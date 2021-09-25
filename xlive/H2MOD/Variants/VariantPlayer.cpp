
#include "VariantPlayer.h"

#include "Blam/Engine/DataArray/DataArray.h"
#include "Blam/Engine/Objects/Objects.h"
#include "Blam/Engine/Players/Players.h"

XUID VariantPlayer::GetXUID(datum object_index, bool player)
{
	s_biped_object_definition* playerUnit = object_get_fast_unsafe<s_biped_object_definition>(object_index);

	if (player)
		return Player::getPlayer(DATUM_ABSOLUTE_INDEX(object_index))->identifier;
	else
	{
		short player_index = DATUM_ABSOLUTE_INDEX(playerUnit->PlayerDatum);
		return Player::getPlayer(player_index)->identifier;
	}
}

datum VariantPlayer::GetPlayerDatum(datum unit_datum)
{
	s_biped_object_definition* playerUnit = object_get_fast_unsafe<s_biped_object_definition>(unit_datum);
	return playerUnit->PlayerDatum;
}

datum VariantPlayer::GetPlayerDatum(XUID xuid)
{
	if (xuid_to_player_datum.find(xuid) == xuid_to_player_datum.end())
	{
		PlayerIterator playersIt;
		while (playersIt.get_next_active_player())
		{
			if (playersIt.get_current_player_data()->identifier == xuid)
			{
				datum player_datum = DATUM_ABSOLUTE_INDEX(playersIt.get_current_player_index());

				SetPlayerDatum(xuid, player_datum);
				return player_datum;
			}
		}
	}
	else
		return xuid_to_player_datum[xuid];


	return -1;
}

datum VariantPlayer::GetUnitDatum(datum player_datum)
{
	PlayerIterator playersIt;
	return playersIt.get_data_at_datum_index(player_datum)->controlled_unit_index;
}

datum VariantPlayer::GetUnitDatum(XUID xuid)
{
	if (xuid_to_unit_datum.find(xuid) == xuid_to_unit_datum.end())
	{
		PlayerIterator playersIt;
		while (playersIt.get_next_active_player())
		{
			if (playersIt.get_current_player_data()->identifier == xuid)
			{
				datum unit_datum = playersIt.get_current_player_data()->controlled_unit_index;
				datum player_datum = DATUM_ABSOLUTE_INDEX(playersIt.get_current_player_index());

				SetPlayerDatum(xuid, player_datum);
				SetUnitDatum(xuid, unit_datum);

				return unit_datum;
			}
		}
	}
	else
		return xuid_to_unit_datum[xuid];


	return -1; // avoid warning.
}

void VariantPlayer::SetPlayerDatum(XUID xuid, datum player_datum)
{
	xuid_to_player_datum[xuid] = player_datum;
}

void VariantPlayer::SetUnitDatum(XUID xuid, datum unit_datum)
{
	xuid_to_unit_datum[xuid] = unit_datum;
}

void VariantPlayer::Deinitialize()
{
	xuid_to_player_datum.clear();
	xuid_to_unit_datum.clear();
}