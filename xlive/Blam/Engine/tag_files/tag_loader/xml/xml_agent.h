#pragma once
#include "xml_definition_block.h"
#include "tag_files/tag_groups.h"

class c_xml_definition_agent
{
private:
	e_tag_group type;
	c_xml_definition_block m_definition;

public:
	c_xml_definition_agent(tag_group type, const char* plugin_path);
	c_xml_definition_agent() = default;
	~c_xml_definition_agent();
	void init(tag_group type, const char* plugin_path);
	c_xml_definition_block* get_definition();
};
