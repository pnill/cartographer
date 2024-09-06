#include "stdafx.h"
#include "structure_bsp_definitions.h"

int16 get_global_structure_bsp_index(void)
{
	return *Memory::GetAddress<int16*>(0x4119A4, 0x3B5290);
}

structure_bsp* get_global_structure_bsp(void)
{
	return *Memory::GetAddress<structure_bsp**>(0x479E6C, 0x4A6428);
}
