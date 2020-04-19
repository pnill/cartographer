#include "stdafx.h"
#include "Globals.h"
#include "Blam/BlamLibrary.h"
#include "H2MOD/Tags/TagInterface.h"

FireFight::FireFight()
{
	this->deinitializer = new FireFightDeinitializer();
	this->initializer = new FireFightInitializer();
	this->preSpawnPlayer = new FireFightPreSpawnHandler();
	this->spawnPlayer = new FireFightSpawnHandler();
	this->playerDeath = new FireFightDeathHandler();
	this->playerKill = new FireFightKillHandler();
}

void FireFight::KilledAI(datum ai_datum, XUID killer)
{
	int points = 0;
	DatumIterator<Actor> actorIt(game_state_actors);
	DatumIterator<ObjectHeader> objectIt(game_state_objects_header);
	BipedObjectDefinition* actorObject = (BipedObjectDefinition*)objectIt.get_data_at_index(ai_datum.Index)->object;

	if (objectIt.get_data_at_index(ai_datum.Index)->type == Objects::ObjectType::biped)
	{
		datum actor_datum = actorObject->ActorDatum; // Grab the actor from the killed AI
		if (actor_datum.Index != -1) // Ensure that it was valid
		{
			datum char_datum = actorIt.get_data_at_index(actor_datum.Index)->character_datum; // get the character tag datum assigned to the actor.
			auto *character = tags::get_tag<'char', character_tag_group>(char_datum);

			if (character && character->SwarmProperties.size > 0)
				points = character->SwarmProperties[0]->scatterKilledCount;

			device_shop->AddPoints(killer, points);
		}
	}

}

XUID FireFightHandler::GetXUID()
{
	return this->xuid;
}

datum FireFightHandler::GetKilledDatum()
{
	return this->killed_datum;
}

void FireFightHandler::SetXUID(XUID xuid)
{
	this->xuid = xuid;
}

/* Should probably inherit variant_player... */
void FireFightHandler::SetPlayerIndex(datum player_datum)
{
	XUID player = variant_player->GetXUID(player_datum, true);
	variant_player->SetPlayerDatum(player,player_datum);

	SetXUID(player);
}

void FireFightHandler::SetUnitDatum(datum unit_datum)
{

	XUID player = variant_player->GetXUID(unit_datum, false);
	variant_player->SetUnitDatum(player, unit_datum);

	SetXUID(player);
}

void FireFightHandler::SetKilledDatum(datum unit_datum)
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
	variant_player->Deinitialize();
	device_shop->deinitialize();
}

void FireFightDeinitializer::onDedi()
{
	variant_player->Deinitialize();
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
	The reason being is it gives us the datum of the AI where the OnPlayerScore function will not.
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
