#pragma once

/* constants */

enum
{
	CACHE_FILE_PAGE_SIZE = 512,
	FIRST_SHARED_TAG_INSTANCE_INDEX = 10000,
	MAXIMUM_SIMULTANEOUS_TAG_INSTANCES_TOTAL = 20000,
	k_cache_file_max_string_ids = 24576,

	k_cache_file_debug_name_buffer_size = 5120000,
	k_cache_file_debug_string_id_index_buffer_size = (k_cache_file_max_string_ids * sizeof(string_id)),
};

/* enums */

enum e_shared_resource_database_type : int32
{
	_shared_resource_database_type_main_menu = 0,
	_shared_resource_database_type_multi_player = 1,
	_shared_resource_database_type_single_player = 2,
	k_shared_resource_database_type_count,
	_shared_resource_database_type_none = NONE
};

/* structures */

struct cache_file_tag_instance
{
	uint32 group_tag;
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

struct cache_file_header
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
	char path[k_maximum_filename_length];
	char version_string[32];
	int16 type;	// e_scenario_type
	int16 pad;
	int32 crc;
	bool crc_valid;
	int8 pad_1[3];
	bool field_158;
	bool tracked;
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
	bool uses_shared_map[k_shared_resource_database_type_count];
	int8 pad_2;

	FILETIME creation_time;
	FILETIME shared_creation_time[k_shared_resource_database_type_count];
	char name[32];
	int32 language;		// e_language
	char scenario_path[k_maximum_filename_length];
	int32 minor_version;
	int32 debug_tag_name_count;
	int32 tag_name_buffer_offset;
	int32 tag_name_buffer_size;
	int32 tag_name_offset;
	int32 language_pack_offset;
	int32 language_pack_size;
	datum secondary_ugh_tag_index;

	int32 geometry_data_offset;
	int32 geometry_data_size;
	
	uint32 checksum;
	uint32 mopp_checksum;
	int8 field_2F8[1284];
	int32 footer_signature;
};
ASSERT_STRUCT_SIZE(cache_file_header, 0x800);

struct s_cache_file_memory_globals
{
	bool tags_loaded;
	bool custom_map;
	int8 pad[2];
	uint32 tag_cache_base_address;
	cache_file_header header;
	cache_file_tags_header* tags_header;
	void* field_80C;
};
ASSERT_STRUCT_SIZE(s_cache_file_memory_globals, 0x810);

struct s_cache_file_debug_globals
{
	char debug_tag_name_buffer[k_cache_file_debug_name_buffer_size];
	int32 debug_tag_name_offsets[MAXIMUM_SIMULTANEOUS_TAG_INSTANCES_TOTAL + 96];
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

/* globals */


/* prototypes */

void cache_files_apply_patches(void);

void* cache_file_handle_get(void);

uintptr_t* tag_data_get(void);

s_cache_file_memory_globals* cache_file_memory_globals_get(void);

void* tag_data_get_from_instance(cache_file_tag_instance const* tag_instance);

bool cache_file_is_loaded(void);

cache_file_header* cache_files_get_header(void);

cache_file_tags_header* cache_files_get_tags_header(void);

cache_file_tag_instance* global_tag_instances_get(void);

int8* cache_get_tag_data(uint32 offset = 0);

cache_file_tag_instance* cache_get_tag_instance(datum tag_index);

tag_iterator* tag_iterator_new(tag_iterator* itr, e_tag_group type);

datum __cdecl tag_iterator_next(tag_iterator* itr);

void __cdecl cache_file_map_clear_all_failures(void);

void __cdecl cache_file_close();

bool __cdecl cache_header_verify(cache_file_header* cache_header);

uint32 __cdecl cache_file_align_read_size_to_cache_page(uint32 size);

bool __cdecl cache_file_blocking_read(intptr_t a1, uint32 cache_offset, uint32 read_size, void* out_buffer);

bool __cdecl scenario_tags_load_internal(const char* scenario_path);

void* tag_get(uint32 expected_group_tag, int32 tag_index);

datum tag_loaded(uint32 group_tag, const char* name);

const char* tag_get_name(datum tag_name_index);

void tag_add_name(datum tag_index, const char* name);

cache_file_tag_instance* tag_instance_get(datum tag_index);
