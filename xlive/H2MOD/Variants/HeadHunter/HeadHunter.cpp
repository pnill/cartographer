#include "stdafx.h"
#include "Globals.h"
#include "Blam/BlamLibrary.h"
#include <algorithm>

extern void __cdecl call_hs_object_destroy_datum(DatumIndex object_datum_index); // need to fix some shit so I don't have to do this.

HeadHunter::HeadHunter()
{
	this->deinitializer = new HeadHunterDeinitializer();
	this->initializer = new HeadHunterInitializer();
	this->preSpawnPlayer = new HeadHunterPreSpawnHandler();
	this->spawnPlayer = new HeadHunterSpawnHandler();
	this->playerDeath = new HeadHunterDeathHandler();
	this->playerKill = new HeadHunterKillHandler();
	this->itemInteraction = new HeadHunterItmInteraction();
}


void HeadHunter::SpawnSkull(DatumIndex unit_datum)
{
	if ((&game_state_objects_header->data[unit_datum.Index])->object->ObjectType == Objects::ObjectType::biped)
	{
		using namespace Blam::EngineDefinitions::Objects;
		ObjectPlacementData* nObject = new ObjectPlacementData;

		call_object_placement_data_new(nObject, Weapon::ball, -1, 0);
		nObject->Placement.X = (&game_state_objects_header->data[unit_datum.Index])->object->Placement.X;
		nObject->Placement.Y = (&game_state_objects_header->data[unit_datum.Index])->object->Placement.Y;
		nObject->Placement.Z = (&game_state_objects_header->data[unit_datum.Index])->object->Placement.Z;

		int GameStateObject = call_object_new(nObject); //TODO: Re-write all these functions to use DatumIndex.
		if (GameStateObject != NULL)
		{
			call_add_object_to_sync(GameStateObject);
		}
		else
			delete nObject;

	}
}

extern void addDebugText(const char* text);

typedef void(__stdcall *update_player_score)(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6);
extern update_player_score pupdate_player_score;

void HeadHunter::PickupSkull(XUID player, DatumIndex SkullDatum)
{
	if (SkullDatum != NULL)
	{
		//typedef void(__stdcall *update_player_score)(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6);
		//	20 / 10 / 2018 18 : 48 : 39.756 update_player_score_hook(thisptr : 3000595C, a2 : 00000001, a3 : 00000000, a4 : 00000001, a5 : FFFFFFFF, a6 : 00000000)

		DatumIndex PlayerDatum = variant_player->GetPlayerDatum(player);
		pupdate_player_score((void*)(h2mod->Server ? 0x30005508 : 0x3000595C), PlayerDatum.Index, 0, 1, -1, 0);
		call_hs_object_destroy_datum(SkullDatum);
		//pupdate_player_score()
	}
}

XUID HeadHunterHandler::GetXUID()
{
	return this->xuid;
}

void HeadHunterHandler::SetXUID(XUID xuid)
{
	this->xuid = xuid;
}

void HeadHunterHandler::SetDeadPlayer(DatumIndex dead_datum)
{
	this->DeadPlayer = dead_datum;
}

bool HeadHunterHandler::SetInteractedObject(DatumIndex object_datum)
{
	if ((&game_state_objects_header->data[object_datum.Index])->object->DefinitionIndex.Index == (Weapon::ball & 0xFFFF))
	{
		this->object_interaction = object_datum;
		return true;
	}
	else
	{
		this->object_interaction = NULL;
		return false;
	}


	return false;
}

DatumIndex HeadHunterHandler::GetInteractedObject()
{
	return this->object_interaction;
}

DatumIndex HeadHunterHandler::GetDeadPlayer()
{
	return this->DeadPlayer;
}

/* Should probably inherit variant_player... */
void HeadHunterHandler::SetPlayerIndex(DatumIndex player_datum)
{
	XUID player = variant_player->GetXUID(player_datum, true);
	variant_player->SetPlayerDatum(player, player_datum);

	SetXUID(player);
}

void HeadHunterHandler::SetUnitDatum(DatumIndex unit_datum)
{

	XUID player = variant_player->GetXUID(unit_datum, false);
	variant_player->SetUnitDatum(player, unit_datum);
	SetXUID(player);
}

void HeadHunter::initClient()
{

}

void HeadHunter::initHost()
{

}

void HeadHunterInitializer::onClient()
{

}

void HeadHunterInitializer::onPeerHost()
{

}

void HeadHunterInitializer::onDedi()
{

}

void HeadHunterDeinitializer::onClient()
{

}

void HeadHunterDeinitializer::onPeerHost()
{
	variant_player->deinitialize();
}

void HeadHunterDeinitializer::onDedi()
{
	variant_player->deinitialize();
}

void HeadHunterPreSpawnHandler::onClient()
{

}

void HeadHunterPreSpawnHandler::onPeerHost()
{

}

void HeadHunterPreSpawnHandler::onDedi()
{

}

void HeadHunterSpawnHandler::onClient()
{

}

void HeadHunterSpawnHandler::onPeerHost()
{

}

void HeadHunterSpawnHandler::onDedi()
{

}

void HeadHunterDeathHandler::onClient()
{

}

void HeadHunterDeathHandler::onPeerHost()
{
	HeadHunter::SpawnSkull(this->GetDeadPlayer());
}

void HeadHunterDeathHandler::onDedi()
{
	HeadHunter::SpawnSkull(this->GetDeadPlayer());
}

void HeadHunterKillHandler::onClient()
{

}

void HeadHunterKillHandler::onPeerHost()
{

}

void HeadHunterKillHandler::onDedi()
{

}

void HeadHunterItmInteraction::onClient()
{

}

void HeadHunterItmInteraction::onPeerHost()
{
	HeadHunter::PickupSkull(this->GetXUID(), this->GetInteractedObject());
}

void HeadHunterItmInteraction::onDedi()
{
	HeadHunter::PickupSkull(this->GetXUID(), this->GetInteractedObject());
}