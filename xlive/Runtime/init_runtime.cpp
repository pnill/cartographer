#include "init_runtime.h"
#include "RuntimeIncludes.h"
#include "..\H2MOD.h"
#include	"Globals\global_tag_interface.h"

Blam::Cache::cache_header* Runtime::Cache::CacheHeader;
Blam::EngineDefinitions::Tag::global_tag_instance** Runtime::Globals::GlobalTagInstances;
void** Runtime::Globals::GlobalTagBase;
void** Runtime::Globals::GlobalScnrBase;

namespace Runtime
{
	void Init()
	{
		Runtime::Cache::CacheHeader = (Blam::Cache::cache_header*)(h2mod->GetAddress(0x47CD68));
		Runtime::Globals::GlobalTagInstances=(Blam::EngineDefinitions::Tag::global_tag_instance**)(h2mod->GetAddress(0x47CD50,0x4A29B8));
		Runtime::Globals::GlobalTagBase = (void**)(h2mod->GetAddress(0x482290));//ETCOFFSET
		Runtime::Globals::GlobalScnrBase = (void**)(h2mod->GetAddress(0x479E74));
		Runtime::Globals::GlobalTagInterface.Init();
	}
}
