#pragma once

#include "..\MapManager.h"

#define VANILLA_MAP_LIMIT 50u
// keep the bitmap preview limit the same, to save memory and performance
#define VANILLA_MAP_BITMAP_PREVIEW_LIMIT 50u
#define NEW_MAP_BITMAP_PREVIEW_LIMIT VANILLA_MAP_BITMAP_PREVIEW_LIMIT
#define NEW_MAP_LIMIT 1000u

#define MAX_BITMAP_PREVIEW_MAX_BUFFER_SIZE 524288u

static const int custom_map_cache_signature = 'mpch';

enum e_directory_id
{
	custom_maps_dir = 4,
	custom_map_data_dir = 7
};

struct s_custom_map_id
{
	wchar_t map_name[32];
	BYTE map_sha256_hash[32];
};
static_assert(sizeof(s_custom_map_id) == 32 + sizeof(wchar_t) * 32);

struct __declspec(align(4)) s_custom_map_entry
{
	BYTE map_sha256_hash[32];
	wchar_t map_name[32]; // actually the name displayed
	wchar_t field_60[9][128];
	BYTE* preview_bitmap_header[2]; // i think one header has the compressed thumbnail, and the other the uncompressed one
	BYTE gap_968[16];
	LONGLONG preview_bitmap_id; // used for bitmap cache file
	wchar_t file_path[256];
	FILETIME file_time;
	bool entry_marked_for_deletion; // marks entry for deletion
	BYTE field_B88[7];
};
static_assert(sizeof(s_custom_map_entry) == 0xB90);
static_assert(offsetof(s_custom_map_entry, preview_bitmap_header) == 0x960);

#pragma pack(push, 1)
struct s_custom_map_entry_linked_list
{
	s_custom_map_entry_linked_list* next;
	BYTE gap_4[4];
	s_custom_map_entry custom_map_entry;
	bool task_in_progress;
	int async_task_id;
	bool map_valid;
	bool task_finished;
	BYTE field_BA2[2];
	DWORD field_BA4;
};
#pragma pack(pop)
static_assert(sizeof(s_custom_map_entry_linked_list) == 0xBA8);

// our custom map file cache structure, to write cache data to
struct s_custom_map_file_cache
{
	int signature;
	unsigned int entries_count;
	long long last_preview_bitmap_index;

	s_custom_map_entry entries[];
};

#pragma pack(push, 1)
struct s_custom_map_data
{
	// we don't re-implement this from scratch, thus we re-use it
	// because the function that goes through all custom maps in the folder is actually decent
	LPCRITICAL_SECTION custom_map_lock;
	BYTE gap_4[4];
	LONGLONG field_8;

	union
	{
		struct
		{
			s_custom_map_entry* new_custom_map_entries_buffer;
			s_custom_map_file_cache* custom_map_data_cache;
		};
		// we replace this buffer bellow with a pointer to some dynamic heap allocated memory
		s_custom_map_entry custom_map_entries[VANILLA_MAP_LIMIT];
	};

	WORD custom_map_count;
	BYTE gap_24232[6];
	s_custom_map_entry_linked_list* custom_map_to_add_linked_list; // un-cached maps that have to be added
	wchar_t custom_maps_folder_path[MAX_PATH];

	static void applyCustomMapExtensionLimitPatches();

	static void create_custom_map_data_directory();

	void __thiscall mark_all_cached_maps_for_deletion();
	bool __thiscall remove_marked_for_deletion();

	bool read_custom_map_data_cache_from_file(const char* path, s_custom_map_file_cache* custom_map_data_cache, DWORD custom_map_data_cache_buffer_size);
	bool write_custom_map_data_cache_to_file(const char* path, s_custom_map_file_cache* custom_map_data_cache);
	void __thiscall save_custom_map_data();
	void load_map_data_cache_from_file_cache(s_custom_map_file_cache* custom_map_file_cache);

	void __thiscall load_custom_map_data_cache();

	// goes through all custom maps in directory
	// and creates a linked list of maps that are not cached/registered
	// and removes custom maps cached entries that are not present anymore in the folder
	void __thiscall start_custom_map_sync();

	unsigned int __thiscall get_custom_map_list_ids(s_custom_map_id* out_ids, unsigned int out_ids_count);
	unsigned int __thiscall find_matching_entries_by_file_path(const wchar_t* file_path, s_custom_map_entry** out_custom_map_entries, unsigned int out_custom_map_entries_count);
	unsigned int __thiscall find_matching_entries_by_sha256_hash(const BYTE* hash, s_custom_map_entry** out_custom_map_entries, unsigned int out_custom_map_entries_count);
	unsigned int __thiscall find_matching_entries_by_map_name_and_hash(const wchar_t* map_name, const BYTE* sha256_hash, s_custom_map_entry** out_custom_map_entries, unsigned int out_custom_map_entries_count);
	unsigned int __thiscall find_matching_entries_by_map_name(const wchar_t* map_name, s_custom_map_entry** out_custom_map_entries, unsigned int out_custom_map_entries_count);
	bool __thiscall get_entry_by_id(const s_custom_map_id* custom_map_id, s_custom_map_entry** out_entry);

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
	void __thiscall remove_entry_by_index(int idx);
};
static_assert(sizeof(s_custom_map_data) == 0x24444);
#pragma pack(pop)

s_custom_map_data* getCustomMapData();
const wchar_t* getCustomMapFolderPath();