#include "stdafx.h"
#include "Globals.h"
#include "Blam/BlamLibrary.h"

#include "H2MOD.h"

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


void HeadHunter::SpawnSkull(datum unit_datum)
{
	DatumIterator<ObjectHeader> objectIt(game_state_objects_header);
	BipedObjectDefinition* biped_unit = (BipedObjectDefinition*)objectIt.get_data_at_index(unit_datum.Index)->object;

	if (objectIt.get_data_at_index(unit_datum.Index)->type == Objects::ObjectType::biped)
	{
		ObjectPlacementData nObject;

		call_object_placement_data_new(&nObject, Weapon::ball, -1, 0);

		nObject.Placement.X = biped_unit->Placement.X;
		nObject.Placement.Y = biped_unit->Placement.Y;
		nObject.Placement.Z = biped_unit->Placement.Z;
		nObject.TranslationalVelocity.X = biped_unit->TranslationalVelocity.X;
		nObject.TranslationalVelocity.Y = biped_unit->TranslationalVelocity.Y;
		nObject.TranslationalVelocity.Z = biped_unit->TranslationalVelocity.Z;

		datum new_object_datum = call_object_new(&nObject);
		if (!new_object_datum.IsNull())
			call_add_object_to_sync(new_object_datum);
	}
}

extern void addDebugText(const char* text);

typedef void(__stdcall *update_player_score)(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6);
extern update_player_score pupdate_player_score;

void HeadHunter::PickupSkull(XUID player, datum SkullDatum)
{
	if (SkullDatum != NULL)
	{
		//typedef void(__stdcall *update_player_score)(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6);
		//	20 / 10 / 2018 18 : 48 : 39.756 update_player_score_hook(thisptr : 3000595C, a2 : 00000001, a3 : 00000000, a4 : 00000001, a5 : FFFFFFFF, a6 : 00000000)

		datum PlayerDatum = variant_player->GetPlayerDatum(player);
		pupdate_player_score((void*)(h2mod->Server ? 0x30005508 : 0x3000595C), PlayerDatum.Index, 0, 1, -1, 0);
		call_hs_object_destroy(SkullDatum);
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

void HeadHunterHandler::SetDeadPlayer(datum dead_datum)
{
	this->DeadPlayer = dead_datum;
}

bool HeadHunterHandler::SetInteractedObject(datum object_datum)
{
	DatumIterator<ObjectHeader> objectIt(game_state_objects_header);
	WeaponObjectDefinition* weaponObject = (WeaponObjectDefinition*)objectIt.get_data_at_index(object_datum.ToAbsoluteIndex())->object;

	if (weaponObject->TagDefinitionIndex.Index == (Weapon::ball & 0xFFFF))
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

datum HeadHunterHandler::GetInteractedObject()
{
	return this->object_interaction;
}

datum HeadHunterHandler::GetDeadPlayer()
{
	return this->DeadPlayer;
}

/* Should probably inherit variant_player... */
void HeadHunterHandler::SetPlayerIndex(datum player_datum)
{
	XUID player = variant_player->GetXUID(player_datum, true);
	variant_player->SetPlayerDatum(player, player_datum);
	SetXUID(player);
}

void HeadHunterHandler::SetUnitDatum(datum unit_datum)
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
	variant_player->Deinitialize();
}

void HeadHunterDeinitializer::onDedi()
{
	variant_player->Deinitialize();
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