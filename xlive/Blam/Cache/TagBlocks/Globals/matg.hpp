#pragma once
#pragma pack(push,1)
#ifndef MATG_H_B
#define MATG_H_B
#include "Blam\Cache\DataTypes\DataTypes.h"
#include "Blam\Cache\TagBlocks.hpp"
#include "Blam\Maths\Maths.h"
#include "Blam\Enums\Tags\TagGroups\matg.hpp"

using namespace Blam::Cache::DataTypes;

namespace Blam
{
	namespace Cache
	{
		namespace TagBlocks
		{
			namespace Globals
			{
				/****************************************
				* size: 8
				* alignment:    4
				* max_count:    1
				*
				****************************************/
				struct havok_cleanup_resources :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					tagRef ObjectCleanupEffect;
				};
				TAG_BLOCK_SIZE_ASSERT(havok_cleanup_resources, 8);
				/****************************************
				* size: 72
				* alignment:    4
				* max_count:    1
				****************************************/
				struct collision_damage :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					tagRef CollisionDamage;
					float minGameAccDefault;
					float maxGameAccDefault;
					float minGamScaleDefault;
					float maxGamScaleDefault;
					float minAbsAccDefault;
					float maxAbsAccDefault;
					float minAbsScaleDefault;
					float maxAbsScaleDefault;
					PAD(32);
				};
				TAG_BLOCK_SIZE_ASSERT(collision_damage, 72);
				/****************************************
				* size: 36
				* alignment:    4
				* max_count:    1
				****************************************/
				struct sound_info :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					tagRef SoundClass;
					tagRef SoundFX;
					tagRef SoundMix;
					tagRef SoundCombatDialogue;
					tagRefN SoundGestalt;
				};
				TAG_BLOCK_SIZE_ASSERT(sound_info, 36);
				/****************************************
				* size: 12
				* alignment:    4
				* max_count:    1
				*
				****************************************/
				struct gravemind_properties :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					float minRetreatTime;
					float idealRetreatTime;
					float maxRetreatTime;
				};
				TAG_BLOCK_SIZE_ASSERT(gravemind_properties, 12);
				/****************************************
				* size: 360
				* alignment:    4
				* max_count:    1
				*
				****************************************/
				struct ai_information :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					float dangerBroadlyFacing;
					PAD(4);
					float dangerShootingNear;
					PAD(4);
					float dangerShootingAt;
					PAD(4);
					float dangerExtremelyClose;
					PAD(4);
					float dangerSheildDamage;
					float dangerExtendedShieldDamage;
					float dangerBodyDamage;
					float dangerExtendedBodyDamage;
					PAD(48);
					tagRef AiDialog;
					StringID defaultmissiondialoguesoundeffect;
					PAD(20);
					float jumpDown;
					float jumpStep;
					float jumpCrouch;
					float jumpStand;
					float jumpStorey;
					float jumpTower;
					float maxJumpDownHeightDown;
					float maxJumpDownHeightStep;
					float maxJumpDownHeightCrouch;
					float maxJumpDownHeightStand;
					float maxJumpDownHeightStorey;
					float maxJumpDownHeightTower;
					float hoistStepMin;
					float hoistStepMax;
					float hoistCrouchMin;
					float hoistCrouchMax;
					float hoistStandMin;
					float hoistStandMax;
					PAD(24);
					float vaultStepMin;
					float vaultStepMax;
					float vaultCrouchMin;
					float vaultCrouchMax;
					PAD(48);
					Reflexive<gravemind_properties> GravemindProperties;
					PAD(48);
					float scaryTargetThrehold;
					float scaryWeaponThrehold;
					float playerScariness;
					float berserkingActorScariness;
				};
				TAG_BLOCK_SIZE_ASSERT(ai_information, 360);
				/****************************************
				* size: 8
				* alignment:    4
				* max_count:    -1
				*
				****************************************/
				struct armor_modifiers :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					StringID name;
					float damageMultiplier;
				};
				TAG_BLOCK_SIZE_ASSERT(armor_modifiers, 8);
				/****************************************
				* size: 12
				* alignment:    4
				* max_count:    -1
				*
				****************************************/
				struct damage_groups :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					StringID name;
					Reflexive<armor_modifiers> ArmorModifiers;
				};
				TAG_BLOCK_SIZE_ASSERT(damage_groups, 12);
				/****************************************
				* size: 8
				* alignment:    4
				* max_count:    1
				*
				****************************************/
				struct damage_table :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					Reflexive<damage_groups> DamageGroups;
				};
				TAG_BLOCK_SIZE_ASSERT(damage_table, 8);


				/****************************************
				* size: 8
				* alignment:    4
				* max_count:    2
				*
				****************************************/
				struct Sounds_Obsolete :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					tagRef Sound;
				};
				TAG_BLOCK_SIZE_ASSERT(Sounds_Obsolete, 8);
				/****************************************
				* size: 20
				* alignment:    4
				* max_count:    1
				*
				****************************************/
				struct camera :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					tagRef defaultUnitCameraTrack;
					float defaultChangePause;
					float FirstPersonChangePause;
					float followingCameraChangePause;
				};
				TAG_BLOCK_SIZE_ASSERT(camera, 20);
				/****************************************
				* size: 4
				* alignment:    4
				* max_count:    16
				*
				****************************************/
				struct look_function :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					float scale;
				};
				TAG_BLOCK_SIZE_ASSERT(look_function, 4);
				/****************************************
				* size: 128
				* alignment:    4
				* max_count:    1
				*
				****************************************/
				struct player_control :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					float magnetismFriction;
					float magnetismAdhesion;
					float inconsequentialTargetScale;
					PAD(12);
					float crosshairLocationX;
					float crosshairLocationY;
					float secondsToStart;
					float secondsToFullSpeed;
					float decayRate;
					float fullSpeedMultiplier;
					float peggedMagnitude;
					float peggedAngularThreshold;
					PAD(8);
					float lookDefaultPitchRate;
					float lookDefaultYawRate;
					float lookPegThreshold;
					float lookYawAccelerationTime;
					float lookYawAccelerationScale;
					float lookPitchAccelerationTime;
					float lookPitchAccelerationScale;
					float lookAutolevelingScale;
					PAD(8);
					float gravityScale;
					__int16 minWeaponSwapTicks;
					__int16 minAutolevelingTicks;
					float minVehicleFlippingAngle;
					Reflexive<look_function> LookFunction;
					float minimmumActionHoldTime;
				};
				TAG_BLOCK_SIZE_ASSERT(player_control, 128);

				/****************************************
				* size: 644
				* alignment:    4
				* max_count:    1
				*
				****************************************/
				struct difficulty :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					float easyEnemyDamage;
					float normalEnemyDamage;
					float heroicEnemyDamage;
					float legendaryEnemyDamage;
					float easyEnemyVitality;
					float normalEnemyVitality;
					float heroicEnemyVitality;
					float legendaryEnemyVitality;
					float easyEnemySheild;
					float normalEnemySheild;
					float heroicEnemySheild;
					float legendaryEnemySheild;
					float easyEnemyRecharge;
					float normalEnemyRecharge;
					float heroicEnemyRecharge;
					float legendaryEnemyRecharge;
					float easyFriendDamage;
					float normalFriendDamage;
					float heroicFriendDamage;
					float legendaryFriendDamage;
					float easyFriendVitality;
					float normalFriendVitality;
					float heroicFriendVitality;
					float legendaryFriendVitality;
					float easyFriendSheild;
					float normalFriendSheild;
					float heroicFriendSheild;
					float legendaryFriendSheild;
					float easyFriendRecharge;
					float normalFriendRecharge;
					float heroicFriendRecharge;
					float legendaryFriendRecharge;
					float easyInfectionForms;
					float normalInfectionForms;
					float heroicInfectionForms;
					float legendaryInfectionForms;
					PAD(16);
					float easyRateOfFire;
					float normalRateOfFire;
					float heroicRateOfFire;
					float legendaryRateOfFire;
					float easyProjectileError;
					float normalProjectileError;
					float heroicProjectileError;
					float legendaryProjectileError;
					float easyBurstError;
					float normalBurstError;
					float heroicBurstError;
					float legendaryBurstError;
					float easyNewTargetDelay;
					float normalNewTargetDelay;
					float heroicNewTargetDelay;
					float legendaryNewTargetDelay;
					float easyBurstSeparation;
					float normalBurstSeparation;
					float heroicBurstSeparation;
					float legendaryBurstSeparation;
					float easyTargetTracking;
					float normalTargetTracking;
					float heroicTargetTracking;
					float legendaryTargetTracking;
					float easyTargetLeading;
					float normalTargetLeading;
					float heroicTargetLeading;
					float legendaryTargetLeading;
					float easyOverchargeChance;
					float normalOverchargeChance;
					float heroicOverchargeChance;
					float legendaryOverchargeChance;
					float easySpecialFireDelay;
					float normalSpecialFireDelay;
					float heroicSpecialFireDelay;
					float legendarySpecialFireDelay;
					float easyGuidanceVsPlayer;
					float normalGuidanceVsPlayer;
					float heroicGuidanceVsPlayer;
					float legendaryGuidanceVsPlayer;
					float easyMeleeDelayBase;
					float normalMeleeDelayBase;
					float heroicMeleeDelayBase;
					float legendaryMeleeDelayBase;
					float easyMeleeDelayScale;
					float normalMeleeDelayScale;
					float heroicMeleeDelayScale;
					float legendaryMeleeDelayScale;
					PAD(16);
					float easyGrenadeChanceScale;
					float normalGrenadeChanceScale;
					float heroicGrenadeChanceScale;
					float legendaryGrenadeChanceScale;
					float easyGrenadeTimerScale;
					float normalGrenadeTimerScale;
					float heroicGrenadeTimerScale;
					float legendaryGrenadeTimerScale;
					PAD(48);
					float easyMajorUpgradeNormal;
					float normalMajorUpgradeNormal;
					float heroicMajorUpgradeNormal;
					float legendaryMajorUpgradeNormal;
					float easyMajorUpgradeFew;
					float normalMajorUpgradeFew;
					float heroicMajorUpgradeFew;
					float legendaryMajorUpgradeFew;
					float easyMajorUpgradeMany;
					float normalMajorUpgradeMany;
					float heroicMajorUpgradeMany;
					float legendaryMajorUpgradeMany;
					float easyPlayerVehicleRamChance;
					float normalPlayerVehicleRamChance;
					float heroicPlayerVehicleRamChance;
					float legendaryPlayerVehicleRamChance;
					PAD(132);
				};
				TAG_BLOCK_SIZE_ASSERT(difficulty, 644);
				/****************************************
				* size: 44
				* alignment:    4
				* max_count:    2
				*
				****************************************/
				struct grenades :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					__int16 maxCount;
					PAD(2);
					tagRef ThrowingEffect;
					PAD(16);
					tagRef Equipment;
					tagRef Projectile;
				};
				TAG_BLOCK_SIZE_ASSERT(grenades, 44);
				/****************************************
				* size: 8
				* alignment:    4
				* max_count:    32
				*
				****************************************/
				struct global_vertex_shaders :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					tagRef VertexShader;
				};
				TAG_BLOCK_SIZE_ASSERT(global_vertex_shaders, 8);
				/****************************************
				* size: 264
				* alignment:    4
				* max_count:    1
				*
				****************************************/
				struct rasterizer_data :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					tagRef DistanceAttenuation;
					tagRef VectorNormalization;
					tagRef Gradients;
					tagRef LoadingScreen;
					tagRef LoadingScreenSweep;
					tagRef LoadingScreenSpinner;
					tagRef Glow;
					tagRef LoadingScreenLogos;
					tagRef LoadingScreenTickers;
					PAD(16);
					Reflexive<global_vertex_shaders> GlobalVertexShaders;
					tagRef Default2DTex;
					tagRef Default3DTex;
					tagRef DefaultCubeMap;
					PAD(48);
					PAD(36);
					tagRef GlobalShader;
					BitField16<Blam::Enums::Tags::TagGroups::matg::ActiveCamoFlags> ActiveCamoFlags;
					PAD(2);
					float activeCamoRefractionAmount;
					float activeCamoDistanceFalloff;
					float activeCamoTintColorR;
					float activeCamoTintColorG;
					float activeCamoTintColorB;
					float activeCamoHyperStealthRefraction;
					float activeCamoHyperStealthDistanceFalloff;
					float activeCamoHyperStealthTintR;
					float activeCamoHyperStealthTintG;
					float activeCamoHyperStealthTintB;
					tagRef Unused;
				};
				TAG_BLOCK_SIZE_ASSERT(rasterizer_data, 264);
				/****************************************
				* size: 152
				* alignment:    4
				* max_count:    1
				*
				****************************************/
				struct interface_tag_refernces :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					tagRef FontSystem;
					tagRef FontTerminal;
					tagRef ScreenColorTable;
					tagRef HudColorTable;
					tagRef EditorColorTable;
					tagRef DialogColorTable;
					tagRef HudGlobals;
					tagRef MotionSensorSweepBitmap;
					tagRef MotionSensorSweepBitmapMask;
					tagRef MultiplayerHudBitmap;
					PAD(8);
					tagRef HudDigitsDefinition;
					tagRef MotionSensorBlipBitmap;
					tagRef InterfaceGooMap1;
					tagRef InterfaceGooMap2;
					tagRef InterfaceGooMap3;
					tagRef MainmenuUIGlobals;
					tagRef SingleplayerUIGlobals;
					tagRef MultiplayerUIGlobals;
				};
				TAG_BLOCK_SIZE_ASSERT(interface_tag_refernces, 152);
				/****************************************
				* size: 152
				* alignment:    4
				* max_count:    20
				*
				****************************************/
				struct weapon_list_to_do_with_hard_coded_enum :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					tagRef Weapon;
					PAD(144);
				};
				TAG_BLOCK_SIZE_ASSERT(weapon_list_to_do_with_hard_coded_enum, 152);
				/****************************************
				* size: 152
				* alignment:    4
				* max_count:    20
				*
				****************************************/
				struct cheat_powerups :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					tagRef Powerup;
					PAD(144);
				};
				TAG_BLOCK_SIZE_ASSERT(cheat_powerups, 152);
				/****************************************
				* size: 152
				* alignment:    4
				* max_count:    20
				*
				****************************************/
				struct vehicles :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					tagRef vehicle;
					PAD(144);
				};
				TAG_BLOCK_SIZE_ASSERT(vehicles, 152);
				/****************************************
				* size: 152
				* alignment:    4
				* max_count:    20
				*
				****************************************/
				struct sounds :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					tagRef Sound;
					PAD(144);
				};
				TAG_BLOCK_SIZE_ASSERT(sounds, 152);
				/****************************************
				* size: 152
				* alignment:    4
				* max_count:    1
				*
				****************************************/
				struct multiplayer_information :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					tagRef Flag;
					tagRef Unit;
					Reflexive<vehicles> Vehicles;
					tagRef HillShader;
					tagRef FlagShader;
					tagRef Ball;
					Reflexive<sounds> Sounds;
					tagRef InGameText;
					PAD(88);
				};
				TAG_BLOCK_SIZE_ASSERT(multiplayer_information, 152);
				/****************************************
				* size: 284
				* alignment:    4
				* max_count:    1
				*
				****************************************/
				struct player_information :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					tagRef unused;
					PAD(28);
					float walkingSpeed;
					PAD(4);
					float forwardRunSpeed;
					float backwardRunSpeed;
					float sidewaysRunSpeed;
					float runAcceleration;
					float forwardCrouchSpeed;
					float backwardCrouchSpeed;
					float sidewaysCrouchSpeed;
					float crouchAcceleration;
					float airbornAcceleration;
					PAD(16);
					float grenadeOriginX;
					float grenadeOriginY;
					float grenadeOriginZ;
					PAD(12);
					float stunPenaltyMovement;
					float stunPenaltyTurning;
					float stunPenaltyJumping;
					float minStunTime;
					float maxStunTime;
					PAD(8);
					float fpIdleTimeMin;
					float fpIdleTimeMax;
					float fpSkipFraction;
					PAD(16);
					tagRef CoopRespawnEffect;
					float binocularsZoomCount;
					float binocularsZoomRangeMin;
					float binocularsZoomRangeMax;
					tagRef BinocularsZoomInSound;
					tagRef BinocularsZoomOutSound;
					PAD(16);
					tagRef ActiveCamoOn;
					tagRef ActiveCamoOff;
					tagRef ActiveCamoError;
					tagRef ActiveCamoReady;
					tagRef AlashlightOn;
					tagRef AlashlightOff;
					tagRef IceCream;
				};
				TAG_BLOCK_SIZE_ASSERT(player_information, 284);
				/****************************************
				* size: 188
				* alignment:    4
				* max_count:    4
				*
				****************************************/
				struct player_representation :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					tagRef FirstPersonHands;
					tagRef FirstPersonBody;
					PAD(160);
					tagRef ThirdPersonUnit;
					StringID ThirdPersonUnitVariant;
				};
				TAG_BLOCK_SIZE_ASSERT(player_representation, 188);
				/****************************************
				* size: 104
				* alignment:    4
				* max_count:    1
				*
				****************************************/
				struct falling_damage :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					PAD(8);
					float harmfulFallingDistanceMin;
					float harmfulFallingDistanceMax;
					tagRef FallingDamage;
					PAD(8);
					float maximumFallingDistance;
					tagRef DistanceDamage;
					tagRef VehicleEnvironmentCollisionDamage;
					tagRef VehicleKilledUnitDamageEffect;
					tagRef VehicleCollisionDamage;
					tagRef FlamingDeathDamage;
					PAD(28);
				};
				TAG_BLOCK_SIZE_ASSERT(falling_damage, 104);
				/****************************************
				* size: 36
				* alignment:    4
				* max_count:    33
				*
				****************************************/
				struct old_materials :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					StringID MaterialName;
					StringID GeneralMaterialName;
					float groundFrictionScale;
					float groundFrictionNormalK0Scale;
					float groundFrictionNormalK1Scale;
					float groundDepthScale;
					float groundDampFractionScale;
					tagRef MeleeHitSound;
				};
				TAG_BLOCK_SIZE_ASSERT(old_materials, 36);
				/****************************************
				* size: 180
				* alignment:    4
				* max_count:    256
				*
				****************************************/
				struct materials :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					StringID Name;
					StringID ParentName;
					PAD(2);
					BitField16<Blam::Enums::Tags::TagGroups::matg::MatFlags> MaterialFlags;
					Blam::Enums::Tags::TagGroups::matg::OldMaterialType OldMaterialType;
					PAD(2);
					StringID GeneralArmor;
					StringID SpecificArmor;
					PAD(4);
					float friction;
					float restitution;
					float density;
					tagRef OldMaterialPhysics;
					tagRef breakableSurface;
					tagRef SoundSweetenersmall;
					tagRef SoundSweetenermedium;
					tagRef SoundSweetenerlarge;
					tagRef SoundSweetenerRolling;
					tagRef SoundSweetenerGrinding;
					tagRef SoundSweetenermelee;
					PAD(8);
					tagRef EffectSweetenersmall;
					tagRef EffectSweetenermedium;
					tagRef EffectSweetenerlarge;
					tagRef EffectSweetenerRolling;
					tagRef EffectSweetenerGrinding;
					tagRef EffectSweetenermelee;
					PAD(8);
					BitField32<Blam::Enums::Tags::TagGroups::matg::SweetenerInheritanceFlags> SweetenerInheritanceFlags;
					tagRef MaterialEffects;
				};
				TAG_BLOCK_SIZE_ASSERT(materials, 180);
				struct obsolete_profile_colors :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					Blam::Maths::Real::ColorRGB Color;
				};
				TAG_BLOCK_SIZE_ASSERT(obsolete_profile_colors, 12);
				struct team_colors :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					Blam::Maths::Real::ColorRGB Color;
				};
				TAG_BLOCK_SIZE_ASSERT(team_colors, 12);
				/****************************************
				* size: 32
				* alignment:    4
				* max_count:    1
				*
				****************************************/
				struct multiplayer_ui_obselete :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					tagRef RandomPlayerNames;
					Reflexive<obsolete_profile_colors> ObsPlayerProfilesColors;
					Reflexive<team_colors> teamColors;
					tagRef teamNames;
				};
				TAG_BLOCK_SIZE_ASSERT(multiplayer_ui_obselete, 32);
				/****************************************
				* size: 12
				* alignment:    4
				* max_count:    32
				*
				****************************************/
				struct profile_colors :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					Blam::Maths::Real::ColorRGB Color;
				};
				TAG_BLOCK_SIZE_ASSERT(profile_colors, 12);
				/****************************************
				* size: 264
				* alignment:    4
				* max_count:    20
				*
				****************************************/
				struct campaign_levels :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					int campaignID;
					int mapID;
					String256 ScenarioPath;
				};
				TAG_BLOCK_SIZE_ASSERT(campaign_levels, 264);
				/****************************************
				* size: 8
				* alignment:    4
				* max_count:    1
				*
				****************************************/
				struct runtime_level_data :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					Reflexive<campaign_levels> CampaignLevels;
				};
				TAG_BLOCK_SIZE_ASSERT(runtime_level_data, 8);
				/****************************************
				* size: 2884
				* alignment:    4
				* max_count:    4
				*
				****************************************/
				struct lvl_data_campaigns :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					int campaignID;
					PAD(2880);
				};
				TAG_BLOCK_SIZE_ASSERT(lvl_data_campaigns, 2884);
				/****************************************
				* size: 2896
				* alignment:    4
				* max_count:    20
				*
				****************************************/
				struct lvl_data_campaign_levels :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					int campaignID;
					int mapID;
					tagRef PreviewImage;
					Unicode32 EnglishName;
					Unicode32 JapaneseName;
					Unicode32 GermanName;
					Unicode32 FrenchName;
					Unicode32 SpanishName;
					Unicode32 ItalianName;
					Unicode32 KoreanName;
					Unicode32 ChineseName;
					Unicode32 PortugueseName;
					Unicode128 EnglishDescription;
					Unicode128 JapaneseDescription;
					Unicode128 GermanDescription;
					Unicode128 FrenchDescription;
					Unicode128 SpanishDescription;
					Unicode128 ItalianDescription;
					Unicode128 KoreanDescription;
					Unicode128 ChineseDescription;
					Unicode128 PortugueseDescription;
				};

				TAG_BLOCK_SIZE_ASSERT(lvl_data_campaign_levels, 2896);
				/****************************************
				* size: 3172
				* alignment:    4
				* max_count:    50
				*
				****************************************/
				struct lvl_data_multiplayer :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					int mapID;
					tagRef PreviewImage;
					Unicode32 EnglishName;
					Unicode32 JapaneseName;
					Unicode32 GermanName;
					Unicode32 FrenchName;
					Unicode32 SpanishName;
					Unicode32 ItalianName;
					Unicode32 KoreanName;
					Unicode32 ChineseName;
					Unicode32 PortugueseName;
					Unicode128 EnglishDescription;
					Unicode128 JapaneseDescription;
					Unicode128 GermanDescription;
					Unicode128 FrenchDescription;
					Unicode128 SpanishDescription;
					Unicode128 ItalianDescription;
					Unicode128 KoreanDescription;
					Unicode128 ChineseDescription;
					Unicode128 PortugueseDescription;
					String256 ScenarioPath;
					int sortOrder;
					Blam::Enums::Tags::TagGroups::matg::lvlFlags Lvlflags;
					PAD(3);
					BYTE maxTeamsNone;
					BYTE maxTeamsCTF;
					BYTE maxTeamsSlayer;
					BYTE maxTeamsOddball;
					BYTE maxTeamsKOTH;
					BYTE maxTeamsRace;
					BYTE maxTeamsHeadhunter;
					BYTE maxTeamsJuggernaught;
					BYTE maxTeamsTerritories;
					BYTE maxTeamsAssault;
					BYTE maxTeamsStub10;
					BYTE maxTeamsStub11;
					BYTE maxTeamsStub12;
					BYTE maxTeamsStub13;
					BYTE maxTeamsStub14;
					BYTE maxTeamsStub15;
				};
				TAG_BLOCK_SIZE_ASSERT(lvl_data_multiplayer, 3172);
				/****************************************
				* size: 24
				* alignment:    4
				* max_count:    1
				*
				****************************************/
				struct ui_level_data :Blam::Cache::TagBlock<Blam::Enums::Tags::TagGroupTypes::globals>
				{
					Reflexive<lvl_data_campaigns> Campaigns;
					Reflexive<lvl_data_campaign_levels>  CampaignLevels;
					Reflexive<lvl_data_multiplayer>  Multiplayer;
				};
				TAG_BLOCK_SIZE_ASSERT(ui_level_data, 24);
			}
		}
	}


}
#pragma pack(pop)
#endif

