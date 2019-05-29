#include "stdafx.h"
#include "H2X.h"

#include <Windows.h>
#include "H2MOD.h"

void H2X::Initialize()
{
	if (h2mod->Server)
		return;
	
	DWORD FloatOffsets = *(DWORD*)(h2mod->GetBase() + 0x47CD54);

	// Weapon fire rates
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

	// H2X Sound_Classes
	*(float*)(FloatOffsets + 0x4821C) = 0.0f; /*H2X projectile_impact Index 0 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x48220) = -4.0f; /*H2X projectile_impact Index 0 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x48278) = 0.0f; /*H2X projectile_detonation Index 1 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x4827C) = -4.0f; /*H2X projectile_detonation Index 1 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x482D4) = 0.0f; /*H2X projectile_flyby Index 2 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x482D8) = -4.0f; /*H2X projectile_flyby Index 2 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x4838C) = 0.0f; /*H2X weapon_fire Index 4 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x48390) = -4.0f; /*H2X weapon_fire Index 4 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x483E8) = 0.0f; /*H2X weapon_ready Index 5 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x483EC) = -4.0f; /*H2X weapon_ready Index 5 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x48444) = 0.0f; /*H2X weapon_reload Index 6 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x48448) = -4.0f; /*H2X weapon_reload Index 6 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x484A0) = 0.0f; /*H2X weapon_empty Index 7 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x484A4) = -4.0f; /*H2X weapon_empty Index 7 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x484FC) = 0.0f; /*H2X weapon_charge Index 8 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x48500) = -4.0f; /*H2X weapon_charge Index 8 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x48558) = 0.0f; /*H2X weapon_overheat Index 9 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x4855C) = -4.0f; /*H2X weapon_overheat Index 9 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x485B4) = 0.0f; /*H2X weapon_idle Index 10 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x485B8) = -4.0f; /*H2X weapon_idle Index 10 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x48610) = 0.0f; /*H2X weapon_melee Index 11 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x48614) = -4.0f; /*H2X weapon_melee Index 11 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x4866C) = 0.0f; /*H2X weapon_animation Index 12 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x48670) = -4.0f; /*H2X weapon_animation Index 12 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x486C8) = -64.0f; /*H2X object_impacts Index 13 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x486CC) = -4.0f; /*H2X object_impacts Index 13 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x48724) = -12.0f; /*H2X particle_impacts Index 14 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x48728) = -4.0f; /*H2X particle_impacts Index 14 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x48894) = -32.0f; /*H2X unit_footsteps Index 18 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x48898) = -9.0f; /*H2X unit_footsteps Index 18 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x4894C) = 0.0f; /*H2X unit_animation Index 20 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x48950) = -2.0f; /*H2X unit_animation Index 20 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x48A04) = 0.0f; /*H2X vehicle_collision Index 22 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x48A08) = -4.0f; /*H2X vehicle_collision Index 22 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x48A60) = 0.0f; /*H2X vehicle_engine Index 23 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x48A64) = -4.0f; /*H2X vehicle_engine Index 23 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x48ABC) = 0.0f; /*H2X vehicle_animation Index 24 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x48AC0) = -4.0f; /*H2X vehicle_animation Index 24 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x48C2C) = 0.0f; /*H2X device_machinery Index 28 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x48C30) = -4.0f; /*H2X device_machinery Index 28 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x48C2C) = 0.0f; /*H2X device_stationary Index 29 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x48C30) = -4.0f; /*H2X device_stationary Index 29 Gains Bounds upper*/

	*(float*)(FloatOffsets + 0x48E54) = 0.0f; /*H2X ambient_machinery Index 34 Gains Bounds lower*/
	*(float*)(FloatOffsets + 0x48E58) = -4.0f; /*H2X ambient_machinery Index 34 Gains Bounds upper*/
}

void H2X::Deinitialize()
{
	if (h2mod->Server)
		return;

	DWORD FloatOffsets = *(DWORD*)(h2mod->GetBase() + 0x47CD54);

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
	