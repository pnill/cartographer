#pragma once

#include "Blam/Cache/DataTypes/BlamDataTypes.h"

class VariantPlayer
{
public:
	XUID GetXUID(datum object_index, bool player);
	datum GetPlayerDatum(datum unit_datum);
	datum GetPlayerDatum(XUID xuid);
	datum GetUnitDatum(datum player_datum);
	datum GetUnitDatum(XUID xuid);
	void SetPlayerDatum(XUID xuid, datum player_datum);
	void SetUnitDatum(XUID xuid, datum unit_datum);
	void Deinitialize();

private:
	std::unordered_map<XUID, datum> xuid_to_player_datum;
	std::unordered_map<XUID, datum> xuid_to_unit_datum;

};