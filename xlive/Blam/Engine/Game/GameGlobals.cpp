#include "GameGlobals.h"

s_game_globals* s_game_globals::get()
{
	return *Memory::GetAddress<s_game_globals**>(0x482d3c, 0x4CB520);
}
