#pragma once
#include "stdafx.h"
#include "..\Blam\Shared\SharedDefinitions.h"
#include "..\Blam\Enums\Enums.h"

using namespace Blam::SharedDefinitions;
namespace Blam
{
	namespace EngineDefinitions
	{
		namespace Objects
		{

			//size : depends on the object, there are object definitions for bipeds, projectiles etc...
			// for example, the size of the biped object definition is 1152 bytes and for the projectiles is 428 bytes
			struct BipedObjectDefinition//To Do
			{
				datum TagDefinitionIndex;//0
				DWORD ObjectFlags;//4
				DWORD unk_0;//8
				datum NextIndex;//0xC
				datum CurrentWeaponDatum;//0x10
				datum ParentIndex;//0x14
				WORD UnitInVehicleFlag;//0x18
				INT16 PlacementIndex ;//0x1A
				DWORD unk_2[3];//0x1C
				DWORD Location[2];//0x28
				Blam::Maths::Real::Point3D Center;//0x30
				FLOAT Radius;//0x3C
				DWORD unk_3[9];//0x40
				Blam::Maths::Real::Point3D Placement;//0x64
				Blam::Maths::Real::Vector3D Orientation;//0x70
				Blam::Maths::Real::Vector3D Up;//0x7C
				Blam::Maths::Real::Point3D TranslationalVelocity;//0x88
				Blam::Maths::Real::Vector3D AngularVelocity;//0x94
				FLOAT Scale;//0xA0
				BYTE unk_4[6];//0xA4
				Blam::Enums::Objects::ObjectType ObjectType;//0xAA;
				BYTE unk;//0xAB
				INT16 NameListIndex;//0xAC
				BYTE unk_5;//0xAE
				BYTE NetgameEquipmentIndex;//0xAF
				DWORD unk_6;//0xB0
				datum HavokComponentDatum;//0xB4
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
				BYTE CollisionFlags;//0x10A - flags in general not just collision, it's how the player is killed for instance.
				BYTE HealthFlags;//0x10B
				DWORD unk_9[3];//0x10C
				WORD UnkFlags;//0x118
				WORD UnkFlags2;//0x11A
				BYTE unk_10[14];//0x11C
				WORD AnimationUnk;//0x12A
				BYTE ObjectsAttach;//0x12C
				BYTE unk_11[3];//0x12D
				datum ActorDatum; // 0x130
				BYTE unk_17[4]; //0x138
				DWORD Flags;//0x138
				Blam::Enums::Player::Team Team;//0x13C
				WORD unk_12;//0x13D
				datum PlayerDatum;//0x140
				BYTE unk_13[9];//0x144
				WORD CrouchJumpRelated;//0x150
				FLOAT UnitShoot;//0x152
				BYTE unk_14[102];//0x156
				Blam::Maths::RealMovement UnitMovement;//0x1BC
				BYTE unk_15[16];//0x1C4
				datum TargetObject;//0x1D4
				BYTE unk_16[82];//0x1D8
				Blam::Enums::Objects::Unit::WeaponIndex UnitSwitchWeapon;//0x22A
				datum PrimaryWeapon; // 0x22C
				datum SecondaryWeapon; // 0x230
				datum DualWieldWeapon; // 0x234
				BYTE pad[0x18]; //  0x238 
				
				BYTE CurrentGrenadesIndex; //0x250
				BYTE CurrentGrenadesIndex2; //0x251
				BYTE Frag_Grenades; //0x252
				BYTE Plasma_Grenades; //0x253

				FLOAT ActiveCamoFlagePower;//0x2C4
				FLOAT ActiveCamoFlageTimer;//0x2C8
				FLOAT ActiveCamoFlageDepletionPower;//0x2CC

				Blam::Enums::Objects::Unit::UnitState UnitState;//0x3F4
				BYTE unk_18[0x21C];
			};
			static_assert(sizeof(BipedObjectDefinition) == 0x480, "Invalid BipedObjectDefinition size");

			struct WeaponObjectDefinition
			{
				datum TagDefinitionIndex;//0
				char unk[0x258];
			};
			static_assert(sizeof(WeaponObjectDefinition) == 0x25C, "Invalid WeaponObjectDefinition size");

			struct ObjectHeader {
				__int16 datum_salt; //0x00
				BYTE flags; // 0x02
				Blam::Enums::Objects::ObjectType type; // 0x03
				__int16 unk__;  // 0x04
				__int16 unk_size;  //0x06
				char* object; //0x08 - 
			};
			static_assert(sizeof(ObjectHeader) == 0xC, "Invalid GameStateObjectHeader size");
		}
	}
}



