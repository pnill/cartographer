#include <sqrat/include/sqrat.h>
#include "..\..\Modules\Networking\Networking.h"
#include "..\..\Modules\OnScreenDebug\OnscreenDebug.h"
#include "Squirrel.h"
#include "H2MOD.h"
#include "Globals.h"
#include "xliveless.h"

bool g_sqScriptLoaded = false;
int g_sqScriptId = 0;
char* g_sqScriptPath = NULL;
extern bool g_sqScriptDownloaded;

void sqAddDebugText(char* Msg)
{
	LOG_INFO_FUNC("{}",Msg);
}

static SQInteger _sqstd_aux_printerror(HSQUIRRELVM v)
{
	SQPRINTFUNCTION pf = sq_geterrorfunc(v);
	if (pf) {
		const SQChar *sErr = 0;
		if (sq_gettop(v) >= 1) {
			if (SQ_SUCCEEDED(sq_getstring(v, 2, &sErr))) {
				pf(v, _SC("\nAN ERROR HAS OCCURRED [%s]\n"), sErr);
			}
			else {
				pf(v, _SC("\nAN ERROR HAS OCCURRED [unknown]\n"));
			}
			sqstd_printcallstack(v);
		}
	}
	return 0;
}


void sqPrint(HSQUIRRELVM v, const SQChar* desc, ...)
{
	char nDbgMsg[2555];
	std::string dbMsg;
	va_list arglist;
	va_start(arglist, desc);
	vsprintf(nDbgMsg, desc, arglist);
	va_end(arglist);

	LOG_INFO_FUNC("{}", nDbgMsg);
}

void sqError(HSQUIRRELVM v, const SQChar* desc, ...)
{
	char nDbgMsg[2555];
	std::string dbMsg;
	va_list arglist;
	va_start(arglist, desc);
	vsprintf(nDbgMsg, desc, arglist);
	va_end(arglist);

	LOG_CRITICAL_FUNC("{}", nDbgMsg);

}

void compile_error_handler(HSQUIRRELVM v, const SQChar* desc, const SQChar* source, SQInteger line, SQInteger column)
{	
	LOG_CRITICAL_FUNC("[Squirrel] Compiler error - line {}, column {}, desc {}, souce {}", line, column, desc, source);
}

void debughook(HSQUIRRELVM vm, SQInteger type, const SQChar *sourcename, SQInteger line, const SQChar *funcname)
{
	LOG_TRACE_FUNC("[Squirrel] Debug type {}, source {}, line {}, function {}", type, sourcename, line, funcname);
}


#define SCRAT_USE_EXCEPTIONS

namespace ScriptEngine {


	std::string loaded_script;
	bool g_sqTeamGame = false;
	bool g_sqMotionSensor = true;
	bool g_sgAlwaysInvisible = false;
	bool g_sgRoundSwitchResetsMap = false;
	bool g_sgTieResolution = false;
	bool g_sgObservers = false;
	bool g_sgAllowChangingTeams = true;
	bool g_sgFriendlyFire = false;
	bool g_sgOvershieldsOnMap = true;
	bool g_sgInvisibilityOnMap = true;
	bool g_sgGrenadesOnMap = true;
	bool g_sgStartingGrenades = true;
	bool g_sgExtraDamage = false;
	bool g_sgDamageResistant = false;
	bool g_sgForceEvenTeams = false;
	bool g_sgRoundSetting1Round = false;

	short g_sqRoundTime = 0;
	
	int g_unit_datum = 0;

	void sqLoadScriptFromString(char* script_data)
	{
		LOG_TRACE_FUNC("[Squirrel] - Loading script data");
		sqSessionEnd();
		sqSessionStart();
		Sqrat::Script varScript;
		sq_setprintfunc(Sqrat::DefaultVM::Get(), sqPrint, sqError);
		sq_enabledebuginfo(Sqrat::DefaultVM::Get(), true);
		sq_notifyallexceptions(Sqrat::DefaultVM::Get(), true);
		sq_setcompilererrorhandler(Sqrat::DefaultVM::Get(), compile_error_handler);
		sq_setnativedebughook(Sqrat::DefaultVM::Get(), debughook);
		sq_newclosure(Sqrat::DefaultVM::Get(), _sqstd_aux_printerror, 0);
		sq_seterrorhandler(Sqrat::DefaultVM::Get());
		sqstd_seterrorhandlers(Sqrat::DefaultVM::Get());

		Sqrat::Table tblgSettings(Sqrat::DefaultVM::Get());
		tblgSettings.SetValue<int>("isTeamGame", 0);
		tblgSettings.SetValue<int>("motionSensor", 1);
		tblgSettings.SetValue<int>("roundTime", 0);
		tblgSettings.SetValue<int>("alwaysInvisible", 0);
		tblgSettings.SetValue<int>("roundSwitchResetMap", 0);
		tblgSettings.SetValue<int>("tieResolution", 0);
		tblgSettings.SetValue<int>("observers", 0);
		tblgSettings.SetValue<int>("allowChangingTeams", 1);
		tblgSettings.SetValue<int>("friendlyFire", 0);
		tblgSettings.SetValue<int>("overshieldOnMap", 1);
		tblgSettings.SetValue<int>("invisibilityOnMap", 1);
		tblgSettings.SetValue<int>("grenadesOnMap", 1);
		tblgSettings.SetValue<int>("startingGrenades", 1);
		tblgSettings.SetValue<int>("extraDamage", 0);
		tblgSettings.SetValue<int>("damageResistant", 0);
		tblgSettings.SetValue<int>("forceEvenTeams", 0);
		tblgSettings.SetValue<int>("roundsEnable", 0);

		Sqrat::RootTable(Sqrat::DefaultVM::Get()).Bind(_SC("gameTypeSettings"), tblgSettings);


		std::string errMsg;
		if (varScript.CompileString(script_data, errMsg, "downloaded"))
		{
			varScript.Run();
			LOG_INFO_FUNC("[Squirrel] - Downloaded script compiled succesfully!");
			g_sqScriptLoaded = true;
		}
		else
			LOG_CRITICAL_FUNC("[Squirrel] Failed to load downloaded script!");

	}

	void sqLoadScript(std::string path)
	{
		
		
		LOG_INFO_FUNC("[Squirrel] - Loading Script: {}", path);

		Sqrat::Script varScript;
		sq_setprintfunc(Sqrat::DefaultVM::Get(), sqPrint, sqError);
		sq_enabledebuginfo(Sqrat::DefaultVM::Get(), true);
		sq_notifyallexceptions(Sqrat::DefaultVM::Get(), true);
		sq_setcompilererrorhandler(Sqrat::DefaultVM::Get(), compile_error_handler);
		sq_setnativedebughook(Sqrat::DefaultVM::Get(), debughook);
		sq_newclosure(Sqrat::DefaultVM::Get(), _sqstd_aux_printerror, 0);
		sq_seterrorhandler(Sqrat::DefaultVM::Get());
		sqstd_seterrorhandlers(Sqrat::DefaultVM::Get());
	
		Sqrat::Table tblgSettings(Sqrat::DefaultVM::Get());

		tblgSettings.SetValue<int>("isTeamGame", 0);
		tblgSettings.SetValue<int>("motionSensor", 1);
		tblgSettings.SetValue<int>("roundTime", 0);
		tblgSettings.SetValue<int>("alwaysInvisible", 0);
		tblgSettings.SetValue<int>("roundSwitchResetMap", 0);
		tblgSettings.SetValue<int>("tieResolution", 0);
		tblgSettings.SetValue<int>("observers", 0);
		tblgSettings.SetValue<int>("allowChangingTeams", 1);
		tblgSettings.SetValue<int>("friendlyFire", 0);
		tblgSettings.SetValue<int>("overshieldOnMap", 1);
		tblgSettings.SetValue<int>("invisibilityOnMap", 1);
		tblgSettings.SetValue<int>("grenadesOnMap", 1);
		tblgSettings.SetValue<int>("startingGrenades", 1);
		tblgSettings.SetValue<int>("extraDamage", 0);
		tblgSettings.SetValue<int>("damageResistant", 0);
		tblgSettings.SetValue<int>("forceEvenTeams", 0);
		tblgSettings.SetValue<int>("roundsEnable", 0);

		Sqrat::RootTable(Sqrat::DefaultVM::Get()).Bind(_SC("gameTypeSettings"), tblgSettings);

		std::string errMsg;
		if (varScript.CompileFile(path.c_str(), errMsg))
		{
			
			varScript.Run();

			Sqrat::SharedPtr<int> spIsTeamGame = tblgSettings.GetValue<int>("isTeamGame");
			if (spIsTeamGame != NULL)
				g_sqTeamGame = *spIsTeamGame;
			else
				LOG_WARNING_FUNC("No gametype setting for isTeamGame was set.");

			Sqrat::SharedPtr<int> spMotionSensor = tblgSettings.GetValue<int>("motionSensor");
			if (spMotionSensor != NULL)
				g_sqMotionSensor = *spMotionSensor;
			else
				LOG_WARNING_FUNC("No gametype setting for motionSensor was set.");

			Sqrat::SharedPtr<int> spRoundTime = tblgSettings.GetValue<int>("roundTime");
			if (spRoundTime != NULL)
				g_sqRoundTime = *spRoundTime;
			else
				LOG_WARNING_FUNC("No gametype setting for roundTime was set.");

			Sqrat::SharedPtr<int> spalwaysInvisible = tblgSettings.GetValue<int>("alwaysInvisible");
			if (spalwaysInvisible != NULL)
				g_sgAlwaysInvisible = *spalwaysInvisible;
			else
				LOG_WARNING_FUNC("No gametype setting for alwaysInvisible was set.");

			Sqrat::SharedPtr<int> spRoundResetMap = tblgSettings.GetValue<int>("roundSwitchResetMap");
			if (spRoundResetMap != NULL)
				g_sgRoundSwitchResetsMap = *spRoundResetMap;
			else
				LOG_WARNING_FUNC("No gametype setting for roundSwitchResetMap was set");

			Sqrat::SharedPtr<int> sptieResolution = tblgSettings.GetValue<int>("tieResolution");
			if (sptieResolution != NULL)
				g_sgTieResolution = *sptieResolution;
			else
				LOG_WARNING_FUNC("No gametype setting for tieResolution was set.");

			Sqrat::SharedPtr<int> spObservers = tblgSettings.GetValue<int>("observers");
			if (spObservers != NULL)
				g_sgObservers = *spObservers;
			else
				LOG_WARNING_FUNC("No gametype setting for observers was set.");

			Sqrat::SharedPtr<int> spallowChangingTeams = tblgSettings.GetValue<int>("allowChangingTeams");
			if (spallowChangingTeams != NULL)
				g_sgAllowChangingTeams = *spallowChangingTeams;
			else
				LOG_WARNING_FUNC("No gametype setting for allowChangingTeams was set.");

			Sqrat::SharedPtr<int> spfriendlyFire = tblgSettings.GetValue<int>("friendlyFire");
			if (spfriendlyFire != NULL)
				g_sgFriendlyFire = *spfriendlyFire;
			else
				LOG_WARNING_FUNC("No gametype setting for friendlyFire was set.");

			Sqrat::SharedPtr<int> spoverShieldOnmap = tblgSettings.GetValue<int>("overshieldOnMap");
			if (spoverShieldOnmap != NULL)
				g_sgOvershieldsOnMap = *spoverShieldOnmap;
			else
				LOG_WARNING_FUNC("No gametype setting for overshieldOnMap was set.");

			Sqrat::SharedPtr<int> spinvisiblityOnmap = tblgSettings.GetValue<int>("invisibilityOnMap");
			if (spinvisiblityOnmap != NULL)
				g_sgInvisibilityOnMap = *spinvisiblityOnmap;
			else
				LOG_WARNING_FUNC("No gametype setting for invisibilityOnMap was set.");

			Sqrat::SharedPtr<int> spgrenadesOnMap = tblgSettings.GetValue<int>("grenadesOnMap");
			if (spgrenadesOnMap != NULL)
			{
				LOG_INFO_FUNC("Updating gametype setting grenadesOnMap to {}", *spgrenadesOnMap);
				g_sgGrenadesOnMap = *spgrenadesOnMap;
			}
			else
				LOG_WARNING_FUNC("No gametype setting for grenadesOnMap was set.");

			Sqrat::SharedPtr<int> spextramDamage = tblgSettings.GetValue<int>("extraDamage");
			if (spextramDamage != NULL)
				g_sgExtraDamage = *spextramDamage;
			else
				LOG_WARNING_FUNC("No gametype setting for extraDamage was set.");

			Sqrat::SharedPtr<int> spdamageResistant = tblgSettings.GetValue<int>("damageResistant");
			if (spdamageResistant != NULL)
				g_sgDamageResistant = *spdamageResistant;
			else
				LOG_WARNING_FUNC("No gametype setting for damageResistant was set.");

			Sqrat::SharedPtr<int> spforceEventTeams = tblgSettings.GetValue<int>("forceEvenTeams");
			if (spforceEventTeams != NULL)
				g_sgForceEvenTeams = *spforceEventTeams;
			else
				LOG_WARNING_FUNC("No gametype setting for forceEvenTeams was set.");

			Sqrat::SharedPtr<int> spRoundsEnable = tblgSettings.GetValue<int>("roundsEnable");
			if (spRoundsEnable != NULL)
				g_sgRoundSetting1Round = *spRoundsEnable;
			else
				LOG_WARNING_FUNC("No gametype setting for roundsEnable was set.");

			Sqrat::SharedPtr<int> spstartingGrenades = tblgSettings.GetValue<int>("startingGrenades");
			if (spstartingGrenades != NULL)
				g_sgStartingGrenades = *spstartingGrenades;
			else
				LOG_WARNING_FUNC("No gametype setting for startingGrenades was set.");


			LOG_INFO_FUNC("Script compiled succesfully.");
			
			
	
			g_sqScriptLoaded = true;
			g_sqScriptId = rand();
			loaded_script = path;
		}
		else
			sqAddDebugText((char*)errMsg.c_str());

	}

	void sqSessionStart()
	{
		HSQUIRRELVM vm = sq_open(1000);

		BindSquirrel(vm);
		Sqrat::Script GlobalScript;
		
		sq_enabledebuginfo(Sqrat::DefaultVM::Get(), true);
		sq_notifyallexceptions(Sqrat::DefaultVM::Get(), true);
		sq_setcompilererrorhandler(Sqrat::DefaultVM::Get(), compile_error_handler);
		sq_setnativedebughook(Sqrat::DefaultVM::Get(), debughook);
		sqstd_seterrorhandlers(Sqrat::DefaultVM::Get());
		sqstd_printcallstack(Sqrat::DefaultVM::Get());

		GlobalScript.CompileFile("Scripts/Globals.nut");
		
		GlobalScript.Run();

		LOG_INFO_FUNC("[Squirrel] - Starting VM");
	}

	void sqSessionEnd()
	{
		LOG_INFO_FUNC("[Squirrel] - Killing VM");

		if (sq_getvmstate(Sqrat::DefaultVM::Get()) == SQ_VMSTATE_RUNNING)
			sq_close(Sqrat::DefaultVM::Get());

		g_sqScriptLoaded = false;
		g_sqScriptDownloaded = true;
	}

	void sqOnMapLoad()
	{
		
		Sqrat::Function sqMapLoadFunc = Sqrat::RootTable().GetFunction("OnMapLoad");

		if (!sqMapLoadFunc.IsNull())
				sqMapLoadFunc.Execute();
		else
			LOG_WARNING_FUNC("[Squirrel] - No OnMapLoad function was found!");
	}

	void sqPrePlayerSpawn(HSQUIRRELVM vm, int unit_datum)
	{
		Sqrat::Function sqFunc = Sqrat::RootTable().GetFunction("OnPrePlayerSpawn");

		if (!sqFunc.IsNull())
		{
			try {
				sqFunc.Execute(unit_datum);
			}
			catch (Sqrat::Exception e)
			{
				sqAddDebugText((char*)e.Message().c_str());
			}
		}
		else {
			LOG_WARNING_FUNC("[Squirrel] - No OnPrePlayerSpawn function was found!");
		}
	}

	void sqPostPlayerSpawn(HSQUIRRELVM vm,int unit_datum)
	{
		Sqrat::ErrorHandling::Enable(true);

		Sqrat::Function sqFunc = Sqrat::RootTable().GetFunction("OnPostPlayerSpawn");
		if (!sqFunc.IsNull())
			sqFunc.Execute(unit_datum);
		else
			LOG_WARNING_FUNC("[Squirrel] - No OnPostPlayerSpawn function was found!");
	}

	
	bool sqOnAutoPickup(HSQUIRRELVM vm,int unit_datum, int object_datum)
	{
		Sqrat::Function sqFunc = Sqrat::RootTable().GetFunction("OnAutoPickup");
		if (!sqFunc.IsNull())
			return *sqFunc.Evaluate<bool>(unit_datum, object_datum).Get();
		else
		{
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
			LOG_WARNING_FUNC("[Squirrel] - No OnChangeTeam function was found!");
			return true;
		}
	}

	void sqOnPlayerDeath(HSQUIRRELVM vm, int unit_datum, int damaging_datum)
	{
		g_unit_datum = unit_datum;
		Sqrat::Function sqFunc = Sqrat::RootTable().GetFunction("OnPlayerDeath");
		if (!sqFunc.IsNull())
			sqFunc.Execute(unit_datum, damaging_datum);
		else
			LOG_WARNING_FUNC("[Squirrel] - No OnPlayerDeath function was found!");
	}

	bool sqOnUpdateScore(HSQUIRRELVM vm, int playerDatum, int killType)
	{
		Sqrat::Function sqFunc = Sqrat::RootTable().GetFunction("OnPlayerScoreUpdate");
		if (!sqFunc.IsNull())
			return *sqFunc.Evaluate<bool>(playerDatum, killType);
		else
		{
			LOG_WARNING_FUNC("[Squirrel] - No OnPlayerScoreUpdate function was found!");
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
	if (PlayerIndex == -1)
		return;

	call_give_player_weapon(PlayerIndex, WeaponID,bReset);
}

void sqPlaySound(char* name, int sleep)
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
	if (playerIndex == -1)
		return false;

	return NetworkSession::playerIsActive(playerIndex);
}

wchar_t* sqGetPlayerNameFromIndex(int playerIndex)
{
	if (playerIndex == -1)
		return L"";

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
	if (peerIndex == -1)
		return;

	CustomPackets::sendTeamChange(peerIndex, teamIndex);
}

void sqSetLocalTeam(int peerIndex,int teamIndex)
{
	if (peerIndex == -1)
		return;

	h2mod->set_local_team_index(peerIndex, teamIndex);
}

BYTE sqGetLocalTeam()
{
	return h2mod->get_local_team_index();
}

BYTE sqGetPlayerTeamFromXUID(std::string xuid)
{
	if (xuid.empty())
		return 0;

	long long ll = std::stoll(xuid);
	return NetworkSession::getPlayerTeamFromXuid(ll);
}

std::string sqGetPlayerXUID(int playerIndex)
{
	playerIndex = playerIndex & 0xFFFF;
	if (playerIndex == -1)
		return std::string("");

	return std::to_string(NetworkSession::getPlayerXuid(playerIndex));
}

wchar_t* sqGetPlayerNameFromXUID(long long xuid)
{
	return NULL;
	//TODO?
}

void sqSetTeamGame(bool state)
{
	ScriptEngine::g_sqTeamGame = state;
}

void sqToggleTeamGame()
{

	BYTE *nIsTeamGame = (BYTE*)h2mod->GetAddress(0x50A5A4);
	*nIsTeamGame ^= 1 << 0;
}

void sqSetUnitSpeedPatch(bool enabled)
{
	h2mod->set_unit_speed_patch(enabled);
}

void sqSetPlayerSpeed(int playerIndex, float speed)
{
	if(playerIndex == -1)
		return;
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
	if (playerIndex == -1)
		return;

	h2mod->set_unit_biped((Player::Biped)biped, playerIndex);
}

int sqGet_unit_datum_from_player_index(int playerIndex)
{
	if (playerIndex == -1)
		return -1;

	return h2mod->get_unit_datum_from_player_index(playerIndex).ToInt();
}

int sqget_player_index_from_unit_datum(int unit_datum)
{
	if (unit_datum == -1)
		return -1;

	return h2mod->get_player_index_from_unit_datum(datum(unit_datum));
}

int sqObject_try_and_get_with_type(int objectDatum,int objectType)
{
	if (objectDatum == -1)
		return -1;

	return call_object_try_and_get_with_type(objectDatum, objectType);
}

BYTE sqGet_team_from_unit(int unitDatum)
{
	if (unitDatum == -1)
		return -1;

	return h2mod->get_unit_team_index(unitDatum);
}

bool sqIsUnitAlive(int objectDatum)
{
	if (objectDatum == -1)
		return false;

	return (*(BYTE*)(objectDatum + 0xAA) == 0);
}

void squnit_reset_equipment(int unitDatum)
{
	if (unitDatum == -1)
	{
		LOG_WARNING_FUNC("unitDatum == -1");
		return;
	}
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
	if (playerIndex == -1 || playerIndex2 == -1)
		return 0;

	return h2mod->get_distance(playerIndex, playerIndex2);
}

void sqset_player_grenades(int playerIndex, BYTE type, BYTE count, bool resetEquipment)
{
	if (playerIndex == -1)
		return;

	h2mod->set_player_unit_grenades_count(playerIndex, type, count, resetEquipment);
}

void sqhs_object_destroy(int object_datum)
{
	if (object_datum == -1)
		return;

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
	if (playerIndex == -1)
		return -1;

	return NetworkSession::getPeerIndex(playerIndex);
}

int sqgetUnitTeam(int unit_index)
{
	if (unit_index == -1)
		return -1;

	return h2mod->get_unit_team_index(datum(unit_index));
}

int sqgetPlayerTeam(int playerIndex)
{
	if (playerIndex == -1)
		return -1;

	return NetworkSession::getPlayerTeam(playerIndex);
}

typedef void(__stdcall *update_player_score)(void* thisptr, unsigned short a2, int a3, int a4, int a5, char a6);
extern update_player_score pupdate_player_score;


void squpdate_player_score(int PlayerDatumIndex)
{
	if (PlayerDatumIndex == -1)
		return;

	pupdate_player_score((void*)(h2mod->Server ? 0x30005508 : 0x3000595C), PlayerDatumIndex, 0, 1, -1, 0);
}


bool sqvalidate_object_type(int object_datum,int type)
{
	if (object_datum == -1)
		return false;

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
	if (object_datum == -1)
		return false;

	DatumIterator<ObjectHeader> objectIt(game_state_objects_header);
	WeaponObjectDefinition* weaponObject = (WeaponObjectDefinition*)objectIt.get_data_at_index(datum(object_datum).ToAbsoluteIndex())->object;

	if (weaponObject->TagDefinitionIndex.Index == (object_validate & 0xFFFF))
	{
		return true;
	}

	return false;
}

int sqcontroller_2_player(int controller_index)
{
	if (controller_index == -1)
		return 0;

	int player_index = h2mod->get_player_datum_index_from_controller_index(controller_index).ToInt();
	return player_index;
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

	RootTable().Func("chacterDatumFromIndex", &character_datum_from_index);
	RootTable().Func("objectGetWithType", &sqObject_try_and_get_with_type);
	RootTable().Func("resetPlayerEquipment", &squnit_reset_equipment);
	RootTable().Func("getGameLifeCycle", &sqget_game_life_cycle);
	RootTable().Func("leaveSession", &sqleave_session);
	RootTable().Func("getPlayerDistanceFromPlayer", &sqget_player_distance_from_player);
	RootTable().Func("setPlayerGrenades", &sqset_player_grenades);
	RootTable().Func("destroyObject", &sqhs_object_destroy);
	RootTable().Func("updatePlayerScore", &squpdate_player_score);
	RootTable().Func("objectValidateType", &sqvalidate_object_type);
	RootTable().Func("spawnObjectAtPlayer", &sqspawn_object_at_player);
	RootTable().Func("objectValidateDatum", &sqvalidate_object_datum);
	RootTable().Func("getMapType", &SqGetMapType);
	RootTable().Func("addDebugText", &sqAddDebugText);
	RootTable().Func("givePlayerWeapon", &sqGivePlayerWeapon);
	RootTable().Func("playSound", &sqPlaySound);
	RootTable().Func("disableSound", &sqDisableSound);
	RootTable().Func("getPlayerCount", &sqGetPlayerCount);
	RootTable().Func("getPlayerNameFromIndex", &sqGetPlayerNameFromIndex);
	RootTable().Func("localPeerIsSessionHost", &sqlocalPeerIsSessionHost);
	RootTable().Func("getLocalPeerIndex", &sqGetLocalPeerIndex);
	RootTable().Func("sendTeamChange", &sqSendTeamChange);
	RootTable().Func("setLocalTeam", &sqSetLocalTeam);
	RootTable().Func("getLocalTeam", &sqGetLocalTeam);
	RootTable().Func("getPlayerXUID", &sqGetPlayerXUID);
	RootTable().Func("getPlayerTeamFromXUID", &sqGetPlayerTeamFromXUID);
	RootTable().Func("setUnitSpeedPatch", &sqSetUnitSpeedPatch);
	RootTable().Func("setUnitSpeed", &sqSetPlayerSpeed);
	RootTable().Func("toggleWeaponPickup", &sqToggleWeaponPickup);
	RootTable().Func("setWeaponPickup", &sqSetWeaponPickup);
	RootTable().Func("setIndicatorVisibility", &sqSetTeamPlayerIndicatorVisibility);
	RootTable().Func("setPlayerBiped", &sqSetPlayerBiped);
	RootTable().Func("isUnitAlive", &sqIsUnitAlive);
	RootTable().Func("sqRandom", &sqrand);
	RootTable().Func("playerIsActive", &sqPlayerIsActive);
	RootTable().Func("getPeerIndex", &sqgetPeerIndex);
	RootTable().Func("getPlayerTeam", &sqgetPlayerTeam);
	RootTable().Func("controller2player", &sqcontroller_2_player);
	RootTable().Func("endGame", &sqendGame);
	RootTable().Func("toggleTeamGame", &sqToggleTeamGame);
	RootTable().Func("setTeamGame", &sqSetTeamGame);
	RootTable().Func("playerIndex2Datum", &sqGet_unit_datum_from_player_index);
	RootTable().Func("unitDatum2PlayerIndex", &sqget_player_index_from_unit_datum);
	RootTable().Func("getUnitTeam", &sqgetUnitTeam);
}