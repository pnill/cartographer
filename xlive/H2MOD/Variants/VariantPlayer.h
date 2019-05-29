#pragma once
#include "Blam/BlamLibrary.h"

class VariantPlayer {
	public:
		XUID GetXUID(DatumIndex unit_datum,bool player);
		DatumIndex GetPlayerDatum(DatumIndex unit_datum);
		DatumIndex GetPlayerDatum(XUID xuid);
		DatumIndex GetUnitDatum(DatumIndex player_datum);
		DatumIndex GetUnitDatum(XUID xuid);
		void SetPlayerDatum(XUID xuid,DatumIndex player_datum);
		void SetUnitDatum(XUID xuid, DatumIndex unit_datum);
		void deinitialize();
	private:
		std::unordered_map<XUID, DatumIndex> xuid_to_player_datum;
		std::unordered_map<XUID, DatumIndex> xuid_to_unit_datum;

};