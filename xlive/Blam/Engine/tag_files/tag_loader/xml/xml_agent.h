#pragma once
#include "xml_definition_block.h"
#include "tag_files/tag_groups.h"

class c_xml_definition_agent
{
	tag_group m_type;
	c_xml_definition_block m_definition;

	c_static_string260 m_plugin_path;
	tinyxml2::XMLDocument m_document;

public:
	c_xml_definition_agent(tag_group type, const char* plugin_path);
	~c_xml_definition_agent();
	c_xml_definition_block* get_definition();
};
