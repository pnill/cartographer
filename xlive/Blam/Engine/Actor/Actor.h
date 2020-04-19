#pragma once
#include "..\Blam\Enums\Enums.h"
#include "..\Blam\Shared\SharedDefinitions.h"

using namespace Blam::SharedDefinitions;
using namespace Blam::Enums;

namespace Blam
{
	namespace EngineDefinitions
	{
		namespace Actors
		{

			/*
			   TODO: Reverse engineer more of the actor struct and fill this data in appropriately.
			   For now, we only know where the character datum which was used to create the actor is.
			*/

			struct Actor {
				BYTE pad[0x54];
				datum character_datum;
				BYTE pad2[0x840];
			};
			static_assert(sizeof(Actor) == 0x898, "Invalid Actor size");
		}
	}
}