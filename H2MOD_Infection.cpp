#include <windows.h>
#include "H2MOD_Infection.h"
#include "H2MOD.h"
#include "xliveless.h"
#include "h2mod.pb.h"
#include <stdlib.h>
#include <mutex>

extern bool isHost;

void Infection::FindZombie()
{
	TRACE_GAME("[H2MOD-Infection] - FindZombie()");

	int zombie = rand() % h2mod->NetworkPlayers.size();

	if (zombie == 0)
	{
		TRACE_GAME("[H2Mod-Infection] - Local host is zombie!");
		infected_players[h2mod->get_local_player_name()] = true;
		h2mod->set_local_team_index(3);
	}
	
	int i = 1;
	for (auto it = h2mod->NetworkPlayers.begin(); it != h2mod->NetworkPlayers.end(); ++it)
	{
		if (i == zombie)
		{
			infected_players[it->first->PlayerName] = true;
			H2ModPacket teampak;

			teampak.set_type(H2ModPacket_Type_set_player_team);
			h2mod_set_team *set_team = teampak.mutable_h2_set_player_team();

			set_team->set_team(3);
			set_team->set_name((char*)it->first->PlayerName, 32);

			char* SendBuf = new char[teampak.ByteSize()];
			teampak.SerializeToArray(SendBuf, teampak.ByteSize());

			it->first->PacketData = SendBuf;
			it->first->PacketSize = teampak.ByteSize();
			it->first->PacketsAvailable = true;

		}
		else 
		{
			H2ModPacket teampak;
			teampak.set_type(H2ModPacket_Type_set_player_team);

			h2mod_set_team *set_team = teampak.mutable_h2_set_player_team();
			set_team->set_team(0);
			set_team->set_name((char*)it->first->PlayerName, 32);

			char* SendBuf = new char[teampak.ByteSize()];
			teampak.SerializeToArray(SendBuf, teampak.ByteSize());

			it->first->PacketData = SendBuf;
			it->first->PacketSize = teampak.ByteSize();
			it->first->PacketsAvailable = true;
		}
		
		i++;
	}
}



void Infection::Initialize()
{
	if (isHost)
	{
		TRACE_GAME("[H2Mod-Infection] - Initializing!");
		
		if (this->infected_players.size() > 0)
		{
			for (auto it = this->infected_players.begin(); it != this->infected_players.end(); ++it)
			{
				it->second = false;
			}
		}
		
		this->FindZombie();
	}

}

void Infection::PreSpawn(int PlayerIndex)
{
	TRACE_GAME("[H2Mod-Infection] - Prespawn");

	if (isHost)
	{
		wchar_t* playername = h2mod->get_player_name_from_index(PlayerIndex);
		for (auto it = this->infected_players.begin(); it != this->infected_players.end(); ++it)
		{
			if (it->second == true)
			{
				if (wcscmp(playername, it->first) == 0)
				{
					h2mod->set_unit_biped(BipedType::Elite, PlayerIndex);
				}
			}
			else
			{
				h2mod->set_unit_biped(BipedType::MasterChief, PlayerIndex);
			}
		}

	}
}

void Infection::SpawnPlayer(int PlayerIndex)
{
	TRACE_GAME("[H2Mod-Infection] - SpawnPlayer: %i", PlayerIndex);

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
	TRACE_GAME("[H2Mod-Infection] - PlayerInfected: %08X", unit_datum_index);

	int pIndex = h2mod->get_player_index_from_unit_datum(unit_datum_index);
	
	wchar_t* playername = h2mod->get_player_name_from_index(pIndex);
	for (auto it = this->infected_players.begin(); it != this->infected_players.end(); ++it)
	{
		if (wcscmp(playername, it->first) == 0)
		{
			it->second = true;
		}
	}

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