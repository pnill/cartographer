#pragma once
#include "..\stdafx.h"
namespace Blam
{
	namespace Enums
	{
		namespace Objects
		{
			enum class ObjectType : BYTE
			{
				_object_type_object = -4,
				_object_type_device = -3,
				_object_type_item = -2,
				_object_type_unit = -1,

				biped = 0,
				vehicle,
				weapon,
				equipment,
				garbage,
				projectile,
				scenery,
				machine,
				control,
				light_fixture,
				sound_scenery,
				crate,
				creature,				
			};
		}
	}
}
			