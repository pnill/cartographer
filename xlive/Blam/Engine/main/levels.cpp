#include "stdafx.h"
#include "levels.h"

#include "cache/cache_files.h"
#include "filesys/pc_file_system.h"
#include "main/level_definitions.h"
#include "scenario/scenario_definitions.h"

/* prototypes */

static bool map_file_read(FILE* map_handle, uint32 file_size, uint32 file_offset, uint32 read_size, void* out_buffer);

static bool map_file_get_scenario_offset(FILE* map_handle, uint32 file_size, const cache_file_header* cache_header, uint32* out_scenario_offset);

static bool map_file_get_first_block_element_offset(FILE* map_handle, uint32 file_size, const cache_file_header* cache_header, uint32 block_offset, uint32* out_element_offset);

static bool map_file_tag_address_to_offset(const cache_file_header* cache_header, uint32 tag_address, uint32* out_file_offset);

// unlocks all single player maps
int32 __cdecl get_last_single_player_level_id_unlocked_from_profile(void);

int32 __cdecl get_last_single_player_level_id_unlocked_from_profile(void)
{
	return 805; // return the id of the last level
}

s_multiplayer_ui_level_definition* __cdecl levels_get_multiplayer_ui_level(int32 map_id)
{
	return INVOKE(0x5997C, 0x0, levels_get_multiplayer_ui_level, map_id);
}

s_campaign_ui_level_definition* __cdecl levels_get_campaign_ui_level(int campaign_id, int map_id)
{
	return INVOKE(0x5948C, 0x0, levels_get_campaign_ui_level, campaign_id, map_id);
}

bool __cdecl levels_get_custom_map_ui_level_definition(int32 custom_map_id, void* out_level_definition)
{
	return INVOKE(0x59369, 0x0, levels_get_custom_map_ui_level_definition, custom_map_id, out_level_definition);
}

s_campaign_runtime_level_definition* __cdecl levels_get_runtime_campaign_map(int32 campaign_id, int32 map_id)
{
	return INVOKE(0x59526, 0x0, levels_get_runtime_campaign_map, campaign_id, map_id);
}


bool levels_read_multiplayer_level_data_from_map_file(FILE* map_handle, s_multiplayer_ui_level_definition* out_level_data)
{
	ASSERT(map_handle);
	ASSERT(out_level_data);

	if (fseek(map_handle, 0, SEEK_END) != 0)
	{
		return false;
	}
	const long file_size = ftell(map_handle);
	if (file_size <= 0)
	{
		return false;
	}

	cache_file_header cache_header;
	if (!map_file_read(map_handle, file_size, 0, sizeof(cache_header), &cache_header)
		|| cache_header.header_signature != 'head'
		|| cache_header.footer_signature != 'foot'
		|| cache_header.version != 8)
	{
		return false;
	}

	uint32 scenario_offset;
	uint32 level_data_offset;
	uint32 multiplayer_level_data_offset;
	return map_file_get_scenario_offset(map_handle, file_size, &cache_header, &scenario_offset)
		&& map_file_get_first_block_element_offset(map_handle, file_size, &cache_header, scenario_offset + offsetof(scenario, level_data), &level_data_offset)
		&& map_file_get_first_block_element_offset(map_handle, file_size, &cache_header, level_data_offset + offsetof(s_scenario_level_data, multiplayer), &multiplayer_level_data_offset)
		&& map_file_read(map_handle, file_size, multiplayer_level_data_offset, sizeof(*out_level_data), out_level_data);
}

void levels_apply_patches(void)
{
	PatchCall(Memory::GetAddress(0x2422C8), get_last_single_player_level_id_unlocked_from_profile);
	return;
}

/* private code */

static bool map_file_read(FILE* map_handle, uint32 file_size, uint32 file_offset, uint32 read_size, void* out_buffer)
{
	if (file_offset > file_size || read_size > file_size - file_offset)
	{
		return false;
	}

	file_seek_and_read(map_handle, file_offset, read_size, 1, out_buffer);
	return true;
}

// The scenario is found through the tags header, not assumed to be the first tag
static bool map_file_get_scenario_offset(FILE* map_handle, uint32 file_size, const cache_file_header* cache_header, uint32* out_scenario_offset)
{
	cache_file_tags_header tags_header;
	if (!map_file_read(map_handle, file_size, cache_header->tag_offset, sizeof(tags_header), &tags_header))
	{
		return false;
	}

	const uint16 scenario_absolute_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(tags_header.scenario_index);
	if (tags_header.scenario_index == NONE || scenario_absolute_index >= tags_header.tag_count)
	{
		return false;
	}

	const uint32 instance_table_offset = cache_header->tag_offset + sizeof(tags_header) + sizeof(s_tag_group_link) * tags_header.tag_group_link_set_count;
	cache_file_tag_instance scenario_instance;
	if (!map_file_read(map_handle, file_size, instance_table_offset + sizeof(scenario_instance) * scenario_absolute_index, sizeof(scenario_instance), &scenario_instance)
		|| scenario_instance.group_tag != _tag_group_scenario
		|| scenario_instance.size < sizeof(scenario))
	{
		return false;
	}

	return map_file_tag_address_to_offset(cache_header, scenario_instance.data_offset, out_scenario_offset);
}

static bool map_file_get_first_block_element_offset(FILE* map_handle, uint32 file_size, const cache_file_header* cache_header, uint32 block_offset, uint32* out_element_offset)
{
	s_tag_block block;
	if (!map_file_read(map_handle, file_size, block_offset, sizeof(block), &block) || block.count <= 0)
	{
		return false;
	}

	return map_file_tag_address_to_offset(cache_header, block.data, out_element_offset);
}

// Tag data addresses of a cache file start at tag_offset_mask, the tag section starts at tag_offset in the file
static bool map_file_tag_address_to_offset(const cache_file_header* cache_header, uint32 tag_address, uint32* out_file_offset)
{
	const uint32 tag_base_address = (uint32)cache_header->tag_offset_mask;
	if (tag_address == (uint32)NONE || tag_address < tag_base_address)
	{
		return false;
	}

	*out_file_offset = cache_header->tag_offset + (tag_address - tag_base_address);
	return true;
}
