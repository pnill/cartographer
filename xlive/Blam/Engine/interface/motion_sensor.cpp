#include "stdafx.h"
#include "motion_sensor.h"

#include "Util/Hooks/Hook.h"

#define SIZE_MULTIPLIER 1

void radar_patch()
{
	WriteValue<byte>(Memory::GetAddress(0x2849C4, 0x0), (byte)D3DBLEND_INVSRCCOLOR);
}

void motion_sensor_apply_patches()
{
	if (Memory::IsDedicatedServer()) { return; }

	radar_patch();
	motion_sensor_fix_size();
}

void motion_sensor_fix_size()
{
	float og_res = *Memory::GetAddress<float*>(0x3E6A9C);
	WriteValue<float_t>(Memory::GetAddress(0x3E6A9C), og_res * SIZE_MULTIPLIER);
}