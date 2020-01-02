#include "stdafx.h"
#include "Globals.h"
#include "VariantPlayer.h"

XUID VariantPlayer::GetXUID(DatumIndex datum, bool player)
{
	PlayerIterator playersIt;
	DatumIterator<ObjectHeader> objectIt(game_state_objects_header);
	BipedObjectDefinition* playerUnit = (BipedObjectDefinition*)objectIt.get_data_at_index(datum.ToAbsoluteIndex())->object;

	if (player)
		return playersIt.get_data_at_index(datum.ToAbsoluteIndex())->xuid;
	else
	{
		short player_index = playerUnit->PlayerDatum.ToAbsoluteIndex();
		return playersIt.get_data_at_index(player_index)->xuid;
	}
}

DatumIndex VariantPlayer::GetPlayerDatum(DatumIndex unit_datum)
{
	DatumIterator<ObjectHeader> objectIt(game_state_objects_header);
	BipedObjectDefinition* playerUnit = (BipedObjectDefinition*)objectIt.get_data_at_index(unit_datum.ToAbsoluteIndex())->object;

	return playerUnit->PlayerDatum;
}

DatumIndex VariantPlayer::GetPlayerDatum(XUID xuid)
{
	if (xuid_to_player_datum.find(xuid) == xuid_to_player_datum.end())
	{
		PlayerIterator playersIt;
		while (playersIt.get_next_player())
		{
			if (playersIt.get_current_player_data()->xuid == xuid)
			{
				DatumIndex player_datum;
				player_datum.Index = playersIt.get_current_player_index();

				SetPlayerDatum(xuid, player_datum);
				return player_datum;
			}
		}
	}
	else
		return xuid_to_player_datum[xuid];


	return -1;
}

DatumIndex VariantPlayer::GetUnitDatum(DatumIndex player_datum)
{
	PlayerIterator playersIt;
	return playersIt.get_data_at_index(player_datum.Index)->BipedUnitDatum;
}

DatumIndex VariantPlayer::GetUnitDatum(XUID xuid)
{
	if (xuid_to_unit_datum.find(xuid) == xuid_to_unit_datum.end())
	{
		PlayerIterator playersIt;
		while (playersIt.get_next_player())
		{
			if (playersIt.get_current_player_data()->xuid == xuid)
			{
				DatumIndex unit_datum = playersIt.get_current_player_data()->BipedUnitDatum;
				DatumIndex player_datum;
				player_datum.Index = playersIt.get_current_player_index();

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

void VariantPlayer::SetPlayerDatum(XUID xuid, DatumIndex player_datum)
{
	xuid_to_player_datum[xuid] = player_datum;
}

void VariantPlayer::SetUnitDatum(XUID xuid, DatumIndex unit_datum)
{
	xuid_to_unit_datum[xuid] = unit_datum;
}

void VariantPlayer::Deinitialize()
{
	xuid_to_player_datum.clear();
	xuid_to_unit_datum.clear();
}