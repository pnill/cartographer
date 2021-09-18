#include  "MapSlots.h"

#include "H2MOD/Tags/MetaLoader/tag_loader.h"
#include "H2MOD.h"
#include "Util/filesys.h"
#include "Util/Hooks/Hook.h"

#include "Blam/Cache/TagGroups/globals_definition.hpp"

namespace MapSlots
{
	const int FIRSTUNUSEDSLOT = 23;
	const int MAXSLOTS = 49;
	const int MULTIPLAYER_SIZE = 3172;
	const int MapIndex = 3000;
	std::vector<std::string> AddedMaps;
	std::vector<s_globals_group_definition::s_ui_level_data_block::s_multiplayer_levels_block> MapData;
	std::map<datum, std::string> BitmapsToLoad;
	void CacheMapData()
	{
		//lots copied over from Tag Loader, using this function to grab the Level data in the scenario tag
		//And using that to construct a new s_multiplayer_levels_block and grab the bitmap datum for tag loading
		std::string game_dir(GetExeDirectoryNarrow());
		std::string def_maps_loc = game_dir + "\\maps";
		LOG_TRACE_GAME("[Map Slots]: Startup - Caching map data");
		for (auto map : AddedMaps)
		{
			std::string map_location = def_maps_loc + "\\" + map;
			if (PathFileExistsA(map_location.c_str()))
			{
				LOG_TRACE_GAME("[Map Slots]: Startup - Caching {}", map);
				std::ifstream* fin = new std::ifstream(map_location.c_str(), std::ios::binary | std::ios::binary);
				if (fin->is_open())
				{
					int table_off, table_size = 0;

					fin->seekg(0x10);
					fin->read((char*)&table_off, 4);
					fin->read((char*)&table_size, 4);


					fin->seekg(table_off + 4);
					int temp;
					fin->read((char*)&temp, 4);

					int table_start = table_off + 0xC * temp + 0x20;
					int scnr_off = table_off + table_size;

					int scnr_memaddr;
					fin->seekg(table_start + 0x8);
					fin->read((char*)&scnr_memaddr, 4);

					//Get the tag block info for the Sencario Level Data
					int l_count, l_offset;
					fin->seekg(scnr_off + 0x398);
					fin->read((char*)&l_count, 4);
					fin->read((char*)&l_offset, 4);

					//Get the tag block info for the Multiplayer Level Description
					int m_count, m_offset;
					temp = scnr_off + (l_offset - scnr_memaddr);
					fin->seekg(temp + 0x10);
					fin->read((char*)&m_count, 4);
					fin->read((char*)&m_offset, 4);

					temp = scnr_off + (m_offset - scnr_memaddr);
					s_globals_group_definition::s_ui_level_data_block::s_multiplayer_levels_block newBlock;

					//The struct in the scenario is the same as the one inside the globals ui level data so just copy it into one
					fin->seekg(temp);
					fin->read((char*)&newBlock, sizeof(s_globals_group_definition::s_ui_level_data_block::s_multiplayer_levels_block));

					//Fix incase the maps level data is incorrectly setup
					if (strlen(newBlock.path.Text) == 0) {
						fin->seekg(0x1C8);
						char* buffer = new char[128];
						fin->read(buffer, 128);
						newBlock.path = buffer;
					}

					MapData.push_back(newBlock);

					//Store the bitmap datum for loading it into the main menu
					BitmapsToLoad.emplace(newBlock.bitmap.TagIndex, map);
					fin->close();
				}
			}
			else
			{
				LOG_TRACE_GAME("[Map Slots]: Startup - Map File Missing {}", map);
			}
		}
	}
	void OnMapLoad()
	{
		if (h2mod->GetEngineType() == MainMenu)
		{
			if (!AddedMaps.empty())
			{
				//Load all the added maps bitmaps
				LOG_TRACE_GAME("[Map Slots]: OnMapLoad - Tag Loading Bitmaps");
				for (auto item : BitmapsToLoad)
				{

					tag_loader::Load_tag(item.first, false, item.second);
				}
				tag_loader::Push_Back();

				//Grab the globals tag
				auto matg_datum = tags::find_tag(blam_tag::tag_group_type::globals, "globals\\globals");
				BYTE* matg_data = tags::get_tag<blam_tag::tag_group_type::globals, BYTE>(matg_datum);

				if (matg_data != nullptr)
				{
					//Grab the tag block for UI Level Data
					auto* p_ui_levels = reinterpret_cast<tags::tag_data_block*>(matg_data + 0x178);
					if (p_ui_levels->block_count > 0 && p_ui_levels->block_data_offset != -1)
					{
						auto ui_levels = tags::get_tag_data() + p_ui_levels->block_data_offset;
						//Grab the tag block for Multiplayer Levels
						auto* p_mul_levels = reinterpret_cast<tags::tag_data_block*>(ui_levels + 0x10);
						if (p_mul_levels->block_count > 0 && p_mul_levels->block_data_offset != -1)
						{
							auto mul_levels = tags::get_tag_data() + p_mul_levels->block_data_offset;

							int i = 0;
							for (auto newSlot : MapData)
							{
								if (i + FIRSTUNUSEDSLOT < MAXSLOTS) {
									LOG_TRACE_GAME(L"[Map Slots]: OnMapLoad Adding {}", newSlot.english_name.Text);
									auto slot = reinterpret_cast<s_globals_group_definition::s_ui_level_data_block::s_multiplayer_levels_block*>(mul_levels + (MULTIPLAYER_SIZE * (FIRSTUNUSEDSLOT + i)));

									//Write the data loaded from the maps into the unused slot
									*slot = newSlot;
									//Resolve the loaded bitmap datum
									slot->bitmap.TagIndex = tag_loader::ResolveNewDatum(newSlot.bitmap.TagIndex);
									//Change the map id and sort ID so that the maps are 
									//placed in order at the end of the list
									slot->map_id = MapIndex + i;
									slot->sort_order = MapIndex + i;
									i++;
								}
								else
								{
									LOG_ERROR_GAME("[Map Slots]: Max Multiplayer added slots reached");
									break;
								}
							}

							/*slot->map_id = 1234;
							slot->bitmap.TagIndex = tag_loader::ResolveNewDatum(0xE81E010A);
							const wchar_t* temp_name = L"Tombstone";
							slot->english_name = temp_name;
							const wchar_t* temp_desc = L"The UNSC decommissioned this munitions testing complex after safety concerns came to light.";
							slot->english_description = temp_desc;
							slot->path = "scenarios\\multi\\halo\\highplains\\highplains";
							slot->sort_order = 100;*/
						}
					}
				}
			}
		}
	}

	//H2Server reads the level data from mainmenu.map
	/*typedef bool(__cdecl p_store_multiplayer_level_data)();
	p_store_multiplayer_level_data* c_store_multiplayer_level_data;*/

	typedef int(__cdecl p_sub_map_slot)(int a1);
	p_sub_map_slot* sub_map_slot;

	const int StaticLevelData = 0x419510;
	int* MapSlotCount;
	int __cdecl store_multiplayer_level_data(int a1)
		//bool __cdecl store_multiplayer_level_data()
	{
		//auto result = c_store_multiplayer_level_data();
		//auto result = sub_map_slot(a1);
		//if(result)
		//{
		int i = 0;
		for (auto newSlot : MapData)
		{
			if (i + FIRSTUNUSEDSLOT < MAXSLOTS) {
				LOG_TRACE_GAME(L"[Map Slots]: store_mutliplayer_level_data Adding {}", newSlot.english_name.Text);
				auto slotAddr = Memory::GetAddress(0, StaticLevelData) + (MULTIPLAYER_SIZE * (FIRSTUNUSEDSLOT + i));
				DWORD dwBack[2];
				VirtualProtect(reinterpret_cast<LPVOID>(slotAddr), 3172, PAGE_EXECUTE_READWRITE, &dwBack[0]);
				auto slot = reinterpret_cast<s_globals_group_definition::s_ui_level_data_block::s_multiplayer_levels_block*>(slotAddr);

				//Write the data loaded from the maps into the unused slot
				*slot = newSlot;

				//Change the map id and sort ID so that the maps are 
				//placed in order at the end of the list
				slot->map_id = MapIndex + i;
				slot->sort_order = MapIndex + i;
				VirtualProtect(reinterpret_cast<LPVOID>(slotAddr), 3172, dwBack[0], &dwBack[1]);
				i++;

			}
			else
			{
				LOG_ERROR_GAME("[Map Slots]: Max Multiplayer added slots reached");
				break;
			}
		}
		//}
		*MapSlotCount = *MapSlotCount + i;
		return sub_map_slot(a1);
	}

	void ApplyHooks()
	{
		MapSlotCount = Memory::GetAddress<int*>(0, 0x41950C);
		//c_store_multiplayer_level_data = Memory::GetAddress<p_store_multiplayer_level_data*>(0, 0x6A22);
		sub_map_slot = Memory::GetAddress<p_sub_map_slot*>(0, 0x3C8C3);
		//PatchCall(Memory::GetAddress(0, 0xBBAE), store_multiplayer_level_data);
		PatchCall(Memory::GetAddress(0, 0x6ACC), store_multiplayer_level_data);
	}

	void Initialize()
	{
		AddedMaps.emplace_back("highplains.map");
		//AddedMaps.emplace_back("salvation.map");
		CacheMapData();

		if (!Memory::isDedicatedServer())
			tags::on_map_load(OnMapLoad);
		else
			ApplyHooks();
	}
}