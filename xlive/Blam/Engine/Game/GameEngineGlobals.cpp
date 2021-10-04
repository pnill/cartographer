#include "GameEngineGlobals.h"

s_game_engine_globals* s_game_engine_globals::get()
{
	return *Memory::GetAddress<s_game_engine_globals**>(0x4BF8F3);
}
