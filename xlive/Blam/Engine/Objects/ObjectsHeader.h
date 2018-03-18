#pragma once
#include "stdafx.h"
#include "..\Blam\Shared\SharedDefinitions.h"
#include "..\Blam\Engine\Objects\Objects.h"

namespace Blam
{
	namespace EngineDefinitions
	{
		namespace Objects
		{	
			//size : 0xC
			struct  GameStateObjectsHeader
			{
				DWORD unk_offset_0;
				DWORD unk_offset_1;
				Blam::EngineDefinitions::Objects::ObjectEntityDefinition& ObjectEntity;
			};
			
		}
	}
	
	
}


