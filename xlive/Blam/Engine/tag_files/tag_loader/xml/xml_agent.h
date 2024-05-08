#pragma once
#include "xml_definition_block.h"
#include "tag_files/tag_groups.h"
#include "tag_files/tag_loader/tag_injection_define.h"
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

#if K_TAG_INJECTION_DEBUG
	static void print_definition_internal(c_xml_definition_block* definition, uint32 depth);
	void print_definition();
#endif
};
