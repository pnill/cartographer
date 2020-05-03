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
}