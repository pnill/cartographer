#pragma once
#include <sqrat/include/sqrat.h>

void BindSquirrel(HSQUIRRELVM vm);


namespace ScriptEngine {
	bool sqOnTeamChange(HSQUIRRELVM vm = NULL,int playerIndex = 0,int teamIndex = 0);
	void sqOnPlayerDeath(HSQUIRRELVM vm = NULL, int unit_datum = 0, int damaging_datum = 0);
	bool sqOnUpdateScore(HSQUIRRELVM vm = NULL, int playerIndex = 0, int killType = 0);
	bool sqOnAutoPickup(HSQUIRRELVM vm = NULL, int unit_datum = 0, int object_datum = 0);
	void sqPrePlayerSpawn(HSQUIRRELVM vm = NULL, int unit_datum = 0);
	void sqPostPlayerSpawn(HSQUIRRELVM vm = NULL, int unit_datum = 0);
	void sqOnMapLoad();
	void sqSessionEnd();
	void sqSessionStart();
	void sqLoadScript(std::string);
	void sqLoadScriptFromString(char* script_data);
	extern std::string loaded_script;
	extern bool g_sqTeamGame;
	extern bool g_sqMotionSensor;
	extern short g_sqRoundTime;
	extern bool g_sgAlwaysInvisible;
	extern bool g_sgRoundSwitchResetsMap;
	extern bool g_sgTieResolution;
	extern bool g_sgObservers;
	extern bool g_sgAllowChangingTeams;
	extern bool g_sgFriendlyFire;
	extern bool g_sgOvershieldsOnMap;
	extern bool g_sgInvisibilityOnMap;
	extern bool g_sgGrenadesOnMap;
	extern bool g_sgStartingGrenades;
	extern bool g_sgExtraDamage;
	extern bool g_sgDamageResistant;
	extern bool g_sgForceEvenTeams;
	extern bool g_sgRoundSetting1Round;

}