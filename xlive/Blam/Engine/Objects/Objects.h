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
			struct ObjectEntityDefinition//To Do
			{
				Blam::SharedDefinitions::DatumIndex DatumIndex;//0
				DWORD ObjectFlags;//4
				DWORD unk_0;//8
				Blam::SharedDefinitions::c_object_index VehicleSeat1;//0xC
				Blam::SharedDefinitions::c_object_index CurrentWeaponDatum;//0x10
				Blam::SharedDefinitions::c_object_index VehicleSeat2;//0x14
				WORD UnitInVehicleFlag;//0x18
				WORD unk_1;				//0x1A
				DWORD unk_2;//0x1C
			};
		}
	}
}
