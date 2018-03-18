#pragma once
#include "stdafx.h"
#include "..\Blam\Shared\SharedDefinitions.h"
#include "..\Blam\Enums\Enums.h"
namespace Blam
{
	namespace EngineDefinitions
	{
		namespace Objects
		{
			//size : unknown
			struct ObjectEntityDefinition//To Do
			{
				Blam::SharedDefinitions::DatumIndex DefinitionIndex;//0
				DWORD ObjectFlags;//4
				DWORD unk_0;//8
				Blam::SharedDefinitions::c_object_index NextIndex;//0xC
				Blam::SharedDefinitions::c_object_index CurrentWeaponDatum;//0x10
				Blam::SharedDefinitions::c_object_index ParentIndex;//0x14
				WORD UnitInVehicleFlag;//0x18
				INT16 PlacementIndex ;//0x1A
				DWORD unk_2[3];//0x1C
				DWORD Location[2];//0x28
				Blam::Maths::RealPoint3D Center;//0x30
				FLOAT Radius;//0x3C
				DWORD unk_3[9];//0x40
				Blam::Maths::RealPoint3D Placement;//0x64
				Blam::Maths::RealVector3D Orientation;//0x70
				Blam::Maths::RealVector3D Up;//0x7C
				Blam::Maths::RealPoint3D TranslationalVelocity;//0x88
				Blam::Maths::RealVector3D AngularVelocity;//0x94
				FLOAT Scale;//0xA0
				BYTE unk_4[6];//0xA4
				Blam::Enums::Objects::ObjectType ObjectType;//0xAA;
				BYTE unk;//0xAB
				INT16 NameListIndex;//0xAC
				BYTE unk_5;//0xAE
				BYTE NetgameEquipmentIndex;//0xAF
				DWORD unk_6;//0xB0
				Blam::SharedDefinitions::c_object_index HavokComponentDatum;//0xB4
				DWORD unk_7[11];//0xB8
				FLOAT BodyMaxVitality;//0xE4
				FLOAT ShieldMaxVitality;//0xE8
				FLOAT BodyCurrentVitality;//0xEC
				FLOAT ShieldCurrentVitality;//0xF0
				DWORD ShieldEffects;//0xF4
				DWORD ShieldEffects2;//0xF8
				DWORD ShieldEffects3;//0xFC
				DWORD unk_8;//0x100
				FLOAT ShieldsRechargeTimer;//0x104
				WORD ShieldStun2;//0x108
				BYTE CollisionFlags;//0x10A
				BYTE HealthFlags;//0x10B
				DWORD unk_9[3];//0x10C
				WORD UnkFlags;//0x118
				WORD UnkFlags2;//0x11A
				BYTE unk_10[14];//0x11C
				WORD AnimationUnk;//0x12A
				BYTE ObjectsAttach;//0x12C
				BYTE unk_11[11];//0x12D
				DWORD Flags;//0x138
				Blam::Enums::Player::Team Team;//0x13C
				WORD unk_12;//0x13D
				BYTE PlayerIndex;//0x140
				BYTE unk_13[12];//0x144
				WORD CrouchJumpRelated;//0x150
				FLOAT UnitShoot;//0x152
				BYTE unk_14[102];//0x156
				Blam::Maths::RealMovement UnitMovement;//0x1BC
				BYTE unk_15[16];//0x1C4
				Blam::SharedDefinitions::c_object_index TargetObject;//0x1D4
				BYTE unk_16[82];//0x1D8
				Blam::Enums::Objects::Unit::WeaponIndex UnitSwitchWeapon;//0x22A
				Blam::SharedDefinitions::c_object_index Blam::Enums::Objects::Unit::UnitWeapons::PrimaryWeapon;//0x22C
				Blam::SharedDefinitions::c_object_index Blam::Enums::Objects::Unit::UnitWeapons::SecondaryWeapon;//0x230
				Blam::SharedDefinitions::c_object_index Blam::Enums::Objects::Unit::UnitWeapons::DualWeildWeapon;//0x234
				Blam::Enums::Objects::Unit::Grenades CurrentGrenadesIndex;//0x250
				Blam::Enums::Objects::Unit::Grenades CurrentGrenadesIndex2;//0x251
				Blam::Enums::Objects::Unit::Grenades Blam::Enums::Objects::Unit::Grenades::Fragmentation;//0x252
				Blam::Enums::Objects::Unit::Grenades Blam::Enums::Objects::Unit::Grenades::Plasma;//0x253

				FLOAT ActiveCamoFlagePower;//0x2C4
				FLOAT ActiveCamoFlageTimer;//0x2C8
				FLOAT ActiveCamoFlageDepletionPower;//0x2CC

				Blam::Enums::Objects::Unit::UnitState UnitState;//0x3F4
		
				
			};
		}
	}
}



