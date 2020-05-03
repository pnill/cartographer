#include <sqrat/include/sqrat.h>
#include "..\..\Modules\Networking\Networking.h"
#include "..\..\Modules\OnScreenDebug\OnscreenDebug.h"
#include "Squirrel.h"
#include "H2MOD.h"
#include "Globals.h"


void sqAddDebugText(char* Msg)
{
	std::string dbgMsg;
	dbgMsg.append("[Squirrel] - ");
	dbgMsg.append(Msg);
	addDebugText(dbgMsg.c_str());
}


void compile_error_handler(HSQUIRRELVM v, const SQChar* desc, const SQChar* source, SQInteger line, SQInteger column)
{
	char* dbgMsg = new char[2000];
	sprintf(dbgMsg, "[Squirrel] - Compiler Error: line: %i, column: %i, desc: %s, source: %s", line, column, desc, source);
	addDebugText(dbgMsg);

}

void debughook(HSQUIRRELVM vm, SQInteger type, const SQChar *sourcename, SQInteger line, const SQChar *funcname)
{
	char* dbgMsg = new char[2000];
	//std::string dbgMsg;
	sprintf(dbgMsg, "Debug: type: %i, source: %s, line: %i, function: %s", type, sourcename, line, funcname);
	sqAddDebugText(dbgMsg);


}




namespace ScriptEngine {

	void sqLoadScript(std::string path)
	{
		Sqrat::Script varScript;
		varScript.CompileFile(path.c_str());
		varScript.Run();

		std::string dbgMsg;
		dbgMsg.append("[Squirrel] - Loading script: ");
		dbgMsg.append(path);
		addDebugText(dbgMsg.c_str());
	}

	void sqSessionStart()
	{
		BindSquirrel(sq_open(1000));
		Sqrat::Script GlobalScript;
		
		sq_enabledebuginfo(Sqrat::DefaultVM::Get(), true);
		sq_notifyallexceptions(Sqrat::DefaultVM::Get(), true);
		sq_setcompilererrorhandler(Sqrat::DefaultVM::Get(), compile_error_handler);
		sq_setnativedebughook(Sqrat::DefaultVM::Get(), debughook);
		sqstd_seterrorhandlers(Sqrat::DefaultVM::Get());
		sqstd_printcallstack(Sqrat::DefaultVM::Get());

		GlobalScript.CompileFile("Scripts/Globals.nut");
		
		GlobalScript.Run();

		addDebugText("[Squirrel] - Starting VM");
	}

	void sqSessionEnd()
	{
		addDebugText("[Squirrel] - Killing VM");

		if (sq_getvmstate(Sqrat::DefaultVM::Get()) == SQ_VMSTATE_RUNNING)
			sq_close(Sqrat::DefaultVM::Get());

	}

	void sqOnMapLoad()
	{
		Sqrat::Function sqMapLoadFunc = Sqrat::RootTable().GetFunction("OnMapLoad");

		if (!sqMapLoadFunc.IsNull())
			sqMapLoadFunc.Execute();
		else
			addDebugText("[Squirrel] - No OnMapLoad function was found!");
	}

	void sqPrePlayerSpawn(HSQUIRRELVM vm, int unit_datum)
	{
		Sqrat::Function sqFunc = Sqrat::RootTable().GetFunction("OnPrePlayerSpawn");
		if (!sqFunc.IsNull())
			sqFunc.Execute(unit_datum);
		else
			addDebugText("[Squirrel] - No OnPrePlayerSpawn function was found!");
	}

	void sqPostPlayerSpawn(HSQUIRRELVM vm,int unit_datum)
	{
		Sqrat::ErrorHandling::Enable(true);

		Sqrat::Function sqFunc = Sqrat::RootTable().GetFunction("OnPostPlayerSpawn");
		if (!sqFunc.IsNull())
			sqFunc.Execute(unit_datum);
		else
			addDebugText("[Squirrel] - No OnPostPlayerSpawn function was found!");
	}
	
	bool sqOnAutoPickup(HSQUIRRELVM vm,int unit_datum, int object_datum)
	{
		Sqrat::Function sqFunc = Sqrat::RootTable().GetFunction("OnAutoPickup");
		if (!sqFunc.IsNull())
			return *sqFunc.Evaluate<bool>(unit_datum, object_datum).Get();
		else
		{
			addDebugText("[Squirrel] - No OnAutoPickup function was found!");
			return true;
		}
	}

	bool sqOnTeamChange(HSQUIRRELVM vm,int playerIndex, int teamIndex)
	{
		Sqrat::Function sqFunc = Sqrat::RootTable().GetFunction("OnChangeTeam");
		if (!sqFunc.IsNull())
			return *sqFunc.Evaluate<bool>(playerIndex,teamIndex).Get();
		else
		{
			addDebugText("[Squirrel] - No OnChangeTeam function was found!");
			return true;
		}
	}

	void sqOnPlayerDeath(HSQUIRRELVM vm, int unit_datum, int damaging_datum)
	{
		Sqrat::Function sqFunc = Sqrat::RootTable().GetFunction("OnPlayerDeath");
		if (!sqFunc.IsNull())
			sqFunc.Execute(unit_datum, damaging_datum);
		else
			addDebugText("[Squirrel] - No OnPlayerDeath function was found!");
	}

	bool sqOnUpdateScore(HSQUIRRELVM vm, int playerDatum, int killType)
	{
		Sqrat::Function sqFunc = Sqrat::RootTable().GetFunction("OnPlayerScoreUpdate");
		if (!sqFunc.IsNull())
			return *sqFunc.Evaluate<bool>(playerDatum, killType);
		else
		{
			addDebugText("[Squirrel] - No OnPlayerScoreUpdate function was found!");
			return true;
		}
	}

};



BYTE SqGetMapType()
{
	return h2mod->GetMapType();
}

void sqGivePlayerWeapon(int PlayerIndex, int WeaponID,bool bReset)
{
	call_give_player_weapon(PlayerIndex, WeaponID,bReset);
}

void sqPlaySound(wchar_t* name, int sleep)
{
	h2mod->custom_sound_play(name, sleep);
}

void sqDisableSound(int sound)
{
	h2mod->disable_sound(sound);
}

int sqGetPlayerCount()
{
	return NetworkSession::getPlayerCount();
}

bool sqPlayerIsActive(int playerIndex)
{
	return NetworkSession::playerIsActive(playerIndex);
}

wchar_t* sqGetPlayerNameFromIndex(int playerIndex)
{
	return NetworkSession::getPlayerName(playerIndex);
}

bool sqlocalPeerIsSessionHost()
{
	return NetworkSession::localPeerIsSessionHost();
}

int sqGetLocalPeerIndex()
{
	return NetworkSession::getLocalPeerIndex();
}

void sqSendTeamChange(int peerIndex,int teamIndex)
{
	CustomPackets::sendTeamChange(peerIndex, teamIndex);
}

void sqSetLocalTeam(int peerIndex,int teamIndex)
{
	h2mod->set_local_team_index(peerIndex, teamIndex);
}

BYTE sqGetLocalTeam()
{
	return h2mod->get_local_team_index();
}

BYTE sqGetPlayerTeamFromXUID(long long xuid)
{
	return NetworkSession::getPlayerTeamFromXuid(xuid);
}

long long sqGetPlayerXUID(int playerIndex)
{
	return NetworkSession::getPlayerXuid(playerIndex);
}

wchar_t* sqGetPlayerNameFromXUID(long long xuid)
{
	return NULL;
	//TODO?
}

void sqSetUnitSpeedPatch(bool enabled)
{
	h2mod->set_unit_speed_patch(enabled);
}

void sqSetPlayerSpeed(int playerIndex, float speed)
{
	h2mod->set_unit_speed(speed, playerIndex);
}

void sqToggleWeaponPickup()
{
	h2mod->toggle_weapon_pickup();
}

void sqSetWeaponPickup(bool bEnable)
{
	h2mod->toggle_weapon_pickup(bEnable);
}

void sqSetTeamPlayerIndicatorVisibility(bool bEnable)
{
	h2mod->team_player_indicator_visibility(bEnable);
}

void sqSetPlayerBiped(int playerIndex,BYTE biped)
{
	h2mod->set_unit_biped((Player::Biped)biped, playerIndex);
}

int sqGet_unit_datum_from_player_index(int playerIndex)
{
	return h2mod->get_unit_datum_from_player_index(playerIndex).ToInt();
}

int sqObject_try_and_get_with_type(int objectDatum,int objectType)
{
	return call_object_try_and_get_with_type(objectDatum, objectType);
}

BYTE sqGet_team_from_unit(int unitDatum)
{
	return h2mod->get_unit_team_index(unitDatum);
}

bool sqIsObjectBiped(int objectDatum)
{
	return (*(BYTE*)(objectDatum + 0xAA) == 0);
}

void squnit_reset_equipment(int unitDatum)
{
	call_unit_reset_equipment(unitDatum);
}

int sqget_game_life_cycle()
{
	return (int)get_game_life_cycle();
}

void sqleave_session()
{
	h2mod->leave_session();
}

float sqget_player_distance_from_player(int playerIndex, int playerIndex2)
{
	return h2mod->get_distance(playerIndex, playerIndex2);
}

void sqset_player_grenades(int playerIndex, BYTE type, BYTE count, bool resetEquipment)
{
	h2mod->set_player_unit_grenades_count(playerIndex, type, count, resetEquipment);
}

void sqhs_object_destroy(int object_datum)
{
	call_hs_object_destroy(object_datum);
}

int sqrand(int min_num, int max_num)
{
	std::mt19937 mt_rand(rd());
	std::uniform_int_distribution<int> dist(min_num,max_num);

	return dist(mt_rand);
}

int sqgetPeerIndex(int playerIndex)
{
	return NetworkSession::getPeerIndex(playerIndex);
}

int sqgetPlayerTeam(int playerIndex)
{
	return NetworkSession::getPlayerTeam(playerIndex);
}

typedef void(__stdcall *update_player_score)(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6);
extern update_player_score pupdate_player_score;


void squpdate_player_score(int PlayerDatumIndex)
{
	pupdate_player_score((void*)(h2mod->Server ? 0x30005508 : 0x3000595C), PlayerDatumIndex, 0, 1, -1, 0);
}


bool sqvalidate_object_type(int object_datum,int type)
{
	DatumIterator<ObjectHeader> objectIt(game_state_objects_header);
	BipedObjectDefinition* biped_unit = (BipedObjectDefinition*)objectIt.get_data_at_index(datum(object_datum).Index)->object;

	if (objectIt.get_data_at_index(datum(object_datum).Index)->type == (ObjectType)type)
	{
		return true;
	}

	return false;
}

bool sqvalidate_object_datum(int object_datum,int object_validate)
{
	DatumIterator<ObjectHeader> objectIt(game_state_objects_header);
	WeaponObjectDefinition* weaponObject = (WeaponObjectDefinition*)objectIt.get_data_at_index(datum(object_datum).ToAbsoluteIndex())->object;

	if (weaponObject->TagDefinitionIndex.Index == (object_validate & 0xFFFF))
	{
		return true;
	}

	return false;
}

void sqendGame()
{
	NetworkSession::endGame();
}
//TODO: Implement spawning at specific position (http://www.squirrel-lang.org/squirreldoc/reference/language/classes.html#metamethods)
void sqspawn_object_at_player(int player_datum, int object_datum)
{
	DatumIterator<ObjectHeader> objectIt(game_state_objects_header);
	BipedObjectDefinition* biped_unit = (BipedObjectDefinition*)objectIt.get_data_at_index(datum(player_datum).Index)->object;

	if (objectIt.get_data_at_index(datum(player_datum).Index)->type == ObjectType::biped)
	{
		ObjectPlacementData nObject;

		call_object_placement_data_new(&nObject, object_datum, -1, 0);

		nObject.Placement.x = biped_unit->Placement.x;
		nObject.Placement.y = biped_unit->Placement.y;
		nObject.Placement.z = biped_unit->Placement.z;
		nObject.TranslationalVelocity.x = biped_unit->TranslationalVelocity.x;
		nObject.TranslationalVelocity.y = biped_unit->TranslationalVelocity.y;
		nObject.TranslationalVelocity.z = biped_unit->TranslationalVelocity.z;

		datum new_object_datum = call_object_new(&nObject);
		if (!new_object_datum.IsNull())
			call_add_object_to_sync(new_object_datum);
	}
}



void BindSquirrel(HSQUIRRELVM vm)
{
	using namespace Sqrat;

	DefaultVM::Set(vm);

	RootTable().Func("character_datum_from_index", &character_datum_from_index);
	RootTable().Func("object_try_and_get_with_type", &sqObject_try_and_get_with_type);
	RootTable().Func("unit_reset_equipment", &squnit_reset_equipment);
	RootTable().Func("get_game_life_cylce", &sqget_game_life_cycle);
	RootTable().Func("leave_session", &sqleave_session);
	RootTable().Func("get_player_distance_from_player", &sqget_player_distance_from_player);
	RootTable().Func("set_player_grenades", &sqset_player_grenades);
	RootTable().Func("hs_object_destroy", &sqhs_object_destroy);
	RootTable().Func("update_player_score", &squpdate_player_score);
	RootTable().Func("validate_object_type", &sqvalidate_object_type);
	RootTable().Func("spawn_object_at_player", &sqspawn_object_at_player);
	RootTable().Func("validate_object_datum", &sqvalidate_object_datum);


	RootTable().Func("GetMapType", &SqGetMapType);
	RootTable().Func("addDebugText", &sqAddDebugText);
	RootTable().Func("GivePlayerWeapon", &sqGivePlayerWeapon);
	RootTable().Func("PlaySound", &sqPlaySound);
	RootTable().Func("DisableSound", &sqDisableSound);
	RootTable().Func("GetPlayerCount", &sqGetPlayerCount);
	RootTable().Func("GetPlayerNameFromIndex", &sqGetPlayerNameFromIndex);
	RootTable().Func("localPeerIsSessionHost", &sqlocalPeerIsSessionHost);
	RootTable().Func("GetLocalPeerIndex", &sqGetLocalPeerIndex);
	RootTable().Func("SendTeamChange", &sqSendTeamChange);
	RootTable().Func("SetLocalTeam", &sqSetLocalTeam);
	RootTable().Func("GetLocalTeam", &sqGetLocalTeam);
	RootTable().Func("GetPlayerXUID", &sqGetPlayerXUID);
	RootTable().Func("GetPlayerTeamFromXUID", &sqGetPlayerTeamFromXUID);
	RootTable().Func("SetUnitSpeedPatch", &sqSetUnitSpeedPatch);
	RootTable().Func("SetUnitSpeed", &sqSetPlayerSpeed);
	RootTable().Func("ToggleWeaponPickup", &sqToggleWeaponPickup);
	RootTable().Func("SetWeaponPickup", &sqSetWeaponPickup);
	RootTable().Func("SetIndicatorVisiblity", &sqSetTeamPlayerIndicatorVisibility);
	RootTable().Func("SetPlayerBiped", &sqSetPlayerBiped);
	RootTable().Func("IsObjectBiped", &sqIsObjectBiped);
	RootTable().Func("sqRandom", &sqrand);
	RootTable().Func("playerIsActive", &sqPlayerIsActive);
	RootTable().Func("getPeerIndex", &sqgetPeerIndex);
	RootTable().Func("getPlayerTeam", &sqgetPlayerTeam);
	RootTable().Func("endGame", &sqendGame);
}