#include "stdafx.h"
#include "GraveRobber.h"

#include "H2MOD.h"
#include "Blam/Engine/Game/GameGlobals.h"
#include "H2MOD/Modules/CustomMenu/CustomLanguage.h"
#include "H2MOD/Modules/Shell/Config.h"
#include "H2MOD/Modules/HaloScript/HaloScript.h"
#include "H2MOD/Utils/Utils.h"

bool b_firstSpawn;
wchar_t* headhunterSoundTable[e_graverobber_sounds::_graverobber_end] { nullptr };

void GraveRobber::TriggerSound(e_graverobber_sounds sound, int sleep)
{
	if (headhunterSoundTable[sound] != nullptr)
	{
		LOG_TRACE_GAME(L"[h2mod-graverobber] Triggering sound {}", headhunterSoundTable[sound]);
		h2mod->custom_sound_play(headhunterSoundTable[sound], sleep);
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

typedef void(__thiscall *update_player_score_t)(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6);
extern update_player_score_t p_update_player_score;

void GraveRobber::PickupSkull(datum playerIdx, datum skullDatum)
{
	const short absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx);

	if (!DATUM_IS_NONE(skullDatum))
	{
		typedef char* (__cdecl* get_score_data_ptr)();
		auto p_get_score_data_ptr = Memory::GetAddress<get_score_data_ptr>(0x6B8A7, 0x6AD32);

		char* player_score_data = p_get_score_data_ptr();
		if (player_score_data)
		{
			if (!s_game_globals::game_is_predicted())
			{
				p_update_player_score(player_score_data, absPlayerIdx, 0, 1, -1, 0);
			}
			HaloScript::ObjectDestroy(skullDatum);
			
			if (DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(0)) == absPlayerIdx ||
				DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(1)) == absPlayerIdx ||
				DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(2)) == absPlayerIdx ||
				DATUM_INDEX_TO_ABSOLUTE_INDEX(h2mod->get_player_datum_index_from_controller_index(3)) == absPlayerIdx)
			{
				TriggerSound(_snd_skull_scored, 500);
			}
		}
	}
}

void GraveRobber::initClient()
{
	const int language_id = *Memory::GetAddress<int*>(0x412818);

	h2mod->disable_sounds(FLAG(_sound_type_slayer) | ALL_SOUNDS_NO_SLAYER);
	b_firstSpawn = true;

	switch (language_id)
	{
	case _lang_id_chinese:
		break;
		headhunterSoundTable[e_graverobber_sounds::_snd_head_hunter] = new wchar_t[sizeof(SND_HEADHUNTER_CH) / 2] {SND_HEADHUNTER_CH};
		headhunterSoundTable[e_graverobber_sounds::_snd_skull_scored] = new wchar_t[sizeof(SND_SKULL_SCORED_CH) / 2] {SND_SKULL_SCORED_CH};
	case _lang_id_english:
		headhunterSoundTable[e_graverobber_sounds::_snd_head_hunter] = new wchar_t[sizeof(SND_HEADHUNTER_EN) / 2] {SND_HEADHUNTER_EN};
		headhunterSoundTable[e_graverobber_sounds::_snd_skull_scored] = new wchar_t[sizeof(SND_SKULL_SCORED_EN) / 2] {SND_SKULL_SCORED_EN};
		break;
	case _lang_id_spanish:
		headhunterSoundTable[e_graverobber_sounds::_snd_head_hunter] = new wchar_t[sizeof(SND_HEADHUNTER_ES) / 2] {SND_HEADHUNTER_ES};
		headhunterSoundTable[e_graverobber_sounds::_snd_skull_scored] = new wchar_t[sizeof(SND_SKULL_SCORED_ES) / 2] {SND_SKULL_SCORED_ES};
		break;
	case _lang_id_french:
		headhunterSoundTable[e_graverobber_sounds::_snd_head_hunter] = new wchar_t[sizeof(SND_HEADHUNTER_FR) / 2] {SND_HEADHUNTER_FR};
		headhunterSoundTable[e_graverobber_sounds::_snd_skull_scored] = new wchar_t[sizeof(SND_SKULL_SCORED_FR) / 2] {SND_SKULL_SCORED_FR};
		break;
	case _lang_id_german:
		headhunterSoundTable[e_graverobber_sounds::_snd_head_hunter] = new wchar_t[sizeof(SND_HEADHUNTER_GE) / 2] {SND_HEADHUNTER_GE};
		headhunterSoundTable[e_graverobber_sounds::_snd_skull_scored] = new wchar_t[sizeof(SND_SKULL_SCORED_GE) / 2] {SND_SKULL_SCORED_GE};
		break;
	case _lang_id_italian:
		headhunterSoundTable[e_graverobber_sounds::_snd_head_hunter] = new wchar_t[sizeof(SND_HEADHUNTER_IT) / 2] {SND_HEADHUNTER_IT};
		headhunterSoundTable[e_graverobber_sounds::_snd_skull_scored] = new wchar_t[sizeof(SND_SKULL_SCORED_IT) / 2] {SND_SKULL_SCORED_IT};
		break;
	case _lang_id_japanese:
		headhunterSoundTable[e_graverobber_sounds::_snd_head_hunter] = new wchar_t[sizeof(SND_HEADHUNTER_JP) / 2] {SND_HEADHUNTER_JP};
		headhunterSoundTable[e_graverobber_sounds::_snd_skull_scored] = new wchar_t[sizeof(SND_SKULL_SCORED_JP) / 2] {SND_SKULL_SCORED_JP};
		break;
	case _lang_id_korean:
		headhunterSoundTable[e_graverobber_sounds::_snd_head_hunter] = new wchar_t[sizeof(SND_HEADHUNTER_KO) / 2] {SND_HEADHUNTER_KO};
		headhunterSoundTable[e_graverobber_sounds::_snd_skull_scored] = new wchar_t[sizeof(SND_SKULL_SCORED_KO) / 2] {SND_SKULL_SCORED_KO};
		break;
	default:
		headhunterSoundTable[e_graverobber_sounds::_snd_head_hunter] = new wchar_t[sizeof(SND_HEADHUNTER_EN) / 2] {SND_HEADHUNTER_EN};
		headhunterSoundTable[e_graverobber_sounds::_snd_skull_scored] = new wchar_t[sizeof(SND_SKULL_SCORED_EN) / 2] {SND_SKULL_SCORED_EN};
	}
}

void GraveRobber::Initialize()
{
	if (!Memory::IsDedicatedServer())
	{
		GraveRobber::initClient();
	}
}

void GraveRobber::Dispose()
{
	for (size_t i = 0; i < _graverobber_end; i++)
	{
		if (headhunterSoundTable[i] != nullptr)
		{
			delete headhunterSoundTable[i];
		}
	}
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

bool GraveRobber::OnPlayerScore(ExecTime execTime, void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6)
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
