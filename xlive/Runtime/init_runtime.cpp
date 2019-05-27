#include "init_runtime.h"
#include "RuntimeIncludes.h"
#include "..\H2MOD.h"

Blam::Cache::cache_header* Runtime::Cache::CacheHeader;

namespace Runtime
{
	void Init()
	{
		Runtime::Cache::CacheHeader = (Blam::Cache::cache_header*)(h2mod->GetAddress(0x47CD68));
	}
}
