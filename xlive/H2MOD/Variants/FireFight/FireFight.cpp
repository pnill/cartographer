#include "Globals.h"
#include "Blam/BlamLibrary.h"

FireFight::FireFight()
{
	this->deinitializer = new FireFightDeinitializer();
	this->initializer = new FireFightInitializer();
	this->preSpawnPlayer = new FireFightPreSpawnHandler();
	this->spawnPlayer = new FireFightSpawnHandler();
	this->playerDeath = new FireFightDeathHandler();
	this->playerKill = new FireFightKillHandler();
}

/*
DEPRECATED - Use tag_instances, tag_group method.
int get_swarm_scatter_killed_count(DatumIndex character_datum)
{
	__int16 character_index = character_datum.Index;
	character_index = character_index << 4; //shift left 4

	DWORD tag_header = *(DWORD*)((BYTE*)h2mod->GetBase() + 0x47CD54);
	DWORD global_tag_instances = *(DWORD*)((BYTE*)h2mod->GetBase() + 0x47CD50);


	printf("global_tag_instances: %08X", global_tag_instances);
	DWORD character_offset = *(DWORD*)((BYTE*)global_tag_instances + character_index + 8);
	DWORD character_tag = (DWORD)((BYTE*)character_offset + tag_header);

	printf("character_offset: %08X", character_offset);
	printf("character_tag: %08X", character_tag);

	int swarm_count = *(int*)((BYTE*)character_tag + 0x64);

	if (swarm_count > 0)
	{
		DWORD swarm_offset = *(DWORD*)((BYTE*)character_tag + 0x68);
		DWORD tag_instance_pointer = *(DWORD*)((BYTE*)h2mod->GetBase() + 0x482290);
		DWORD swarm_pointer = (DWORD)((BYTE*)tag_instance_pointer + swarm_offset);
		DWORD swarm_scatter_killed_count = *(DWORD*)((BYTE*)swarm_pointer);
		return swarm_scatter_killed_count;
	}


	return 0;
}*/

void FireFight::KilledAI(DatumIndex ai_datum,XUID killer)
{
	int points = 0;

	DatumIndex actor_datum = (&game_state_objects_header->object_header[ai_datum.Index])->object->ActorDatum; // Grab the actor from the killed AI
	if (actor_datum.Index != -1) // Ensure that it was valid
	{
		DatumIndex char_datum = (&game_state_actors->actors[actor_datum.Index])->character_datum; // get the character tag datum assigned to the actor.

		void* character_tag_offset = tag_instances[char_datum.Index].offset;

		DWORD tag_header = *(DWORD*)((BYTE*)h2mod->GetBase() + 0x47CD54);
		character_tag_group *char_tag = (character_tag_group*)((BYTE*)character_tag_offset + tag_header);

		if(char_tag->SwarmProperties.size > 0)
			points = char_tag->SwarmProperties[0]->scatterKilledCount;
	

		device_shop->AddPoints(killer, points);
	}

}

XUID FireFightHandler::GetXUID()
{
	return this->xuid;
}

DatumIndex FireFightHandler::GetKilledDatum()
{
	return this->killed_datum;
}

void FireFightHandler::SetXUID(XUID xuid)
{
	this->xuid = xuid;
}

/* Should probably inherit variant_player... */
void FireFightHandler::SetPlayerIndex(DatumIndex player_datum)
{
	XUID player = variant_player->GetXUID(player_datum, true);
	variant_player->SetPlayerDatum(player,player_datum);

	SetXUID(player);
}

void FireFightHandler::SetUnitDatum(DatumIndex unit_datum)
{

	XUID player = variant_player->GetXUID(unit_datum, false);
	variant_player->SetUnitDatum(player, unit_datum);

	SetXUID(player);
}

void FireFightHandler::SetKilledDatum(DatumIndex unit_datum)
{
	this->killed_datum = unit_datum;
}

void FireFight::initClient()
{

}

void FireFight::initHost()
{

}

void FireFightInitializer::onClient()
{

}

void FireFightInitializer::onPeerHost()
{

}

void FireFightInitializer::onDedi()
{

}

void FireFightDeinitializer::onClient()
{
	
}

void FireFightDeinitializer::onPeerHost()
{
	variant_player->deinitialize();
	device_shop->deinitialize();
}

void FireFightDeinitializer::onDedi()
{
	variant_player->deinitialize();
	device_shop->deinitialize();
}

void FireFightPreSpawnHandler::onClient()
{

}

void FireFightPreSpawnHandler::onPeerHost()
{

}

void FireFightPreSpawnHandler::onDedi()
{

}

void FireFightSpawnHandler::onClient()
{

}

void FireFightSpawnHandler::onPeerHost()
{

}

void FireFightSpawnHandler::onDedi()
{

}


/*
	In the case of FireFight currently we use the OnDeath handler to also determine that someone killed an AI,
	The reason being is it gives us the DatumIndex of the AI where the OnPlayerScore function will not.
*/
void FireFightDeathHandler::onClient()
{

}

void FireFightDeathHandler::onPeerHost()
{
	FireFight::KilledAI(this->GetKilledDatum(), this->GetXUID());
}

void FireFightDeathHandler::onDedi()
{
	FireFight::KilledAI(this->GetKilledDatum(),this->GetXUID());
}

void FireFightKillHandler::onClient()
{
	
}

void FireFightKillHandler::onPeerHost()
{

}

void FireFightKillHandler::onDedi()
{

}
