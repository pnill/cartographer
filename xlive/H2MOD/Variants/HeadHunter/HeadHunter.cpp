#include "HeadHunter.h"

#include "H2MOD/Modules/Utils/Utils.h"
#include "H2MOD/Modules/Config/Config.h"
#include "H2MOD/Modules/CustomMenu/CustomLanguage.h"
#include "H2MOD/Modules/HaloScript/HaloScript.h"
#include "H2MOD/Engine/Engine.h"

#include "H2MOD.h"

int soundBuffer = 0;
std::map<int, std::map<e_headhunter_sounds, const wchar_t*>> H_SoundsTable;
bool h_firstSpawn = true;

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
void HeadHunter::triggerSound(e_headhunter_sounds sound, int sleep)
{
	if (H_SoundsTable.count(H2Config_language.code_main)) 
	{
		LOG_TRACE_GAME(L"[h2mod-headhunter] Triggering sound {}", H_SoundsTable[H2Config_language.code_main][sound]);
		h2mod->custom_sound_play(H_SoundsTable[H2Config_language.code_main][sound], sleep);
	}
	else 
	{
		LOG_TRACE_GAME(L"[h2mod-headhunter] Triggering sound {}", H_SoundsTable[0][sound]);
		h2mod->custom_sound_play(H_SoundsTable[0][sound], sleep);
	}
}

void HeadHunter::spawnPlayerClientSetup()
{
	if (h_firstSpawn)
	{
		triggerSound(head_hunter, 1000);
		h_firstSpawn = false;
	}
}

void HeadHunter::SpawnSkull(datum unit_datum)
{
	s_biped_object_definition* biped_unit = (s_biped_object_definition*)object_try_and_get_and_verify_type(unit_datum, FLAG(e_object_type::biped));

	if (biped_unit != NULL)
	{
		s_object_placement_data nObject;

		Engine::Objects::create_new_placement_data(&nObject, e_weapons_datum_index::ball, -1, 0);

		nObject.placement = biped_unit->placement;
		nObject.translational_velocity = biped_unit->translational_velocity;

		datum new_object_datum = Engine::Objects::call_object_new(&nObject);
		if (!DATUM_IS_NONE(new_object_datum))
			call_add_object_to_sync(new_object_datum);
	}
}

typedef void(__stdcall *update_player_score)(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6);
extern update_player_score pupdate_player_score;

void HeadHunter::PickupSkull(XUID player, datum SkullDatum)
{
	if (SkullDatum != NULL)
	{
		//typedef void(__stdcall *update_player_score)(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6);
		//	20 / 10 / 2018 18 : 48 : 39.756 update_player_score_hook(thisptr : 3000595C, a2 : 00000001, a3 : 00000000, a4 : 00000001, a5 : FFFFFFFF, a6 : 00000000)

		typedef char*(__cdecl* get_score_data_ptr)();
		auto p_get_score_data_ptr = Memory::GetAddress<get_score_data_ptr>(0x6B8A7, 0x6AD32);

		char* player_score_data = p_get_score_data_ptr();
		if (player_score_data)
		{
			datum PlayerDatum = variant_player->GetPlayerDatum(player);
			pupdate_player_score(player_score_data, DATUM_ABSOLUTE_INDEX(PlayerDatum), 0, 1, -1, 0);
			HaloScript::ObjectDestroy(SkullDatum);
			if(TimeElapsedMS(soundBuffer) > 2500)
			{
				soundBuffer = GetCurrentTimeMS();
				triggerSound(skull_scored, 500);
			}
		}
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
	s_weapon_object_definition* weaponObject = object_get_fast_unsafe<s_weapon_object_definition>(object_datum);

	if (DATUM_ABSOLUTE_INDEX(weaponObject->tag_definition_index) == DATUM_ABSOLUTE_INDEX(e_weapons_datum_index::ball))
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
	h_firstSpawn = true;
	h2mod->disable_sounds(FLAG(SoundType::Slayer) | ALL_SOUNDS_NO_SLAYER);
	H_SoundsTable[language_ids::english][e_headhunter_sounds::head_hunter] = L"sounds/en/headhunter.wav";
	H_SoundsTable[language_ids::english][e_headhunter_sounds::skull_scored] = L"sounds/en/skull_scored.wav";
	H_SoundsTable[language_ids::spanish][e_headhunter_sounds::head_hunter] = L"sounds/es/headhunter.wav";
	H_SoundsTable[language_ids::spanish][e_headhunter_sounds::skull_scored] = L"sounds/es/skull_scored.wav";
}

void HeadHunter::initHost()
{

}



void HeadHunterInitializer::onClient()
{
	HeadHunter::initClient();
}

void HeadHunterInitializer::onPeerHost()
{
	HeadHunter::initClient();
}

void HeadHunterInitializer::onDedi()
{
	HeadHunter::initClient();
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
	HeadHunter::spawnPlayerClientSetup();
}

void HeadHunterSpawnHandler::onPeerHost()
{
	HeadHunter::spawnPlayerClientSetup();
}

void HeadHunterSpawnHandler::onDedi()
{
	HeadHunter::spawnPlayerClientSetup();
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