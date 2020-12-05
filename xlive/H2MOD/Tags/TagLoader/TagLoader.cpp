#include "TagLoader.h"

constexpr int _INJECTED_TAG_START_ = 0x3BA4;
constexpr int _MAX_ADDITIONAL_TAG_SIZE_ = 0x1400000; //20 MB
constexpr int _MAX_TAG_TABLE_SIZE_ = 65535;

namespace TagLoader
{
	namespace
	{
		std::wstring mapsPath;
		int originalMetaSize; //def_meta_size assuming it's original meta size
		int newMetaSize;
		unsigned int nextDatumIndex = _INJECTED_TAG_START_;
		s_tag_instance* movedTagTable;

		std::vector<s_slim_tag_block> scenaryTagBlocks()
		{
			//std::map<int, std::map<int, void*>> base;
			//std::map<int, std::map<int, void*>> child;
			std::vector<s_slim_tag_block> base;
			std::vector<s_slim_tag_block> child;
			//s_ai_properties_block
			base.push_back({ 0x5C, 0x10 });
			//s_functions_block
			base.push_back({ 0x64, 0x20 });
			//s_attachments_block
			base.push_back({ 0x94, 0x18 });
			//s_widgets_block
			base.push_back({ 0x9C, 0x8 });
			//s_old_functions_block
			base.push_back({ 0xA4, 0x50 });
			//s_change_colors_block::s_initial_permutations_block
			child.push_back({ 0x0, 0x20 });
			//s_change_colors_block::s_functions_block
			child.push_back({ 0x8, 0x28 });
			//s_change_colors_block
			base.push_back({ 0xAC, 0x10, child });
			//s_predicted_resources_block
			base.push_back({ 0xB4, 0x8 });

			return base;
		}
		//TODO: Refactor into a more accessable area
		s_cache_header *getCacheHeader()
		{
			return h2mod->GetAddress<s_cache_header*>(0x47CD68, 0x4A29D0);
		}

		unsigned int __cdecl datum_header_allocate(int size, char a2)
		{
			typedef unsigned int(_cdecl *Allocate_memory)(int size, char arg_4);
			Allocate_memory pAllocate_memory;
			pAllocate_memory = h2mod->GetAddress<Allocate_memory>(0x37E69);
			//i need to allocate more space
			int new_size = size + _MAX_ADDITIONAL_TAG_SIZE_;
			originalMetaSize = size + 0x20;//spacing
			return pAllocate_memory(new_size, a2);
		}
		bool __cdecl map_verify_and_load(int a1)
		{
			typedef bool(_cdecl *LoadTagsandSetMapBases)(int a);
			LoadTagsandSetMapBases pLoadTagsandSetMapBases;
			pLoadTagsandSetMapBases = h2mod->GetAddress<LoadTagsandSetMapBases>(0x31348);
			bool result = pLoadTagsandSetMapBases(a1);

			// reset starting_datum index
			newMetaSize = 0;
			nextDatumIndex = _INJECTED_TAG_START_;

			// adding all shared references
			//tag_loader::Add_all_shared_refs();

			// extending tag_tables and loading tag for all mutiplayer maps and mainmenu map
			if (tags::get_cache_header()->type != scnr_type::SinglePlayerShared)
			{
				DWORD *TagTableStart = h2mod->GetAddress<DWORD*>(0x47CD50);
				///---------------TABLE EXTENSION  STUFF
				//TODO: Clean this up? it also copies tag data
				memcpy((BYTE*)movedTagTable, (BYTE*)*TagTableStart, 0x3BA40);
				*TagTableStart = (DWORD)movedTagTable;
			}
			return result;
		}
	}
	bool LoadTags(std::vector<datum> tags, bool recursive, std::wstring map)
	{
		std::ifstream* fileStream;
		std::wstring mapLocation = mapsPath;
		mapLocation += map;
		if(!PathFileExistsW(mapLocation.c_str()))
		{
			LOG_ERROR_GAME(L"[TagLoader] Could not find map file - {}", map);
			return false;
		}
		fileStream = new std::ifstream(mapLocation.c_str(), std::ios::binary | std::ios::in);
		if(fileStream->is_open())
		{
			LOG_TRACE_GAME(L"[TagLoader] Opened map file - {}", map);
			int tagTableOff = 0;
			int tagTableSize = 0;

			fileStream->seekg(0x10);
			fileStream->read((char*)&tagTableOff, 4);
			fileStream->read((char*)&tagTableSize, 4);
			
			LOG_TRACE_GAME(L"[TagLoader] Tag Table Offset - {} Tag Table Size - {}", tagTableOff, tagTableSize);

			fileStream->seekg(tagTableOff + 4);
			int tagParentCount; //?
			fileStream->read((char*)&tagParentCount, 4);
			
			int tagTableStart = tagTableOff + 0xC * tagParentCount + 0x20;

			LOG_TRACE_GAME(L"[TagLoader] Tag Parent Count? - {} Tag Table Start - {}", tagParentCount, tagTableStart);

			int scenarioOffset = tagTableOff + tagTableSize;
			int scenarioAddress;

			fileStream->seekg(tagTableStart + 0x8);
			fileStream->read((char*)&scenarioAddress, 4);

			LOG_TRACE_GAME(L"[TagLoader] Scenario Offset - {} Scenario Address - {}", scenarioOffset, scenarioAddress);

			s_tag_instance tagInstance;

			std::vector<s_injected_tag> tagList;
			std::vector<s_injected_tag> injectedTags;
			for (const datum &datum : tags) {
				tagList.emplace_back(s_injected_tag{ datum, datum.Null, false });
			}
			
			while(tagList.size())
			{
				s_injected_tag currentInstance = tagList[0];
				datum currentTag = currentInstance.originalDatum;
				//Make sure the entry has a valid-esque datum
				if(currentTag != -1 && currentTag != 0)
				{
					
					fileStream->seekg(tagTableStart + currentTag.ToAbsoluteIndex() * sizeof(s_tag_instance));
					fileStream->read((char*)&tagInstance, sizeof(s_tag_instance));

					if(currentTag != tagInstance.datum_index.ToInt())
					{
						int a = 2312313;
						//LOG_ERROR_GAME("[TagLoader] Wrong tag was found - {} {} {} {}", tagInstance.type.as_string(), tagInstance.datum_index, tagInstance.data_offset, tagInstance.size);
					}
					else
					{
						char* tagData = new char[tagInstance.size];

						int tagDataOffset;
						if (getCacheHeader()->type == SinglePlayerShared || getCacheHeader()->type == MultiplayerShared)
							tagDataOffset = scenarioOffset + (tagInstance.data_offset - 0x3C000);
						else
							tagDataOffset = scenarioOffset + (tagInstance.data_offset - scenarioAddress);

						fileStream->seekg(tagDataOffset);
						fileStream->read(tagData, tagInstance.size);

						tagList[0].newDatum = nextDatumIndex++;
						currentInstance = tagList[0];

						int newTagOffset = originalMetaSize + newMetaSize;


						auto holyHell = scenaryTagBlocks();

						//Right now it's two deep so manually do a second level but refactor.. refactor
						for(auto &block : holyHell)
						{
							LOG_INFO_GAME(L"[TagLoader] Offset {} - Size {}", block.offset, block.size);
							*(int*)(tagData + block.offset + 4) = newTagOffset + (*(int*)(tagData + block.offset + 4) - tagInstance.data_offset);
							if(block.blocks.size())
							{
								for(auto &child : block.blocks)
								{
									*(int*)(tagData + child.offset + 4) = newTagOffset + (*(int*)(tagData + child.offset + 4) - tagInstance.data_offset);
								}
							}
						}

						s_tag_instance *newInstance = &movedTagTable[currentInstance.newDatum.ToInt() && 0xFFFF];
						memcpy(newInstance, &tagInstance, sizeof(s_tag_instance));
						memcpy(tags::get_tag_data() + newTagOffset, tagData, tagInstance.size);

						delete[] tagData;
						newMetaSize += tagInstance.size;
					}
				}
				tagList.erase(tagList.begin());
			}
			fileStream->close();
		}
		delete fileStream;
		return true;
	}

	void PatchCalls()
	{
		//allocating more space for meta loading
		PatchCall(h2mod->GetAddress(0x313B2), datum_header_allocate);
		PatchCall(h2mod->GetAddress(0x3166B), map_verify_and_load);
		//client side desync fix
		///(noping out jump instructions)	
		NopFill(h2mod->GetAddress(0x316CE), 2);
		NopFill(h2mod->GetAddress(0x316DC), 2);
	}

	void Initialize()
	{
		mapsPath = std::wstring(H2ProcessFilePath);
		mapsPath += L"\\mods\\";
		if (CreateDirectory(mapsPath.c_str(), NULL) || ERROR_ALREADY_EXISTS == GetLastError()) {
			PatchCalls();
			movedTagTable = new s_tag_instance[_MAX_TAG_TABLE_SIZE_];
			LOG_TRACE_GAME(L"[TagLoader] Patched calls");
		}
		else
		{
			LOG_ERROR_GAME(L"[TagLoader] failed to create mods folder");
		}
	}
}
