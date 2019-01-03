#include <Windows.h>
#include "H2MOD\Modules\HitFix\Hitfix.h"
#include "H2MOD.h"

void HitFix::Initialize()
{
	int offset = 0x47CD54;
	if (h2mod->Server) {
		offset = 0x4A29BC;
	}

	DWORD AddressOffset = *(DWORD*)((char*)h2mod->GetBase() + offset);

	*(float*)(AddressOffset + 0xA4EC88) = 800.0f;	//battle_rifle_bullet.proj Initial Velocity 400
	*(float*)(AddressOffset + 0xA4EC8C) = 800.0f;	//battle_rifle_bullet.proj Final Velocity 400
	*(float*)(AddressOffset + 0xA84DD4) = 800.0f;	//carbine_slug.proj Initial Velocity 400
	*(float*)(AddressOffset + 0xA84DD8) = 800.0f;	//carbine_slug.proj Final Velocity 400
	*(float*)(AddressOffset + 0xB7F914) = 2400.0f;	//sniper_bullet.proj Initial Velocity 1200
	*(float*)(AddressOffset + 0xB7F918) = 2400.0f;	//sniper_bullet.proj Final Velocity 1200
	//FIXME COOP will break because of one of these tags not existing.
	*(float*)(AddressOffset + 0xCE4598) = 2400.0f;	//beam_rifle_beam.proj Initial Velocity 1200
	*(float*)(AddressOffset + 0xCE459C) = 2400.0f;	//beam_rifle_beam.proj Final Velocity 1200
	*(float*)(AddressOffset + 0x81113C) = 180.0f;	//gauss_turret.proj Initial Velocity def 90
	*(float*)(AddressOffset + 0x811140) = 180.0f;	//gauss_turret.proj Final Velocity def 90
	*(float*)(AddressOffset + 0x97A194) = 800.0f;	//magnum_bullet.proj initial def 400
	*(float*)(AddressOffset + 0x97A198) = 800.0f;	//magnum_bullet.proj final def 400
	*(float*)(AddressOffset + 0x7E7E20) = 1600.0f;	//bullet.proj (chaingun) initial def 800
	*(float*)(AddressOffset + 0x7E7E24) = 1600.0f;	//bullet.proj (chaingun) final def 800
}