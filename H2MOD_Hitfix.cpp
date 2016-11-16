#include <Windows.h>
#include "H2MOD_Hitfix.h"
#include "xliveless.h"
#include "H2MOD.h"

void H2Hitfix::Initialize(bool isDedicatedServer)
{
	int offset;

	if (isDedicatedServer)
	{
		TRACE_GAME("[h2mod] Hitfix is being run on the Dedicated Server!");
		offset = 0x4A29BC;
	}
	else
	{
		offset = 0x47CD54;
	}

	DWORD AddressOffset = *(DWORD*)((char*)h2mod->GetBase() + offset);

	*(float*)(AddressOffset + 0xA4EC88) = 2400.0f; // battle_rifle_bullet.proj Initial Velocity 
	*(float*)(AddressOffset + 0xA4EC8C) = 2400.0f; //battle_rifle_bullet.proj Final Velocity
	*(float*)(AddressOffset + 0xB7F914) = 5000.0f; //sniper_bullet.proj Initial Velocity
	*(float*)(AddressOffset + 0xB7F918) = 5000.0f; //sniper_bullet.proj Final Velocity
}