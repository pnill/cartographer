#include "stdafx.h"

#include "FireFight.h"

#include "Blam/Cache/TagGroups/character_definition.hpp"
#include "Blam/Engine/Game/ai/actors.h"
#include "Blam/Engine/Game/game/game.h"
#include "Blam/Engine/Game/objects/objects.h"
#include "Blam/Engine/Game/units/bipeds.h"
#include "Blam/Engine/Networking/Session/NetworkSession.h"
#include "H2MOD/Tags/TagInterface.h"

#include "H2MOD.h"

DeviceShop* deviceShop = new DeviceShop();

FireFight::FireFight()
{
}

void FireFight::KilledAI(datum killedAi, datum killerPlayerIdx)
{
	int points = 0;
	s_data_iterator<s_actor> actorIt(actors::get_actor_table());
	auto actorObject = (s_biped_data_definition*)object_try_and_get_and_verify_type(killedAi, FLAG(e_object_type::biped));

	if (actorObject != NULL)
	{
		datum actor_datum = actorObject->simulation_actor_index; // Grab the actor from the killed AI
		if (!DATUM_IS_NONE(actor_datum)) // Ensure that it was valid
		{
			datum char_datum = actorIt.get_data_at_datum_index(actor_datum)->character_datum; // get the character tag datum assigned to the actor.
			auto* character = tags::get_tag<blam_tag::tag_group_type::character, character_tag_group>(char_datum);

			if (character && character->SwarmProperties.size > 0)
				points = character->SwarmProperties[0]->scatterKilledCount;

			deviceShop->AddPoints(killerPlayerIdx, points);
		}
	}
}

void FireFight::Initialize()
{
	
}

void FireFight::Dispose()
{
	if (NetworkSession::LocalPeerIsSessionHost())
	{
		deviceShop->Dispose();
	}
}

CustomVariantId FireFight::GetVariantId()
{
	return CustomVariantId::_id_firefigth;
}

void FireFight::OnObjectDamage(ExecTime execTime, datum unitDatumIdx, int a2, bool a3, bool a4)
{
	switch (execTime)
	{
	case ExecTime::_preEventExec:
		break;

	case ExecTime::_postEventExec:
		KilledAI(unitDatumIdx, *(datum*)(a2));
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

void FireFight::OnMapLoad(ExecTime execTime, s_game_options* gameOptions)
{
	switch (execTime)
	{
	case ExecTime::_preEventExec:
		break;

	case ExecTime::_postEventExec:
		switch (s_game_globals::get_engine_type())
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

void FireFight::OnPlayerDeath(ExecTime execTime, datum playerIdx)
{
	int absPlayerIdx = DATUM_INDEX_TO_ABSOLUTE_INDEX(playerIdx);
	datum playerUnitDatum = s_player::GetPlayerUnitDatumIndex(absPlayerIdx);

	switch (execTime)
	{
	case ExecTime::_preEventExec:
		// to note after the original function executes, the controlled unit by this player is set to NONE
		break;

	case ExecTime::_postEventExec:
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}


void FireFight::OnPlayerSpawn(ExecTime execTime, datum playerIdx)
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
		// host only (dedicated server and client)
		break;

	case ExecTime::_ExecTimeUnknown:
	default:
		LOG_TRACE_GAME("{} - unknown execTime", __FUNCTION__);
		break;
	}
}

bool FireFight::OnPlayerScore(ExecTime execTime, void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6)
{
	int absPlayerIdx = a2;
	datum playerUnitDatum = s_player::GetPlayerUnitDatumIndex(absPlayerIdx);

	bool handled = false;

	switch (execTime)
	{
	case ExecTime::_preEventExec:
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
