#include "Globals.h"
#include "VariantPlayer.h"

/* 
	This would replace the current "Players" class... 
	Need to go through and make gun game and infection use this one before removing it.
*/
XUID VariantPlayer::GetXUID(DatumIndex datum, bool player)
{
	if (player)
		return (&game_state_players->players[datum.ToAbsoluteIndex()])->xuid;
	else
	{
		short player_index = (&game_state_objects_header->object_header[datum.ToAbsoluteIndex()])->object->PlayerDatum.ToAbsoluteIndex();
		return (&game_state_players->players[player_index])->xuid;
	}
}

DatumIndex VariantPlayer::GetPlayerDatum(DatumIndex unit_datum)
{
	return (&game_state_objects_header->object_header[unit_datum.ToAbsoluteIndex()])->object->PlayerDatum;
}

DatumIndex VariantPlayer::GetPlayerDatum(XUID xuid)
{
	if (xuid_to_player_datum.find(xuid) == xuid_to_player_datum.end())
	{
		for (auto i = 0; i < 15; i++)
		{
			if ((&game_state_players->players[i])->xuid == xuid)
			{

				DatumIndex player_datum;
				player_datum.Index = i;

				SetPlayerDatum(xuid, player_datum);
				return player_datum;
			
			}
		}
	}
	else
		return xuid_to_player_datum[xuid];

}

DatumIndex VariantPlayer::GetUnitDatum(DatumIndex player_datum)
{
	return (&game_state_players->players[player_datum.Index])->unit_index;
}

DatumIndex VariantPlayer::GetUnitDatum(XUID xuid)
{
	if (xuid_to_unit_datum.find(xuid) == xuid_to_unit_datum.end())
	{
		for (auto i = 0; i < 15; i++)
		{

			if ((&game_state_players->players[i])->xuid == xuid)
			{
				DatumIndex unit_datum = (&game_state_players->players[i])->unit_index;
				DatumIndex player_datum;
				player_datum.Index = i;

				SetPlayerDatum(xuid, player_datum);
				SetUnitDatum(xuid, unit_datum);

				return unit_datum;
			}
		}
	}
}

void VariantPlayer::SetPlayerDatum(XUID xuid, DatumIndex player_datum)
{
	xuid_to_player_datum[xuid] = player_datum;
}

void VariantPlayer::SetUnitDatum(XUID xuid, DatumIndex unit_datum)
{
	xuid_to_unit_datum[xuid] = unit_datum;
}

void VariantPlayer::deinitialize()
{
	xuid_to_player_datum.clear();
	xuid_to_unit_datum.clear();
}