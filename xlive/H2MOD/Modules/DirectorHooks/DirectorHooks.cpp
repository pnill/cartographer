#include "stdafx.h"

#include "DirectorHooks.h"
#include "Util/Hooks/Hook.h"

namespace DirectorHooks
{
	e_director_mode* DirectorMode;
	bool* DirectorModeChanged;
	void SetDirectorMode(e_director_mode mode)
	{
		if (*DirectorMode != mode)
		{
			*DirectorMode = mode;
			*DirectorModeChanged = true;
		}
	}

	float __cdecl get_deathcam_fov()
	{
		return 1.36136f; //78deg in radians
	}

	void ApplyHooks()
	{
		PatchCall(Memory::GetAddress(0xCDF90), get_deathcam_fov);
	}

	void Initialize()
	{
		DirectorMode = Memory::GetAddress<e_director_mode*>(0x4A849C);
		DirectorModeChanged = Memory::GetAddress<bool*>(0x4A84A0);

		ApplyHooks();
	}
}
