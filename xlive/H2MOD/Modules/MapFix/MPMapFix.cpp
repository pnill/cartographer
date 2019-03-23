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

	*(float*)(FloatOffsets + 0xE65D88) = 5.0f; /*elite_mp hlmt min game acc default value doubled*/
	*(float*)(FloatOffsets + 0xE65D8C) = 9.0f; /*elite_mp hlmt max game acc default value doubled*/

	*(float*)(FloatOffsets + 0xE65D98) = 14.0f; /*elite_mp hlmt max abs acc default value doubled*/
	*(float*)(FloatOffsets + 0xE65D9C) = 20.0f; /*elite_mp hlmt max abs acc default value doubled*/
}