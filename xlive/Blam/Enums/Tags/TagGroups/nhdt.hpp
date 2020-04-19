#pragma once
namespace Blam
{
	namespace Enums
	{
		namespace Tags
		{
			namespace TagGroups
			{
				namespace nhdt
				{
					enum class Input : char
					{
						BASICZero = 0,
						BASICOne = 1,
						BASICTime = 2,
						BASICGlobalHUDFade = 3,
						UNITSheild = 16,
						UNITBody = 17,
						UNITAutoAimed = 18,
						UNITHasNoGrenades = 19,
						UNITFragGrenadeCount = 20,
						UNITPlasmaGrenadeCount = 21,
						UNITTimeOnDPLSheild = 22,
						UNITZoomFraction = 23,
						UNITCamoValue = 24,
						PARENTSheild = 33,
						PARENTBody = 34,
						WEAPONClipAmmo = 49,
						WEAPONHeat = 50,
						WEAPONBattery = 51,
						WEAPONTotalAmmo = 52,
						WEAPONBarrelSpin = 53,
						WEAPONOverheated = 54,
						WEAPONClipAmmoFraction = 55,
						WEAPONTimeOnOverheat = 56,
						WEAPONBatteryFraction = 57,
						WEAPONLockingFraction = 58,
						UserScoreFraction = 66,
						OtherUserScoreFraction = 67,
						UserWinning = 68,
						BombArmingAmount = 69,
					};
					
				
					enum class Anchor : short
					{
						HealthAndSheild = 0,
						WeaponHUD = 1,
						MotionSensor = 2,
						Scoreboard = 3,
						Crosshair = 4,
						LockonTarget = 5,
					};					

					enum class SpecialHUDType : int
					{
						Unspecial = 0,
						SBPlayerEmblem = 1,
						SBOtherPlayerEmblem = 2,
						SBPlayerScoreMeter = 3,
						SBOtherPlayerScoreMeter = 4,
						UnitSheildMeter = 5,
						MotionSensor = 6,
						TerritoryMeter = 7,
					};
					enum class Justification : short
					{
						Left = 0,
						Center = 1,
						Right = 2,
					};
					enum class ScreenFontIndex : short
					{
						Default = 0,
						NumberFont = 1,
					};
				}
			}
		}
	}
}
