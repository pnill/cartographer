#include <Windows.h>
#include "H2MOD_H2X.h"
#include "xliveless.h"
#include "H2MOD.h"

void H2X::Initialize(void)

{
	int offset = 0x47CD54;
	if (h2mod->Server)
		offset = 0x4A29BC;
	TRACE_GAME("[h2mod] H2X is being run on client game");
	DWORD FloatOffsets = *(DWORD*)((char*)h2mod->GetBase() + offset);

	*(float*)(FloatOffsets + 0xA49A7C) = 0.295f; /*H2X BR recovery time*/
	*(float*)(FloatOffsets + 0xB7A330) = 0.5175; /*H2X Sniper Rifle recovery time*/
	*(float*)(FloatOffsets + 0xCE049C) = 0.25875; /*H2X Beam Rifle recovery time*/
}