#include "stdafx.h"
#include "scenario.h"

scenario* get_global_scenario() 
{
	return *Memory::GetAddress<scenario**>(0x479E74, 0x4A6430);
}
