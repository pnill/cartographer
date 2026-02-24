#include "stdafx.h"
#include "map_repository.h"

#include "cache/cache_files.h"
#include "interface/signal_slot.h"
#include "interface/user_interface_memory.h"
#include "memory/data.h"
#include "multithreading/synchronization.h"
#include "networking/network_event.h"
#include "scenario/scenario_definitions.h"
#include "shell/shell.h"
#include "tag_files/files.h"
#include "text/unicode.h"

/* prototypes */

static int32 get_directory_path_from_id(e_directory_id id, LPCWSTR pMore, LPWSTR pszPath, bool is_folder);

static bool open_cache_header(const wchar_t* file_path, void* cache_header_ptr, HANDLE* map_handle);

static void __cdecl close_cache_header(HANDLE* map_handle);

static bool __cdecl validate_and_read_custom_map_data(s_custom_map_entry* custom_map_entry);


void map_repository_apply_sapien_patches(void)
{
	NopFill(Memory::GetAddress(0x31FB3E), 52);	// Prevent "m_map_count < k_maximum_number_of_multiplayer_maps" assert from triggering
	return;
}

#pragma region 50 map limit removal

// TODO: consider replacing this implementation with a linked list

const wchar_t* custom_map_cache_filename_client = L".h2mdat";

static s_custom_map_entry* get_custom_map_entry_list_from_header(s_custom_map_file_cache_header* header)
{
	return (s_custom_map_entry*)((uint8*)header + sizeof(s_custom_map_file_cache_header));
}

c_map_manager* map_manager_get()
{
	c_map_manager* custom_map_manager = Memory::GetAddress<c_map_manager*>(0x482D70, 0x4A70D8);
	return custom_map_manager;
}

const wchar_t* get_custom_map_folder_path()
{
	return map_manager_get()->m_custom_maps_folder_path;
}

void c_map_manager::load_data(void)
{
	map_repository_load();
	return;
}

void c_map_manager::map_synchronize_process(bool async)
{
	INVOKE_TYPE(0x4D31F, 0x0, void(__thiscall*)(c_map_manager*, bool), this, async);
	return;
}

void c_map_manager::create_custom_map_data_directory()
{
	auto p__create_custom_map_data_directory = Memory::GetAddressRelative<void(__cdecl*)()>(0x4C1FD4, 0x48F715);
	p__create_custom_map_data_directory();
}


CLASS_HOOK_DECLARE_LABEL(c_custom_map_manager__mark_all_cached_maps_for_deletion, c_map_manager::mark_all_cached_maps_for_deletion);
void __thiscall c_map_manager::mark_all_cached_maps_for_deletion()
{
	c_critical_section_scope lock(m_lock);

	if (m_new_custom_map_entry_list_buffer == nullptr)
	{
		return;
	}

	for (uint16 i = 0; i < m_map_count; i++)
		m_new_custom_map_entry_list_buffer[i].entry_marked_for_deletion = true;

}

CLASS_HOOK_DECLARE_LABEL(c_custom_map_manager__remove_marked_for_deletion, c_map_manager::remove_marked_for_deletion);
bool __thiscall c_map_manager::remove_marked_for_deletion()
{
	c_critical_section_scope lock(m_lock);

	bool maps_removed = false;
	uint16 map_count_before_remove = m_map_count;

	for (uint16 i = 0; i < m_map_count; )
	{
		if (m_new_custom_map_entry_list_buffer[i].entry_marked_for_deletion)
		{
			remove_entry_by_index(i);
		}
		else
		{
			i++;
		}
	}

	maps_removed = map_count_before_remove > m_map_count;
	return maps_removed;
}

bool c_map_manager::read_custom_map_data_cache_from_file(const utf8* path, 
	s_custom_map_file_cache_header* custom_map_data_header, 
	uint32 custom_map_data_cache_buffer_size
)
{
	s_file_reference cache_file;

	bool success = false;
	bool invalid_file = false;
	e_file_open_error open_file_error_code;

	file_reference_create_from_path(&cache_file, path, false);
	e_file_open_flags flags = (e_file_open_flags)(_file_open_sequential_scan_bit | _permission_read_bit);

	// ### TODO FIXME clean this up
	uint32 entry_count = 0;
	s_custom_map_entry* entry_list = get_custom_map_entry_list_from_header(custom_map_data_header);

	do 
	{
		if (!file_open(&cache_file, flags, &open_file_error_code))
		{
			event(
				_event_verbose,
				"h2mod:map_repository: %s - failed to open custom map data cache file!",
				__FUNCTION__
			);
			break;
		}

		// first we read the header
		if (!file_read(&cache_file, sizeof(s_custom_map_file_cache_header), true, custom_map_data_header))
		{
			invalid_file = true;
			event(
				_event_verbose,
				"h2mod:map_repository: %s - failed to read custom map data cache file header!",
				__FUNCTION__
			);
			break;
		}

		if (custom_map_data_header->signature != custom_map_cache_signature)
		{
			event(
				_event_verbose,
				"h2mod:map_repository: %s - invalid custom map data cache file signature! deleting file if possible, name is reserved!",
				__FUNCTION__
			);
			invalid_file = true;
			break;
		}

		if (custom_map_data_header->version != k_custom_map_data_cache_header_version)
		{
			event(
				_event_verbose,
				"h2mod:map_repository: %s - custom map data cache file header version mismatch! deleting file if possible",
				__FUNCTION__
			);
			invalid_file = true;
			break;
		}

		entry_count = custom_map_data_header->entry_count;
		if (entry_count > k_maximum_number_of_custom_multiplayer_maps_new)
		{
			event(
				_event_verbose,
				"h2mod:map_repository: %s - custom map data cache file exceeds new map limit size, {} > max: {}",
				__FUNCTION__,
				entry_count,
				k_maximum_number_of_custom_multiplayer_maps_new
			);
			break;
		}

		// if signature matches, read the cache contents
		uint32 file_custom_map_entries_size = entry_count * sizeof(s_custom_map_entry);

		if (!file_read(&cache_file, file_custom_map_entries_size, true, entry_list))
		{
			event(
				_event_error,
				"h2mod:map_repository: %s - failed reading custom map data cache file!",
				__FUNCTION__
			);
			break;
		}

		// clear unused custom map entries
		size_t remaining_size = custom_map_data_cache_buffer_size - sizeof(s_custom_map_file_cache_header) - file_custom_map_entries_size;
		csmemset((uint8*)entry_list + file_custom_map_entries_size, 0, remaining_size);
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
		event(_event_verbose, "h2mod:map_repository loaded custom map paths: ");
		for (uint32 i = 0; i < entry_count; i++)
		{
			event(
				_event_verbose,
				"h2mod:map_repository:	custom map path: %ws",
				entry_list[i].file_path
			);
		}
	}

	return success;
}

bool c_map_manager::write_custom_map_data_cache_to_file(const utf8* path, s_custom_map_file_cache_header* custom_map_data_cache)
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
			event(
				_event_error,
				"h2mod:map_repository: %s - failed to open custom map cache file, error code: %u",
				__FUNCTION__,
				(uint32)open_file_error_code
			);
			break;
		}

		file_set_hidden(&cache_file, true);
		
		uint32 size_to_write = sizeof(s_custom_map_file_cache_header) + sizeof(s_custom_map_entry) * custom_map_data_cache->entry_count;

		if (!file_write(&cache_file, size_to_write, custom_map_data_cache))
		{
			event(
				_event_error,
				"h2mod:map_repository: %s - failed to write custom map cache to file!",
				__FUNCTION__
			);
			break;
		}

		event(
			_event_verbose,
			"h2mod:map_repository: %s - saved custom map data cache, map count: %u, write size: %u",
			__FUNCTION__,
			custom_map_data_cache->entry_count,
			size_to_write
		);

		if (!file_set_eof(&cache_file, cache_file.api_result))
		{
			event(
				_event_error,
				"h2mod:map_repository: %s - failed to set custom map cache file size!",
				__FUNCTION__
			);
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
		event(
			_event_error,
			"h2mod:map_repository: %s - failed to create custom map data cache while file not present!",
			__FUNCTION__
		);
	}

	return success;
}

CLASS_HOOK_DECLARE_LABEL(c_custom_map_manager__save_custom_map_data, c_map_manager::save_custom_map_data);
void __thiscall c_map_manager::save_custom_map_data()
{
	wchar_t path_wide[MAX_PATH];
	char path_multibyte[MAX_PATH];
	bool custom_map_data_path_available = false;

	c_critical_section_scope lock(m_lock);

	s_custom_map_file_cache_header* custom_map_data_to_save = m_custom_map_file_data_cache;

	custom_map_data_to_save->signature = custom_map_cache_signature;
	custom_map_data_to_save->entry_count = m_map_count;
	custom_map_data_to_save->index_of_last_previewed_bitmap = m_index_of_last_previewed_bitmap;

	if (get_directory_path_from_id(e_directory_id::_custom_map_data_dir, L"\0", path_wide, true))
	{
		custom_map_data_path_available = true;
		ustrncat(path_wide, custom_map_cache_filename_client, NUMBEROF(path_wide));
	}

	if (!custom_map_data_path_available
		|| !WideCharToMultiByte(CP_UTF8, 0, path_wide, -1, path_multibyte, MAX_PATH, 0, 0)
		|| !write_custom_map_data_cache_to_file(path_multibyte, custom_map_data_to_save))
	{
		event(
			_event_error,
			"h2mod:map_repository: %s - failed to save custom map data cache!",
			__FUNCTION__
		);
	}
}

void c_map_manager::load_map_data_cache_from_file_cache(s_custom_map_file_cache_header* custom_map_file_cache)
{

}

CLASS_HOOK_DECLARE_LABEL(c_custom_map_manager__load_custom_map_data_cache, c_map_manager::map_repository_load);
void __thiscall c_map_manager::map_repository_load()
{
	//typedef void(__thiscall* cache_custom_map_file_image_preview)(c_map_manager* thisx);
	//auto p_cache_custom_map_file_image_previews = Memory::GetAddress<cache_custom_map_file_image_preview>(0x4CC30, 0x41501);
	//return p_cache_custom_map_file_image_previews(this);

	c_critical_section_scope lock(m_lock);

	wchar_t path_wide[MAX_PATH];
	char path_multibyte[MAX_PATH];
	bool custom_map_data_path_available = false;

	uint32 map_file_cache_buffer_size = sizeof(s_custom_map_file_cache_header) + sizeof(s_custom_map_entry) * k_maximum_number_of_custom_multiplayer_maps_new;
	s_custom_map_file_cache_header* custom_map_cache_file_header = (s_custom_map_file_cache_header*)new char[map_file_cache_buffer_size];

	// first create the custom map data directory (if not created already)
	create_custom_map_data_directory();

	if (get_directory_path_from_id(e_directory_id::_custom_map_data_dir, L"\0", path_wide, true))
	{
		custom_map_data_path_available = true;
		ustrncat(path_wide, custom_map_cache_filename_client, NUMBEROF(path_wide));
	}

	if (!custom_map_data_path_available
		|| !WideCharToMultiByte(CP_UTF8, 0, path_wide, -1, path_multibyte, MAX_PATH, 0, 0)
		|| !read_custom_map_data_cache_from_file(path_multibyte, custom_map_cache_file_header, map_file_cache_buffer_size))
	{
		// if we fail clear the buffer
		csmemset(custom_map_cache_file_header, 0, map_file_cache_buffer_size);
		custom_map_cache_file_header->signature = custom_map_cache_signature;
		custom_map_cache_file_header->version = k_custom_map_data_cache_header_version;

		event(
			_event_error,
			"h2mod:map_repository: %s - failed to read custom map data cache!",
			__FUNCTION__
		);
	}

	//load_map_data_cache_from_file_cache(custom_map_cache_file_header);

	if (m_custom_map_file_data_cache != nullptr)
		delete[] m_custom_map_file_data_cache;

	this->m_custom_map_file_data_cache = custom_map_cache_file_header;
	this->m_map_count = (uint16)custom_map_cache_file_header->entry_count;
	this->m_new_custom_map_entry_list_buffer	
		= get_custom_map_entry_list_from_header(custom_map_cache_file_header);

	this->m_index_of_last_previewed_bitmap = custom_map_cache_file_header->index_of_last_previewed_bitmap;
}

CLASS_HOOK_DECLARE_LABEL(c_custom_map_manager__start_custom_map_sync, c_map_manager::start_map_synchronize);
void __thiscall c_map_manager::start_map_synchronize(void)
{
	INVOKE_TYPE(0x4D021, 0x419B5, void(__thiscall*)(c_map_manager*), this);
	return;
}

uint32 __thiscall c_map_manager::get_custom_map_list_ids(s_secure_map_id* out_ids, uint32 out_ids_count)
{
	c_critical_section_scope lock(m_lock);
	for (uint16 i = 0; i < m_map_count; i++)
	{
		m_new_custom_map_entry_list_buffer[i].entry_marked_for_deletion = false;
		if (out_ids != nullptr
			&& out_ids_count != 0)
		{
			if (i < out_ids_count)
			{
				csmemcpy(out_ids[i].hash, m_new_custom_map_entry_list_buffer[i].hash, k_sha256_hash_size_bytes);
				ustrncpy(out_ids[i].map_name, m_new_custom_map_entry_list_buffer[i].map_name, k_custom_map_name_length);
			}
		}
	}

	uint32 map_count = this->m_map_count;
	return map_count;
}

CLASS_HOOK_DECLARE_LABEL(c_custom_map_manager__get_custom_map_list_ids_by_map_name, c_map_manager::get_custom_map_list_ids_by_map_name);
uint32 __thiscall c_map_manager::get_custom_map_list_ids_by_map_name(const wchar_t* map_name, s_secure_map_id* out_ids, uint32 out_ids_count)
{
	c_critical_section_scope lock(m_lock);

	uint32 matching_count_found = 0;

	// used by dedicated server
	for (uint16 i = 0; i < m_map_count; i++)
	{
		if (!_wcsnicmp(m_new_custom_map_entry_list_buffer[i].map_name, map_name, k_custom_map_name_length))
		{
			m_new_custom_map_entry_list_buffer[i].entry_marked_for_deletion = false;
			if (out_ids != nullptr
				&& out_ids_count != 0)
			{
				if (matching_count_found < out_ids_count)
				{
					csmemcpy(out_ids[matching_count_found].hash, m_new_custom_map_entry_list_buffer[i].hash, k_sha256_hash_size_bytes);
					ustrncpy(out_ids[matching_count_found].map_name, m_new_custom_map_entry_list_buffer[i].map_name, k_custom_map_name_length);
				}
			}

			matching_count_found++;
		}
	}

	return matching_count_found;
}

// TODO de-duplicate custom map entry matching code
// because just for 1 line of code we re-write same code over and over

CLASS_HOOK_DECLARE_LABEL(c_custom_map_manager__find_matching_entries_by_file_path, c_map_manager::find_matching_entries_by_file_path);
uint32 __thiscall c_map_manager::find_matching_entries_by_file_path(const wchar_t* file_path, s_custom_map_entry** out_custom_map_entries, uint32 out_custom_map_entries_count)
{
	c_critical_section_scope lock(m_lock);

	if (m_new_custom_map_entry_list_buffer == nullptr
		|| file_path == nullptr)
	{
		return 0;
	}

	uint32 matching_count_found = 0;

	for (uint16 i = 0; i < m_map_count; i++)
	{
		if (!_wcsnicmp(m_new_custom_map_entry_list_buffer[i].file_path, file_path, k_map_file_path_size))
		{
			m_new_custom_map_entry_list_buffer[i].entry_marked_for_deletion = false;
			if (out_custom_map_entries)
			{
				if (matching_count_found < out_custom_map_entries_count)
					out_custom_map_entries[matching_count_found] = &m_new_custom_map_entry_list_buffer[i];
			}

			matching_count_found++;
		}
	}
	return matching_count_found;
}

uint32 __thiscall c_map_manager::find_matching_entries_by_sha256_hash(const BYTE* hash, s_custom_map_entry** out_custom_map_entries, uint32 out_custom_map_entries_count)
{
	c_critical_section_scope lock(m_lock);

	if (m_new_custom_map_entry_list_buffer == nullptr
		|| hash == nullptr)
	{
		return 0;
	}

	uint32 matching_count_found = 0;

	for (uint16 i = 0; i < m_map_count; i++)
	{
		if (!memcmp(m_new_custom_map_entry_list_buffer[i].hash, hash, k_sha256_hash_size_bytes))
		{
			m_new_custom_map_entry_list_buffer[i].entry_marked_for_deletion = false;
			if (out_custom_map_entries)
			{
				if (matching_count_found < out_custom_map_entries_count)
					out_custom_map_entries[matching_count_found] = &m_new_custom_map_entry_list_buffer[i];
			}

			matching_count_found++;
		}
	}
	return matching_count_found;
}

CLASS_HOOK_DECLARE_LABEL(c_custom_map_manager__find_matching_entries_by_map_name_and_hash, c_map_manager::find_matching_entries_by_map_name_and_hash);
uint32 __thiscall c_map_manager::find_matching_entries_by_map_name_and_hash(const wchar_t* map_name, const BYTE* sha256_hash, s_custom_map_entry** out_custom_map_entries, uint32 out_custom_map_entries_count)
{
	c_critical_section_scope lock(m_lock);
	if (m_new_custom_map_entry_list_buffer == nullptr
		|| map_name == nullptr)
	{
		return 0;
	}

	uint32 matching_count_found = 0;

	for (uint16 i = 0; i < m_map_count; i++)
	{
		if (!memcmp(m_new_custom_map_entry_list_buffer[i].hash, sha256_hash, k_sha256_hash_size_bytes) && !_wcsnicmp(m_new_custom_map_entry_list_buffer[i].map_name, map_name, k_custom_map_name_length))
		{
			m_new_custom_map_entry_list_buffer[i].entry_marked_for_deletion = false;
			if (out_custom_map_entries)
			{
				if (matching_count_found < out_custom_map_entries_count)
					out_custom_map_entries[matching_count_found] = &m_new_custom_map_entry_list_buffer[i];
			}

			matching_count_found++;
		}
	}
	return matching_count_found;
}

uint32 __thiscall c_map_manager::find_matching_entries_by_map_name(const wchar_t* map_name, s_custom_map_entry** out_custom_map_entries, uint32 out_custom_map_entries_count)
{
	c_critical_section_scope lock(m_lock);

	if (m_new_custom_map_entry_list_buffer == nullptr
		|| map_name == nullptr)
	{
		return 0;
	}

	uint32 matching_count_found = 0;

	for (uint16 i = 0; i < m_map_count; i++)
	{
		if (!_wcsnicmp(m_new_custom_map_entry_list_buffer[i].map_name, map_name, k_custom_map_name_length))
		{
			m_new_custom_map_entry_list_buffer[i].entry_marked_for_deletion = false;
			if (out_custom_map_entries)
			{
				if (matching_count_found < out_custom_map_entries_count)
					out_custom_map_entries[matching_count_found] = &m_new_custom_map_entry_list_buffer[i];
			}

			matching_count_found++;
		}
	}

	return matching_count_found;
}

CLASS_HOOK_DECLARE_LABEL(c_custom_map_manager__get_entry_by_id, c_map_manager::get_entry_by_id);
bool __thiscall c_map_manager::get_entry_by_id(const s_secure_map_id* custom_map_id, s_custom_map_entry** out_entry)
{
	// custom_scenario_test_map_name_instead_of_hash
	if (shell_command_line_flag_is_set(_shell_command_line_flag_custom_map_entry_test_map_name_instead_of_hash))
		return find_matching_entries_by_map_name(custom_map_id->map_name, out_entry, 1) != 0;
	else
		return find_matching_entries_by_sha256_hash(custom_map_id->hash, out_entry, 1) != 0;
}

bool __thiscall c_map_manager::entry_is_duplicate(const s_custom_map_entry* entry)
{
	c_critical_section_scope lock(m_lock);

	bool duplicate = false;

	if (find_matching_entries_by_file_path(entry->file_path, 0, 0) > 0u
		|| find_matching_entries_by_map_name_and_hash(entry->map_name, entry->hash, 0, 0) > 0u)
	{
		duplicate = true;
	}

	return duplicate;
}

bool __thiscall c_map_manager::validate_entry_data(const s_custom_map_entry* entries, uint32 count)
{
	typedef bool(__thiscall* validate_entry_data_t)(c_map_manager*, const s_custom_map_entry* entry, uint32 count);
	auto p_validate_entry_data = Memory::GetAddressRelative<validate_entry_data_t>(0x4C1E01, 0x48F542);
	return p_validate_entry_data(this, entries, count * sizeof(s_custom_map_entry));
}

void __thiscall c_map_manager::remove_entry_by_index(uint16 idx)
{
	// check if the element to be deleted is the last one
	// if so just memset to 0
	if (idx == m_map_count - 1)
	{
		csmemset(&m_new_custom_map_entry_list_buffer[idx], 0, sizeof(s_custom_map_entry));
	}
	else
	{
		// otherwise just move the memory from the next element to last in the now empty entry
		// TODO: consider replacing this implementation with a linked list
		// this might get very expensive
		// but it shouldn't execute too many times
		csmemmove(&m_new_custom_map_entry_list_buffer[idx], &m_new_custom_map_entry_list_buffer[idx + 1], (m_map_count - 1 - idx) * sizeof(s_custom_map_entry));
	}

	m_map_count--;
}

bool __thiscall c_map_manager::remove_entries_matching_file_path(const s_custom_map_entry* entry)
{
	c_critical_section_scope lock(m_lock);

	bool removed = false;
	for (uint16 i = 0; i < m_map_count; )
	{
		if (!_wcsnicmp(m_new_custom_map_entry_list_buffer[i].file_path, entry->file_path, k_map_file_path_size))
		{
			removed = true;
			remove_entry_by_index(i);
		}
		else
		{
			i++;
		}
	}

	return removed;
}

bool __thiscall c_map_manager::remove_duplicates_by_map_name_and_hash(const s_custom_map_entry* entry)
{
	c_critical_section_scope lock(m_lock);

	bool removed = false;
	for (uint16 i = 0; i < m_map_count; i++)
	{
		if (!memcmp(m_new_custom_map_entry_list_buffer[i].hash, entry->hash, k_sha256_hash_size_bytes) 
			&& !_wcsnicmp(m_new_custom_map_entry_list_buffer[i].map_name, entry->map_name, k_custom_map_name_length))
		{
			//LOG_TRACE_GAME(L"{} - removed: {} from cache matching cached entry: map name: {} map path: {}",
				//__FUNCTIONW__, entry->file_path, new_custom_map_entries_buffer[i].map_name, new_custom_map_entries_buffer[i].file_path);
			remove_entry_by_index(i);
			removed = true;
		}
	}

	return removed;
}

CLASS_HOOK_DECLARE_LABEL(c_custom_map_manager__delete_single_entry, c_map_manager::delete_single_entry);
bool __thiscall c_map_manager::delete_single_entry(const s_custom_map_entry* entry)
{
	c_critical_section_scope lock(m_lock);

	bool removed = false;

	for (uint16 i = 0; i < m_map_count; i++)
	{
		if (!memcmp(&m_new_custom_map_entry_list_buffer[i], entry, sizeof(s_custom_map_entry)))
		{
			remove_entry_by_index(i);
			removed = true;
			break;
		}
	}

	return removed;
}

CLASS_HOOK_DECLARE_LABEL(c_custom_map_manager__add_entry, c_map_manager::add_entry);
bool __thiscall c_map_manager::add_entry(const s_custom_map_entry* entry)
{
	c_critical_section_scope lock(m_lock);

	bool success = true;

	if (m_map_count < k_maximum_number_of_custom_multiplayer_maps_new && validate_entry_data(entry, 1) && !entry_is_duplicate(entry))
	{
		csmemcpy(&m_new_custom_map_entry_list_buffer[m_map_count++], entry, sizeof(s_custom_map_entry));
	}
	else
	{
		success = false;
	}

	return success;
}

CLASS_HOOK_DECLARE_LABEL(c_custom_map_manager__remove_duplicates_and_add_entry, c_map_manager::remove_duplicates_and_add_entry);
bool __thiscall c_map_manager::remove_duplicates_and_add_entry(const s_custom_map_entry* entry)
{
	remove_entries_matching_file_path(entry);
	remove_duplicates_by_map_name_and_hash(entry);
	return add_entry(entry);
}

// compared to the other function
// this reads the file and populates the map entry datas
CLASS_HOOK_DECLARE_LABEL(c_custom_map_manager__remove_duplicates_write_entry_data_and_add, c_map_manager::remove_duplicates_write_entry_data_and_add);
bool __thiscall c_map_manager::remove_duplicates_write_entry_data_and_add(s_custom_map_entry* entry)
{
	remove_entries_matching_file_path(entry);
	remove_duplicates_by_map_name_and_hash(entry);

	typedef bool(__thiscall* read_map_data_and_add_entry_t)(c_map_manager*, s_custom_map_entry* entry);
	auto p_read_map_data_and_add_entry = Memory::GetAddressRelative<read_map_data_and_add_entry_t>(0x44CC35);

	return p_read_map_data_and_add_entry(this, entry);
}

bool __thiscall c_map_manager::add_custom_map_entry_by_map_file_path(const std::wstring& file_path)
{
	return add_custom_map_entry_by_map_file_path(file_path.c_str());
}

bool __thiscall c_map_manager::add_custom_map_entry_by_map_file_path(const wchar_t* file_path)
{
	s_custom_map_entry custom_map_new_entry;
	ZeroMemory(&custom_map_new_entry, sizeof(s_custom_map_entry));

	wcscpy_s(custom_map_new_entry.file_path, k_map_file_path_size, file_path);

	bool map_loaded = false;
	if (validate_and_read_custom_map_data(&custom_map_new_entry)) {

		if (remove_duplicates_write_entry_data_and_add(&custom_map_new_entry)) {
			map_loaded = true;
		}
	}

	return map_loaded;
}

CLASS_HOOK_DECLARE_LABEL(c_custom_map_manager__initialize, c_map_manager::initialize);
void __thiscall c_map_manager::initialize()
{
	m_index_of_last_previewed_bitmap = 0;
	m_map_count = 0;
	m_lock = new CRITICAL_SECTION;
	InitializeCriticalSection(m_lock);
}

// atexit
CLASS_HOOK_DECLARE_LABEL(c_custom_map_manager__cleanup, c_map_manager::cleanup);
void __thiscall c_map_manager::cleanup()
{
	m_new_custom_map_entry_list_buffer = nullptr;
	if (m_custom_map_file_data_cache)
		delete[] m_custom_map_file_data_cache;

	m_custom_map_file_data_cache = nullptr;

	DeleteCriticalSection(m_lock);
	delete m_lock;
}

static __declspec(naked) void jmp_get_entry_by_id() 
{ 
	CLASS_HOOK_JMP(c_custom_map_manager__get_entry_by_id, c_map_manager::get_entry_by_id);
}
static __declspec(naked) void jmp_load_custom_map_data_cache()
{
	CLASS_HOOK_JMP(c_custom_map_manager__load_custom_map_data_cache, c_map_manager::map_repository_load);
}
static __declspec(naked) void jmp_start_custom_map_sync()
{
	CLASS_HOOK_JMP(c_custom_map_manager__start_custom_map_sync, c_map_manager::start_map_synchronize);
}
static __declspec(naked) void jmp_mark_all_cached_maps_for_deletion()
{
	CLASS_HOOK_JMP(c_custom_map_manager__mark_all_cached_maps_for_deletion, c_map_manager::mark_all_cached_maps_for_deletion);
}
static __declspec(naked) void jmp_find_matching_entries_by_file_path()
{
	CLASS_HOOK_JMP(c_custom_map_manager__find_matching_entries_by_file_path, c_map_manager::find_matching_entries_by_file_path);
}
static __declspec(naked) void jmp_find_matching_entries_by_map_name_and_hash()
{
	CLASS_HOOK_JMP(c_custom_map_manager__find_matching_entries_by_map_name_and_hash, c_map_manager::find_matching_entries_by_map_name_and_hash);
}
static __declspec(naked) void jmp_delete_single_entry()
{
	CLASS_HOOK_JMP(c_custom_map_manager__delete_single_entry, c_map_manager::delete_single_entry);
}
static __declspec(naked) void jmp_remove_duplicates_and_add_entry()
{
	CLASS_HOOK_JMP(c_custom_map_manager__remove_duplicates_and_add_entry, c_map_manager::remove_duplicates_and_add_entry);
}
static __declspec(naked) void jmp_save_custom_map_data()
{
	CLASS_HOOK_JMP(c_custom_map_manager__save_custom_map_data, c_map_manager::save_custom_map_data);
}
static __declspec(naked) void jmp_remove_marked_for_deletion()
{
	CLASS_HOOK_JMP(c_custom_map_manager__remove_marked_for_deletion, c_map_manager::remove_marked_for_deletion);
}
static __declspec(naked) void jmp_add_entry()
{
	CLASS_HOOK_JMP(c_custom_map_manager__add_entry, c_map_manager::add_entry);
}
static __declspec(naked) void jmp_remove_duplicates_write_entry_data_and_add()
{
	CLASS_HOOK_JMP(c_custom_map_manager__remove_duplicates_write_entry_data_and_add, c_map_manager::remove_duplicates_write_entry_data_and_add);
}
static __declspec(naked) void jmp_cleanup()
{
	CLASS_HOOK_JMP(c_custom_map_manager__cleanup, c_map_manager::cleanup);
}
static __declspec(naked) void jmp_initialize()
{
	CLASS_HOOK_JMP(c_custom_map_manager__initialize, c_map_manager::initialize);
}
static __declspec(naked) void jmp_get_custom_map_list_ids_by_map_name()
{
	CLASS_HOOK_JMP(c_custom_map_manager__get_custom_map_list_ids_by_map_name, c_map_manager::get_custom_map_list_ids_by_map_name);
}

// custom map selection list code
class c_custom_game_custom_map_list // : public c_list_widget
{
public:
	//c_list_item_widget item_widget[14];

	c_custom_game_custom_map_list* constructor_hook(int a2)
	{
		typedef c_custom_game_custom_map_list*(__thiscall* original_constructor_t)(c_custom_game_custom_map_list*, int);
		auto p_original_constructor = Memory::GetAddress<original_constructor_t>(0x25AE3B);

		// execute first part of the function
		p_original_constructor(this, a2);
		// then load the map list

		// here we replace the custom map list allocator
		DWORD thisptr = (DWORD)this;

		data_array** custom_map_menu_list = (data_array**)(thisptr + 112);

		// first we get the map count
		*custom_map_menu_list = nullptr;
		s_secure_map_id* map_ids_buffer = nullptr;
		uint32 custom_map_available_count = map_manager_get()->get_custom_map_list_ids(nullptr, 0);

		if (custom_map_available_count > 0)
		{
			*custom_map_menu_list = ui_list_data_new("custom game custom maps", custom_map_available_count, sizeof(s_secure_map_id));
			data_make_valid(*custom_map_menu_list);
			map_ids_buffer = new s_secure_map_id[custom_map_available_count];
			map_manager_get()->get_custom_map_list_ids(map_ids_buffer, custom_map_available_count);

			// ### FIXME ugly...
			for (uint32 i = custom_map_available_count; i-- > 0; )
			{
				s_secure_map_id* list_entry
					= &((s_secure_map_id*)(*custom_map_menu_list)->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_new(*custom_map_menu_list))];
				csmemcpy(list_entry, &map_ids_buffer[i], sizeof(s_secure_map_id));
			}
		}

		if (map_ids_buffer != nullptr)
			delete[] map_ids_buffer;

		_slot_linker* linker = (_slot_linker*)(thisptr + 172);
		_slot* slot = (_slot*)(thisptr + 2032);
		linker->link(slot);
		return this;
	}
};
//ASSERT_STRUCT_SIZE(c_custom_game_custom_map_list, 3292);

CLASS_HOOK_DECLARE_LABEL(c_custom_game_custom_map_list__constructor_hook, c_custom_game_custom_map_list::constructor_hook);
static __declspec(naked) void jmp_c_custom_game_custom_map_list_constructor_hook()
{
	CLASS_HOOK_JMP(c_custom_game_custom_map_list__constructor_hook, c_custom_game_custom_map_list::constructor_hook);
}

void c_map_manager::ApplyCustomMapExtensionLimitPatches()
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
	if (!shell_is_dedicated_server())
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
}

/* private code */

static int32 get_directory_path_from_id(e_directory_id id, LPCWSTR pMore, LPWSTR pszPath, bool is_folder)
{
	typedef int(__cdecl* t_get_directory_path_from_id)(e_directory_id id, LPCWSTR pMore, LPWSTR pszPath, bool is_folder);
	auto p_get_directory_path_by_id = Memory::GetAddressRelative<t_get_directory_path_from_id>(0x48EF9E, 0x474A15);
	return p_get_directory_path_by_id(id, pMore, pszPath, is_folder);
}

static bool open_cache_header(const wchar_t* file_path, void* cache_header_ptr, HANDLE* map_handle)
{
	typedef char(__cdecl open_cache_header_t)(const wchar_t* file_path, void* lpBuffer, HANDLE* map_handle, DWORD NumberOfBytesRead);
	auto p_open_cache_header = Memory::GetAddress<open_cache_header_t*>(0x642D0, 0x4C327);
	return p_open_cache_header(file_path, cache_header_ptr, map_handle, 0);
}

static void __cdecl close_cache_header(HANDLE* map_handle)
{
	INVOKE(0x64C03, 0x4CC5A, close_cache_header, map_handle);
	return;
}

static bool __cdecl validate_and_read_custom_map_data(s_custom_map_entry* custom_map_entry)
{
	cache_file_header header;
	HANDLE map_cache_handle;
	wchar_t* file_name = custom_map_entry->file_path;
	if (!open_cache_header(file_name, &header, &map_cache_handle))
		return false;
	if (header.header_signature != 'head' || header.footer_signature != 'foot' || header.file_size <= 0 || header.version != 8)
	{
		event(
			_event_error,
			"h2mod:map_repository: \"%ws\" has invalid header",
			file_name
		);
		return false;
	}
	if (header.type > 5 || header.type < 0)
	{
		event(
			_event_error,
			"h2mod:map_repository: \"%ws\" has bad scenario type",
			file_name
		);
		return false;
	}
	if (strnlen_s(header.name, k_custom_map_name_length) >= 32 || strnlen_s(header.version_string, 32) >= 32)
	{
		event(
			_event_error,
			"h2mod:map_repository: \"%ws\" has invalid version or name string",
			file_name
		);
		return false;
	}
	if (header.type != _scenario_type_multiplayer && header.type != _scenario_type_solo)
	{
		event(
			_event_error,
			"h2mod:map_repository: \"%ws\" is not playable",
			file_name
		);
		return false;
	}

	close_cache_header(&map_cache_handle);
	// needed because the game loads the human readable map name and description from scenario after checks
	// without this the map is just called by it's file name

	// todo move the code for loading the descriptions to our code and get rid of this
	typedef int(__cdecl* validate_and_add_custom_map_internal_t)(s_custom_map_entry*);
	auto validate_and_add_custom_map_internal = Memory::GetAddress<validate_and_add_custom_map_internal_t>(0x4F690, 0x56890);
	if (!validate_and_add_custom_map_internal(custom_map_entry))
	{
		event(
			_event_error,
			"h2mod:map_repository: \"%ws\" has bad checksums or is blacklisted, map may not work correctly",
			file_name
		);
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

// '50 map limit removal' region end
#pragma endregion
