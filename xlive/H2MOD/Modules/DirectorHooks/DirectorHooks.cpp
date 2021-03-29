#include "DirectorHooks.h"
#include "H2MOD.h"
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

	typedef void( director_choose_camera_game)();

	double get_deathcam_fov()
	{
		return 1.36136; //78deg in radians
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
