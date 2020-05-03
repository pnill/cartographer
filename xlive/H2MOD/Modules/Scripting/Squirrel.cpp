#include <sqrat/include/sqrat.h>
#include "Squirrel.h"
#include "H2MOD.h"

void MessageBoxTest(char* Msg)
{
	MessageBoxA(NULL, Msg, "Squirrel Test", MB_OK);
}

void sqLoadScript(std::string path)
{
	Sqrat::Script varScript;
	varScript.CompileFile(path);
	varScript.Run();
}

void sqSessionStart()
{
	BindSquirrel(sq_open(1000));
	Sqrat::Script GlobalScript;
	GlobalScript.CompileFile("Scripts/Globals.nut");
	GlobalScript.Run();
}

void sqSessionEnd()
{
	if (sq_getvmstate(Sqrat::DefaultVM::Get()) == SQ_VMSTATE_RUNNING)
		sq_close(Sqrat::DefaultVM::Get());

}

BYTE SqGetMapType()
{
	return h2mod->GetMapType();
}

void GivePlayerWeapon(int PlayerIndex, int WeaponID,bool bReset)
{
	call_give_player_weapon(PlayerIndex, WeaponID,bReset);
}

void BindSquirrel(HSQUIRRELVM vm)
{
	using namespace Sqrat;

	DefaultVM::Set(vm);
	RootTable().Func("character_datum_from_index", &character_datum_from_index);
	RootTable().Func("GetMapType", &SqGetMapType);
	RootTable().Func("MessageBoxTest", &MessageBoxTest);
	RootTable().Func("GivePlayerWeapon", &GivePlayerWeapon);

}