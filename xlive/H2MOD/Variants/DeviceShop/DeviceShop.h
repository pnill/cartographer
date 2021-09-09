#pragma once

#include "Blam/Cache/DataTypes/BlamDataTypes.h"

enum item_type {
	generic_item,
	weapon_item,
	ai_item,
	vehicle_item
};

class DeviceShop {
	public:
		bool BuyItem(datum device_datum,datum type);
		void AddPoints(XUID xuid, int points);
		void deinitialize();
	private:
		void SpawnAI(datum character_datum);
		void SpawnVehicle(datum vehicle_datum);
		void GiveWeapon(datum unit_datum, datum weapon);
		int GetCost(datum device_datum);
		item_type GetType(datum device_datum);
		datum DeviceShop::GetItemDatum(datum device_datum);
		std::unordered_map<XUID, int> player_points;
};