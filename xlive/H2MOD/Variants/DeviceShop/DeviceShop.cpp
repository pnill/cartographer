#include "DeviceShop.h"

#include "H2MOD.h"
#include "H2MOD/Tags/TagInterface.h"

extern void __cdecl print_to_console(char *output);
extern void GivePlayerWeaponDatum(datum unit_datum, datum weapon_datum);

// power transition time
// TODO: Convert to TagGroup/Block
float get_device_power_transition_time(datum device_datum)
{
	DWORD tag_data = (DWORD)tags::get_tag_data();
	DWORD tag_instances = (DWORD)tags::get_tag_instances();
	BYTE* game_state_objects_header_table = (BYTE*)get_objects_header()->data;

	int device_gamestate_offset = DATUM_ABSOLUTE_INDEX(device_datum) + DATUM_ABSOLUTE_INDEX(device_datum) * 2;
	DWORD device_gamestate_datum_pointer = *(DWORD*)(game_state_objects_header_table + device_gamestate_offset * 4 + 8);
	DWORD device_control_datum = *(DWORD*)((BYTE*)device_gamestate_datum_pointer);

	__int16 device_control_index = device_control_datum & 0xFFFF;
	device_control_index = device_control_index << 4;

	DWORD device_control_tag_offset = *(DWORD*)((BYTE*)device_control_index + tag_instances + 8);
	float acceleration_scale = *(float*)((BYTE*)device_control_tag_offset + tag_data + 0xC0);

	return acceleration_scale;

}

//this is actually the open up sound tag.
//TODO: Convert to TagGroup/Block
datum get_device_open_up_weapon_datum(datum device_datum)
{
	DWORD tag_data = (DWORD)tags::get_tag_data();
	DWORD global_tag_instances = (DWORD)tags::get_tag_instances();
	BYTE* game_state_objects_header_table = (BYTE*)get_objects_header()->data;

	int device_gamestate_offset = DATUM_ABSOLUTE_INDEX(device_datum) + DATUM_ABSOLUTE_INDEX(device_datum) * 2;
	DWORD device_gamestate_datum_pointer = *(DWORD*)(game_state_objects_header_table + device_gamestate_offset * 4 + 8);
	datum device_control_datum = *(DWORD*)((BYTE*)device_gamestate_datum_pointer);

	device_control_datum = DATUM_ABSOLUTE_INDEX(device_control_datum) << 4;

	DWORD device_control_tag_offset = *(DWORD*)((BYTE*)DATUM_ABSOLUTE_INDEX(device_control_datum) + global_tag_instances + 8);
	datum weapon_datum = *(datum*)((BYTE*)device_control_tag_offset + tag_data + 0xE0);

	return weapon_datum;
}



int DeviceShop::GetCost(datum device_datum)
{
	return (int)get_device_power_transition_time(device_datum);
}

item_type DeviceShop::GetType(datum device_datum)
{
	return weapon_item;
}

datum DeviceShop::GetItemDatum(datum device_datum)
{
	return get_device_open_up_weapon_datum(device_datum);
}

bool DeviceShop::BuyItem(datum device_datum, datum unit_datum)
{

	XUID player = variant_player->GetXUID(unit_datum, false);
	
	int cost = GetCost(device_datum);
	int points = player_points[player];

	if (cost > points)
	{
		print_to_console("Player did not have enough points to buy item");
		std::string debug_points;
		debug_points.append("Points: ");
		debug_points.append(std::to_string(points));
		debug_points.append(" player: ");
		debug_points.append(std::to_string(player));
		print_to_console((char*)debug_points.c_str());
		return false;
	}
	else
	{
		player_points[player] -= cost;

		datum item_datum = GetItemDatum(device_datum);

		switch (GetType(item_datum))
		{
		case ai_item:
			SpawnAI(item_datum);
			break;

		case vehicle_item:
			SpawnVehicle(item_datum);
			break;

		case weapon_item:
			print_to_console("Player purchased a weapon!");
			std::string debug_text;
			debug_text.append("Player: ");
			debug_text.append(std::to_string(player));
			debug_text.append(" Points left: ");
			debug_text.append(std::to_string(player_points[player]));
			debug_text.append(" Cost: ");
			debug_text.append(std::to_string(GetCost(device_datum)));

			print_to_console((char*)debug_text.c_str());
			GiveWeapon(unit_datum, item_datum);
			break;
		}
	}

	return true;
}

void DeviceShop::SpawnAI(datum character_datum)
{

}

void DeviceShop::SpawnVehicle(datum vehicle_datum)
{

}

void DeviceShop::GiveWeapon(datum unit_datum, datum weapon_datum)
{
	GivePlayerWeaponDatum(unit_datum, weapon_datum);
}

void DeviceShop::AddPoints(XUID xuid, int points)
{
	std::string debug_txt;
	debug_txt.append("Adding points: ");
	debug_txt.append(std::to_string(points));
	debug_txt.append(" to player: ");
	debug_txt.append(std::to_string(xuid));

	print_to_console((char*)debug_txt.c_str());

	player_points[xuid] += points;
}

void DeviceShop::deinitialize()
{
	player_points.clear();
}