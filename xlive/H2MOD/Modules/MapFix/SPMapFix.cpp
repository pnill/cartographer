#include "stdafx.h"
#include "H2MOD\Modules\MapFix\SPMapFix.h"
#include "H2MOD.h"

void SPMapFix::Initialize()
{
	if (h2mod->Server)
		return;

	DWORD FloatOffsets = *(DWORD*)(h2mod->GetBase() + 0x47CD54);
	DWORD map_checksum = *(DWORD*)(h2mod->GetBase() + 0x47CD68 + 0x2F0);

	if (map_checksum == 0xF45A0114)/*05a_deltaapproach map checksum*/
	{
		DWORD tree_ancient_leaves = *(DWORD*)(h2mod->GetBase() + 0x00A3FF00);
		*(DWORD*)(tree_ancient_leaves + 0x1CC) = 0xFFFFFFFF;/*Setting bitmap index 4 in tree_ancient_leaves to null*/
	}
}