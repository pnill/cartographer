#pragma once
enum item_type {
	generic_item,
	weapon_item,
	ai_item,
	vehicle_item
};

class DeviceShop {
	public:
		bool BuyItem(DatumIndex device_datum,DatumIndex type);
		void AddPoints(XUID xuid, int points);
		void deinitialize();
	private:
		void SpawnAI(DatumIndex character_datum);
		void SpawnVehicle(DatumIndex vehicle_datum);
		void GiveWeapon(DatumIndex weapon, DatumIndex unit_datum);
		int GetCost(DatumIndex device_datum);
		item_type GetType(DatumIndex device_datum);
		DatumIndex DeviceShop::GetItemDatum(DatumIndex device_datum);
		std::unordered_map<XUID, int> player_points;
};