#include "stdafx.h"
#include "player_control.h"

s_player_control_globals* s_player_control_globals::get()
{
	return *Memory::GetAddress<s_player_control_globals**>(0x4CA37C, 0x4CF0E8);
}
