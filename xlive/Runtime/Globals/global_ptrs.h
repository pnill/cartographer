#pragma once
#include "..\Blam\Engine\Tags\TagInstance.h"
namespace Runtime
{
	namespace Globals
	{
		extern Blam::EngineDefinitions::Tag::global_tag_instance** GlobalTagInstances;
		extern void** GlobalTagBase;
		extern void** GlobalScnrBase;
	}
}