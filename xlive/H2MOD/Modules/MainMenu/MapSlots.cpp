#include "stdafx.h"
#include "MapSlots.h"

#include "game/game_globals.h"
#include "main/level_definitions.h"

#include "tag_files/tag_loader/tag_injection.h"
#include "Util/filesys.h"


// TODO Move to Blam/Engine/main/levels once the below are complete
// TODO remove std:: references used in here
// TODO Cleanup this code
// TODO Add support for singleplayer maps in the future

#define k_multiplayer_first_unused_slot 23
#define k_max_map_slots 49
#define k_starting_map_index 3000

namespace MapSlots
{
	std::vector<std::wstring> AddedMaps;
	std::vector<s_multiplayer_ui_level_definition> MapData;
	std::map<datum, std::wstring> BitmapsToLoad;

	void CacheMapData(void)
	{
		//lots copied over from Tag Loader, using this function to grab the Level data in the scenario tag
		//And using that to construct a new s_multiplayer_levels_block and grab the bitmap datum for tag loading
		std::wstring def_maps_loc = GetExeDirectoryWide() + L"\\maps";
		LOG_TRACE_GAME("[Map Slots]: Startup - Caching map data");
		for (const auto& map : AddedMaps)
		{
			std::wstring map_location = def_maps_loc + L"\\" + map + L".map";
			if (std::filesystem::exists(map_location))
			{
				LOG_TRACE_GAME(L"[Map Slots]: Startup - Caching {}", map);
				std::ifstream fin(map_location.c_str(), std::ios::binary);
				if (fin.is_open())
				{
					int table_off, table_size = 0;

					fin.seekg(0x10);
					fin.read((char*)&table_off, 4);
					fin.read((char*)&table_size, 4);

					fin.seekg(table_off + 4);
					int temp;
					fin.read((char*)&temp, 4);

					int table_start = table_off + 0xC * temp + 0x20;
					int scnr_off = table_off + table_size;

					int scnr_memaddr;
					fin.seekg(table_start + 0x8);
					fin.read((char*)&scnr_memaddr, 4);

					//Get the tag block info for the Sencario Level Data
					int l_count, l_offset;
					fin.seekg(scnr_off + 0x398);
					fin.read((char*)&l_count, 4);
					fin.read((char*)&l_offset, 4);

					//Get the tag block info for the Multiplayer Level Description
					int m_count, m_offset;
					temp = scnr_off + (l_offset - scnr_memaddr);
					fin.seekg(temp + 0x10);
					fin.read((char*)&m_count, 4);
					fin.read((char*)&m_offset, 4);

					temp = scnr_off + (m_offset - scnr_memaddr);
					s_multiplayer_ui_level_definition newBlock;

					//The struct in the scenario is the same as the one inside the globals ui level data so just copy it into one
					fin.seekg(temp);
					fin.read((char*)&newBlock, sizeof(s_multiplayer_ui_level_definition));

					//Fix incase the maps level data is incorrectly setup
					if (strlen(newBlock.path) == 0) {
						fin.seekg(0x1C8);
						fin.read(newBlock.path, NUMBEROF(s_multiplayer_ui_level_definition::path));
					}

					MapData.emplace_back(newBlock);

					//Store the bitmap datum for loading it into the main menu
					BitmapsToLoad.emplace(newBlock.bitmap.index, map);
					fin.close();
				}
			}
			else
			{
				LOG_TRACE_GAME(L"[Map Slots]: Startup - Map File Missing {}", map);
			}
		}

		return;
	}

	void add_new_multiplayer_map_slots_game(void)
	{
		s_ui_levels_definition* ui_levels = game_globals_get_ui_levels();
		
		if (ui_levels)
		{
			int32 added_maps = 0;
			for (const s_multiplayer_ui_level_definition& newSlot : MapData)
			{
				if (added_maps + k_multiplayer_first_unused_slot < k_max_map_slots)
				{
					LOG_TRACE_FUNCW(L"Adding {}", newSlot.level_descriptions.name[_language_english]);
					s_multiplayer_ui_level_definition* slot = ui_levels->multiplayer_levels[added_maps + k_multiplayer_first_unused_slot];

					//Write the data loaded from the maps into the unused slot
					memcpy(slot, &newSlot, sizeof(newSlot));
					//Resolve the loaded bitmap datum
					slot->bitmap.index = tag_injection_resolve_cache_datum(newSlot.bitmap.index);

					//Change the map id and sort ID so that the maps are 
					//placed in order at the end of the list
					int32 new_map_id = k_starting_map_index + added_maps;
					slot->map_id = new_map_id;
					slot->sort_order = new_map_id;
					added_maps++;
				}
				else
				{
					LOG_TRACE_FUNC("Max Multiplayer added slots reached");
					break;
				}
			}
		}

		return;
	}

	int32 add_new_multiplayer_map_slots_server(void)
	{
		int32 added_map_count = 0;
		s_multiplayer_ui_level_definition* multiplayer_levels = Memory::GetAddress<s_multiplayer_ui_level_definition*>(0, 0x419510);
		for (const s_multiplayer_ui_level_definition& newSlot : MapData)
		{
			if (k_multiplayer_first_unused_slot + added_map_count < k_max_map_slots)
			{
				s_multiplayer_ui_level_definition* slot = &multiplayer_levels[k_multiplayer_first_unused_slot + added_map_count];
				LOG_TRACE_FUNCW("Adding {}", newSlot.level_descriptions.name[_language_english]);
				DWORD dwBack[2];
				VirtualProtect(slot, sizeof(s_multiplayer_ui_level_definition), PAGE_EXECUTE_READWRITE, &dwBack[0]);

				//Write the data loaded from the maps into the unused slot
				memcpy(slot, &newSlot, sizeof(s_multiplayer_ui_level_definition));

				//Change the map id and sort ID so that the maps are 
				//placed in order at the end of the list
				int32 new_map_id = k_starting_map_index + added_map_count;
				slot->map_id = new_map_id;
				slot->sort_order = new_map_id;
				VirtualProtect(slot, sizeof(s_multiplayer_ui_level_definition), dwBack[0], &dwBack[1]);
				added_map_count++;
			}
			else
			{
				LOG_TRACE_FUNC("Max Multiplayer added slots reached");
				break;
			}
		}

		return added_map_count;
	}
	
	//H2Server reads the level data from mainmenu.map
	/*typedef bool(__cdecl p_store_multiplayer_level_data)();
	p_store_multiplayer_level_data* c_store_multiplayer_level_data;*/

	typedef int32(__cdecl sub_map_slot_t)(int32 a1);
	sub_map_slot_t* p_sub_map_slot;

	int32* MapSlotCount;
	int32 __cdecl store_multiplayer_level_data(int32 a1)
	{
		*MapSlotCount += add_new_multiplayer_map_slots_server();

		return p_sub_map_slot(a1);
	}

	void map_slots_apply_dedi_hooks(void)
	{
		MapSlotCount = Memory::GetAddress<int32*>(0, 0x41950C);
		//c_store_multiplayer_level_data = Memory::GetAddress<p_store_multiplayer_level_data*>(0, 0x6A22);
		p_sub_map_slot = Memory::GetAddress<sub_map_slot_t*>(0, 0x3C8C3);
		//PatchCall(Memory::GetAddress(0, 0xBBAE), store_multiplayer_level_data);
		PatchCall(Memory::GetAddress(0, 0x6ACC), store_multiplayer_level_data);
		return;
	}

	void OnMapLoad(void)
	{
		//if (!AddedMaps.empty())
		//{
		//	////Load all the added maps bitmaps
		//	LOG_TRACE_GAME("[Map Slots]: OnMapLoad - Tag Loading Bitmaps");
		//	for (const auto& item : BitmapsToLoad)
		//	{
		//		tag_injection_set_active_map(item.second.c_str());
		//		tag_injection_load(_tag_group_bitmap, item.first, false);
		//		tag_injection_inject();
		//	}

		//	add_new_multiplayer_map_slots_game();
		//}

		return;
	}

	void Initialize(void)
	{
		AddedMaps.emplace_back(L"highplains");
		AddedMaps.emplace_back(L"derelict");
		CacheMapData();

		if (Memory::IsDedicatedServer())
		{
			map_slots_apply_dedi_hooks();
		}

		return;
	}
}