#pragma once
#include "Blam\Enums\Enums.h"
#include "Blam\Shared\SharedDefinitions.h"

using namespace Blam::SharedDefinitions;
namespace Blam
{
	namespace EngineDefinitions
	{

		namespace Tag
		{
			struct global_tag_instance
			{
				int type;
				DatumIndex tag_index;
				void *offset;
				int size;
			};
		}
	}
}