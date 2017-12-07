#include <Windows.h>
#include "H2MOD_H2X.h"
#include "xliveless.h"
#include "H2MOD.h"

void H2X_Initialize(bool init)
{
	int offset = 0x47CD54;
	if (h2mod->Server)
		offset = 0x4A29BC;
	TRACE_GAME("[h2mod] H2X is being run on client game");
	DWORD FloatOffsets = *(DWORD*)((char*)h2mod->GetBase() + offset);

	*(float*)(FloatOffsets + 0xA49A7C) = init ? 0.295f : 0.26f; /*H2X/H2V BR fire recovery time*/
	*(float*)(FloatOffsets + 0xB7A330) = init ? 0.535f : 0.5f; /*H2X/H2V Sniper Rifle fire recovery time*/
	*(float*)(FloatOffsets + 0xCE049C) = init ? 0.25875f : 0.25f; /*H2X/H2V Beam Rifle fire recovery time*/
	*(float*)(FloatOffsets + 0xA7C3D4) = init ? 0.20f : 0.14f; /*H2X/H2V Carbine fire recovery time*/
	*(float*)(FloatOffsets + 0xAF1DA4) = init ? 1.035f : 1.0f; /*H2X/H2V Shotgun fire recovery time*/
	*(float*)(FloatOffsets + 0x96EC34) = init ? 0.13f : 0.1f; /*H2X/H2V Magnum fire recovery time*/
	*(float*)(FloatOffsets + 0xC0EABC) = init ? 0.39f : 0.3f; /*H2X/H2V Brute Shot fire recovery time*/
	*(float*)(FloatOffsets + 0xA03250) = init ? 0.11f : 0.05f; /*H2X/H2V Plasma Pistol fire recovery time*/
	*(float*)(FloatOffsets + 0xBDBF50) = init ? 0.85f : 0.8f; /*H2X/H2V Rocket Launcher fire recovery time*/
	*(float*)(FloatOffsets + 0xAAE544) = init ? 8.5f : 9.0f; /*H2X/H2V Plasma Rifle rounds per second max*/
	*(float*)(FloatOffsets + 0xD0F960) = init ? 10.0f : 11.0f ; /*H2X/H2V Brute Plasma Rifle rounds per second max*/
}
	