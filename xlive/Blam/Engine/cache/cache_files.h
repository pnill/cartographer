#pragma once
#include "main/game_preferences.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_groups.h"

#define CACHE_FILE_PAGE_SIZE 512
#define FIRST_SHARED_TAG_INSTANCE_INDEX 10000
#define MAXIMUM_SIMULTANEOUS_TAG_INSTANCES_TOTAL 20000
#define k_cache_file_max_string_ids 24576

#define k_cache_file_debug_name_buffer_size 5120000
#define k_cache_file_debug_string_id_index_buffer_size (k_cache_file_max_string_ids * sizeof(string_id))
#define k_cache_file_debug_string_id_storage_size 393216

enum e_shared_resource_database_type : int32
{
	_shared_resource_database_type_main_menu = 0,
	_shared_resource_database_type_multi_player = 1,
	_shared_resource_database_type_single_player = 2,
	_shared_resource_database_type_none = -1
};

struct cache_file_tag_instance
{
	tag_group group_tag;
	int32 tag_index;
	uint32 data_offset;
	uint32 size;
};
ASSERT_STRUCT_SIZE(cache_file_tag_instance, 16);

// Stores information about currently loaded tags
struct cache_file_tags_header
{
	s_tag_group_link* tag_group_link_set;
	int32 tag_group_link_set_count;
	cache_file_tag_instance* tag_instances;
	datum scenario_index;
	datum globals_index;
	int32 field_14;
	int32 tag_count;
	uint32 signature;
};
ASSERT_STRUCT_SIZE(cache_file_tags_header, 32);

struct s_cache_header
{
	int32 header_signature;
	int32 version;
	int32 file_size;
	int32 field_C;
	int32 tag_offset;
	int32 data_offset;
	int32 data_size;
	int32 tag_size;

	// Offset mask is used for cache files that rely on a shared resource database
	int32 tag_offset_mask;
	int32 shared_tag_dependency_offset;
	int32 shared_tag_dependency_count;
	int8 some_path[256];
	char version_string[32];
	int16 type;	// e_scenario_type
	int16 pad;
	int32 crc;
	bool crc_valid;
	int8 pad_1[3];
	bool field_158;
	bool tracked__maybe;
	bool field_15A;
	bool field_15B;
	int32 field_15C;
	int32 field_160;
	int32 field_164;
	int32 field_168;
	int32 string_block_offset;
	int32 string_table_count;
	int32 string_table_size;
	int32 string_idx_offset;
	int32 string_table_offset;
	int8 uses_shared_map[3];
	int8 pad_2;
	FILETIME time;
	FILETIME main_menu_time;
	FILETIME shared_time;
	FILETIME campaign_time;
	char name[32];
	e_language language;
	char scenario_path[256];
	int32 minor_version;
	int32 debug_tag_name_count;
	int32 tag_name_buffer_offset;
	int32 tag_name_buffer_size;
	int32 tag_name_offsets_offset;
	int32 language_pack_offset;
	int32 language_pack_size;
	datum secondary_ugh_tag_index;
	int32 geometry_data_offset;
	int32 geometry_data_size;
	int32 checksum;
	int32 mopp_code_checksum;
	int8 field_2F8[1284];
	int32 footer_signature;
};
ASSERT_STRUCT_SIZE(s_cache_header, 0x800);

struct s_cache_file_memory_globals
{
	bool tags_loaded;
	bool custom_map;
	int8 pad[2];
	uint32 tag_cache_base_address;
	s_cache_header header;
	cache_file_tags_header* tags_header;
	void* field_80C;
};
ASSERT_STRUCT_SIZE(s_cache_file_memory_globals, 0x810);

struct s_cache_file_debug_globals
{
	char debug_tag_name_buffer[k_cache_file_debug_name_buffer_size];
	int32 debug_tag_name_offsets[MAXIMUM_SIMULTANEOUS_TAG_INSTANCES_TOTAL + 96];
	int8 debug_string_id_storage[k_cache_file_debug_string_id_storage_size];
	int8 debug_string_id_index[k_cache_file_debug_string_id_index_buffer_size];
};

struct tag_iterator
{
	int32 field_0;
	int32 field_4;
	datum current_tag_index;
	datum next_tag_index;
	tag_group tag_type;
};
ASSERT_STRUCT_SIZE(tag_iterator, 20);

/* prototypes */

void cache_files_apply_patches(void);

s_cache_file_memory_globals* cache_file_memory_globals_get(void);

s_cache_header* cache_files_get_header(void);

cache_file_tags_header* cache_files_get_tags_header(void);

cache_file_tag_instance* global_tag_instances_get(void);

tag_iterator* tag_iterator_new(tag_iterator* itr, e_tag_group type);

datum __cdecl tag_iterator_next(tag_iterator* itr);

void cache_file_map_clear_all_failures(void);

// Get tag data from tag index
void* __cdecl tag_get_fast(datum tag_index);

void __cdecl cache_file_close();

bool __cdecl cache_header_verify(s_cache_header* cache_header);

uint32 __cdecl cache_file_align_read_size_to_cache_page(uint32 size);

bool __cdecl cache_file_blocking_read(uint32 a1, uint32 cache_offset, uint32 read_size, void* out_buffer);

bool __cdecl scenario_tags_load(const char* scenario_path);

datum tag_loaded(uint32 group_tag, const char* name);

const char* tag_get_name(datum tag_name_index);
