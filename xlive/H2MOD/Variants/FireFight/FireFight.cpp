#include "Globals.h"
#include "Blam/BlamLibrary.h"
std::unordered_map<XUID, int> FireFight::player_points;

FireFight::FireFight()
{
	this->deinitializer = new FireFightDeinitializer();
	this->initializer = new FireFightInitializer();
	this->preSpawnPlayer = new FireFightPreSpawnHandler();
	this->spawnPlayer = new FireFightSpawnHandler();
	this->playerDeath = new FireFightDeathHandler();
	this->playerKill = new FireFightKillHandler();
}

void FireFight::KilledAI(DatumIndex ai_datum)
{
	DatumIndex actor_datum = (&game_state_objects_header->object_header[ai_datum.Index])->object->ActorDatum; // Grab the actor from the killed AI
	if (actor_datum.Index != -1) // Ensure that it was valid
	{
		DatumIndex char_tag_datum = (&game_state_actors->actors[ai_datum.Index])->character_datum; // get the character tag datum assigned to the actor.

	}

}

bool FireFight::BuyWeapon(int cost, DatumIndex weapon, XUID xuid)
{

	return false;
	/*if (cost > this->player_points[xuid])
		return false;
	else
	{
		this->player_points[xuid] = this->player_points[xuid] - cost;

		return true;
	}*/
}

/*
	Maybe we should have a shop handler instead of directly handling this in infection?
	e.x. 
		device_shop->IsShopDevice(datum);
		device_shop->GetCost(datum);
		device_shop->GetWeapon(datum);
	
*/
bool FireFightHandler::BuyWeapon(DatumIndex device_datum)
{
	
	//FireFight::BuyWeapon(cost, Weapon, this->GetXUID());
	return false;

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

/* We only handle points via XUID so if an player datum_index or unit_datum index is attempted to be set we'll simply resolve the XUID and store it */
void FireFightHandler::SetPlayerIndex(DatumIndex player_datum)
{
	this->xuid = (&game_state_players->players[player_datum.Index])->xuid;
}

void FireFightHandler::SetUnitDatum(DatumIndex unit_datum)
{
	short player_index = (&game_state_objects_header->object_header[unit_datum.Index])->object->PlayerDatum.Index;
	this->xuid = (&game_state_players->players[player_index])->xuid;
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

}

void FireFightDeinitializer::onDedi()
{

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
	FireFight::KilledAI(this->GetKilledDatum());
}

void FireFightDeathHandler::onPeerHost()
{
	FireFight::KilledAI(this->GetKilledDatum());
}

void FireFightDeathHandler::onDedi()
{
	FireFight::KilledAI(this->GetKilledDatum());
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
