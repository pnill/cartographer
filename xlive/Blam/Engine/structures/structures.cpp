#include "stdafx.h"
#include "structures.h"

__int16 get_global_structure_bsp_index()
{
	return *Memory::GetAddress<__int16*>(0x4119A4, 0x3B5290);
}
