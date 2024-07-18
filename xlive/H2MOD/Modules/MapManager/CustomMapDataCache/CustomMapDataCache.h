#pragma once

#include "../MapManager.h"

#define MAX_MAP_NAME_SIZE 32
#define MAX_MAP_FILE_PATH_SIZE (255 + 1)

#define SHA256_HASH_SIZE 32

#define k_maximum_number_of_custom_multiplayer_maps_default 50u

// TODO: load each map bitmap asynchronously
//#define VANILLA_MAP_BITMAP_PREVIEW_LIMIT 50u
//#define NEW_MAP_BITMAP_PREVIEW_LIMIT VANILLA_MAP_BITMAP_PREVIEW_LIMIT
#define k_maximum_number_of_custom_multiplayer_maps_new 1000u

#define MAX_BITMAP_PREVIEW_MAX_BUFFER_SIZE 524288u

static const int custom_map_cache_signature = 'mpch';
#define k_custom_map_data_cache_header_version (1)

enum e_directory_id
{
	_custom_maps_dir = 4,
	_custom_map_data_dir = 7
};

struct s_custom_map_id
{
	wchar_t map_name[MAX_MAP_NAME_SIZE];
	BYTE map_sha256_hash[SHA256_HASH_SIZE];
};
static_assert(sizeof(s_custom_map_id) == SHA256_HASH_SIZE + sizeof(wchar_t) * MAX_MAP_NAME_SIZE);

#pragma pack(push, 4)
struct s_custom_map_entry
{
	BYTE map_sha256_hash[SHA256_HASH_SIZE];
	wchar_t map_name[MAX_MAP_NAME_SIZE]; // actually the name displayed
	wchar_t field_60[9][128];
	// one header might have the compressed thumbnail, and the other the uncompressed one
	uint8* preview_bitmap_header[2]; 
	BYTE gap_968[16];
	LONGLONG preview_bitmap_id; // used for bitmap cache file
	wchar_t file_path[MAX_MAP_FILE_PATH_SIZE];
	FILETIME file_time;
	bool entry_marked_for_deletion; // marks entry for deletion
	BYTE field_B88[7];
};
#pragma pack(pop)
static_assert(sizeof(s_custom_map_entry) == 0xB90);
static_assert(offsetof(s_custom_map_entry, preview_bitmap_header) == 0x960);

struct s_custom_map_entry_linked_list
{
	s_custom_map_entry_linked_list* next;
	BYTE gap_4[4];
	s_custom_map_entry custom_map_entry;
	bool task_in_progress;
	int async_task_id;
	bool map_valid;
	bool task_finished;
	DWORD field_BA4;
};
static_assert(sizeof(s_custom_map_entry_linked_list) == 0xBA8);

// custom map file cache header structure
// has at address + sizeof(s_custom_map_file_cache_header) the actual map data
struct s_custom_map_file_cache_header
{
	int32	signature;
	uint32	version;
	uint32	entry_count;
	int64	index_of_last_previewed_bitmap;
};

#pragma pack(push, 4)
class c_custom_map_manager
{
public:
	// we don't re-implement this from scratch, thus we re-use it
	// because the function that goes through all custom maps in the folder is actually decent
	LPCRITICAL_SECTION m_lock;
	BYTE gap_4[4];
	uint64 m_index_of_last_previewed_bitmap;

	union
	{
		struct
		{
			s_custom_map_file_cache_header* m_custom_map_file_data_cache;
			s_custom_map_entry*				m_new_custom_map_entry_list_buffer;
		};
		// we replace this buffer below with a pointer to some dynamic heap allocated memory
		s_custom_map_entry custom_map_entries[k_maximum_number_of_custom_multiplayer_maps_default];
	};

	uint16 m_loaded_count;
	BYTE gap_24232[6];
	s_custom_map_entry_linked_list* m_list_of_maps_to_insert_to_cache; // un-cached maps to be added
	wchar_t m_custom_maps_folder_path[MAX_PATH];

	static void ApplyCustomMapExtensionLimitPatches();

	static void create_custom_map_data_directory();

	void __thiscall mark_all_cached_maps_for_deletion();
	bool __thiscall remove_marked_for_deletion();

	bool read_custom_map_data_cache_from_file(const utf8* path, s_custom_map_file_cache_header* custom_map_data_cache, uint32 custom_map_data_cache_buffer_size);
	bool write_custom_map_data_cache_to_file(const utf8* path, s_custom_map_file_cache_header* custom_map_data_cache);
	void __thiscall save_custom_map_data();
	void load_map_data_cache_from_file_cache(s_custom_map_file_cache_header* custom_map_file_cache);

	void __thiscall load_custom_map_data_cache();

	// goes through all custom maps in directory
	// and creates a linked list of maps that are not cached/registered
	// and removes custom maps cached entries that are not present anymore in the folder
	void __thiscall start_custom_map_sync();

	uint32 __thiscall get_custom_map_list_ids(s_custom_map_id* out_ids, uint32 out_ids_count);
	uint32 __thiscall get_custom_map_list_ids_by_map_name(const wchar_t* map_name, s_custom_map_id* out_ids, uint32 out_ids_count);
	uint32 __thiscall find_matching_entries_by_file_path(const wchar_t* file_path, s_custom_map_entry** out_custom_map_entries, uint32 out_custom_map_entries_count);
	uint32 __thiscall find_matching_entries_by_sha256_hash(const BYTE* hash, s_custom_map_entry** out_custom_map_entries, uint32 out_custom_map_entries_count);
	uint32 __thiscall find_matching_entries_by_map_name_and_hash(const wchar_t* map_name, const BYTE* sha256_hash, s_custom_map_entry** out_custom_map_entries, uint32 out_custom_map_entries_count);
	uint32 __thiscall find_matching_entries_by_map_name(const wchar_t* map_name, s_custom_map_entry** out_custom_map_entries, uint32 out_custom_map_entries_count);
	bool __thiscall get_entry_by_id(const s_custom_map_id* custom_map_id, s_custom_map_entry** out_entry);

	bool __thiscall entry_is_duplicate(const s_custom_map_entry* entry);
	bool __thiscall validate_entry_data(const s_custom_map_entry* entry, uint32 count);

	bool __thiscall add_entry(const s_custom_map_entry* entry);
	bool __thiscall add_custom_map_entry_by_map_file_path(const wchar_t* file_path);
	bool __thiscall add_custom_map_entry_by_map_file_path(const std::wstring& file_path);
	bool __thiscall remove_duplicates_and_add_entry(const s_custom_map_entry* entry);
	bool __thiscall remove_duplicates_write_entry_data_and_add(s_custom_map_entry* entry);

	bool __thiscall remove_entries_matching_file_path(const s_custom_map_entry* entry);
	bool __thiscall remove_duplicates_by_map_name_and_hash(const s_custom_map_entry* entry);
	bool __thiscall delete_single_entry(const s_custom_map_entry* entry);

	void __thiscall initialize();
	void __thiscall cleanup();
private:
	void __thiscall remove_entry_by_index(uint16 idx);
};
#pragma pack(pop)
static_assert(sizeof(c_custom_map_manager) == 0x24444);

c_custom_map_manager* get_custom_map_manager();
const wchar_t* get_custom_map_folder_path();