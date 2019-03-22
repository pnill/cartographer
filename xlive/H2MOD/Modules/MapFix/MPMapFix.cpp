#include <Windows.h>
#include "H2MOD\Modules\MapFix\MPMapFix.h"
#include "H2MOD.h"

void MPMapFix::Initialize()
{
	int offset = 0x47CD54;
	if (h2mod->Server) {
		offset = 0x4A29BC;
	}

	DWORD FloatOffsets = *(DWORD*)(h2mod->GetBase() + offset);
	DWORD map_checksum = *(DWORD*)(h2mod->GetBase() + 0x47CD68 + 0x2F0);

	*(float*)(FloatOffsets + 0xE610A0) = 5.0f; /*masterchief_mp hlmt min game acc default value doubled*/
	*(float*)(FloatOffsets + 0xE610A4) = 9.0f; /*masterchief_mp hlmt max game acc default value doubled*/

	*(float*)(FloatOffsets + 0xE610B0) = 14.0f; /*masterchief_mp hlmt max abs acc default value doubled*/
	*(float*)(FloatOffsets + 0xE6CA50) = 20.0f; /*masterchief_mp hlmt max abs acc default value doubled*/
}