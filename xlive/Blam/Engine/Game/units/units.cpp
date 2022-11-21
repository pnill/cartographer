#include "stdafx.h"

#include "units.h"

#include "Blam/Engine/Game/game/players.h"
#include "Blam/Engine/Game/objects/objects.h"
#include "Blam/Engine/Game/units/bipeds.h"
#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Engine/Networking/Session/NetworkSession.h"
#include "Util/Hooks/Hook.h"

namespace units
{
	void __cdecl remove_equipment(datum unit_idx)
	{
		//LOG_TRACE_GAME("unit_reset_equipment(unit_datum_index: %08X)", unit_datum_index);
		typedef void(__cdecl* unit_reset_equipment_t)(datum unit_idx);
		auto p_unit_reset_equipment = Memory::GetAddress<unit_reset_equipment_t>(0x1441E0, 0x133030);
		if (!DATUM_IS_NONE(unit_idx))
		{
			p_unit_reset_equipment(unit_idx);
		}
	}

	signed int __cdecl inventory_next_weapon(datum unit_idx)
	{
		//LOG_TRACE_GAME("unit_inventory_next_weapon(unit: %08X)", unit);
		typedef signed int(__cdecl* unit_inventory_next_weapon_t)(unsigned short unit);
		auto p_unit_inventory_next_weapon = Memory::GetAddress<unit_inventory_next_weapon_t>(0x139E04, 0x0);
		
		return p_unit_inventory_next_weapon(unit_idx);
	}

	bool __cdecl assign_equipment_to_unit(datum unit_idx, datum object_idx, short a3)
	{
		//LOG_TRACE_GAME("assign_equipment_to_unit(unit: %08X,object_index: %08X,unk: %08X)", unit, object_index, unk);
		typedef bool(__cdecl* assign_equipment_to_unit_t)(datum unit_idx, int object_idx, short unk);
		auto passign_equipment_to_unit = Memory::GetAddress<assign_equipment_to_unit_t>(0x1442AA, 0x1330FA);

		return passign_equipment_to_unit(unit_idx, object_idx, a3);
	}

	float carto_get_distance(int playerIndex1, int playerIndex2) 
	{
		real_point3d points_distance;
		real_point3d* player1 = nullptr;
		real_point3d* player2 = nullptr;

		player1 = units::carto_get_player_unit_coords(playerIndex1);
		player2 = units::carto_get_player_unit_coords(playerIndex2);

		points_distance.x = abs(player1->x - player2->x);
		points_distance.y = abs(player1->y - player2->y);
		points_distance.z = abs(player1->z - player2->z);

		return sqrt(pow(points_distance.x, 2) + pow(points_distance.y, 2) + pow(points_distance.z, 2));
	}

	real_point3d* carto_get_player_unit_coords(int playerIndex) 
	{
		BYTE* player_unit = carto_get_player_unit_from_player_index(playerIndex);
		if (player_unit != nullptr)
			return reinterpret_cast<real_point3d*>(player_unit + 0x64);

		return nullptr;
	}

	BYTE* carto_get_player_unit_from_player_index(int playerIndex) 
	{
		datum unit_datum = s_player::GetPlayerUnitDatumIndex(playerIndex);
		if (DATUM_IS_NONE(unit_datum))
			return nullptr;

		return (BYTE*)object_get_fast_unsafe(unit_datum);
	}

	void carto_call_give_player_weapon(int playerIndex, datum weaponId, bool bReset)
	{
		//LOG_TRACE_GAME("GivePlayerWeapon(PlayerIndex: %08X, WeaponId: %08X)", PlayerIndex, WeaponId);

		datum unit_datum = s_player::GetPlayerUnitDatumIndex(playerIndex);
		if (!DATUM_IS_NONE(unit_datum))
		{
			s_object_placement_data nObject;

			objects::create_new_placement_data(&nObject, weaponId, unit_datum, 0);

			datum object_idx = objects::object_new(&nObject);

			if (bReset)
				units::remove_equipment(unit_datum);

			units::assign_equipment_to_unit(unit_datum, object_idx, 1);
		}
	}

	const wchar_t* carto_get_local_player_name(int local_player_index)
	{
		return s_player::GetName(DATUM_INDEX_TO_ABSOLUTE_INDEX(players::get_player_datum_index_from_controller_index(local_player_index)));
	}

	int carto_get_player_index_from_unit_datum_index(datum unit_datum_index)
	{
		PlayerIterator playersIt;
		while (playersIt.get_next_active_player())
		{
			datum unit_datum_index_check = playersIt.get_current_player_data()->unit_index;
			LOG_TRACE_FUNC("Checking datum: {0:x} - index: {1} against datum: {2:x}", unit_datum_index_check, playersIt.get_current_player_index(), unit_datum_index);

			if (unit_datum_index == unit_datum_index_check)
				return playersIt.get_current_player_index();
		}
		return NONE;
	}

	BYTE carto_get_unit_team_index(datum unit_datum_index)
	{
		BYTE team_index = NONE;
		char* unit_object = (char*)object_try_and_get_and_verify_type(unit_datum_index, FLAG(e_object_type::biped));
		if (unit_object != NULL)
		{
			team_index = *(BYTE*)(unit_object + 0x13C);
		}
		return team_index;
	}

	void carto_set_unit_speed_patch(bool hackit) {
		static BYTE oldBytes[8];
		static bool oldBytesRead = false;
		DWORD address = Memory::GetAddress(0x6AB7F, 0x6A3BA);

		if (oldBytesRead == false)
		{
			ReadBytesProtected(address, oldBytes, sizeof(oldBytes));
			oldBytesRead = true;
		}

		if (hackit)
		{
			NopFill(address, sizeof(oldBytes));
		}
		else
		{
			WriteBytes(address, oldBytes, sizeof(oldBytes));
		}
	}

	void carto_set_player_unit_grenades_count(int playerIndex, e_grenades type, BYTE count, bool resetEquipment)
	{
		if (type > e_grenades::Plasma)
		{
			LOG_TRACE_GAME("[H2MOD] set_player_unit_grenades_count() Invalid argument: type");
			return;
		}

		static const std::string grenadeEquipamentTagName[2] =
		{
			"objects\\weapons\\grenade\\frag_grenade\\frag_grenade",
			"objects\\weapons\\grenade\\plasma_grenade\\plasma_grenade"
		};

		datum unit_datum_index = s_player::GetPlayerUnitDatumIndex(playerIndex);
		//datum grenade_eqip_tag_datum_index = tags::find_tag(blam_tag::tag_group_type::equipment, grenadeEquipamentTagName[type]);

		char* unit_object = (char*)object_try_and_get_and_verify_type(unit_datum_index, FLAG(e_object_type::biped));
		if (unit_object != NULL)
		{
			// not sure what these flags are, but this is called when picking up grenades
			typedef void(__cdecl* entity_set_unk_flags_t)(datum objectIndex, int flags);
			auto p_simulation_action_object_update = Memory::GetAddress<entity_set_unk_flags_t>(0x1B6685, 0x1B05B5);

			typedef void(__cdecl* unit_add_grenade_to_inventory_send_t)(datum unitDatumIndex, datum equipamentTagIndex);
			auto p_unit_add_grenade_to_inventory_send = Memory::GetAddress<unit_add_grenade_to_inventory_send_t>(0x1B6F12, 0x1B0E42);

			// send simulation update for grenades if we control the simulation
			if (!s_game_globals::game_is_predicted())
			{
				// delete all weapons if required
				if (resetEquipment)
					units::remove_equipment(unit_datum_index);

				// set grenade count
				*(BYTE*)(unit_object + 0x252 + type) = count;

				p_simulation_action_object_update(unit_datum_index, FLAG(22)); // flag 22 seems to be sync entity grenade count (TODO: list all of the update types)
				//p_unit_add_grenade_to_inventory_send(unit_datum_index, grenade_eqip_tag_datum_index);
			}

			LOG_TRACE_GAME("set_player_unit_grenades_count() - sending grenade simulation update, playerIndex={0}, peerIndex={1}", playerIndex, NetworkSession::GetPeerIndex(playerIndex));
		}
	}

	void carto_give_unit_weapon_from_datum(datum unit_datum, datum weapon_tag_index)
	{
		if (!DATUM_IS_NONE(unit_datum))
		{
			s_object_placement_data object_placement;

			objects::create_new_placement_data(&object_placement, weapon_tag_index, unit_datum, 0);

			datum object_idx = objects::object_new(&object_placement);
			if (!DATUM_IS_NONE(object_idx))
			{
				units::remove_equipment(unit_datum);
				units::assign_equipment_to_unit(unit_datum, object_idx, 1);
			}
		}
	}
}