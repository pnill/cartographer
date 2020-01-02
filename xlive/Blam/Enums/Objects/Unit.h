#pragma once
#include "..\stdafx.h"
namespace Blam
{
	namespace Enums
	{
		namespace Objects
		{
			namespace Unit
			{
				enum class UnitWeapons
				{
					PrimaryWeapon,
					SecondaryWeapon,
					DualWeildWeapon
				};
				enum class Grenades : BYTE
				{
					Fragmentation,
					Plasma
				};
				enum class WeaponIndex : WORD
				{
					Primary = 0xFF00,
					Secondary = 0xFF01,
					DualWeild = 0x0201
				};
				enum class UnitState : BYTE
				{
					mode_ground = 1,
					mode_flying,
					mode_dead,
					mode_sentinel,
					mode_sentinel_2,
					mode_melee
				};
			}

		}
	}
}