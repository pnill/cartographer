#include "stdafx.h"
#include "FireFight.h"

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

void FireFight::KilledAI(DatumIndex ai_datum,XUID killer)
{
	int points = 0;
	if ((&game_state_objects_header->object_header[ai_datum.Index])->object->ObjectType == Objects::ObjectType::biped)
	{
		DatumIndex actor_datum = (&game_state_objects_header->object_header[ai_datum.Index])->object->ActorDatum; // Grab the actor from the killed AI
		if (actor_datum.Index != -1) // Ensure that it was valid
		{
			DatumIndex char_datum = (&game_state_actors->actors[actor_datum.Index])->character_datum; // get the character tag datum assigned to the actor.
			character_tag_group *char_tag = (character_tag_group*)tag_instances[char_datum.Index].GetTag(); // get the character tag.

			if (char_tag->SwarmProperties.size > 0)
				points = char_tag->SwarmProperties[0]->scatterKilledCount;


			device_shop->AddPoints(killer, points);
		}
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
