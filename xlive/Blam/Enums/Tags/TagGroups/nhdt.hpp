#pragma once
#include "Blam\Cache\DataTypes\Bitfield.h"
using namespace Blam::Cache;
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
					struct DoDrawIfUnitFlags
					{
						DataTypes::Flags Default = 0;
						DataTypes::Flags GrenadeTypeIsNONE = 1;
						DataTypes::Flags GrenadeTypeIsFrag = 2;
						DataTypes::Flags GrenadeTypeIsPlasma = 3;
						DataTypes::Flags UnitIsSingleWielding = 4;
						DataTypes::Flags UnitIsDualWielding = 5;
						DataTypes::Flags UnitIsUnzoomed = 6;
						DataTypes::Flags UnitIsZoomedLevel1 = 7;
						DataTypes::Flags UnitIsZoomedLevel2 = 8;
						DataTypes::Flags GrenadesDisabled = 9;
						DataTypes::Flags BinocularsEnabled = 10;
						DataTypes::Flags MotionSensorEnabled = 11;
						DataTypes::Flags SheildEnabled = 12;
						DataTypes::Flags Dervish = 13;
					};
					struct DoDrawIfExtraFlags
					{
						DataTypes::Flags AutoAimFriendly = 0;
						DataTypes::Flags AutoAimPlasma = 1;
						DataTypes::Flags AutoAimHeadshot = 2;
						DataTypes::Flags AutoAimVulnerable = 3;
						DataTypes::Flags AutoAimInvincible = 4;
					};
					struct DoDrawIfWeaponFlags
					{
						DataTypes::Flags PrimaryWeapon = 0;
						DataTypes::Flags SecondaryWeapon = 1;
						DataTypes::Flags BackpackWeapon = 2;
						DataTypes::Flags AgeBelowCutoff = 3;
						DataTypes::Flags ClipBelowCutoff = 4;
						DataTypes::Flags TotalBelowCutoff = 5;
						DataTypes::Flags Overheated = 6;
						DataTypes::Flags OutOfAmmo = 7;
						DataTypes::Flags LockTargetAvailable = 8;
						DataTypes::Flags Locking = 9;
						DataTypes::Flags Locked = 10;
					};
					struct DoDrawIfGameEngineStateFlags
					{
						DataTypes::Flags CampaignSolo = 0;
						DataTypes::Flags CampaignCoop = 1;
						DataTypes::Flags FreeForAll = 2;
						DataTypes::Flags TeamGame = 3;
						DataTypes::Flags UserLeading = 4;
						DataTypes::Flags UserNotLeading = 5;
						DataTypes::Flags TimedGame = 6;
						DataTypes::Flags UntimedGame = 7;
						DataTypes::Flags OtherScoreValid = 8;
						DataTypes::Flags OtherScoreInvalid = 9;
						DataTypes::Flags PlayerIsArmingBomb = 10;
						DataTypes::Flags PlayerTalking = 11;
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
					struct Flags
					{
						DataTypes::Flags FlipHorizontally = 0;
						DataTypes::Flags FlipVertically = 1;
						DataTypes::Flags ScopeMirrorHorizontally = 2;
						DataTypes::Flags ScopeMirrorVertically = 3;
						DataTypes::Flags ScopeStretch = 4;
					};
					namespace NumericalElements
					{
						struct Flags
						{
							DataTypes::Flags StringIsANumber = 0;
							DataTypes::Flags Force2DigitNumber = 1;
							DataTypes::Flags Force3DigitNumber = 2;
							DataTypes::Flags TalkingPlayerHack = 2;
						};						
					}
					namespace Effect
					{
						struct Flags 
						{
							DataTypes::Flags ApplyScale = 0;
							DataTypes::Flags ApplyTheta = 1;
							DataTypes::Flags ApplyOffset = 2;
						};
					}

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
