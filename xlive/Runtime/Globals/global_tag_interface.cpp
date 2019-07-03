#include"global_tag_interface.h"
#include"global_ptrs.h"
#include "..\H2MOD.h"
#include"..\Blam\Cache\Tags\tag_definitons.h"
#include"..\Blam\Enums\Tags\TagGroups.h"

void _cdecl ResetMapData();

namespace Runtime
{
	namespace Globals
	{
		//var to store the created interfaces
		static map<DatumIndex, void*> global_tag_interface_list;

		void* global_tag_interface::GetTagInterface(DatumIndex arg0,int type)
		{
			if (global_tag_interface_list.find(arg0)!=global_tag_interface_list.end())
			{
				return global_tag_interface_list[arg0];
			}
			auto global_tag_tables = (Blam::EngineDefinitions::Tag::global_tag_instance*)(*Runtime::Globals::GlobalTagInstances);

			//tag type verification/protection against accidental type conversion
			if (global_tag_tables[arg0.Index].type != type)
				return nullptr;

			void* tag_mem_addr = *(DWORD*)Runtime::Globals::GlobalTagBase+ (char*)global_tag_tables[arg0.Index].offset;
							
			Open(global_tag_tables[arg0.Index].type, tag_mem_addr);

			global_tag_interface_list.emplace(arg0, tag_mem_addr);
			return tag_mem_addr;
		}
		template<typename TagDefinition>
		inline TagDefinition* global_tag_interface::GetTagInterface(DatumIndex arg0,int type)
		{
			return (TagDefinition*)(GetTagInterface(arg0,type));
		}		
		void global_tag_interface::Release()
		{			
			map<DatumIndex, void*>::iterator  i = global_tag_interface_list.begin();
			while (i != global_tag_interface_list.end())
			{
				auto global_tag_tables = (Blam::EngineDefinitions::Tag::global_tag_instance*)(*Runtime::Globals::GlobalTagInstances);
				Close(global_tag_tables[i->first.Index].type, i->second);
				i++;
			}
			global_tag_interface_list.clear();		
		}
		void global_tag_interface::Open(__int32 type, void* tag_mem)
		{
			switch ((TagGroupTypes)type)
			{
			case TagGroupTypes::itemcollection:
			{
				auto tptr = (Blam::Cache::Tags::itmc*)tag_mem;
				tptr->Open();

				break;
			}
			case TagGroupTypes::scenario:
			{
				auto tptr = (Blam::Cache::Tags::scnr*)tag_mem;
				tptr->Open();

				break;
			}
			default: nullptr;
			}
		}
		void global_tag_interface::Close(__int32 type, void* tag_mem)
		{
			switch ((TagGroupTypes)type)
			{
			case TagGroupTypes::itemcollection:
			{
				auto tptr = (Blam::Cache::Tags::itmc*)tag_mem;
				tptr->Close();

				break;
			}
			case TagGroupTypes::scenario:
			{
				auto tptr = (Blam::Cache::Tags::scnr*)tag_mem;
				tptr->Close();

				break;
			}
			default: nullptr;
			}
		}
		void global_tag_interface::Init()
		{
			PatchCall(h2mod->GetBase() + 0x27683, (DWORD)ResetMapData);
		}
	}
}
void _cdecl ResetMapData()
{
	//clear up all the interfaces
	Runtime::Globals::GlobalTagInterface.Release();

	//and then call unloading procedure
	typedef void(_cdecl *ResetMapData)();
	auto pResetMapData = (ResetMapData)((char*)h2mod->GetBase() + 0x30CD4);
	pResetMapData();
}
