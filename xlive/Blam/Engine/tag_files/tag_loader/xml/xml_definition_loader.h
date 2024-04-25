#pragma once
#include "xml_definition_block.h"
#include "cache/cache_files.h"
#include "H2MOD/Tags/TagInterface.h"

struct s_offset_link
{
	uint32 cache_offset;
	uint32 memory_offset;
};

struct s_memory_link : s_offset_link
{
	int8* data;
	uint32 size;
};

class c_xml_definition_loader
{
	bool m_persist;

	int8* m_data;
	uint32 m_used_data;
	uint32 m_total_data_size;
	c_xml_definition_block* m_definition;

	FILE* m_file_handle;

	s_cache_header m_cache_header;
	s_tags_header m_tags_header;

	datum m_cache_index;

	// offset in file to tag data from 0
	uint32 m_file_offset;
	// tag instance loaded from cache
	tags::tag_instance m_instance;
	// scenario tag instance from cache
	// todo: might be removable
	tags::tag_instance m_scenario_instance;

	uint32 m_tag_reference_offset_count;
	s_offset_link* m_tag_reference_offsets;

	uint32 m_classless_tag_reference_offset_count;
	s_offset_link* m_classless_tag_reference_offsets;

	uint32 m_tag_reference_count;
	datum* m_tag_references;

	uint32 m_data_reference_offset_count;
	s_memory_link* m_data_reference_offsets;

	uint32 m_string_id_offset_count;
	s_offset_link* m_string_id_offsets;

	uint32 m_tag_block_offset_count;
	s_memory_link* m_tag_block_offsets;

	void load_cache_info();
	uint32 resolve_cache_tag_data_offset(uint32 offset) const;
	void reset_counts();
	void clear() const;
	static void initialize_arrays_internal(c_xml_definition_loader* loader, const c_xml_definition_block* definition, uint32 file_offset);
	void initialize_arrays();
	int8* reserve_data(uint32 size);
	static void load_tag_data_internal(c_xml_definition_loader* loader, c_xml_definition_block* definition, uint32 file_offset, int8* buffer, uint32 block_count);
	void load_tag_data();
	void calculate_total_data_size(const c_xml_definition_block* definition, uint32 base_offset);
public:
	c_xml_definition_loader(c_xml_definition_block* definition, FILE* file_handle, datum cache_index);
	void validate_data() const;

	bool is_persisted() const;
	void set_persistence(bool state);
};

