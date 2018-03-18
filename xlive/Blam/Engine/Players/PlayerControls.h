#pragma once
#include "..\stdafx.h"
#include "..\Blam\Enums\Enums.h"

typedef DWORD c_object_entity;
namespace Blam 
{
	namespace EngineDefinitions
	{
		namespace Players
		{
			//size  : 0xB0
			struct GameStatePlayerControls//TO DO
			{
				DWORD unk_0[5];
				c_object_entity ControllingObjectDatum;

			};
		}
	}
}
