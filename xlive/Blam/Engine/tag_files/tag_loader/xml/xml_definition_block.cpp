#include "stdafx.h"
#include "xml_definition_block.h"

#include "cseries/cseries_strings.h"

c_xml_definition_block::c_xml_definition_block(tinyxml2::XMLElement* base_element, uint32 offset, uint32 size)
{
	// Checks to make sure the correct parameters were passed.
	// offset and size are default -1 and overwritten if the attribute exists
	// for the given base_element if somehow they are being passed as -1 it will most
	// likely indicate there is an issue in the plugin xml file.
	//ASSERT(offset == UINT_MAX)
	//ASSERT(size == UINT_MAX)

	this->m_element = base_element;
	this->m_offset = offset;
	this->m_size = size;
	this->m_tag_references = nullptr;
	this->m_classless_tag_references = nullptr;
	this->m_string_ids = nullptr;
	this->m_data_references = nullptr;
	this->m_tag_blocks = nullptr;
	this->reset_counts();

	if (this->m_element->Attribute("name"))
		this->m_name.set(this->m_element->Attribute("name"));
	
	// do a first pass of the current element to get the counts we need
	// for allocating the buffers
	this->get_element_counts();
	this->allocate_buffers();

	// counts are reset to be re-used to keep track of which index
	// inside the array the current item should be placed
	this->reset_counts();
	this->populate_buffers();
}

void c_xml_definition_block::reset_counts()
{
	this->m_tag_reference_count = 0;
	this->m_classless_tag_reference_count = 0;
	this->m_string_id_count = 0;
	this->m_data_reference_count = 0;
	this->m_tag_block_count = 0;
}

void c_xml_definition_block::get_element_counts()
{
	tinyxml2::XMLElement* element = this->m_element->FirstChildElement();
	while (element)
	{
		const char* element_name = element->Name();

		if (strcmp(element_name, "tagBlock") == 0 || strcmp(element_name, "reflexive") == 0)
		{
			this->m_tag_block_count++;

			element = element->NextSiblingElement();
			continue;
		}
		if (strcmp(element_name, "tagRef") == 0 || strcmp(element_name, "tagref") == 0)
		{
			if (element->BoolAttribute("withClass") && element->Attribute("withClass"))
				this->m_classless_tag_reference_count++;
			else
				this->m_tag_reference_count++;

			element = element->NextSiblingElement();
			continue;
		}
		if (strcmp(element_name, "stringId") == 0 || strcmp(element_name, "stringid") == 0)
		{
			this->m_string_id_count++;

			element = element->NextSiblingElement();
			continue;
		}
		if (strcmp(element_name, "dataref") == 0)
		{
			this->m_data_reference_count++;

			element = element->NextSiblingElement();
			continue;
		}

		element = element->NextSiblingElement();
	}
}

void c_xml_definition_block::allocate_buffers()
{
	if (this->m_tag_reference_count)
		this->m_tag_references = (uint32*)malloc(sizeof(uint32) * this->m_tag_reference_count);

	if (this->m_classless_tag_reference_count)
		this->m_classless_tag_references = (uint32*)malloc(sizeof(uint32) * this->m_classless_tag_reference_count);

	if (this->m_string_id_count)
		this->m_string_ids = (uint32*)malloc(sizeof(uint32) * this->m_string_id_count);

	if (this->m_data_reference_count)
		this->m_data_references = (uint32*)malloc(sizeof(uint32) * this->m_data_reference_count);

	if (this->m_tag_block_count)
		this->m_tag_blocks = (c_xml_definition_block*)malloc(sizeof(c_xml_definition_block) * this->m_tag_block_count);
}

void c_xml_definition_block::populate_buffers()
{
	tinyxml2::XMLElement* element = this->m_element->FirstChildElement();
	while (element)
	{
		const char* element_name = element->Name();
		uint32 offset = UINT_MAX;
		if (element->Attribute("offset") != nullptr)
			offset = strtoul(element->Attribute("offset"), nullptr, 16);
		if (strcmp(element_name, "tagBlock") == 0 || strcmp(element_name, "reflexive") == 0)
		{
			uint32 size = UINT_MAX;
			if (element->Attribute("entrySize") != nullptr)
				size = strtoul(element->Attribute("entrySize"), nullptr, 16);
			else if (element->Attribute("elementSize") != nullptr)
				size = strtoul(element->Attribute("elementSize"), nullptr, 16);

			new (&this->m_tag_blocks[this->m_tag_block_count]) c_xml_definition_block(element, offset, size);
			this->m_tag_block_count++;

			element = element->NextSiblingElement();
			continue;
		}
		if (strcmp(element_name, "tagRef") == 0 || strcmp(element_name, "tagref") == 0)
		{
			if (element->BoolAttribute("withClass") && element->Attribute("withClass"))
			{
				this->m_classless_tag_references[this->m_classless_tag_reference_count] = offset;
				this->m_classless_tag_reference_count++;
			}
			else
			{
				this->m_tag_references[this->m_tag_reference_count] = offset;
				this->m_tag_reference_count++;
			}

			element = element->NextSiblingElement();
			continue;
		}
		if (strcmp(element_name, "stringId") == 0 || strcmp(element_name, "stringid") == 0)
		{
			this->m_string_ids[this->m_string_id_count] = offset;
			this->m_string_id_count++;

			element = element->NextSiblingElement();
			continue;
		}
		if (strcmp(element_name, "dataref") == 0)
		{
			this->m_data_references[this->m_data_reference_count] = offset;
			this->m_data_reference_count++;

			element = element->NextSiblingElement();
			continue;
		}
		element = element->NextSiblingElement();
	}
}

void c_xml_definition_block::clear()
{
	if (this->m_tag_reference_count)
		free(this->m_tag_references);

	if (this->m_classless_tag_reference_count)
		free(this->m_classless_tag_references);

	if (this->m_string_id_count)
		free(this->m_string_ids);

	if (this->m_tag_reference_count)
		free(this->m_data_references);

	if (this->m_tag_block_count)
	{
		for (uint32 i = 0; i < this->m_tag_block_count; i++)
		{
			this->m_tag_blocks[i].clear();
		}
		free(this->m_tag_blocks);
	}

	this->reset_counts();
}

uint32 c_xml_definition_block::get_size() const
{
	return this->m_size;
}

uint32 c_xml_definition_block::get_offset() const
{
	return this->m_offset;
}

c_static_string64* c_xml_definition_block::get_name()
{
	return &this->m_name;
}

uint32 c_xml_definition_block::get_tag_references_count() const
{
	return this->m_tag_reference_count;
}

uint32 c_xml_definition_block::get_tag_reference_offset(uint32 index) const
{
	return this->m_tag_references[index];
}

uint32 c_xml_definition_block::get_classless_tag_references_count() const
{
	return this->m_classless_tag_reference_count;
}

uint32 c_xml_definition_block::get_classless_tag_reference_offset(uint32 index) const
{
	return this->m_classless_tag_references[index];
}

uint32 c_xml_definition_block::get_data_references_count() const
{
	return this->m_data_reference_count;
}

uint32 c_xml_definition_block::get_data_reference_offset(uint32 index) const
{
	return this->m_data_references[index];
}

uint32 c_xml_definition_block::get_string_id_count() const
{
	return this->m_string_id_count;
}

uint32 c_xml_definition_block::get_string_id_offset(uint32 index) const
{
	return this->m_string_ids[index];
}

uint32 c_xml_definition_block::get_tag_block_count() const
{
	return this->m_tag_block_count;
}

c_xml_definition_block* c_xml_definition_block::get_tag_block(uint32 index) const
{
	return &this->m_tag_blocks[index];
}
