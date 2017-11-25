#include <Windows.h>
#include "H2MOD_H2X.h"
#include "xliveless.h"
#include "H2MOD.h"

struct tag_vaules
{
	float BR_recovery         = 0.295f;   /*H2X BR fire recovery time*/
	float Sniper_recovery     = 0.535f;   /*H2X Sniper Rifle fire recovery time*/
	float Beam_Rifle_recovery = 0.25875f; /*H2X Beam Rifle fire recovery time*/
	float Carbine_recovery    = 0.20f;    /*H2X Carbine fire recovery time*/
	float Shotgun_recovery    = 1.035f;   /*H2X Shotgun fire recovery time*/
	float Magnum_recovery     = 0.13f;    /*H2X Magnum fire recovery time*/
	float Brute_recovery      = 0.39f;    /*H2X Brute Shot fire recovery time*/
	float Plasma_recovery     = 0.11f;   /*H2X Plasma Pistol fire recovery time*/
};

tag_vaules default_vaules;
bool currently_enabled = false;

DWORD get_floatoffsets()
{
	int offset = 0x47CD54;
	if (h2mod->Server)
		offset = 0x4A29BC;
	TRACE_GAME("[h2mod] H2X is being run on client game");
	return *(DWORD*)((char*)h2mod->GetBase() + offset);
}

void Set(tag_vaules &values)
{
	DWORD FloatOffsets = get_floatoffsets();

	*(float*)(FloatOffsets + 0xA49A7C) = values.BR_recovery;
	*(float*)(FloatOffsets + 0xB7A330) = values.Sniper_recovery;
	*(float*)(FloatOffsets + 0xCE049C) = values.Beam_Rifle_recovery;
	*(float*)(FloatOffsets + 0xA7C3D4) = values.Carbine_recovery;
	*(float*)(FloatOffsets + 0xAF1DA4) = values.Shotgun_recovery;
	*(float*)(FloatOffsets + 0x96EC34) = values.Magnum_recovery;
	*(float*)(FloatOffsets + 0xC0EABC) = values.Brute_recovery;
	*(float*)(FloatOffsets + 0xA03250) = values.Plasma_recovery;
}

void Save(tag_vaules &values)
{
	DWORD FloatOffsets = get_floatoffsets();

	values.BR_recovery         = *(float*)(FloatOffsets + 0xA49A7C);
	values.Sniper_recovery     = *(float*)(FloatOffsets + 0xB7A330);
	values.Beam_Rifle_recovery = *(float*)(FloatOffsets + 0xCE049C);
	values.Carbine_recovery    = *(float*)(FloatOffsets + 0xA7C3D4);
	values.Shotgun_recovery    = *(float*)(FloatOffsets + 0xAF1DA4);
	values.Magnum_recovery     = *(float*)(FloatOffsets + 0x96EC34);
	values.Brute_recovery      = *(float*)(FloatOffsets + 0xC0EABC);
	values.Plasma_recovery     = *(float*)(FloatOffsets + 0xA03250);
}

void H2X_Enable()
{
	if (currently_enabled)
		return;
	Save(default_vaules);
	tag_vaules new_vaules;
	Set(new_vaules);
	currently_enabled = true;
}

void H2X_Disable()
{
	if (currently_enabled) {
		Set(default_vaules);
		currently_enabled = false;
	}
}
