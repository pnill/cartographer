#include <unordered_map>
#include "Globals.h"
#include "DeviceShop.h"


DatumIndex get_device_open_up_weapon_datum(DatumIndex device_datum)
{

	DWORD tag_header = *(DWORD*)((BYTE*)h2mod->GetBase() + 0x47CD54);
	DWORD global_tag_instances = *(DWORD*)((BYTE*)h2mod->GetBase() + 0x47CD50);
	DWORD game_state_objects_header = *(DWORD*)((BYTE*)h2mod->GetBase() + 0x4E461C);
	DWORD game_state_objects_header_table = *(DWORD*)((BYTE*)game_state_objects_header + 0x44);

	int device_gamestate_offset = device_datum.Index + device_datum.Index * 2;
	DWORD device_gamestate_datum_pointer = *(DWORD*)((BYTE*)game_state_objects_header_table + device_gamestate_offset * 4 + 8);
	DatumIndex device_control_datum = *(DWORD*)((BYTE*)device_gamestate_datum_pointer);

	device_control_datum.Index = device_control_datum.Index << 4;

	DWORD device_control_tag_offset = *(DWORD*)((BYTE*)device_control_datum.Index + global_tag_instances + 8);
	DatumIndex weapon_datum = *(DatumIndex*)((BYTE*)device_control_tag_offset + tag_header + 0xE0);

	return weapon_datum;
}

int DeviceShop::GetCost(DatumIndex device_datum)
{
	return 0;
}

item_type DeviceShop::GetType(DatumIndex device_datum)
{
	return weapon_item;
}

DatumIndex DeviceShop::GetItemDatum(DatumIndex device_datum)
{
	return get_device_open_up_weapon_datum(device_datum);
}

bool DeviceShop::BuyItem(DatumIndex device_datum, DatumIndex unit_datum)
{

	XUID player = variant_player->GetXUID(unit_datum, false);

	if (GetCost(device_datum) > player_points[player])
	{
		return false;
	}
	else
	{
		switch (GetType(device_datum))
		{
		case ai_item:
			SpawnAI(device_datum);
			break;

		case vehicle_item:
			SpawnVehicle(device_datum);
			break;

		case weapon_item:
			GiveWeapon(device_datum, unit_datum);
			break;
		}
	}

	return true;
}

void DeviceShop::SpawnAI(DatumIndex character_datum)
{

}

void DeviceShop::SpawnVehicle(DatumIndex vehicle_datum)
{

}

void DeviceShop::GiveWeapon(DatumIndex weapon_datum, DatumIndex unit_datum)
{
	
}

void DeviceShop::AddPoints(XUID xuid, int points)
{
	player_points[xuid] = points;
}

void DeviceShop::deinitialize()
{
	player_points.clear();
}