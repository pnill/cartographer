#include "stdafx.h"
#include "GraveRobber.h"

#include "H2MOD.h"
#include "Blam/Engine/game/game_globals.h"
#include "Blam/Engine/Players/LocalPlayers.h"
#include "H2MOD/Modules/CustomMenu/CustomLanguage.h"
#include "H2MOD/Modules/HaloScript/HaloScript.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Utils/Utils.h"

bool b_firstSpawn;
const wchar_t* headhunterSoundTable[e_language_ids::_lang_id_end][e_graverobber_sounds::_graverobber_end]
{
	{SND_HEADHUNTER_EN, SND_SKULL_SCORED_EN},
	{SND_HEADHUNTER_JP, SND_SKULL_SCORED_JP},
	{SND_HEADHUNTER_GE, SND_SKULL_SCORED_GE},
	{SND_HEADHUNTER_FR, SND_SKULL_SCORED_FR},
	{SND_HEADHUNTER_ES, SND_SKULL_SCORED_ES},
	{SND_HEADHUNTER_IT, SND_SKULL_SCORED_IT},
	{SND_HEADHUNTER_KO, SND_SKULL_SCORED_KO},
	{SND_HEADHUNTER_CH, SND_SKULL_SCORED_CH}
};

void GraveRobber::TriggerSound(e_graverobber_sounds sound, int sleep)
{
	const int language_id = *Memory::GetAddress<int*>(0x412818);

	if (Memory::IsDedicatedServer()) return;

	if (headhunterSoundTable[language_id][sound] != nullptr)
	{
		LOG_TRACE_GAME(L"[h2mod-graverobber] Triggering sound {}", headhunterSoundTable[language_id][sound]);
		h2mod->custom_sound_play(headhunterSoundTable[language_id][sound], sleep);
	}
}

void GraveRobber::SpawnPlayerClientSetup()
{
	if (b_firstSpawn)
	{
		TriggerSound(_snd_head_hunter, 1000);
		b_firstSpawn = false;
	}
}

void GraveRobber::SpawnSkull(datum unit_datum)
{
	const s_biped_data_definition* biped_unit = (s_biped_data_definition*)object_try_and_get_and_verify_type(unit_datum, FLAG(e_object_type::biped));

	if (biped_unit != NULL)
	{
		s_object_placement_data nObject;

		Engine::Objects::create_new_placement_data(&nObject, e_weapons_datum_index::ball, -1, 0);

		nObject.position = biped_unit->position;
		nObject.translational_velocity = biped_unit->translational_velocity;

		datum new_object_datum = Engine::Objects::object_new(&nObject);
		if (!DATUM_IS_NONE(new_object_datum))
			Engine::Objects::simulation_action_object_create(new_object_datum);
	}
}

extern update_player_score_t p_update_player_score;

void GraveRobber::PickupSkull(datum playerIdx, datum skullDatum)
{
	typedef char* (__cdecl* get_score_data_t)();
	auto p_get_score_data = Memory::GetAddress<get_score_data_t>(0x6B8A7, 0x6AD32);
	
	int absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx);

	if (DATUM_IS_NONE(skullDatum)) { return; }

	char* player_score_data = p_get_score_data();
	if (!player_score_data) { return; }

	if (!s_game_globals::game_is_predicted())
	{
		p_update_player_score(player_score_data, playerIdx, 0, 1, -1, 0);
	}
	
	HaloScript::ObjectDestroy(skullDatum);

	if (!Memory::IsDedicatedServer())
	{
		for (int i = 0; i < ENGINE_MAX_LOCAL_PLAYERS; i++)
		{
			if (DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(i)) == absPlayerIdx)
			{
				TriggerSound(_snd_skull_scored, 500);
				break;
			}
		}
	}
}

void GraveRobber::InitializeClient()
{
	h2mod->disable_sounds(FLAG(_sound_type_slayer) | ALL_SOUNDS_NO_SLAYER);
	b_firstSpawn = true;
}

void GraveRobber::Initialize()
{
	if (!Memory::IsDedicatedServer())
	{
		GraveRobber::InitializeClient();
	}
}

void GraveRobber::Dispose() 
{
	/*Unused*/
}

CustomVariantId GraveRobber::GetVariantId()
{
	return CustomVariantId::_id_graverobber;
}

void GraveRobber::OnMapLoad(ExecTime execTime, s_game_options* gameOptions)
{
	switch (execTime)
	{
	case ExecTime::_preEventExec:
		break;

	case ExecTime::_postEventExec:
		switch (h2mod->GetEngineType())
		{
		case _multiplayer:
			this->Initialize();
			break;
		case _main_menu:
			break;
		default:
			break;
		}
		break;

	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

void GraveRobber::OnPlayerSpawn(ExecTime execTime, datum playerIdx)
{
	switch (execTime)
	{
		// prespawn handler
	case ExecTime::_preEventExec:
		break;

		// postspawn handler
	case ExecTime::_postEventExec:
		if (!Memory::IsDedicatedServer())
			GraveRobber::SpawnPlayerClientSetup();
		break;
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

void GraveRobber::OnPlayerDeath(ExecTime execTime, datum playerIdx)
{
	const int absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx);
	const datum playerUnitDatum = s_player::GetPlayerUnitDatumIndex(absPlayerIdx);

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		// to note after the original function executes, the controlled unit by this player is set to NONE
		if (!s_game_globals::game_is_predicted())
			GraveRobber::SpawnSkull(playerUnitDatum);
		break;

	case ExecTime::_postEventExec:
		break;
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

bool GraveRobber::OnPlayerScore(ExecTime execTime, void* thisptr, datum playerIdx, int a3, int a4, int a5, char a6)
{
	bool handled = false;

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		// skip recording the score, until we pickup the skull
		if (a5 == -1 || a5 == 7 || a5 == 9)
		{
			handled = true;
		}
		break;

	case ExecTime::_postEventExec:
		// skip recording the score, until we pickup the skull
		if (a5 == -1 || a5 == 7 || a5 == 9)
		{
			handled = true;
		}
		break;

	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}

	return handled;
}

bool GraveRobber::OnAutoPickupHandler(ExecTime execTime, datum playerIdx, datum objectIdx)
{
	const s_weapon_data_definition* weaponObject = object_get_fast_unsafe<s_weapon_data_definition>(objectIdx);

	bool handled = false;

	switch (execTime)
	{
	case ExecTime::_preEventExec:

		if (DATUM_INDEX_TO_ABSOLUTE_INDEX(weaponObject->tag_definition_index) == DATUM_INDEX_TO_ABSOLUTE_INDEX(e_weapons_datum_index::ball))
		{
			GraveRobber::PickupSkull(playerIdx, objectIdx);
			handled = true;
		}
		break;

	case ExecTime::_postEventExec:
		break;

	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}

	return handled;
}
