#pragma once
#include "Blam\Cache\DataTypes\Bitfield.h"
using namespace Blam::Cache::DataTypes;
namespace Blam
{
	namespace Enums
	{
		namespace Tags
		{
			namespace TagGroups
			{
				namespace matg
				{
					enum class Language : __int32
					{
						English = 0x0,
						Japanese = 0x1,
						German = 0x2,
						French = 0x3,
						Spanish = 0x4,
						Italian = 0x5,
						Korean = 0x6,
						Chinese = 0x7,
						Portuguese = 0x8,
					};					
					struct ActiveCamoFlags
					{
						Flags TintEdgeDensity = 0;
					};
					enum class OldMaterialType : __int16
					{
						Dirt = 0x0,
						Sand = 0x1,
						Stone = 0x2,
						Snow = 0x3,
						Wood = 0x4,
						Metalhollow = 0x5,
						Metalthin = 0x6,
						Metalthick = 0x7,
						Rubber = 0x8,
						Glass = 0x9,
						ForceField = 0xA,
						Grunt = 0xB,
						HunterArmor = 0xC,
						HunterSkin = 0xD,
						Elite = 0xE,
						Jackal = 0xF,
						JackalEnergyShield = 0x10,
						EngineerSkin = 0x11,
						EngineerForceField = 0x12,
						FloodCombatForm = 0x13,
						FloodCarrierForm = 0x14,
						CyborgArmor = 0x15,
						CyborgEnergyShield = 0x16,
						HumanArmor = 0x17,
						HumanSkin = 0x18,
						Sentinel = 0x19,
						Monitor = 0x1A,
						Plastic = 0x1B,
						Water = 0x1C,
						Leaves = 0x1D,
						EliteEnergyShield = 0x1E,
						Ice = 0x1F,
						HunterShield = 0x20,
					};					
					struct SweetenerInheritanceFlags
					{
						Flags SoundSmall = 0;
						Flags SoundMedium = 1;
						Flags SoundLarge = 2;
						Flags SoundRolling = 3;
						Flags SoundGrinding = 4;
						Flags SoundMelee = 5;
						Flags EffectSmall = 6;
						Flags EffectMedium = 7;
						Flags EffectLarge = 8;
						Flags EffectRolling = 9;
						Flags EffectGrinding = 10;
						Flags EffectMelee = 11;
					};
					struct MatFlags
					{
						Flags Flammable = 0;
						Flags BioMass = 1;
					};					
					struct lvlFlags
					{
						Flags FUnlockable = 0;
						
					};
				}
			}
		}
	}
}
