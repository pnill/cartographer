
#include "stdafx.h"
#include "CustomMapDataCache.h"

#include "Blam\Engine\DataArray\DataArray.h"
#include "Blam\Engine\FileSystem\FiloInterface.h"

#include "Util\Hooks\Hook.h"

#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"

#pragma region 50 map limit removal

// TODO: consider replacing this implementation with a linked list

const wchar_t* custom_map_cache_filename_client = L"mapset.h2mdat";
const wchar_t* custom_map_cache_filename_server = L"mapsetdedi.h2mdat";

s_custom_map_data* getCustomMapData()
{
	auto customMapCacheData = Memory::GetAddress<s_custom_map_data*>(0x482D70, 0x4A70D8);
	return customMapCacheData;
}

const wchar_t* getCustomMapFolderPath()
{
	return getCustomMapData()->custom_maps_folder_path;
}

int get_path_from_id(e_directory_id id, LPCWSTR pMore, LPWSTR pszPath, char is_folder)
{
	typedef int(__cdecl* get_path_from_id)(e_directory_id id, LPCWSTR pMore, LPWSTR pszPath, char is_folder);
	auto p_get_directory_path_by_id = Memory::GetAddressRelative<get_path_from_id>(0x48EF9E, 0x0); //TODO DEDI 
	return p_get_directory_path_by_id(id, pMore, pszPath, is_folder);
}

// TODO move to util or some other place
static void* system_heap_alloc(SIZE_T dwBytes)
{
	return HeapAlloc(GetProcessHeap(), 0, dwBytes);
}

void s_custom_map_data::create_custom_map_data_directory()
{
	auto p__create_custom_map_data_directory = Memory::GetAddressRelative<void(__cdecl*)()>(0x4C1FD4, 0x0);
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

	bool removed_elements = false;

	for (int i = 0; i < custom_map_count; i++)
	{
		if (new_custom_map_entries_buffer[i].entry_marked_for_deletion)
		{
			remove_entry_by_index(i);
			removed_elements = true;
		}
	}

	LeaveCriticalSection(custom_map_lock);

	return removed_elements;
}


bool s_custom_map_data::read_custom_map_data_cache_from_file(const char* path, s_custom_map_file_cache* custom_map_data_cache, DWORD custom_map_data_cache_buffer_size)
{
	filo cache_file;

	bool success = false;
	bool invalid_file = false;
	DWORD open_file_error_code;
	const __int16 filo_open_file_flags = 0
		| FILO_FILE_SEQUENTIAL_SCAN
		| FILO_READ_FILE
		;

	FiloInterface::filo_init(&cache_file, path, false);
	if (FiloInterface::open(&cache_file, filo_open_file_flags, &open_file_error_code))
	{
		// first we read the header
		if (FiloInterface::read(&cache_file, custom_map_data_cache, sizeof(s_custom_map_file_cache), true))
		{
			if (custom_map_data_cache->signature == custom_map_cache_signature)
			{
				// if signature matches, read the cache contents
				DWORD file_custom_map_entries_size = custom_map_data_cache->entries_count * sizeof(s_custom_map_entry);

				if (FiloInterface::read(&cache_file, custom_map_data_cache->entries, file_custom_map_entries_size, true))
				{
					// clear unused custom map entries
					memset((BYTE*)(&custom_map_data_cache->entries) + file_custom_map_entries_size, 0, custom_map_data_cache_buffer_size - file_custom_map_entries_size);
					success = true;
				}
				else
				{
					LOG_TRACE_GAME("{} - failed reading cache file!",
						__FUNCTION__);

					success = false;
				}
			}
			else
			{
				LOG_TRACE_GAME("{} - invalid fmap cache file signature! deleting file if possible",
					__FUNCTION__);

				invalid_file = true;
				success = false;
			}
		}
		else
		{
			success = false;
			invalid_file = true;
			LOG_TRACE_GAME("{} - failed to read custom map data cache file header!",
				__FUNCTION__);
		}
	}
	else
	{
		LOG_TRACE_GAME("{} - failed to open custom map data cache file!",
			__FUNCTION__);

		success = false;
	}

	if (open_file_error_code == FILO_OPEN_FILE_ERROR_SUCCESS)
	{
		FiloInterface::close(&cache_file);
		if (invalid_file && open_file_error_code != FILO_OPEN_FILE_FILE_NOT_FOUND)
			FiloInterface::delete_existing(&cache_file);
	}

	return success == true ? true : false;
}

bool s_custom_map_data::write_custom_map_data_cache_to_file(const char* path, s_custom_map_file_cache* custom_map_data_cache)
{
	filo cache_file;

	bool success = false;
	DWORD open_file_error_code = FILO_OPEN_FILE_UNKONWN;
	const __int16 filo_open_file_flags = 0
		| FILO_WRITE_FILE
		;

	FiloInterface::filo_init(&cache_file, path, false);
	if (FiloInterface::create_file(&cache_file) && FiloInterface::open(&cache_file, filo_open_file_flags, &open_file_error_code))
	{
		FiloInterface::set_file_attribute_hidden(&cache_file, true);
		// first we read the header
		if (FiloInterface::write(&cache_file, custom_map_data_cache, sizeof(s_custom_map_file_cache) + sizeof(s_custom_map_entry) * custom_map_data_cache->entries_count))
		{
			success = true;
		}
		else
		{
			LOG_TRACE_GAME("{} - failed to write map cache to file!",
				__FUNCTION__);
			success = false;
		}
	}
	else
	{
		success = false;
		LOG_TRACE_GAME("{} - failed to open map cache file!",
			__FUNCTION__);
	}

	if (open_file_error_code == FILO_OPEN_FILE_ERROR_SUCCESS)
	{
		FiloInterface::close(&cache_file);
	}

	return success;
}

void __thiscall s_custom_map_data::save_custom_map_data()
{
	WCHAR path_wide[MAX_PATH];
	CHAR path_multibyte[MAX_PATH];
	bool custom_map_data_path_available = false;
	s_custom_map_file_cache* custom_map_data_to_save = custom_map_data_cache;

	custom_map_data_to_save->signature = custom_map_cache_signature;
	custom_map_data_to_save->entries_count = custom_map_count;
	custom_map_data_to_save->last_preview_bitmap_index = field_8;

	typedef int(__cdecl* get_path_from_id)(int id, LPCWSTR pMore, LPWSTR pszPath, char is_folder);
	auto p_get_directory_path_by_id = Memory::GetAddressRelative<get_path_from_id>(0x48EF9E, 0x0); //TODO DEDI OFFSET
	if (p_get_directory_path_by_id(e_directory_id::custom_map_data_dir, L"\0", path_wide, true))
	{
		custom_map_data_path_available = true;
		wcscat(path_wide, custom_map_cache_filename_client);
	}

	if (!custom_map_data_path_available
		|| !WideCharToMultiByte(CP_UTF8, 0, path_wide, -1, path_multibyte, MAX_PATH, 0, 0)
		|| !write_custom_map_data_cache_to_file(path_multibyte, custom_map_data_to_save))
	{
		LOG_CRITICAL_GAME("{} - failed to save custom map data cache!", __FUNCTION__);
	}
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

	// TODO read cache from file, not registry
	WCHAR path_wide[MAX_PATH];
	CHAR path_multibyte[MAX_PATH];
	bool custom_map_data_path_available = false;

	DWORD map_file_cache_buffer_size = sizeof(s_custom_map_file_cache) + sizeof(s_custom_map_entry) * NEW_MAP_LIMIT;
	s_custom_map_file_cache* custom_map_file_cache = (s_custom_map_file_cache*)new char[map_file_cache_buffer_size];

	// first create the custom map data directory
	create_custom_map_data_directory();

	typedef int(__cdecl* get_path_from_id)(int id, LPCWSTR pMore, LPWSTR pszPath, char is_folder);
	auto p_get_directory_path_by_id = Memory::GetAddressRelative<get_path_from_id>(0x48EF9E, 0x0); //TODO DEDI OFFSET
	if (p_get_directory_path_by_id(e_directory_id::custom_map_data_dir, L"\0", path_wide, true))
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
	}

	//load_map_data_cache_from_file_cache(custom_map_file_cache);

	this->custom_map_data_cache = custom_map_file_cache;
	this->field_8 = custom_map_file_cache->last_preview_bitmap_index;
	this->new_custom_map_entries_buffer = custom_map_file_cache->entries; // we just use the buffer allocated previously
	this->custom_map_count = custom_map_file_cache->entries_count;

	LeaveCriticalSection(custom_map_lock);
}

void __thiscall s_custom_map_data::start_custom_map_sync()
{
	// this code doesn't need any modifications, just some functions replaced inside it
	typedef void(__thiscall* file_iterate_custom_map_directory_def)(s_custom_map_data* thisx);
	auto p_file_iterate_custom_map_directory = Memory::GetAddress<file_iterate_custom_map_directory_def>(0x4D021, 0x419B5);
	return p_file_iterate_custom_map_directory(this);
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
				memcpy(out_ids[i].map_sha256_hash, new_custom_map_entries_buffer[i].map_sha256_hash, 32);
				wcsncpy(out_ids[i].map_name, new_custom_map_entries_buffer[i].map_name, 32);
			}
		}
	}

	unsigned int map_count = this->custom_map_count;
	LeaveCriticalSection(custom_map_lock);

	return map_count;
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
		if (!_wcsnicmp(new_custom_map_entries_buffer[i].file_path, file_path, 256))
		{
			new_custom_map_entries_buffer[i].entry_marked_for_deletion = false;
			if (out_custom_map_entries)
			{
				if (out_custom_map_entries_count > matching_count_found)
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
		if (!memcmp(new_custom_map_entries_buffer[i].map_sha256_hash, hash, 32))
		{
			new_custom_map_entries_buffer[i].entry_marked_for_deletion = false;
			if (out_custom_map_entries)
			{
				if (out_custom_map_entries_count > matching_count_found)
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
		if (!memcmp(new_custom_map_entries_buffer[i].map_sha256_hash, sha256_hash, 32) && !_wcsnicmp(new_custom_map_entries_buffer[i].map_name, map_name, 32))
		{
			new_custom_map_entries_buffer[i].entry_marked_for_deletion = false;
			if (out_custom_map_entries)
			{
				if (out_custom_map_entries_count > matching_count_found)
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
		if (!_wcsnicmp(new_custom_map_entries_buffer[i].map_name, map_name, 32))
		{
			new_custom_map_entries_buffer[i].entry_marked_for_deletion = false;
			if (out_custom_map_entries)
			{
				if (out_custom_map_entries_count > matching_count_found)
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
	DWORD* flags_array = Memory::GetAddress<DWORD*>(0x46d820);
	// custom_scenario_test_map_name_instead_of_hash
	if (flags_array[15])
		return find_matching_entries_by_map_name(custom_map_id->map_name, out_entry, 1) != 0;
	else
		return find_matching_entries_by_sha256_hash(custom_map_id->map_sha256_hash, out_entry, 1) != 0;
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
		memmove(&new_custom_map_entries_buffer[idx], &new_custom_map_entries_buffer[idx + 1], (custom_map_count - 1 - idx) * sizeof(s_custom_map_entry));
	}

	custom_map_count--;
}

bool __thiscall s_custom_map_data::remove_entries_matching_file_path(const s_custom_map_entry* entry)
{
	EnterCriticalSection(custom_map_lock);

	bool removed = false;
	for (int i = 0; i < custom_map_count; i++)
	{
		if (!_wcsnicmp(new_custom_map_entries_buffer[i].file_path, entry->file_path, 256))
		{
			removed = true;
			remove_entry_by_index(i);
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
		if (!memcmp(new_custom_map_entries_buffer[i].map_sha256_hash, entry->map_sha256_hash, 32) 
			&& !_wcsnicmp(new_custom_map_entries_buffer[i].map_name, entry->map_name, 32))
		{
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

	// TODO add validation
	memcpy(&new_custom_map_entries_buffer[custom_map_count++], entry, sizeof(s_custom_map_entry));

	LeaveCriticalSection(custom_map_lock);
	return true;
}

bool __thiscall s_custom_map_data::remove_duplicates_and_add_entry(const s_custom_map_entry* entry)
{
	remove_entries_matching_file_path(entry);
	remove_duplicates_by_map_name_and_hash(entry);
	return add_entry(entry);
}

// atexit
void __thiscall s_custom_map_data::cleanup()
{
	new_custom_map_entries_buffer = nullptr;
	if (custom_map_data_cache)
		delete custom_map_data_cache;

	custom_map_data_cache = nullptr;

	DeleteCriticalSection(custom_map_lock);

	//free(custom_map_lock);
}

static __declspec(naked) void jmp_get_entry_by_id() { __asm jmp s_custom_map_data::get_entry_by_id }
static __declspec(naked) void jmp_load_custom_map_data_cache() { __asm jmp s_custom_map_data::load_custom_map_data_cache }
static __declspec(naked) void jmp_file_iterate_custom_map_directory() { __asm jmp s_custom_map_data::start_custom_map_sync }
static __declspec(naked) void jmp_mark_all_cached_maps_for_deletion() { __asm jmp s_custom_map_data::mark_all_cached_maps_for_deletion }
static __declspec(naked) void jmp_find_matching_entries_by_file_path() { __asm jmp s_custom_map_data::find_matching_entries_by_file_path }
static __declspec(naked) void jmp_find_matching_entries_by_map_name_and_hash() { __asm jmp s_custom_map_data::find_matching_entries_by_map_name_and_hash }
static __declspec(naked) void jmp_delete_single_entry() { __asm jmp s_custom_map_data::delete_single_entry }
static __declspec(naked) void jmp_remove_duplicates_and_add_entry() { __asm jmp s_custom_map_data::remove_duplicates_and_add_entry }
static __declspec(naked) void jmp_save_custom_map_data() { __asm jmp s_custom_map_data::save_custom_map_data }
static __declspec(naked) void jmp_cleanup() { __asm jmp s_custom_map_data::cleanup }
static __declspec(naked) void jmp_remove_marked_for_deletion() { __asm jmp s_custom_map_data::remove_marked_for_deletion }

class c_custom_game_custom_map_list
{
public:

	void __thiscall constructor_hook(int a2)
	{
		typedef void(__thiscall* original_constructor)(c_custom_game_custom_map_list* thisptr, int a2);
		auto p_original_constructor = Memory::GetAddressRelative<original_constructor>(0x65AE3B);

		// execute first part of the function
		// then load the map list
		p_original_constructor(this, a2);
		load_custom_map_list();
	}

	void __thiscall load_custom_map_list()
	{
		// here we replace the custom map list allocator
		DWORD thisptr = (DWORD)this;

		typedef s_data_array* (__cdecl* menu_list_allocate)(const char* data_name, int total_elements, int size);
		auto p_menu_list_allocate = Memory::GetAddressRelative<menu_list_allocate>(0x60D1FD);

		typedef int(__thiscall* sub_6113D3)(int* thisptr, DWORD* a2);
		auto p_sub_6113D3 = Memory::GetAddressRelative<sub_6113D3>(0x6113D3);

		s_data_array** custom_map_menu_list = (s_data_array**)(thisptr + 112);

		// first we get the map count
		s_custom_map_id* map_ids_buffer;
		unsigned int custom_map_available_count = getCustomMapData()->get_custom_map_list_ids(nullptr, 0);

		if (custom_map_available_count > 0)
		{
			*custom_map_menu_list = p_menu_list_allocate("custom game custom maps", custom_map_available_count, sizeof(s_custom_map_id));
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

		addDebugText("c_custom_game_custom_map_list - custom map list count: %d", (*custom_map_menu_list)->total_elements_used);

		if (map_ids_buffer != nullptr)
			delete[] map_ids_buffer;

		if (thisptr == 0xFFFFF814)
			p_sub_6113D3((int*)(thisptr + 172), 0);
		else
			p_sub_6113D3((int*)(thisptr + 172), (DWORD*)(thisptr + 2032));
	}
};

static __declspec(naked) void jmp_c_custom_game_custom_map_list_constructor_hook() { __asm jmp c_custom_game_custom_map_list::constructor_hook }

void s_custom_map_data::applyCustomMapExtensionLimitPatches()
{
	// first we replace the `lea` load effective address instruction with mov move instruction
	// so instead of getting the address of the static 50 limit buffer, we read the pointer to a new heap allocated buffer from that point
	// this allows us to re-use the game's code without entirely rewriting it
	// but at some point we might as well just re-write to increase the performance of the code
	{
		// sub_4C1A5A - gets `s_custom_map_entry` entries matching the map_name
		BYTE opcode_buf[] = { 0x8B }; // lea     esi, [edi+10h] -> mov     esi, [edi+10h]
		WriteBytes(Memory::GetAddressRelative(0x4C1A91, 0x0), opcode_buf, sizeof(opcode_buf));

		// sub_4C1AF0 - gets `s_custom_map_entry` entries matching the map file sha256 hash
		WriteBytes(Memory::GetAddressRelative(0x4C1B3D, 0x0), opcode_buf, sizeof(opcode_buf));

		// sub_4C1D65 - gets `s_custom_map_entry` entries matching the map file location path
		WriteBytes(Memory::GetAddressRelative(0x4C1D9C, 0x0), opcode_buf, sizeof(opcode_buf));

		WriteBytes(Memory::GetAddressRelative(0x4C1C6C, 0x0), opcode_buf, sizeof(opcode_buf));

		// too hard to fully modify, we just re-write this
		// WriteBytes(Memory::GetAddressRelative(0x4C26D5, 0x0), opcode_buf, sizeof(opcode_buf));
	}

	// TODO DEDI OFFSETS
	WriteJmpTo(Memory::GetAddressRelative(0x44CF41, 0x0), jmp_get_entry_by_id);
	WriteJmpTo(Memory::GetAddressRelative(0x44CC30, 0x0), jmp_load_custom_map_data_cache);
	WriteJmpTo(Memory::GetAddressRelative(0x4C19B4, 0x0), jmp_mark_all_cached_maps_for_deletion);
	WriteJmpTo(Memory::GetAddressRelative(0x4C1D65, 0x0), jmp_find_matching_entries_by_file_path);
	WriteJmpTo(Memory::GetAddressRelative(0x4C1C1F, 0x0), jmp_find_matching_entries_by_map_name_and_hash);
	WriteJmpTo(Memory::GetAddressRelative(0x4C2669, 0x0), jmp_delete_single_entry);
	WriteJmpTo(Memory::GetAddressRelative(0x44CDA6, 0x0), jmp_remove_duplicates_and_add_entry);
	WriteJmpTo(Memory::GetAddressRelative(0x4C2D4D, 0x0), jmp_save_custom_map_data);
	WriteJmpTo(Memory::GetAddressRelative(0x4C27F3, 0x0), jmp_remove_marked_for_deletion);

	// custom map data menu list hook/patches
	if (!Memory::isDedicatedServer())
	{
		PatchCall(Memory::GetAddressRelative(0x64F708), jmp_c_custom_game_custom_map_list_constructor_hook);

		// jump to function end just before updating custom map list
		// jmp near 0xB7
		BYTE jmp_to_end[] = { 0xE9, 0xB7, 0x00, 0x00, 0x00 };
		WriteBytes(Memory::GetAddressRelative(0x65AEFD), jmp_to_end, sizeof(jmp_to_end));

		// replace game's atexit cleanup impl
		WriteJmpTo(Memory::GetAddressRelative(0x79A0DC), jmp_cleanup);
	}
}

// '50 map limit removal' region end
#pragma endregion