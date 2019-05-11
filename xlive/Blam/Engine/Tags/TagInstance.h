#pragma once
#include "Blam\Enums\Enums.h"
#include "Blam\Shared\SharedDefinitions.h"

extern DWORD H2BaseAddr;
extern bool H2IsDediServer;
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

				void* GetTag()
				{
					if (tag_index.Index != -1)
					{
						DWORD mapMemBase = *(DWORD*)((BYTE*)H2BaseAddr + (H2IsDediServer ? 0x4A29BC : 0x47CD54));
						return   (void*)(mapMemBase + (BYTE*)this->offset);
					}
					return (void*)-1;
				}
			};
		}
	}
}