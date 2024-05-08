#include "stdafx.h"
#include "xml_agent.h"

#include "xml_definition_block.h"
#include "tag_files/tag_groups.h"
#include "tinyxml/tinyxml2.h"


c_xml_definition_agent::c_xml_definition_agent(tag_group type, const char* plugin_path)
{
	this->init(type, plugin_path);
}

c_xml_definition_agent::~c_xml_definition_agent()
{
	this->m_definition.clear();
}

void c_xml_definition_agent::init(tag_group type, const char* plugin_path)
{
	tinyxml2::XMLDocument document;
	tinyxml2::XMLError error = document.LoadFile(plugin_path);
	this->type = type.group;
	if (error != tinyxml2::XML_SUCCESS)
	{
		char err_buff[MAX_PATH + 40];
		sprintf(err_buff, "[xml_definition_agent] failed to load plugin %s for type %s", plugin_path, type.string);
		LOG_ERROR_GAME("{}", err_buff);
		throw err_buff;
	}

	tinyxml2::XMLElement* root = document.RootElement();
	uint32 root_size = strtoul(root->Attribute("baseSize"), nullptr, 16);

	this->m_definition = c_xml_definition_block(root, 0, root_size);

#if K_TAG_INJECTION_DEBUG
	this->print_definition();
#endif
}

c_xml_definition_block* c_xml_definition_agent::get_definition()
{
	return &this->m_definition;
}

#if K_TAG_INJECTION_DEBUG

void c_xml_definition_agent::print_definition_internal(c_xml_definition_block* definition, uint32 depth)
{
	c_static_string64 pad;
	pad.clear();
	for (uint32 i = 0; i < depth; i++)
		pad.append("\t");

	for (uint32 i = 0; i < definition->get_tag_references_count(); i++)
		LOG_DEBUG_GAME("[print_definition]:{}tag_reference: {} name: {}, offset: {:x}", pad.get_string(), i, definition->get_tag_reference_name(i)->get_string(), definition->get_tag_reference_offset(i));

	for (uint32 i = 0; i < definition->get_classless_tag_references_count(); i++)
		LOG_DEBUG_GAME("[print_definition]:{}classless_tag_reference: {} name: {} offset: {:x}", pad.get_string(), i, definition->get_classless_tag_reference_name(i)->get_string(), definition->get_classless_tag_reference_offset(i));

	for (uint32 i = 0; i < definition->get_data_references_count(); i++)
		LOG_DEBUG_GAME("[print_definition]:{}data_reference: {} name: {} offset: {:x}", pad.get_string(), i, definition->get_data_reference_name(i)->get_string(), definition->get_data_reference_offset(i));

	for (uint32 i = 0; i < definition->get_tag_block_count(); i++)
	{
		LOG_DEBUG_GAME("[print_definition]:{}tag_block: {} name: {} offset: {:x}", pad.get_string(), i, definition->get_tag_block_name(i)->get_string(), definition->get_tag_block(i)->get_offset());
		print_definition_internal(definition->get_tag_block(i), depth + 1);
	}
}

void c_xml_definition_agent::print_definition()
{
	char null_terminated_class[5];
	tag_group ttype;
	ttype.group = this->type;
	null_terminated_class[0] = ttype.string[3];
	null_terminated_class[1] = ttype.string[2];
	null_terminated_class[2] = ttype.string[1];
	null_terminated_class[3] = ttype.string[0];
	null_terminated_class[4] = '\0';
	LOG_DEBUG_GAME("[print_definition]: {}", null_terminated_class);

	for(uint32 i = 0; i < this->m_definition.get_tag_references_count(); i++)
		LOG_DEBUG_GAME("[print_definition]: tag_reference: {} name: {}, offset: {:x}", i, this->m_definition.get_tag_reference_name(i)->get_string(), this->m_definition.get_tag_reference_offset(i));

	for (uint32 i = 0; i < this->m_definition.get_classless_tag_references_count(); i++)
		LOG_DEBUG_GAME("[print_definition]: classless_tag_reference: {} name: {} offset: {:x}", i, this->m_definition.get_classless_tag_reference_name(i)->get_string(), this->m_definition.get_classless_tag_reference_offset(i));

	for (uint32 i = 0; i < this->m_definition.get_data_references_count(); i++)
		LOG_DEBUG_GAME("[print_definition]: data_reference: {} name: {} offset: {:x}", i, this->m_definition.get_data_reference_name(i)->get_string(), this->m_definition.get_data_reference_offset(i));

	for(uint32 i = 0; i < this->m_definition.get_tag_block_count(); i++)
	{
		LOG_DEBUG_GAME("[print_definition]: tag_block: {} name: {} offset: {:x}", i, this->m_definition.get_tag_block_name(i)->get_string(), this->m_definition.get_tag_block(i)->get_offset());
		this->print_definition_internal(this->m_definition.get_tag_block(i), 1);
	}
}

#endif
