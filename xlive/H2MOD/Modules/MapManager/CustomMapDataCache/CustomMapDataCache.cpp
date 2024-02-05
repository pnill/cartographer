#include "stdafx.h"
#include "CustomMapDataCache.h"


#include "memory/data.h"
#include "shell/shell.h"
#include "tag_files/files_windows.h"
#include "text/unicode.h"

#include "Blam/Cache/CacheHeader.h"

#include "H2MOD/Modules/CustomMenu/c_list_widget.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"



#pragma region 50 map limit removal

// TODO: consider replacing this implementation with a linked list

const wchar_t* custom_map_cache_filename_client = L"mapset.h2mdat";

// TODO: determine whether or not the pre-release version of vista's map files are supported (11028.07.03.23.1927.main)
const static char* offically_supported_builds[32] =
{
	"11081.07.04.30.0934.main",
	"11122.07.08.24.1808.main",
};

bool __cdecl scenario_is_supported_build(const char* build)
{
	for (int i = 0; i < 2; i++)
	{
		if (strcmp(build, offically_supported_builds[i]) == 0)
			return true;
	}

	LOG_TRACE_FUNC("Build '{}' is not offically supported, consider repacking and updating map with supported tools!", build);
	// return false;
	return true;
}

s_custom_map_data* getCustomMapData()
{
	auto customMapCacheData = Memory::GetAddress<s_custom_map_data*>(0x482D70, 0x4A70D8);
	return customMapCacheData;
}

const wchar_t* getCustomMapFolderPath()
{
	return getCustomMapData()->custom_maps_folder_path;
}

int get_path_from_id(e_directory_id id, LPCWSTR pMore, LPWSTR pszPath, bool is_folder)
{
	typedef int(__cdecl* get_path_from_id)(e_directory_id id, LPCWSTR pMore, LPWSTR pszPath, bool is_folder);
	auto p_get_directory_path_by_id = Memory::GetAddressRelative<get_path_from_id>(0x48EF9E, 0x474A15);
	return p_get_directory_path_by_id(id, pMore, pszPath, is_folder);
}

// TODO move to util or some other place
static void* system_heap_alloc(SIZE_T dwBytes)
{
	return HeapAlloc(GetProcessHeap(), 0, dwBytes);
}

void s_custom_map_data::create_custom_map_data_directory()
{
	auto p__create_custom_map_data_directory = Memory::GetAddressRelative<void(__cdecl*)()>(0x4C1FD4, 0x48F715);
	p__create_custom_map_data_directory();
}

void __thiscall s_custom_map_data::mark_all_cached_maps_for_deletion()
{
	EnterCriticalSection(custom_map_lock);

	if (new_custom_map_entries_buffer == nullptr)
	{
		LeaveCriticalSection(custom_map_lock);
		return;
	}

	for (int i = 0; i < custom_map_count; i++)
		new_custom_map_entries_buffer[i].entry_marked_for_deletion = true;

	LeaveCriticalSection(custom_map_lock);
}

bool __thiscall s_custom_map_data::remove_marked_for_deletion()
{
	EnterCriticalSection(custom_map_lock);

	bool maps_removed = false;
	WORD map_count_before_remove = custom_map_count;

	for (int i = 0; i < custom_map_count; )
	{
		if (new_custom_map_entries_buffer[i].entry_marked_for_deletion)
		{
			remove_entry_by_index(i);
		}
		else
		{
			i++;
		}
	}

	maps_removed = map_count_before_remove > custom_map_count;

	LeaveCriticalSection(custom_map_lock);

	return maps_removed;
}

bool s_custom_map_data::read_custom_map_data_cache_from_file(const utf8* path, s_custom_map_file_cache* custom_map_data_cache, DWORD custom_map_data_cache_buffer_size)
{
	s_file_reference cache_file;

	bool success = false;
	bool invalid_file = false;
	e_file_open_error open_file_error_code;

	file_reference_create_from_path(&cache_file, path, false);
	e_file_open_flags flags = (e_file_open_flags)(_file_open_sequential_scan_bit | _permission_read_bit);

	do 
	{
		if (!file_open(&cache_file, flags, &open_file_error_code))
		{
			LOG_TRACE_GAME("{} - failed to open custom map data cache file!",
				__FUNCTION__);

			break;
		}

		// first we read the header
		if (!file_read(&cache_file, sizeof(s_custom_map_file_cache), true, custom_map_data_cache))
		{
			invalid_file = true;
			LOG_TRACE_GAME("{} - failed to read custom map data cache file header!",
				__FUNCTION__);

			break;
		}

		if (custom_map_data_cache->signature != custom_map_cache_signature)
		{
			LOG_TRACE_GAME("{} - invalid custom map data cache file signature! deleting file if possible",
				__FUNCTION__);

			invalid_file = true;
			break;
		}

		if (custom_map_data_cache->entries_count > NEW_MAP_LIMIT)
		{
			LOG_TRACE_GAME("{} - custom map data cache file exceeds new map limit size!",
				__FUNCTION__);

			break;
		}

		// if signature matches, read the cache contents
		DWORD file_custom_map_entries_size = custom_map_data_cache->entries_count * sizeof(s_custom_map_entry);

		if (!file_read(&cache_file, file_custom_map_entries_size, true, custom_map_data_cache->entries))
		{
			LOG_TRACE_GAME("{} - failed reading custom map data cache file!",
				__FUNCTION__);

			break;
		}

		// clear unused custom map entries
		size_t remaining_size = custom_map_data_cache_buffer_size - sizeof(s_custom_map_file_cache) - file_custom_map_entries_size;
		memset((BYTE*)(custom_map_data_cache->entries) + file_custom_map_entries_size, 0, remaining_size);
		success = true;

	} while (0);

	if (open_file_error_code == _file_open_error_success)
	{
		file_close(&cache_file);
		if (invalid_file)
			file_delete(&cache_file);
	}

	if (success)
	{
		LOG_TRACE_GAME("loaded custom map paths: ");
		for (int i = 0; i < custom_map_data_cache->entries_count; i++)
		{
			LOG_TRACE_GAME(L"	custom map path: {}", custom_map_data_cache->entries[i].file_path);
		}
	}

	return success;
}

bool s_custom_map_data::write_custom_map_data_cache_to_file(const utf8* path, s_custom_map_file_cache* custom_map_data_cache)
{
	s_file_reference cache_file;

	bool success = false;
	e_file_open_error open_file_error_code = _file_open_error_unknown;

	file_reference_create_from_path(&cache_file, path, false);
	bool create_file_success = file_create(&cache_file);

	do 
	{
		if (!file_open(&cache_file, _permission_write_bit, &open_file_error_code))
		{
			LOG_ERROR_GAME("{} - failed to open custom map cache file, error code: {}",
				__FUNCTION__, open_file_error_code);

			break;
		}

		file_set_hidden(&cache_file, true);
		
		if (!file_write(&cache_file, sizeof(s_custom_map_file_cache) + sizeof(s_custom_map_entry) * custom_map_data_cache->entries_count, custom_map_data_cache))
		{
			LOG_ERROR_GAME("{} - failed to write custom map cache to file!",
				__FUNCTION__);

			break;
		}

		LOG_TRACE_GAME("{} - saved custom map data cache, map count: {}, buffer size: {}",
			__FUNCTION__, custom_map_data_cache->entries_count, sizeof(s_custom_map_file_cache) + sizeof(s_custom_map_entry) * custom_map_data_cache->entries_count);

		if (!file_set_eof(&cache_file))
		{
			LOG_ERROR_GAME("{} - failed to set custom map cache file size!",
				__FUNCTION__);

			break;
		}

		success = true;

	} while (0);


	if (open_file_error_code == _file_open_error_success)
	{
		file_close(&cache_file);
	}
	else if (!create_file_success && open_file_error_code == _file_open_error_not_found)
	{
		LOG_ERROR_GAME("{} - failed to create custom map data cache while file not present!"
			__FUNCTION__);
	}

	return success;
}

void __thiscall s_custom_map_data::save_custom_map_data()
{
	WCHAR path_wide[MAX_PATH];
	CHAR path_multibyte[MAX_PATH];
	bool custom_map_data_path_available = false;

	EnterCriticalSection(custom_map_lock);

	s_custom_map_file_cache* custom_map_data_to_save = custom_map_file_data_cache;

	custom_map_data_to_save->signature = custom_map_cache_signature;
	custom_map_data_to_save->entries_count = custom_map_count;
	custom_map_data_to_save->last_preview_bitmap_index = last_preview_bitmap_index;

	if (get_path_from_id(e_directory_id::custom_map_data_dir, L"\0", path_wide, true))
	{
		custom_map_data_path_available = true;
		wcscat(path_wide, custom_map_cache_filename_client);
	}

	if (!custom_map_data_path_available
		|| !WideCharToMultiByte(CP_UTF8, 0, path_wide, -1, path_multibyte, MAX_PATH, 0, 0)
		|| !write_custom_map_data_cache_to_file(path_multibyte, custom_map_data_to_save))
	{
		LOG_ERROR_GAME("{} - failed to save custom map data cache!", __FUNCTION__);
	}

	LeaveCriticalSection(custom_map_lock);
}

void s_custom_map_data::load_map_data_cache_from_file_cache(s_custom_map_file_cache* custom_map_file_cache)
{

}

void __thiscall s_custom_map_data::load_custom_map_data_cache()
{
	//typedef void(__thiscall* cache_custom_map_file_image_preview)(s_custom_map_data* thisx);
	//auto p_cache_custom_map_file_image_previews = Memory::GetAddress<cache_custom_map_file_image_preview>(0x4CC30, 0x41501);
	//return p_cache_custom_map_file_image_previews(this);

	EnterCriticalSection(custom_map_lock);

	WCHAR path_wide[MAX_PATH];
	CHAR path_multibyte[MAX_PATH];
	bool custom_map_data_path_available = false;

	DWORD map_file_cache_buffer_size = sizeof(s_custom_map_file_cache) + sizeof(s_custom_map_entry) * NEW_MAP_LIMIT;
	s_custom_map_file_cache* custom_map_file_cache = (s_custom_map_file_cache*)new char[map_file_cache_buffer_size];

	// first create the custom map data directory
	create_custom_map_data_directory();

	if (get_path_from_id(e_directory_id::custom_map_data_dir, L"\0", path_wide, true))
	{
		custom_map_data_path_available = true;
		wcscat(path_wide, custom_map_cache_filename_client);
	}

	if (!custom_map_data_path_available
		|| !WideCharToMultiByte(CP_UTF8, 0, path_wide, -1, path_multibyte, MAX_PATH, 0, 0)
		|| !read_custom_map_data_cache_from_file(path_multibyte, custom_map_file_cache, map_file_cache_buffer_size))
	{
		// if we fail clear the buffer
		memset(custom_map_file_cache, 0, map_file_cache_buffer_size);
		custom_map_file_cache->signature = custom_map_cache_signature;

		LOG_ERROR_GAME("{} - failed to read custom map data cache!", 
			__FUNCTION__);
	}

	//load_map_data_cache_from_file_cache(custom_map_file_cache);

	if (custom_map_file_data_cache != nullptr)
		delete[] custom_map_file_data_cache;

	this->custom_map_file_data_cache = custom_map_file_cache;
	this->last_preview_bitmap_index = custom_map_file_cache->last_preview_bitmap_index;
	this->new_custom_map_entries_buffer = custom_map_file_cache->entries; // we just use the buffer allocated previously
	this->custom_map_count = custom_map_file_cache->entries_count;

	LeaveCriticalSection(custom_map_lock);
}

void __thiscall s_custom_map_data::start_custom_map_sync()
{
	// this code doesn't need any modifications, just some functions replaced inside it
	typedef void(__thiscall* file_iterate_custom_map_directory_t)(s_custom_map_data* thisx);
	auto p_file_iterate_custom_map_directory = Memory::GetAddress<file_iterate_custom_map_directory_t>(0x4D021, 0x419B5);
	p_file_iterate_custom_map_directory(this);
}

unsigned int __thiscall s_custom_map_data::get_custom_map_list_ids(s_custom_map_id* out_ids, unsigned int out_ids_count)
{
	EnterCriticalSection(custom_map_lock);
	for (int i = 0; i < custom_map_count; i++)
	{
		new_custom_map_entries_buffer[i].entry_marked_for_deletion = false;
		if (out_ids != nullptr
			&& out_ids_count != 0)
		{
			if (i < out_ids_count)
			{
				memcpy(out_ids[i].map_sha256_hash, new_custom_map_entries_buffer[i].map_sha256_hash, SHA256_HASH_SIZE);
				wcsncpy(out_ids[i].map_name, new_custom_map_entries_buffer[i].map_name, MAX_MAP_NAME_SIZE);
			}
		}
	}

	unsigned int map_count = this->custom_map_count;
	LeaveCriticalSection(custom_map_lock);

	return map_count;
}

unsigned int __thiscall s_custom_map_data::get_custom_map_list_ids_by_map_name(const wchar_t* map_name, s_custom_map_id* out_ids, unsigned int out_ids_count)
{
	EnterCriticalSection(custom_map_lock);

	unsigned int matching_count_found = 0;

	// used by dedicated server
	for (int i = 0; i < custom_map_count; i++)
	{
		if (!_wcsnicmp(new_custom_map_entries_buffer[i].map_name, map_name, MAX_MAP_NAME_SIZE))
		{
			new_custom_map_entries_buffer[i].entry_marked_for_deletion = false;
			if (out_ids != nullptr
				&& out_ids_count != 0)
			{
				if (matching_count_found < out_ids_count)
				{
					memcpy(out_ids[matching_count_found].map_sha256_hash, new_custom_map_entries_buffer[i].map_sha256_hash, SHA256_HASH_SIZE);
					wcsncpy(out_ids[matching_count_found].map_name, new_custom_map_entries_buffer[i].map_name, MAX_MAP_NAME_SIZE);
				}
			}

			matching_count_found++;
		}
	}

	LeaveCriticalSection(custom_map_lock);

	return matching_count_found;
}

// TODO de-duplicate custom map entry matching code
// because just for 1 line of code we re-write same code over and over

unsigned int __thiscall s_custom_map_data::find_matching_entries_by_file_path(const wchar_t* file_path, s_custom_map_entry** out_custom_map_entries, unsigned int out_custom_map_entries_count)
{
	EnterCriticalSection(custom_map_lock);

	if (new_custom_map_entries_buffer == nullptr
		|| file_path == nullptr)
	{
		LeaveCriticalSection(custom_map_lock);
		return 0;
	}

	unsigned int matching_count_found = 0;

	for (int i = 0; i < custom_map_count; i++)
	{
		if (!_wcsnicmp(new_custom_map_entries_buffer[i].file_path, file_path, MAX_MAP_FILE_PATH_SIZE))
		{
			new_custom_map_entries_buffer[i].entry_marked_for_deletion = false;
			if (out_custom_map_entries)
			{
				if (matching_count_found < out_custom_map_entries_count)
					out_custom_map_entries[matching_count_found] = &new_custom_map_entries_buffer[i];
			}

			matching_count_found++;
		}
	}

	LeaveCriticalSection(custom_map_lock);
	return matching_count_found;
}

unsigned int __thiscall s_custom_map_data::find_matching_entries_by_sha256_hash(const BYTE* hash, s_custom_map_entry** out_custom_map_entries, unsigned int out_custom_map_entries_count)
{
	EnterCriticalSection(custom_map_lock);

	if (new_custom_map_entries_buffer == nullptr
		|| hash == nullptr)
	{
		LeaveCriticalSection(custom_map_lock);
		return 0;
	}

	unsigned int matching_count_found = 0;

	for (int i = 0; i < custom_map_count; i++)
	{
		if (!memcmp(new_custom_map_entries_buffer[i].map_sha256_hash, hash, SHA256_HASH_SIZE))
		{
			new_custom_map_entries_buffer[i].entry_marked_for_deletion = false;
			if (out_custom_map_entries)
			{
				if (matching_count_found < out_custom_map_entries_count)
					out_custom_map_entries[matching_count_found] = &new_custom_map_entries_buffer[i];
			}

			matching_count_found++;
		}
	}

	LeaveCriticalSection(custom_map_lock);
	return matching_count_found;
}

unsigned int __thiscall s_custom_map_data::find_matching_entries_by_map_name_and_hash(const wchar_t* map_name, const BYTE* sha256_hash, s_custom_map_entry** out_custom_map_entries, unsigned int out_custom_map_entries_count)
{
	EnterCriticalSection(custom_map_lock);

	if (new_custom_map_entries_buffer == nullptr
		|| map_name == nullptr)
	{
		LeaveCriticalSection(custom_map_lock);
		return 0;
	}

	unsigned int matching_count_found = 0;

	for (int i = 0; i < custom_map_count; i++)
	{
		if (!memcmp(new_custom_map_entries_buffer[i].map_sha256_hash, sha256_hash, SHA256_HASH_SIZE) && !_wcsnicmp(new_custom_map_entries_buffer[i].map_name, map_name, MAX_MAP_NAME_SIZE))
		{
			new_custom_map_entries_buffer[i].entry_marked_for_deletion = false;
			if (out_custom_map_entries)
			{
				if (matching_count_found < out_custom_map_entries_count)
					out_custom_map_entries[matching_count_found] = &new_custom_map_entries_buffer[i];
			}

			matching_count_found++;
		}
	}

	LeaveCriticalSection(custom_map_lock);
	return matching_count_found;
}

unsigned int __thiscall s_custom_map_data::find_matching_entries_by_map_name(const wchar_t* map_name, s_custom_map_entry** out_custom_map_entries, unsigned int out_custom_map_entries_count)
{
	EnterCriticalSection(custom_map_lock);

	if (new_custom_map_entries_buffer == nullptr
		|| map_name == nullptr)
	{
		LeaveCriticalSection(custom_map_lock);
		return 0;
	}

	unsigned int matching_count_found = 0;

	for (int i = 0; i < custom_map_count; i++)
	{
		if (!_wcsnicmp(new_custom_map_entries_buffer[i].map_name, map_name, MAX_MAP_NAME_SIZE))
		{
			new_custom_map_entries_buffer[i].entry_marked_for_deletion = false;
			if (out_custom_map_entries)
			{
				if (matching_count_found < out_custom_map_entries_count)
					out_custom_map_entries[matching_count_found] = &new_custom_map_entries_buffer[i];
			}

			matching_count_found++;
		}
	}

	LeaveCriticalSection(custom_map_lock);
	return matching_count_found;
}

bool __thiscall s_custom_map_data::get_entry_by_id(const s_custom_map_id* custom_map_id, s_custom_map_entry** out_entry)
{
	// custom_scenario_test_map_name_instead_of_hash
	if (shell_startup_flag_is_set(_startup_flag_custom_map_entry_test_map_name_instead_of_hash))
		return find_matching_entries_by_map_name(custom_map_id->map_name, out_entry, 1) != 0;
	else
		return find_matching_entries_by_sha256_hash(custom_map_id->map_sha256_hash, out_entry, 1) != 0;
}

bool __thiscall s_custom_map_data::entry_is_duplicate(const s_custom_map_entry* entry)
{
	EnterCriticalSection(custom_map_lock);

	bool duplicate = false;

	if (find_matching_entries_by_file_path(entry->file_path, 0, 0) > 0u
		|| find_matching_entries_by_map_name_and_hash(entry->map_name, entry->map_sha256_hash, 0, 0) > 0u)
	{
		duplicate = true;
	}

	LeaveCriticalSection(custom_map_lock);

	return duplicate;
}

bool __thiscall s_custom_map_data::validate_entry_data(const s_custom_map_entry* entries, int count)
{
	typedef bool(__thiscall* validate_entry_data_t)(s_custom_map_data*, const s_custom_map_entry* entry, int count);
	auto p_validate_entry_data = Memory::GetAddressRelative<validate_entry_data_t>(0x4C1E01, 0x48F542);
	return p_validate_entry_data(this, entries, count * sizeof(s_custom_map_entry));
}

void __thiscall s_custom_map_data::remove_entry_by_index(int idx)
{
	// check if the element to be deleted is the last one
	// if so just memset to 0
	if (idx == custom_map_count - 1)
	{
		memset(&new_custom_map_entries_buffer[idx], 0, sizeof(s_custom_map_entry));
	}
	else
	{
		// otherwise just move the memory from the next element to last in the now empty entry
		// TODO: consider replacing this implementation with a linked list
		// this might get very expensive
		// but it shouldn't execute too many times
		memmove(&new_custom_map_entries_buffer[idx], &new_custom_map_entries_buffer[idx + 1], (custom_map_count - 1 - idx) * sizeof(s_custom_map_entry));
	}

	custom_map_count--;
}

bool __thiscall s_custom_map_data::remove_entries_matching_file_path(const s_custom_map_entry* entry)
{
	EnterCriticalSection(custom_map_lock);

	bool removed = false;
	for (int i = 0; i < custom_map_count; )
	{
		if (!_wcsnicmp(new_custom_map_entries_buffer[i].file_path, entry->file_path, MAX_MAP_FILE_PATH_SIZE))
		{
			removed = true;
			remove_entry_by_index(i);
		}
		else
		{
			i++;
		}
	}

	LeaveCriticalSection(custom_map_lock);
	return removed;
}

bool __thiscall s_custom_map_data::remove_duplicates_by_map_name_and_hash(const s_custom_map_entry* entry)
{
	EnterCriticalSection(custom_map_lock);

	bool removed = false;
	for (int i = 0; i < custom_map_count; i++)
	{
		if (!memcmp(new_custom_map_entries_buffer[i].map_sha256_hash, entry->map_sha256_hash, SHA256_HASH_SIZE) 
			&& !_wcsnicmp(new_custom_map_entries_buffer[i].map_name, entry->map_name, MAX_MAP_NAME_SIZE))
		{
			//LOG_TRACE_GAME(L"{} - removed: {} from cache matching cached entry: map name: {} map path: {}",
				//__FUNCTIONW__, entry->file_path, new_custom_map_entries_buffer[i].map_name, new_custom_map_entries_buffer[i].file_path);
			remove_entry_by_index(i);
			removed = true;
		}
	}

	LeaveCriticalSection(custom_map_lock);
	return removed;
}

bool __thiscall s_custom_map_data::delete_single_entry(const s_custom_map_entry* entry)
{
	EnterCriticalSection(custom_map_lock);

	bool removed = false;

	for (int i = 0; i < custom_map_count; i++)
	{
		if (!memcmp(&new_custom_map_entries_buffer[i], entry, sizeof(s_custom_map_entry)))
		{
			remove_entry_by_index(i);
			removed = true;
			break;
		}
	}

	LeaveCriticalSection(custom_map_lock);
	return removed;
}

bool __thiscall s_custom_map_data::add_entry(const s_custom_map_entry* entry)
{
	EnterCriticalSection(custom_map_lock);

	bool success = true;

	if (custom_map_count < NEW_MAP_LIMIT && validate_entry_data(entry, 1) && !entry_is_duplicate(entry))
	{
		memcpy(&new_custom_map_entries_buffer[custom_map_count++], entry, sizeof(s_custom_map_entry));
	}
	else
	{
		success = false;
	}

	LeaveCriticalSection(custom_map_lock);
	return success;
}

bool __thiscall s_custom_map_data::remove_duplicates_and_add_entry(const s_custom_map_entry* entry)
{
	remove_entries_matching_file_path(entry);
	remove_duplicates_by_map_name_and_hash(entry);
	return add_entry(entry);
}

// compared to the other function
// this reads the file and populates the map entry datas
bool __thiscall s_custom_map_data::remove_duplicates_write_entry_data_and_add(s_custom_map_entry* entry)
{
	remove_entries_matching_file_path(entry);
	remove_duplicates_by_map_name_and_hash(entry);

	typedef bool(__thiscall* read_map_data_and_add_entry_t)(s_custom_map_data*, s_custom_map_entry* entry);
	auto p_read_map_data_and_add_entry = Memory::GetAddressRelative<read_map_data_and_add_entry_t>(0x44CC35);

	return p_read_map_data_and_add_entry(this, entry);
}

bool open_cache_header(const wchar_t* file_path, void* cache_header_ptr, HANDLE* map_handle)
{
	typedef char(__cdecl open_cache_header_t)(const wchar_t* file_path, void* lpBuffer, HANDLE* map_handle, DWORD NumberOfBytesRead);
	auto p_open_cache_header = Memory::GetAddress<open_cache_header_t*>(0x642D0, 0x4C327);
	return p_open_cache_header(file_path, cache_header_ptr, map_handle, 0);
}

void close_cache_header(HANDLE* map_handle)
{
	typedef void (__cdecl* close_cache_header_t)(HANDLE*);
	auto p_close_cache_header = Memory::GetAddress<close_cache_header_t>(0x64C03, 0x4CC5A);
	p_close_cache_header(map_handle);
}

bool __cdecl validate_and_read_custom_map_data(s_custom_map_entry* custom_map_entry)
{
	s_cache_header header;
	HANDLE map_cache_handle;
	wchar_t* file_name = custom_map_entry->file_path;
	if (!open_cache_header(file_name, &header, &map_cache_handle))
		return false;
	if (header.magic != 'head' || header.foot != 'foot' || header.file_size <= 0 || header.engine_gen != 8)
	{
		LOG_TRACE_FUNCW(L"\"{}\" has invalid header", file_name);
		return false;
	}
	if (header.type > 5 || header.type < 0)
	{
		LOG_TRACE_FUNCW(L"\"{}\" has bad scenario type", file_name);
		return false;
	}
	if (strnlen_s(header.name, MAX_MAP_NAME_SIZE) >= 32 || strnlen_s(header.version, 32) >= 32)
	{
		LOG_TRACE_FUNCW(L"\"{}\" has invalid version or name string", file_name);
		return false;
	}
	if (!header.is_multiplayer() && !header.is_single_player())
	{
		LOG_TRACE_FUNCW(L"\"{}\" is not playable", file_name);
		return false;
	}

	close_cache_header(&map_cache_handle);
	// needed because the game loads the human readable map name and description from scenario after checks
	// without this the map is just called by it's file name

	// todo move the code for loading the descriptions to our code and get rid of this
	typedef int (__cdecl* validate_and_add_custom_map_internal_t)(s_custom_map_entry*);
	auto validate_and_add_custom_map_internal = Memory::GetAddress<validate_and_add_custom_map_internal_t>(0x4F690, 0x56890);
	if (!validate_and_add_custom_map_internal(custom_map_entry))
	{
		LOG_TRACE_FUNCW(L"warning \"{}\" has bad checksums or is blacklisted, map may not work correctly", file_name);
		std::wstring fallback_name;
		if (strnlen_s(header.name, sizeof(header.name)) > 0) {
			wchar_t fallback_name_c[32];
			utf8_string_to_wchar_string(header.name, fallback_name_c, NUMBEROF(fallback_name_c));
			fallback_name.append(fallback_name_c);
		}
		else {
			std::wstring full_file_name = file_name;
			auto start = full_file_name.find_last_of('\\');
			fallback_name = full_file_name.substr(start != std::wstring::npos ? start : 0, full_file_name.find_last_not_of('.'));
		}
		wcsncpy_s(custom_map_entry->map_name, fallback_name.c_str(), fallback_name.length());
	}
	// load the map even if some of the checks failed, will still mostly work
	return true;
}

bool __thiscall s_custom_map_data::add_custom_map_entry_by_map_file_path(const std::wstring& file_path)
{
	return add_custom_map_entry_by_map_file_path(file_path.c_str());
}

bool __thiscall s_custom_map_data::add_custom_map_entry_by_map_file_path(const wchar_t* file_path)
{
	s_custom_map_entry custom_map_new_entry;
	ZeroMemory(&custom_map_new_entry, sizeof(s_custom_map_entry));

	wcscpy_s(custom_map_new_entry.file_path, MAX_MAP_FILE_PATH_SIZE, file_path);

	bool map_loaded = false;
	if (validate_and_read_custom_map_data(&custom_map_new_entry)) {

		if (remove_duplicates_write_entry_data_and_add(&custom_map_new_entry)) {
			map_loaded = true;
		}
	}

	return map_loaded;
}

void __thiscall s_custom_map_data::initialize()
{
	last_preview_bitmap_index = 0;
	custom_map_count = 0;
	custom_map_lock = new CRITICAL_SECTION;
	InitializeCriticalSection(custom_map_lock);
}

// atexit
void __thiscall s_custom_map_data::cleanup()
{
	new_custom_map_entries_buffer = nullptr;
	if (custom_map_file_data_cache)
		delete[] custom_map_file_data_cache;

	custom_map_file_data_cache = nullptr;

	DeleteCriticalSection(custom_map_lock);
	delete custom_map_lock;
}

static __declspec(naked) void jmp_get_entry_by_id() { __asm jmp s_custom_map_data::get_entry_by_id }
static __declspec(naked) void jmp_load_custom_map_data_cache() { __asm jmp s_custom_map_data::load_custom_map_data_cache }
static __declspec(naked) void jmp_start_custom_map_sync() { __asm jmp s_custom_map_data::start_custom_map_sync }
static __declspec(naked) void jmp_mark_all_cached_maps_for_deletion() { __asm jmp s_custom_map_data::mark_all_cached_maps_for_deletion }
static __declspec(naked) void jmp_find_matching_entries_by_file_path() { __asm jmp s_custom_map_data::find_matching_entries_by_file_path }
static __declspec(naked) void jmp_find_matching_entries_by_map_name_and_hash() { __asm jmp s_custom_map_data::find_matching_entries_by_map_name_and_hash }
static __declspec(naked) void jmp_delete_single_entry() { __asm jmp s_custom_map_data::delete_single_entry }
static __declspec(naked) void jmp_remove_duplicates_and_add_entry() { __asm jmp s_custom_map_data::remove_duplicates_and_add_entry }
static __declspec(naked) void jmp_save_custom_map_data() { __asm jmp s_custom_map_data::save_custom_map_data }
static __declspec(naked) void jmp_remove_marked_for_deletion() { __asm jmp s_custom_map_data::remove_marked_for_deletion }
static __declspec(naked) void jmp_add_entry() { __asm jmp s_custom_map_data::add_entry }
static __declspec(naked) void jmp_remove_duplicates_write_entry_data_and_add() { __asm jmp s_custom_map_data::remove_duplicates_write_entry_data_and_add }
static __declspec(naked) void jmp_cleanup() { __asm jmp s_custom_map_data::cleanup }
static __declspec(naked) void jmp_initialize() { __asm jmp s_custom_map_data::initialize }
static __declspec(naked) void jmp_get_custom_map_list_ids_by_map_name() { __asm jmp s_custom_map_data::get_custom_map_list_ids_by_map_name }

// custom map selection list code
class c_custom_game_custom_map_list
{
public:

	c_custom_game_custom_map_list* __thiscall constructor_hook(int a2)
	{
		typedef c_custom_game_custom_map_list*(__thiscall* original_constructor_t)(c_custom_game_custom_map_list*, int);
		auto p_original_constructor = Memory::GetAddressRelative<original_constructor_t>(0x65AE3B);

		// execute first part of the function
		p_original_constructor(this, a2);
		// then load the map list
		load_custom_map_list();
		return this;
	}

	void __thiscall load_custom_map_list()
	{
		// here we replace the custom map list allocator
		DWORD thisptr = (DWORD)this;

		typedef void(__thiscall* sub_6113D3_t)(int* thisptr, DWORD* a2);
		auto p_sub_6113D3 = Memory::GetAddressRelative<sub_6113D3_t>(0x6113D3);

		s_data_array** custom_map_menu_list = (s_data_array**)(thisptr + 112);

		// first we get the map count
		s_custom_map_id* map_ids_buffer;
		unsigned int custom_map_available_count = getCustomMapData()->get_custom_map_list_ids(nullptr, 0);

		if (custom_map_available_count > 0)
		{
			*custom_map_menu_list = c_list_widget::allocate_list_data("custom game custom maps", custom_map_available_count, sizeof(s_custom_map_id));
			s_data_array::data_make_valid(*custom_map_menu_list);
			map_ids_buffer = new s_custom_map_id[custom_map_available_count];
			getCustomMapData()->get_custom_map_list_ids(map_ids_buffer, custom_map_available_count);
		}
		else
		{
			map_ids_buffer = nullptr;
			*custom_map_menu_list = nullptr;
		}

		if (custom_map_available_count > 0)
		{
			for (int i = custom_map_available_count - 1; i >= 0; i--)
			{
				s_custom_map_id* list_entry = (s_custom_map_id*)((*custom_map_menu_list)->data + (sizeof(s_custom_map_id) * DATUM_INDEX_TO_ABSOLUTE_INDEX(s_data_array::datum_new_in_range(*custom_map_menu_list))));
				memcpy(list_entry, &map_ids_buffer[i], sizeof(s_custom_map_id));
			}
		}

		// addDebugText("c_custom_game_custom_map_list - custom map list count: %d", (*custom_map_menu_list)->total_elements_used);

		if (map_ids_buffer != nullptr)
			delete[] map_ids_buffer;

		if (thisptr == 0xFFFFF814)
			p_sub_6113D3((int*)(thisptr + 172), 0);
		else
			p_sub_6113D3((int*)(thisptr + 172), (DWORD*)(thisptr + 2032));
	}
};

static __declspec(naked) void jmp_c_custom_game_custom_map_list_constructor_hook() { __asm jmp c_custom_game_custom_map_list::constructor_hook }

void s_custom_map_data::ApplyCustomMapExtensionLimitPatches()
{
	WriteJmpTo(Memory::GetAddressRelative(0x44CF41, 0x441819), jmp_get_entry_by_id);
	WriteJmpTo(Memory::GetAddressRelative(0x44CC30, 0x490356), jmp_load_custom_map_data_cache);
	WriteJmpTo(Memory::GetAddressRelative(0x4C19B4, 0x48F0F5), jmp_mark_all_cached_maps_for_deletion);
	WriteJmpTo(Memory::GetAddressRelative(0x4C1D65, 0x48F4A6), jmp_find_matching_entries_by_file_path);
	WriteJmpTo(Memory::GetAddressRelative(0x4C1C1F, 0x48F360), jmp_find_matching_entries_by_map_name_and_hash);
	WriteJmpTo(Memory::GetAddressRelative(0x4C2669, 0x48FDAA), jmp_delete_single_entry);
	WriteJmpTo(Memory::GetAddressRelative(0x44CDA6, 0x441677), jmp_remove_duplicates_and_add_entry);
	WriteJmpTo(Memory::GetAddressRelative(0x4C2D4D, 0x49048E), jmp_save_custom_map_data);
	WriteJmpTo(Memory::GetAddressRelative(0x4C27F3, 0x48FF34), jmp_remove_marked_for_deletion);
	WriteJmpTo(Memory::GetAddressRelative(0x4C259B, 0x48FCDC), jmp_add_entry);
	WriteJmpTo(Memory::GetAddressRelative(0x44CD1E, 0x4415EF), jmp_remove_duplicates_write_entry_data_and_add);

	PatchCall(Memory::GetAddressRelative(0x439E56, 0x40BAB8), jmp_start_custom_map_sync);

	// custom map data menu list hook/patches
	if (!Memory::IsDedicatedServer())
	{
		PatchCall(Memory::GetAddressRelative(0x64F708), jmp_c_custom_game_custom_map_list_constructor_hook);

		// jump to function end just before updating custom map list
		// jmp near 0xB7
		BYTE jmp_to_epilog_sub_65AE3B[] = { 0xE9, 0xB7, 0x00, 0x00, 0x00 };
		WriteBytes(Memory::GetAddressRelative(0x65AEFD), jmp_to_epilog_sub_65AE3B, sizeof(jmp_to_epilog_sub_65AE3B));
	}
	else
	{
		WriteJmpTo(Memory::GetAddressRelative(0x0, 0x4418F9), jmp_get_custom_map_list_ids_by_map_name);
	}

	WriteJmpTo(Memory::GetAddressRelative(0x4C18BD, 0x48EFFE), jmp_initialize);

	// replace game's atexit cleanup impl
	WriteJmpTo(Memory::GetAddressRelative(0x79A0DC, 0x73E9E5), jmp_cleanup);

	PatchCall(Memory::GetAddress(0x1E49A2, 0x1EDF0), validate_and_read_custom_map_data);
	PatchCall(Memory::GetAddress(0x4D3BA, 0x417FE), validate_and_read_custom_map_data);
	PatchCall(Memory::GetAddress(0x4CF26, 0x41D4E), validate_and_read_custom_map_data);
	PatchCall(Memory::GetAddress(0x8928, 0x1B6482), validate_and_read_custom_map_data);

	WriteJmpTo(Memory::GetAddress(0x1467, 0x12E2), scenario_is_supported_build);
}

// '50 map limit removal' region end
#pragma endregion