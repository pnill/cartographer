#include "stdafx.h"

#include "HeadHunter.h"

#include "Blam\Engine\Game\GameGlobals.h"
#include "Blam\Engine\Networking\Session\NetworkSession.h"

#include "H2MOD\Utils\Utils.h"
#include "H2MOD\Modules\Shell\Config.h"
#include "H2MOD/Modules/CustomMenu/CustomLanguage.h"
#include "H2MOD/Modules/HaloScript/HaloScript.h"
#include "H2MOD/Engine/Engine.h"

int soundBuffer = 0;
std::unordered_map<int, std::unordered_map<e_headhunter_sounds, const wchar_t*>> headhunterSoundTable;
bool b_firstSpawn = true;

HeadHunter::HeadHunter()
{
}

void HeadHunter::TriggerSound(e_headhunter_sounds sound, int sleep)
{
	if (headhunterSoundTable.count(H2Config_language.code_main)) 
	{
		LOG_TRACE_GAME(L"[h2mod-headhunter] Triggering sound {}", headhunterSoundTable[H2Config_language.code_main][sound]);
		h2mod->custom_sound_play(headhunterSoundTable[H2Config_language.code_main][sound], sleep);
	}
	else 
	{
		LOG_TRACE_GAME(L"[h2mod-headhunter] Triggering sound {}", headhunterSoundTable[0][sound]);
		h2mod->custom_sound_play(headhunterSoundTable[0][sound], sleep);
	}
}

void HeadHunter::SpawnPlayerClientSetup()
{
	if (b_firstSpawn)
	{
		TriggerSound(_snd_head_hunter, 1000);
		b_firstSpawn = false;
	}
}

void HeadHunter::SpawnSkull(datum unit_datum)
{
	s_biped_data_definition* biped_unit = (s_biped_data_definition*)object_try_and_get_and_verify_type(unit_datum, FLAG(e_object_type::biped));

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

typedef void(__thiscall *update_player_score_t)(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6);
extern update_player_score_t p_update_player_score;

void HeadHunter::PickupSkull(datum playerIdx, datum skullDatum)
{
	if (!DATUM_IS_NONE(skullDatum))
	{
		//typedef void(__stdcall *update_player_score)(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6);
		//	20 / 10 / 2018 18 : 48 : 39.756 update_player_score_hook(thisptr : 3000595C, a2 : 00000001, a3 : 00000000, a4 : 00000001, a5 : FFFFFFFF, a6 : 00000000)

		typedef char* (__cdecl* get_score_data_ptr)();
		auto p_get_score_data_ptr = Memory::GetAddress<get_score_data_ptr>(0x6B8A7, 0x6AD32);

		char* player_score_data = p_get_score_data_ptr();
		if (player_score_data)
		{
			p_update_player_score(player_score_data, DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx), 0, 1, -1, 0);
			HaloScript::ObjectDestroy(skullDatum);
			if (TimeElapsedMS(soundBuffer) > 2500)
			{
				soundBuffer = GetCurrentTimeMS();
				TriggerSound(_snd_skull_scored, 500);
			}
		}
	}
}

void HeadHunter::initClient()
{
	b_firstSpawn = true;
	h2mod->disable_sounds(FLAG(_sound_type_slayer) | ALL_SOUNDS_NO_SLAYER);
	headhunterSoundTable[_lang_id_english][e_headhunter_sounds::_snd_head_hunter] = L"sounds/en/headhunter.wav";
	headhunterSoundTable[_lang_id_english][e_headhunter_sounds::_snd_skull_scored] = L"sounds/en/skull_scored.wav";
	headhunterSoundTable[_lang_id_spanish][e_headhunter_sounds::_snd_head_hunter] = L"sounds/es/headhunter.wav";
	headhunterSoundTable[_lang_id_spanish][e_headhunter_sounds::_snd_skull_scored] = L"sounds/es/skull_scored.wav";
}

void HeadHunter::Initialize()
{
	if (!Memory::IsDedicatedServer())
	{
		HeadHunter::initClient();
	}
}

void HeadHunter::Dispose()
{
}

CustomVariantId HeadHunter::GetVariantId()
{
	return CustomVariantId::_id_headhunter;
}

void HeadHunter::OnMapLoad(ExecTime execTime, s_game_options* gameOptions)
{
	switch (execTime)
	{
	case ExecTime::_preEventExec:
		break;

	case ExecTime::_postEventExec:
		switch (h2mod->GetEngineType())
			{
				// cleanup when loading main menu
			case _multiplayer:
				this->Initialize();
				break;
			/*case _main_menu:
				this->Dispose();
				break;*/
			default:
				break;
			}
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

void HeadHunter::OnPlayerSpawn(ExecTime execTime, datum playerIdx)
{
	int absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx);
	datum playerUnitDatum = s_player::GetPlayerUnitDatumIndex(absPlayerIdx);

	switch (execTime)
	{
		// prespawn handler
	case ExecTime::_preEventExec:
		break;

		// postspawn handler
	case ExecTime::_postEventExec:
		if (!Memory::IsDedicatedServer())
			HeadHunter::SpawnPlayerClientSetup();
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

void HeadHunter::OnPlayerDeath(ExecTime execTime, datum playerIdx)
{
	int absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx);
	datum playerUnitDatum = s_player::GetPlayerUnitDatumIndex(absPlayerIdx);

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		// to note after the original function executes, the controlled unit by this player is set to NONE
		if (!s_game_globals::game_is_predicted())
			HeadHunter::SpawnSkull(playerIdx);
		break;

	case ExecTime::_postEventExec:
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

bool HeadHunter::OnPlayerScore(ExecTime execTime, void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6)
{
	int absPlayerIdx = a2;
	datum playerUnitDatum = s_player::GetPlayerUnitDatumIndex(absPlayerIdx);

	bool handled = false;

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		break;

	case ExecTime::_postEventExec:
		// skip recording the score, until we pickup the skull
		if (a5 == -1
			|| a5 == 7)
		{
			handled = true;
		}
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}

	return handled;
}

bool HeadHunter::OnAutoPickupHandler(ExecTime execTime, datum playerIdx, datum objectIdx)
{
	int absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx);
	datum playerUnitDatum = s_player::GetPlayerUnitDatumIndex(absPlayerIdx);

	bool handled = false;

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		if (!s_game_globals::game_is_predicted())
		{
			s_weapon_data_definition* weaponObject = object_get_fast_unsafe<s_weapon_data_definition>(objectIdx);

			if (DATUM_INDEX_TO_ABSOLUTE_INDEX(weaponObject->tag_definition_index) == DATUM_INDEX_TO_ABSOLUTE_INDEX(e_weapons_datum_index::ball))
			{
				HeadHunter::PickupSkull(playerIdx, objectIdx);
				handled = true;
			}
		}
		break;

	case ExecTime::_postEventExec:
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}

	return handled;
}


