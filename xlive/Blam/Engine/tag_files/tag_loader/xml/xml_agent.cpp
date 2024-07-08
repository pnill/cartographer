#include "stdafx.h"
#include "xml_agent.h"

#include "xml_definition_block.h"

#include "tag_files/tag_groups.h"
#include "tinyxml/tinyxml2.h"


/* public code */

c_xml_definition_agent::c_xml_definition_agent(tag_group type, const wchar_t* plugin_path)
{
	this->init(type, plugin_path);
	return;
}

c_xml_definition_agent::~c_xml_definition_agent(void)
{
	this->m_definition.clear();
	return;
}

void c_xml_definition_agent::init(tag_group type, const wchar_t* plugin_path)
{
	FILE* file;
	errno_t error = _wfopen_s(&file, plugin_path, L"rb");
	bool error_occured = error || file == NULL;

	if (!error_occured)
	{
		tinyxml2::XMLDocument document;
		tinyxml2::XMLError error = document.LoadFile(file);
		fclose(file);

		this->m_type = type;

		error_occured = error != tinyxml2::XML_SUCCESS;
		if (!error_occured)
		{
			tinyxml2::XMLElement* root = document.RootElement();
			uint32 root_size = strtoul(root->Attribute("baseSize"), nullptr, 16);

			this->m_definition = c_xml_definition_block(root, 0, root_size);

#if K_TAG_INJECTION_DEBUG
			this->print_definition();
#endif
		}
	}

	// Print out message that says we failed to load the plugin 
	if (error_occured)
	{
		c_xml_definition_agent::log(__FUNCTION__, "failed to load plugin %ls for type %s", plugin_path, type.string);
	}
	
	return;
}

c_xml_definition_block* c_xml_definition_agent::get_definition(void)
{
	return &this->m_definition;
}

void c_xml_definition_agent::log(const char* function_name, const char* format, ...)
{
	va_list va_args;
	va_start(va_args, format);
	
	c_static_string<512> new_format("[%s] ");
	new_format.append(format);

	LOG_DEBUG_GAME(new_format.get_string(), function_name, va_args);
	
	va_end(va_args);
	return;
}

#if K_TAG_INJECTION_DEBUG

void c_xml_definition_agent::print_definition_internal(c_xml_definition_block* definition, uint32 depth)
{
	// Append a tab to the log string for the "depth level" we're in
	c_static_string<64> pad;
	for (uint32 i = 0; i < depth; i++)
	{
		pad.append("\t");
	}

	const char function[] = __FUNCTION__;

	for (uint32 i = 0; i < definition->get_tag_references_count(); i++)
	{
		c_xml_definition_agent::log(
			function,
			"{}tag_reference: {} name: {}, offset: {:x}",
			pad.get_string(),
			i,
			definition->get_tag_reference_name(i)->get_string(),
			definition->get_tag_reference_offset(i));
	}

	for (uint32 i = 0; i < definition->get_classless_tag_references_count(); i++)
	{
		c_xml_definition_agent::log(
			function,
			"{}classless_tag_reference: {} name: {} offset: {:x}",
			pad.get_string(),
			i, 
			definition->get_classless_tag_reference_name(i)->get_string(),
			definition->get_classless_tag_reference_offset(i));
	}

	for (uint32 i = 0; i < definition->get_data_references_count(); i++)
	{
		c_xml_definition_agent::log(
			function,
			"{}data_reference: {} name: {} offset: {:x}",
			pad.get_string(),
			i,
			definition->get_data_reference_name(i)->get_string(),
			definition->get_data_reference_offset(i));
	}

	for (uint32 i = 0; i < definition->get_tag_block_count(); i++)
	{
		c_xml_definition_agent::log(
			function,
			"{} name: {} offset: {:x}",
			pad.get_string(),
			i,
			definition->get_tag_block_name(i)->get_string(),
			definition->get_tag_block(i)->get_offset());
		print_definition_internal(definition->get_tag_block(i), depth + 1);
	}
}

void c_xml_definition_agent::print_definition(void)
{
	char null_terminated_class[5];
	null_terminated_class[0] = this->m_type.string[3];
	null_terminated_class[1] = this->m_type.string[2];
	null_terminated_class[2] = this->m_type.string[1];
	null_terminated_class[3] = this->m_type.string[0];
	null_terminated_class[4] = '\0';

	const char function[] = __FUNCTION__;

	c_xml_definition_agent::log(function, "{}", null_terminated_class);

	for (uint32 i = 0; i < this->m_definition.get_tag_references_count(); i++)
	{
		c_xml_definition_agent::log(
			function,
			"tag_reference: {} name: {}, offset: {:x}",
			i,
			this->m_definition.get_tag_reference_name(i)->get_string(),
			this->m_definition.get_tag_reference_offset(i));
	}

	for (uint32 i = 0; i < this->m_definition.get_classless_tag_references_count(); i++)
	{
		c_xml_definition_agent::log(
			function,
			"classless_tag_reference: {} name: {} offset: {:x}",
			i,
			this->m_definition.get_classless_tag_reference_name(i)->get_string(),
			this->m_definition.get_classless_tag_reference_offset(i));
	}

	for (uint32 i = 0; i < this->m_definition.get_data_references_count(); i++)
	{
		c_xml_definition_agent::log(
			function,
			"data_reference: {} name: {} offset: {:x}",
			i,
			this->m_definition.get_data_reference_name(i)->get_string(),
			this->m_definition.get_data_reference_offset(i));
	}

	for(uint32 i = 0; i < this->m_definition.get_tag_block_count(); i++)
	{
		c_xml_definition_agent::log(
			function,
			"tag_block: {} name: {} offset: {:x}",
			i,
			this->m_definition.get_tag_block_name(i)->get_string(),
			this->m_definition.get_tag_block(i)->get_offset());
		this->print_definition_internal(this->m_definition.get_tag_block(i), 1);
	}
}

#endif
