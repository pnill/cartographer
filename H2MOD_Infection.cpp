#include <windows.h>
#include "H2MOD_Infection.h"
#include "H2MOD.h"
#include "xliveless.h"
#include "h2mod.pb.h"
#include <stdlib.h>
#include <mutex>

extern bool isHost;
bool infected_played = false;
bool first_spawn = true;

void Infection::FindZombie()
{
	TRACE_GAME("[H2MOD-Infection] - FindZombie()");

	int zombie = rand() % (h2mod->NetworkPlayers.size()+1);
	zombie = rand() % (h2mod->NetworkPlayers.size()+1);
	

	InfectionPlayer *Local = new InfectionPlayer;
	wcscpy(&Local->PlayerName[0], h2mod->get_local_player_name());

	TRACE_GAME("[H2Mod-Infection] - Zombie: %i", zombie);
	if (zombie == 0)
	{
		TRACE_GAME("[H2Mod-Infection] - Local host is zombie!");
		

		Local->infected = true;
		infected_players[Local] = true;

		h2mod->set_local_team_index(3);

	}
	else 
	{
		TRACE_GAME("[H2Mod-Infection] - Local host is a human!");
		Local->infected = false;
		infected_players[Local] = true;

		h2mod->set_local_team_index(0);
	}
	
	int i = 1;
	if (h2mod->NetworkPlayers.size() > 0)
	{
		for (auto it = h2mod->NetworkPlayers.begin(); it != h2mod->NetworkPlayers.end(); ++it)
		{
			InfectionPlayer *nPlayer = new InfectionPlayer;
			wcscpy(&nPlayer->PlayerName[0], it->first->PlayerName);

			if (i == zombie)
			{
				TRACE_GAME("[H2Mod-Infection] Found Zombie: %ws", it->first->PlayerName);

				nPlayer->infected = true;
				infected_players[nPlayer] = true;

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
				TRACE_GAME("[H2Mod-Infection] %ws is a human!", it->first->PlayerName);

				nPlayer->infected = false;
				infected_players[nPlayer] = true;
			}

			i++;
		}
	}
}



void Infection::Initialize()
{
	h2mod->DisableSound(SoundType::Slayer);
	h2mod->DisableSound(SoundType::GainedTheLead);
	h2mod->DisableSound(SoundType::LostTheLead);
	h2mod->DisableSound(SoundType::TeamChange);

	infected_played = false;
	first_spawn = true;

	if(h2mod->get_local_team_index() != 3)
		h2mod->set_local_team_index(0);

	if (isHost)
	{
		TRACE_GAME("[H2Mod-Infection] - Initializing!");
		TRACE_GAME("[H2Mod-Infection] - this->infected_players.size(): %i", this->infected_players.size());
		
		if (this->infected_players.size() > 0)
		{
			for (auto it = this->infected_players.begin(); it != this->infected_players.end(); ++it)
			{
				delete[] it->first;
			}

			this->infected_players.clear();
		}

		this->FindZombie();
	}

}

void Infection::PreSpawn(int PlayerIndex)
{

	TRACE_GAME("[H2Mod-Infection] - Prespawn( %i ) ",PlayerIndex);
	
	wchar_t* playername = h2mod->get_player_name_from_index(PlayerIndex);
	
	if (!isHost)
	{
		if (wcscmp(playername, h2mod->get_local_player_name()) == 0)
		{
			if (h2mod->get_local_team_index() == 3)
			{
				h2mod->set_unit_biped(BipedType::Elite, PlayerIndex);

			}
		}
	}

	if (isHost)
	{
		
		for (auto it = this->infected_players.begin(); it != this->infected_players.end(); ++it)
		{
			if (wcscmp(playername, &it->first->PlayerName[0]) == 0)
			{
				TRACE_GAME("[H2mod-Infection] - Prespawn(%i) player %ws infection value is : %i", PlayerIndex, it->first->PlayerName, it->first->infected);
				TRACE_GAME("[H2Mod-Infection] - Prespawn(%i) playername %ws : %ws", PlayerIndex, playername, it->first->PlayerName);

				if (it->first->infected == true)
				{
					h2mod->set_unit_biped(BipedType::Elite, PlayerIndex);
				}
				else
				{
					h2mod->set_unit_biped(BipedType::MasterChief, PlayerIndex);
				}
			}
		}

	}
}

void Infection::SpawnPlayer(int PlayerIndex)
{
	TRACE_GAME("[H2Mod-Infection] - SpawnPlayer(%i)", PlayerIndex);

	wchar_t* playername = h2mod->get_player_name_from_index(PlayerIndex);
	if (wcscmp(playername, h2mod->get_local_player_name()) == 0)
	{
		if (first_spawn == true)
		{
				h2mod->SoundMap[L"infection.wav"] = 1000;
			

			first_spawn = false;
		}

		if (h2mod->get_local_team_index() == 3 && infected_played == false)
		{
			h2mod->SoundMap[L"infected.wav"] = 500;
		
			infected_played = true;
		}
	}
	
	
	if (isHost)
	{
		int unit_datum_index = h2mod->get_unit_datum_from_player_index(PlayerIndex);
		int unit_object = call_get_object(unit_datum_index, 3);

		if (unit_object)
		{
			call_unit_reset_equipment(unit_datum_index);

			if (h2mod->get_unit_team_index(unit_datum_index) == 0)
			{
				h2mod->set_unit_biped(BipedType::MasterChief,PlayerIndex);
				GivePlayerWeapon(PlayerIndex, Weapon::shotgun,1);
				//GivePlayerWeapon(PlayerIndex, Weapon::magnum, 1);
				GivePlayerWeapon(PlayerIndex, Weapon::magnum, 0);
			}

			if (h2mod->get_unit_team_index(unit_datum_index) == 3)
			{
				h2mod->set_unit_biped(BipedType::Elite,PlayerIndex);
				GivePlayerWeapon(PlayerIndex, Weapon::energy_blade,1);
			}
		}
	}
}

void Infection::PlayerInfected(int unit_datum_index)
{
	TRACE_GAME("[H2Mod-Infection] - PlayerInfected()", unit_datum_index);

	int pIndex = h2mod->get_player_index_from_unit_datum(unit_datum_index);

	if (isHost)
	{
		wchar_t* playername = h2mod->get_player_name_from_index(pIndex);
		for (auto it = this->infected_players.begin(); it != this->infected_players.end(); ++it)
		{
			if (wcscmp(playername, &it->first->PlayerName[0]) == 0)
			{
				TRACE_GAME("[H2Mod-Infection] - PlayerInfected() PlayerInfected %ws compared to %ws", playername, &it->first->PlayerName[0]);
				TRACE_GAME("[H2Mod-Infection] PlayerInfected() Made %ws infected!", &it->first->PlayerName[0]);
				it->first->infected = true;
			}
		}
	}

	int unit_object = call_get_object(unit_datum_index, 3);

	if (unit_object)
	{
		if (h2mod->get_unit_team_index(unit_datum_index) != 3)
		{
			if(isHost)
				h2mod->set_unit_biped(BipedType::Elite, pIndex);
		
			TRACE_GAME("[H2Mod-Infection] PlayerInfected() player died: %ws", h2mod->get_player_name_from_index(pIndex));
			TRACE_GAME("[H2Mod-Infection] PlayerInfected() Local Player: %ws", h2mod->get_local_player_name());
			TRACE_GAME("[H2Mod-Infection] PlayerInfected() pIndex: %08X", unit_datum_index, pIndex);
	
#pragma region Infecting Local Player
		
			if (wcscmp(h2mod->get_player_name_from_index(pIndex), h2mod->get_local_player_name()) == 0)
			{
				h2mod->set_local_team_index(3);
			}
			else 
			{
				h2mod->SoundMap[L"new_zombie.wav"] = 1000;
			}
		}
#pragma endregion

		call_unit_reset_equipment(unit_datum_index);
	}

}