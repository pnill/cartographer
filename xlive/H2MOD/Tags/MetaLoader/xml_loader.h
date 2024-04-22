#pragma once
#include "cache/cache_files.h"
#include "cseries/cseries_strings.h"
#include "tag_files/tag_groups.h"
#include "tinyxml/tinyxml2.h"


class c_xml_definition_block
{
	tinyxml2::XMLElement* m_element;
	uint32 m_offset;
	uint32 m_size;

	uint32 m_tag_reference_count;
	uint32* m_tag_references;

	uint32 m_classless_tag_reference_count;
	uint32* m_classless_tag_references;

	uint32 m_data_reference_count;
	uint32* m_data_references;

	uint32 m_string_id_count;
	uint32* m_string_ids;

	uint32 m_tag_block_count;
	c_xml_definition_block* m_tag_blocks;

public:
	c_xml_definition_block(tinyxml2::XMLElement* base_element, uint32 offset, uint32 size);
	c_xml_definition_block() = default;
	~c_xml_definition_block();
	void reset_counts();
	void clear();

	uint32 get_size() const;
	uint32 get_offset() const;

	uint32 get_tag_references_count() const;
	uint32 get_tag_reference_offset(uint32 index) const;

	uint32 get_classless_tag_references_count() const;
	uint32 get_classless_tag_reference_offset(uint32 index) const;

	uint32 get_data_references_count() const;
	uint32 get_data_reference_offset(uint32 index) const;

	uint32 get_string_id_count() const;
	uint32 get_string_id_offset(uint32 index) const;

	uint32 get_tag_block_count() const;
	c_xml_definition_block* get_tag_block(uint32 index) const;
};

class c_xml_definition_loader
{
	int8* m_data;
	uint32 m_total_data_size;
	c_xml_definition_block* m_definition;

	FILE* m_file_handle;
	

	s_cache_header m_cache_header;
	s_tags_header m_tags_header;
	datum m_cache_index;
	uint32 m_file_offset;
	uint32 resolve_cache_tag_data_offset(uint32 offset);
public:
	c_xml_definition_loader(c_xml_definition_block* definition, FILE* file_handle, datum cache_index);
	void calculate_total_data_size(const c_xml_definition_block* definition, uint32 base_offset);
	void load_tag_data();
};

class c_xml_definition_agent
{
	tag_group m_type;
	c_xml_definition_block m_definition;

	c_static_string260 m_plugin_path;
	tinyxml2::XMLDocument m_document;

public:
	c_xml_definition_agent(tag_group type, const char* plugin_path);
	~c_xml_definition_agent();
};
