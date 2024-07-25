#include "stdafx.h"
#include "cache_files.h"

#include "cseries/cseries_system_memory.h"
#include "game/game_globals.h"
#include "main/main_game.h"
#include "scenario/scenario.h"
#include "sound/sound_cache_file_definitions.h"
#include "tag_files/tag_loader/tag_injection.h"
#include "tag_files/tag_loader/tag_injection_manager.h"

// H2V does not include these parameters inside the s_cache_file_memory_globals struct, so we have to create and store them elsewhere
s_cache_file_debug_globals g_cache_file_debug_globals;

s_cache_file_memory_globals* cache_file_memory_globals_get()
{
	return Memory::GetAddress<s_cache_file_memory_globals*>(0x47CD60, 0x4A29C8);
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
	INVOKE(0x64C37, 0x4CC8E, cache_file_close);
}

bool __cdecl cache_header_verify(s_cache_header* cache_header)
{
	return INVOKE(0x310E9, 0x24F99, cache_header_verify, cache_header);
}

uint32 __cdecl cache_file_align_read_size_to_cache_page(uint32 size)
{
	return INVOKE(0x647DA, 0x4C831, cache_file_align_read_size_to_cache_page, size);
}

bool __cdecl cache_file_blocking_read(uint32 unk, uint32 cache_offset, uint32 read_size, int8* out_buffer)
{
	return INVOKE(0x64D01, 0x4CD58, cache_file_blocking_read, unk, cache_offset, read_size, out_buffer);
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

bool __cdecl read_shared_resource_database(e_shared_resource_database_type database_type, int32 unused_flags, uint32 offset, uint32 size, void* out_buffer, bool unk_bool)
{
	return INVOKE(0x64CC7, 0x4CD1E, read_shared_resource_database, database_type, unused_flags, offset, size, out_buffer, unk_bool);
}

bool scenario_tags_load_process_shared_tags()
{
	s_cache_file_memory_globals* cache_file_memory = cache_file_memory_globals_get();

	s_tags_header* tag_header = cache_file_memory->tags_header;
	s_tags_header* unmasked_tag_header = (s_tags_header*)cache_file_memory->tag_cache_base_address;
	s_cache_header* cache_header = cache_files_get_header();
	s_cache_header shared_header;

	csmemset(&shared_header, 0, sizeof(s_cache_header));

	// Read cache header
	if(!read_shared_resource_database(_shared_resource_database_type_multi_player, NONE, 0, 2048, &shared_header, false))
		return false;

	const uint32 adjusted_tag_size = cache_file_align_read_size_to_cache_page(shared_header.tag_size);
	const uint32 adjusted_data_offset = cache_file_align_read_size_to_cache_page(shared_header.data_offset);
	const uint32 adjusted_data_size = cache_file_align_read_size_to_cache_page(shared_header.data_size);

	if (adjusted_tag_size > cache_header->tag_offset_mask)
		return false;

	// Read tags header
	if (!read_shared_resource_database(_shared_resource_database_type_multi_player, NONE, shared_header.tag_offset, adjusted_data_offset, unmasked_tag_header, false))
		return false;

	if (unmasked_tag_header->tag_count < FIRST_SHARED_TAG_INSTANCE_INDEX)
		return false;

	int8* shared_data_start = (int8*)(cache_file_memory->tag_cache_base_address + adjusted_tag_size - shared_header.data_size);

	// Read tag data
	if (!read_shared_resource_database(_shared_resource_database_type_multi_player, NONE, shared_header.data_offset + shared_header.tag_offset, adjusted_data_size, shared_data_start, false))
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

bool __cdecl scenario_tags_load_debug()
{
	s_cache_header* cache_header = cache_files_get_header();

	const uint32 aligned_tag_name_read_size = cache_file_align_read_size_to_cache_page(cache_header->tag_name_buffer_size);
	const uint32 aligned_tag_name_offset_read_size = cache_file_align_read_size_to_cache_page(sizeof(datum) * cache_header->debug_tag_name_count);

	const uint32 aligned_string_id_table_read_size = cache_file_align_read_size_to_cache_page(cache_header->string_table_size);
	const uint32 aligned_string_id_index_buffer_read_size = cache_file_align_read_size_to_cache_page(sizeof(string_id) * cache_header->string_table_count);

	if(!cache_file_blocking_read(NONE, cache_header->tag_name_buffer_offset, aligned_tag_name_read_size, g_cache_file_debug_globals.debug_tag_name_buffer))
	{
		DISPLAY_ASSERT("scenario_tags_load_debug: failed to load tag names from cache");
		return false;
	}

	if (!cache_file_blocking_read(NONE, cache_header->tag_name_offsets_offset, aligned_tag_name_offset_read_size, g_cache_file_debug_globals.debug_tag_name_offsets))
	{
		DISPLAY_ASSERT("scenario_tags_load_debug: failed to load tag name offsets from cache");
		return false;
	}

	if(!cache_file_blocking_read(NONE, cache_header->string_table_offset, aligned_string_id_table_read_size, g_cache_file_debug_globals.debug_string_id_storage))
	{
		DISPLAY_ASSERT("scenario_tags_load_debug: failed to load string table from cache");
		return false;
	}

	if(!cache_file_blocking_read(NONE, cache_header->string_idx_offset, aligned_string_id_index_buffer_read_size, g_cache_file_debug_globals.debug_string_id_index))
	{
		DISPLAY_ASSERT("scenario_tags_load_debug: failed to load string index table from cache");
		return false;
	}

	return true;
}

bool __cdecl scenario_tags_load(const char* scenario_path)
{
	s_cache_header* cache_header = cache_files_get_header();
	s_cache_file_memory_globals* cache_file_memory_globals = cache_file_memory_globals_get();

	const uint32 aligned_tag_size_read = cache_header->tag_size + cache_header->tag_offset_mask;

	if(!cache_header_verify(cache_header) || strlen(cache_header->version_string) > 32)
	{
		scenario_tags_load_internal_panic();
		return false;
	}

	cache_header->tag_size = cache_file_align_read_size_to_cache_page(cache_header->tag_size);

	cache_file_memory_globals->tag_cache_base_address = datum_header_allocate(aligned_tag_size_read + k_injectable_allocation_size, 12);

	if(!cache_file_memory_globals->tag_cache_base_address)
	{
		scenario_tags_load_internal_panic();
		return false;
	}

	csmemset((void*)cache_file_memory_globals->tag_cache_base_address, 0, aligned_tag_size_read);

	set_tag_group_data_info(cache_file_memory_globals->tag_cache_base_address, aligned_tag_size_read);

	const uint32 aligned_tag_header_read_size = cache_file_align_read_size_to_cache_page(cache_header->data_offset);
	const uint32 aligned_tag_data_read_size = cache_file_align_read_size_to_cache_page(cache_header->data_size);

	int8* memory_tag_header_data_start = (int8*)(cache_file_memory_globals->tag_cache_base_address + cache_header->tag_offset_mask);
	int8* memory_tag_data_start = (int8*)(cache_header->tag_size + cache_header->tag_offset_mask + cache_file_memory_globals->tag_cache_base_address - cache_header->data_size);

	// read tag header
	if(!cache_file_blocking_read(NONE, cache_header->tag_offset, aligned_tag_header_read_size, memory_tag_header_data_start))
	{
		game_preferences_flag_dirty();
		scenario_tags_load_internal_panic();
		DISPLAY_ASSERT("scenario_tags_load: failed to load tag header from cache");
		return false;
	}

	// read tag data
	if(!cache_file_blocking_read(NONE, cache_header->data_offset + cache_header->tag_offset, aligned_tag_data_read_size, memory_tag_data_start))
	{
		game_preferences_flag_dirty();
		scenario_tags_load_internal_panic();
		DISPLAY_ASSERT("scenario_tags_load: failed to load tag data from cache");
		return false;
	}

	// read debug info
	if(!scenario_tags_load_debug())
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
	*Memory::GetAddress<cache_file_tag_instance**>(0x47cd50, 0x4A29B8) = tag_header->tag_instances;
	*Memory::GetAddress<uint32*>(0x47cd54, 0x4A29BC) = cache_file_memory_globals->tag_cache_base_address;

	cache_file_memory_globals->tags_header = tag_header;

	if(tag_header->tag_count >= FIRST_SHARED_TAG_INSTANCE_INDEX && !scenario_tags_load_process_shared_tags())
	{
		cache_file_memory_globals->tags_loaded = true;
		scenario_tags_load_internal_panic();
		main_game_set_global_scenario_index(NONE);
		return false;
	}

	cache_file_memory_globals->tags_loaded = true;

	const uint32 scenario_address = cache_file_memory_globals->tag_cache_base_address + tag_header->tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_header->scenario_index)].data_offset;
	const uint32 globals_address = cache_file_memory_globals->tag_cache_base_address + tag_header->tag_instances[DATUM_INDEX_TO_ABSOLUTE_INDEX(tag_header->globals_index)].data_offset;

	main_game_set_global_scenario_index(tag_header->scenario_index);

	set_global_scenario((scenario*)(scenario_address));

	scenario_set_game_globals((s_game_globals*)(globals_address));

	initialize_runtime_sound_gestalt_definition();

	tag_injection_scenario_load_setup(aligned_tag_size_read);

	return true;
}

void cache_files_apply_patches()
{
	// Default Maps
	PatchCall(Memory::GetAddress(0x3166B, 0x2551B), scenario_tags_load);
	// Custom Maps
	PatchCall(Memory::GetAddress(0x315ED, 0x2549D), scenario_tags_load);
}


