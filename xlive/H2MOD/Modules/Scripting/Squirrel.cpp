#include <sqrat/include/sqrat.h>
#include "Squirrel.h"
#include "H2MOD.h"

void MessageBoxTest(char* Msg)
{
	MessageBoxA(NULL, Msg, "Squirrel Test", MB_OK);
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