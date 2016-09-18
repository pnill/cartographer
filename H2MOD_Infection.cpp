#include <windows.h>
#include "H2MOD_Infection.h"
#include "H2MOD.h"
#include "xliveless.h"
extern bool isHost;

void Infection::Initialize()
{
	if (isHost)
	{
		for (int i = 0; i <= 16; i++) this->infected_players[i] = 0;
	}

}

void Infection::PreSpawn(int PlayerIndex)
{
	if (isHost)
	{
		if (this->infected_players[PlayerIndex])
		{
			h2mod->set_unit_biped(BipedType::Elite, PlayerIndex);
		}
		else
		{
			h2mod->set_unit_biped(BipedType::MasterChief, PlayerIndex);
		}
	}
}

void Infection::SpawnPlayer(int PlayerIndex)
{

	if (isHost)
	{
		int unit_datum_index = h2mod->get_unit_datum_from_player_index(PlayerIndex);
		int unit_object = call_get_object(unit_datum_index, 3);

		if (unit_object)
		{
			call_unit_reset_equipment(unit_datum_index);

			if (h2mod->get_unit_team_index(unit_datum_index) == 0)
			{
				GivePlayerWeapon(PlayerIndex, Weapon::shotgun);
			}

			if (h2mod->get_unit_team_index(unit_datum_index) == 3)
			{
				GivePlayerWeapon(PlayerIndex, Weapon::energy_blade);
			}
		}
	}

}

void Infection::PlayerInfected(int unit_datum_index)
{
	int pIndex = h2mod->get_player_index_from_unit_datum(unit_datum_index);

	this->infected_players[pIndex] = 1;

	int unit_object = call_get_object(unit_datum_index, 3);

	if (unit_object)
	{
#pragma region Infecting Local Player
		if (h2mod->get_unit_team_index(unit_datum_index) == 0)
		{
			
			h2mod->set_unit_biped(BipedType::Elite, pIndex);

			TRACE_GAME("[Infection] Player died: %ws", h2mod->get_player_name_from_index(pIndex));
			TRACE_GAME("[Infection] Local Player: %ws", h2mod->get_local_player_name());
			TRACE_GAME("[Infection] PlayerInfected! - unit_datum_index: %08X, pIndex: %08X", unit_datum_index, pIndex);

			if (wcscmp(h2mod->get_player_name_from_index(pIndex), h2mod->get_local_player_name()) == 0)
			{
				h2mod->set_local_team_index(3);
			}
		}
#pragma endregion

		call_unit_reset_equipment(unit_datum_index);
	}

}