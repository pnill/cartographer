#pragma once
#include "xml_definition_block.h"
#include "tag_files/tag_groups.h"
#include "tag_files/tag_loader/tag_injection_define.h"

/* classes */

class c_xml_definition_agent
{
public:
	c_xml_definition_agent(tag_group type, const wchar_t* plugin_path);
	c_xml_definition_agent(void) = default;
	~c_xml_definition_agent(void);
	void init(tag_group type, const wchar_t* plugin_path);
	c_xml_definition_block* get_definition(void);

	static void log(const char* function_name, const char* format, ...);

#if TAG_INJECTION_DEBUG
	static void print_definition_internal(c_xml_definition_block* definition, uint32 depth);
	void print_definition();
#endif

private:
	tag_group m_type;
	c_xml_definition_block m_definition;
};
