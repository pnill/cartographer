#include <Windows.h>
#include "H2MOD_H2X.h"
#include "xliveless.h"
#include "H2MOD.h"

DWORD getOffset() {

	int offset = 0x47CD54;
	if (h2mod->Server)
		offset = 0x4A29BC;
	TRACE_GAME("[h2mod] H2X is being run on client game");
	DWORD FloatOffsets = *(DWORD*)((char*)h2mod->GetBase() + offset);
	return FloatOffsets;
}


void H2X::Initialize()
{
	
	DWORD FloatOffsets = getOffset();

	*(float*)(FloatOffsets + 0xA49A7C) = 0.295f; /*H2X BR fire recovery time*/
	*(float*)(FloatOffsets + 0xB7A330) = 0.535f; /*H2X Sniper Rifle fire recovery time*/
	*(float*)(FloatOffsets + 0xCE049C) = 0.25875f; /*H2X Beam Rifle fire recovery time*/
	*(float*)(FloatOffsets + 0xA7C3D4) = 0.20f; /*H2X Carbine fire recovery time*/
	*(float*)(FloatOffsets + 0xAF1DA4) = 1.035f; /*H2X Shotgun fire recovery time*/
	*(float*)(FloatOffsets + 0x96EC34) = 0.13f; /*H2X Magnum fire recovery time*/
	*(float*)(FloatOffsets + 0xC0EABC) = 0.39f;  /*H2X Brute Shot fire recovery time*/
	*(float*)(FloatOffsets + 0xA03250) = 0.11f; /*H2X Plasma Pistol fire recovery time*/
	*(float*)(FloatOffsets + 0xBDBF50) = 0.85f;  /*H2X Rocket Launcher fire recovery time*/
	*(float*)(FloatOffsets + 0xAAE544) = 8.5f; /*H2X Plasma Rifle rounds per second max*/
	*(float*)(FloatOffsets + 0xD0F960) = 10.0f; /*H2X Brute Plasma Rifle rounds per second max*/
}

void H2X::Deinitialize()
{

	DWORD FloatOffsets = getOffset();

	*(float*)(FloatOffsets + 0xA49A7C) = 0.26f; /*H2V BR fire recovery time*/
	*(float*)(FloatOffsets + 0xB7A330) = 0.5f; /*H2V Sniper Rifle fire recovery time*/
	*(float*)(FloatOffsets + 0xCE049C) = 0.25f; /*H2V Beam Rifle fire recovery time*/
	*(float*)(FloatOffsets + 0xA7C3D4) = 0.14f; /*H2V Carbine fire recovery time*/
	*(float*)(FloatOffsets + 0xAF1DA4) = 1.0f; /*H2V Shotgun fire recovery time*/
	*(float*)(FloatOffsets + 0x96EC34) = 0.1f; /*H2V Magnum fire recovery time*/
	*(float*)(FloatOffsets + 0xC0EABC) = 0.3f; /*H2V Brute Shot fire recovery time*/
	*(float*)(FloatOffsets + 0xA03250) = 0.05f; /*H2V Plasma Pistol fire recovery time*/
	*(float*)(FloatOffsets + 0xBDBF50) = 0.8f; /*H2V Rocket Launcher fire recovery time*/
	*(float*)(FloatOffsets + 0xAAE544) = 9.0f; /*H2V Plasma Rifle rounds per second max*/
	*(float*)(FloatOffsets + 0xD0F960) = 11.0f; /*H2V Brute Plasma Rifle rounds per second max*/
}
	