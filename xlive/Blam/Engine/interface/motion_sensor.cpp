#include "stdafx.h"
#include "motion_sensor.h"

#include "Util/Hooks/Hook.h"

#define k_radar_size_multiplier 1.f

void radar_patch()
{
	WriteValue<byte>(Memory::GetAddress(0x2849C4), (byte)D3DBLEND_INVSRCCOLOR);
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
	WriteValue<float>(Memory::GetAddress(0x3E6A9C), og_res * k_radar_size_multiplier);
}