#include "global_tags_interface.h"

#include "..\H2MOD.h"
#include "..\Blam\Cache\Tags\tag_definitons.h"
#include "..\Blam\Enums\Tags\TagGroups.h"
#include "TagInterface.h"
#include "..\Util\Hooks\Hook.h"

using Blam::Enums::Tags::TagGroupTypes;

void _cdecl ResetMapData();

namespace TagInterface
{
	//var to store the created interfaces
	static map<datum, void*> global_tag_interface_list;

	void* global_tags_interface::GetTagInterface(datum tag, int type)
	{
		if (global_tag_interface_list.find(tag) != global_tag_interface_list.end())
		{
			return global_tag_interface_list[tag];
		}
		auto global_tag_tables = tags::get_tag_instances();

		//tag type verification/protection against accidental type conversion
		if (global_tag_tables[tag.Index].type.as_int() != type)
			return nullptr;

		void* tag_mem_addr = tags::get_tag(tag);

		Open(global_tag_tables[tag.Index].type.as_int(), tag_mem_addr);

		global_tag_interface_list.emplace(tag, tag_mem_addr);
		return tag_mem_addr;
	}
	void global_tags_interface::Release()
	{
		map<datum, void*>::iterator  i = global_tag_interface_list.begin();
		while (i != global_tag_interface_list.end())
		{
			auto global_tag_tables = tags::get_tag_instances();
			Close(global_tag_tables[i->first.Index].type.as_int(), i->second);
			i++;
		}
		global_tag_interface_list.clear();
	}
	void global_tags_interface::Open(__int32 type, void* tag_mem)
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
	void global_tags_interface::Close(__int32 type, void* tag_mem)
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
	void global_tags_interface::Init()
	{
		PatchCall(h2mod->GetAddress(0x27683), (DWORD)ResetMapData);
	}	
}
void _cdecl ResetMapData()
{
	//clear up all the interfaces and properly dispose all the occupied memory
	TagInterface::GlobalTagInterface.Release();

	//and then call unloading procedure
	typedef void(_cdecl ResetMapData)();
	auto pResetMapData = h2mod->GetAddress<ResetMapData*>(0x30CD4);
	pResetMapData();
}