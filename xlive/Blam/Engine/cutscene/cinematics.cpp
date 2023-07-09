#include "stdafx.h"
#include "cinematics.h"

s_cinematic_globals* get_cinematic_globals()
{
	return *Memory::GetAddress<s_cinematic_globals**>(0x48227C, 0x4F3DB0);
}
