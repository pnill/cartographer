#pragma once
#include "Blam\Cache\DataTypes\DataTypes.h"
using Blam::Cache::DataTypes::Flags;
namespace Blam
{
	namespace Enums
	{
		namespace Tags
		{
			namespace TagGroups
			{
				namespace hudg
				{
					enum Anchor : __int16
					{
						TopLeft = 0,
						TopRight = 1,
						BottomLeft = 2,
						BottomRight = 3,
						Center = 4,
					};
					namespace ButtonIcons
					{
						struct ButtonFlags
						{
							Flags UseTextFromStringListInstead = 0;
							Flags OverrideDefaultColor = 1;
							Flags WidthOffsetIsAbsoluteIconWidth = 2;
						};
					}
					namespace Dashlights
					{
						struct FlashFlags
						{
							Flags UseTextFromStringListInstead = 0;
						};
					}
					namespace HudSounds
					{
						struct HudSoundsFlags
						{
							Flags ShieldRecharging = 0;
							Flags ShieldDamaged = 1;
							Flags ShieldLow = 2;
							Flags ShieldEmpty = 3;
							Flags HealthLow = 4;
							Flags HealthMinorDamage = 5;
							Flags HealthMajorDamage = 6;
							Flags RocketLocking = 7;
							Flags RocketLocked = 8;
						};
					}
					namespace WaypointsArrows
					{
						struct waypoints_arrowsFlags
						{
							Flags DoNotRotateWhenPointingOffscreen = 0;
						};
					}
					struct FlashFlags
					{
						Flags ReverseDefaultFlashingColors = 0;
					};
				}
			}
		}
	}
}
