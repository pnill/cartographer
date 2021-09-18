#include "TagInterface.h"

tags::tag_instance* tags::get_tag_instances()
{
	return *Memory::GetAddress<tag_instance**>(0x47CD50, 0x4A29B8);
}

char* tags::get_tag_data()
{
	return *Memory::GetAddress<char**>(0x47CD54, 0x4A29BC);
}

char* tags::get_matg_globals_ptr()
{
	return *Memory::GetAddress<char**>(0x479E70, 0x4A642C);
}

tags::cache_header* tags::get_cache_header()
{
	return Memory::GetAddress<tags::cache_header*>(0x47CD68, 0x4A29D0);
}

HANDLE tags::get_cache_handle()
{
	return *Memory::GetAddress<HANDLE*>(0x4AE8A8, 0x4CF128);
}

bool tags::cache_file_loaded()
{
	return *Memory::GetAddress<bool*>(0x47CD60, 0x4A29C8);
}

static char* tag_debug_names = nullptr;
static std::map<uint32_t, const char*> tag_datum_name_map;

void clear_tag_debug_names()
{
	tag_datum_name_map.clear();
	delete[] tag_debug_names;
}

bool tags::load_tag_debug_name()
{
	clear_tag_debug_names();

	// need to load a cache file first
	if (!cache_file_loaded())
		return false;

	HANDLE old_file_handle = get_cache_handle();
	if (old_file_handle == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR_FUNC("Cache file handle is invalid??!");
		return false;
	}

	// reopen the file so we don't have to use async operations
	HANDLE cache_handle = ReOpenFile(old_file_handle, FILE_GENERIC_READ, FILE_SHARE_READ, FILE_FLAG_SEQUENTIAL_SCAN);
	if (!LOG_CHECK(cache_handle != INVALID_HANDLE_VALUE))
		return false;

	cache_header* header = get_cache_header();

	size_t name_buffer_offset = header->TagNamesBufferOffset;
	size_t name_buffer_size = header->TagNamesBufferSize;
	size_t tag_indices_offset = header->TagIndicesToName;
	size_t tag_count = header->TagNamesCount;

	if (name_buffer_offset == 0 || name_buffer_size == 0 || tag_indices_offset == 0 || tag_count == 0
		|| (name_buffer_offset + name_buffer_size) > header->file_size || (tag_indices_offset + (tag_count * 4)) > header->file_size)
	{
		LOG_ERROR_FUNC("Debug tag names data in cache seems to be invalid?");
		CloseHandle(cache_handle);
		return false;
	}

	tag_debug_names = new char[name_buffer_size];
	uint32_t* tag_name_offsets = new uint32_t[tag_count];

	DWORD size_read; // maybe this should be checked?
	SetFilePointer(cache_handle, name_buffer_offset, NULL, FILE_BEGIN);
	ReadFile(cache_handle, tag_debug_names, name_buffer_size, &size_read, NULL);

	SetFilePointer(cache_handle, tag_indices_offset, NULL, FILE_BEGIN);
	ReadFile(cache_handle, tag_name_offsets, tag_count * 4, &size_read, NULL);

	for (size_t i = 0; i < tag_count; i++)
	{
		size_t name_offset = tag_name_offsets[i];
		if (name_offset == 0xFFFFFFFF) // either there is no such tag or it wasn't added to map
			continue;

		if (name_offset >= name_buffer_size)
		{
			LOG_ERROR_FUNC("Out of bounds of debug name buffer!, corrupt map?");
			delete[] tag_name_offsets;
			CloseHandle(cache_handle);
			return false;
		}

		tag_datum_name_map[i] = &tag_debug_names[name_offset];
	}

	delete[] tag_name_offsets;
	CloseHandle(cache_handle);
	return true;
}

std::string tags::get_tag_name(datum tag_index)
{
	auto ilter = tag_datum_name_map.find(DATUM_ABSOLUTE_INDEX(tag_index));
	if (ilter != tag_datum_name_map.end())
		return ilter->second;
	LOG_INFO_FUNC("Tag name not found?, this shouldn't happen.");
	return "tag name lost"; // tool does something similar if it can't find the name of a tag from the shared cache
}

datum tags::find_tag(blam_tag type, const std::string& name)
{
	for (auto& it = tag_datum_name_map.begin(); it != tag_datum_name_map.end(); it++)
	{
		if (_strnicmp(name.c_str(), it->second, 256) == 0)
		{
			auto instance = tags::get_tag_instances()[it->first];
			if (is_tag_or_parent_tag(instance.type, type))
				return index_to_datum(it->first);
		}
	}
	return DATUM_NONE;
}

std::map<datum, std::string> tags::find_tags(blam_tag type)
{
	std::map<datum, std::string> result;
	for (auto& it = tag_datum_name_map.begin(); it != tag_datum_name_map.end(); it++)
	{
		auto instance = tags::get_tag_instances()[it->first];
		if (is_tag_or_parent_tag(instance.type, type)) {
			result.emplace(index_to_datum(it->first), std::string(it->second));
		}
	}
	return result;
}

static std::vector<void(*)()> load_callbacks;
void tags::on_map_load(void(*callback)())
{
	load_callbacks.push_back(callback);
}

void __cdecl tags::run_callbacks()
{
	// load debug names before any callbacks are called
	load_tag_debug_name();
	for (auto callback : load_callbacks)
		callback();
}