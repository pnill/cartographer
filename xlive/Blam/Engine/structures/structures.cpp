#include "stdafx.h"
#include "structures.h"

s_structure_globals* structure_globals_get(void)
{
	return Memory::GetAddress<s_structure_globals*>(0x4D12A1, 0x4F5159);
}
