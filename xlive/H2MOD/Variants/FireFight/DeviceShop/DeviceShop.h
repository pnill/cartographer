#pragma once
#include "Blam/Cache/DataTypes/BlamDataTypes.h"

enum e_item_type {
	_generic_item,
	_weapon_item,
	_ai_item,
	_vehicle_item
};

class DeviceShop {
public:
	bool BuyItem(datum device_datum, datum type);
	void AddPoints(datum playerIdx, int points);
	void Dispose();
private:
	void SpawnAI(datum character_datum);
	void SpawnVehicle(datum vehicle_datum);
	void GiveWeapon(datum unit_datum, datum weapon_tag_index);
	int GetCost(datum device_datum);
	e_item_type GetType(datum device_datum);
	datum GetItemDatum(datum device_datum);
};