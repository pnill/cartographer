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
					Frozen = 0x00,
					Normal = 0x01,
					Frozen2 = 0x02,
					Dead = 0x03,
					frozen3 = 0x04,
					frozen4 = 0x05
				};
			}

		}
	}
}