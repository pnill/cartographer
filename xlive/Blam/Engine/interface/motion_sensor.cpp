#include "stdafx.h"
#include "motion_sensor.h"

#include "Util/Hooks/Hook.h"

void radar_patch()
{
	WriteValue<byte>(Memory::GetAddress(0x2849C4, 0x0), (byte)D3DBLEND_INVSRCCOLOR);
}

void motion_sensor_apply_patches()
{
	if (Memory::IsDedicatedServer()) { return; }

	radar_patch();
}