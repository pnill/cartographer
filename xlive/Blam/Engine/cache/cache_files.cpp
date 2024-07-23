#include "stdafx.h"
#include "cache_files.h"

#include "cseries/cseries_system_memory.h"
#include "game/game_globals.h"
#include "main/main_game.h"
#include "scenario/scenario.h"
#include "sound/sound_cache_file_definitions.h"
#include "tag_files/tag_loader/tag_injection.h"
#include "tag_files/tag_loader/tag_injection_manager.h"

s_cache_file_memory_globals* cache_file_memory_globals_get()
{
	return Memory::GetAddress<s_cache_file_memory_globals*>(0x47CD60, 0);
}

s_cache_header* cache_files_get_header(void)
{
	return Memory::GetAddress<s_cache_header*>(0x47CD68, 0x4A29D0);
}

s_tags_header* cache_files_get_tags_header(void)
{
	return tags::get_at_tag_data_offset<s_tags_header>(cache_files_get_header()->tag_offset_mask);
}

cache_file_tag_instance* global_tag_instances_get(void)
{
	return *Memory::GetAddress<cache_file_tag_instance**>(0x47CD50, 0x4A29B8);
}

tag_iterator* tag_iterator_new(tag_iterator* itr, tag_group type)
{
	itr->next_tag_index = 0;
	itr->tag_type.group = type.group;
	return itr;
}

datum __cdecl tag_iterator_next(tag_iterator* itr)
{
	return INVOKE(0x3172C, 0x255DC, tag_iterator_next, itr);
}

void cache_file_map_clear_all_failures(void)
{
	typedef void(__cdecl *cache_file_map_clear_all_failures_t)(void);
	auto p_cache_file_map_clear_all_failures = Memory::GetAddress<cache_file_map_clear_all_failures_t>(0x64551, 0x4C5A8);
	p_cache_file_map_clear_all_failures();
	return;
}

void* __cdecl tag_get_fast(datum tag_index)
{
	return INVOKE(0x239623, 0x217295, tag_get_fast, tag_index);
}

void __cdecl cache_file_close()
{
	INVOKE(0x64C37, 0, cache_file_close);
}

bool __cdecl cache_header_verify(s_cache_header* cache_header)
{
	return INVOKE(0x310E9, 0, cache_header_verify, cache_header);
}

uint32 __cdecl cache_file_round_up_read_size(uint32 size)
{
	return INVOKE(0x647DA, 0, cache_file_round_up_read_size, size);
}

bool __cdecl cache_file_blocking_read(uint32 unk, uint32 cache_offset, uint32 read_size, int8* out_buffer)
{
	return INVOKE(0x64D01, 0, cache_file_blocking_read, unk, cache_offset, read_size, out_buffer);
}

void scenario_tags_load_internal_panic()
{
	const s_cache_file_memory_globals* cache_file_memory_globals = cache_file_memory_globals_get();

	if(cache_file_memory_globals->tag_cache_base_address)
	{
		system_heap_free((int*)(cache_file_memory_globals->tag_cache_base_address - *(int*)(cache_file_memory_globals->tag_cache_base_address - 4)));
		cache_file_memory_globals = nullptr;
	}

	cache_file_close();
}

bool __declspec(noinline) cache_validate_shared_tag_instances(int8* tag_data, uint32 offset_mask)
{
	bool result = 0;
	void* cache_validate_shared_tag_instances_usercall = (void*)Memory::GetAddress(0x311AF);
	__asm {
		mov esi, tag_data
		mov edx, offset_mask
		push edx

		call cache_validate_shared_tag_instances_usercall
		mov result, al
		//add esp, 4
	}
	return result;

	//s_tags_header* g_tags_header = *Memory::GetAddress<s_tags_header**>(0x47d568, 0);


}

bool __cdecl read_shared_resource_database(e_shared_resource_database_type database_type, int32 unused_flags, uint32 offset, uint32 size, void* out_buffer, bool unk_bool)
{
	return INVOKE(0x64CC7, 0, read_shared_resource_database, database_type, unused_flags, offset, size, out_buffer, unk_bool);
}

bool scenario_tags_load_process_shared_tags()
{
	s_cache_file_memory_globals* cache_file_memory = cache_file_memory_globals_get();

	// TODO: Find a home for this
	s_tags_header* tag_header = *Memory::GetAddress<s_tags_header**>(0x47d568, 0);
	s_tags_header* unmasked_tag_header = (s_tags_header*)cache_file_memory->tag_cache_base_address;
	s_cache_header* cache_header = cache_files_get_header();
	s_cache_header shared_header;

	csmemset(&shared_header, 0, sizeof(s_cache_header));



	// Read cache header
	if(!read_shared_resource_database(_shared_resource_database_type_multi_player, -1, 0, 2048, &shared_header, false))
		return false;

	const uint32 adjusted_tag_size = cache_file_round_up_read_size(shared_header.tag_size);
	const uint32 adjusted_data_offset = cache_file_round_up_read_size(shared_header.data_offset);
	const uint32 adjusted_data_size = cache_file_round_up_read_size(shared_header.data_size);

	if (adjusted_tag_size > cache_header->tag_offset_mask)
		return false;

	// Read tags header
	if (!read_shared_resource_database(_shared_resource_database_type_multi_player, -1, shared_header.tag_offset, adjusted_data_offset, unmasked_tag_header, false))
		return false;

	if (unmasked_tag_header->tag_count < FIRST_SHARED_TAG_INSTANCE_INDEX)
		return false;

	int8* shared_data_start = (int8*)(cache_file_memory->tag_cache_base_address + adjusted_tag_size - shared_header.data_size);

	// Read tag data
	if (!read_shared_resource_database(_shared_resource_database_type_multi_player, -1, shared_header.data_offset + shared_header.tag_offset, adjusted_data_size, shared_data_start, false))
		return false;
	 
	unmasked_tag_header->parent_info = &unmasked_tag_header[1];
	unmasked_tag_header->tag_instances = (cache_file_tag_instance*)(((char*)&unmasked_tag_header[1]) + 0xC * unmasked_tag_header->tag_parent_info_count);

	if(tag_header->tag_count >= FIRST_SHARED_TAG_INSTANCE_INDEX)
	{
		// Update cache tag_header's instances referencing shared tags
		datum current_shared_datum = FIRST_SHARED_TAG_INSTANCE_INDEX;
		do
		{
			if(unmasked_tag_header->tag_instances[current_shared_datum].tag_index != NONE)
			{
				tag_header->tag_instances[current_shared_datum].data_offset = unmasked_tag_header->tag_instances[current_shared_datum].data_offset;
				tag_header->tag_instances[current_shared_datum].size = unmasked_tag_header->tag_instances[current_shared_datum].size;

				current_shared_datum++;
			}
		} while (current_shared_datum < tag_header->tag_count);
	}

	return true;
}

bool __cdecl scenario_tags_load_internal(const char* scenario_path)
{
	s_cache_header* cache_header = cache_files_get_header();
	s_cache_file_memory_globals* cache_file_memory_globals = cache_file_memory_globals_get();

	const uint32 adj_tag_size = cache_header->tag_size + cache_header->tag_offset_mask;

	if(!cache_header_verify(cache_header) || strlen(cache_header->version_string) > 32 || !cache_header->version_string)
	{
		scenario_tags_load_internal_panic();
		return false;
	}

	cache_header->tag_size = cache_file_round_up_read_size(cache_header->tag_size);

	cache_file_memory_globals->tag_cache_base_address = datum_header_allocate(adj_tag_size + k_injectable_allocation_size, 12);

	if(!cache_file_memory_globals->tag_cache_base_address)
	{
		scenario_tags_load_internal_panic();
		return false;
	}

	csmemset((void*)cache_file_memory_globals->tag_cache_base_address, 0, adj_tag_size);

	set_tag_group_data_info(cache_file_memory_globals->tag_cache_base_address, adj_tag_size);

	int8* memory_tag_header_data_start = (int8*)(cache_file_memory_globals->tag_cache_base_address + cache_header->tag_offset_mask);
	uint32 cache_tag_header_data_size = cache_file_round_up_read_size(cache_header->data_offset);

	int8* memory_tag_data_start = (int8*)(cache_header->tag_size + cache_header->tag_offset_mask + cache_file_memory_globals->tag_cache_base_address - cache_header->data_size);
	uint32 cache_tag_data_size = cache_file_round_up_read_size(cache_header->data_size);

	bool read_success = false;

	// Read tag header and tag data
	read_success = 
		cache_file_blocking_read(-1, cache_header->tag_offset, cache_tag_header_data_size, memory_tag_header_data_start) &&
		cache_file_blocking_read(-1, cache_header->data_offset + cache_header->tag_offset, cache_tag_data_size, memory_tag_data_start);

	if(!read_success)
	{
		game_preferences_flag_dirty();
		scenario_tags_load_internal_panic();
		return false;
	}

	s_tags_header* tag_header = (s_tags_header*)memory_tag_header_data_start;

	if(!tag_header->tag_instances || tag_header->tag_count <= 0 || !csstricmp(tag_header->type, "tags"))
	{
		game_preferences_flag_dirty();
		scenario_tags_load_internal_panic();
		return false;
	}

	// Change the pointers in the header to point to the new locations in memory
	tag_header->parent_info = &tag_header[1];
	tag_header->tag_instances = (cache_file_tag_instance*)(((char*)&tag_header[1]) + 0xC * tag_header->tag_parent_info_count);


	// TODO: Where do these go.
	*Memory::GetAddress<cache_file_tag_instance**>(0x47cd50, 0) = tag_header->tag_instances;
	*Memory::GetAddress<int32*>(0x47cd54, 0) = cache_file_memory_globals->tag_cache_base_address;
	*Memory::GetAddress<s_tags_header**>(0x47d568, 0) = tag_header;

	bool result = scenario_tags_load_process_shared_tags();//cache_validate_shared_tag_instances((int8*)cache_file_memory_globals->tag_cache_base_address, cache_header->tag_offset_mask);

	if(tag_header->tag_count >= FIRST_SHARED_TAG_INSTANCE_INDEX && !result)
	{
		cache_file_memory_globals->tags_loaded = true;
		scenario_tags_load_internal_panic();
		main_game_set_global_scenario_index(NONE);
		return false;
	}

	cache_file_memory_globals->tags_loaded = 1;


	uint32 scenario_offset = cache_file_memory_globals->tag_cache_base_address + tag_header->tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_header->scenario_index)].data_offset;
	uint32 globals_offset = cache_file_memory_globals->tag_cache_base_address + tag_header->tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_header->globals_index)].data_offset;

	set_global_scenario((scenario*)(scenario_offset), tag_header->scenario_index);
	scenario_set_game_globals((s_game_globals*)(globals_offset));
	initialize_runtime_sound_gestalt_definition();
	tag_injection_scenario_load_setup(adj_tag_size + k_injectable_allocation_size);

	return true;
}

void cache_files_apply_patches()
{
	// Default Maps
	PatchCall(Memory::GetAddress(0x3166B), scenario_tags_load_internal);
	// Custom Maps
	PatchCall(Memory::GetAddress(0x315ED), scenario_tags_load_internal);
}


