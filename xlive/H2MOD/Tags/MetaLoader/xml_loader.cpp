#include "stdafx.h"
#include "xml_loader.h"

#include "tag_files/data_reference.h"
#include "tag_files/tag_block.h"
#include "tinyxml/tinyxml2.h"



c_xml_definition_block::c_xml_definition_block(tinyxml2::XMLElement* base_element, uint32 offset, uint32 size)
{
	// Checks to make sure the correct parameters were passed.
	// offset and size are default -1 and overwritten if the attribute exists
	// for the given base_element if somehow they are being passed as -1 it will most
	// likely indicate there is an issue in the plugin xml file.
	ASSERT(offset == UINT_MAX)
	ASSERT(size == UINT_MAX)

	this->m_element = base_element;
	this->m_offset = offset;
	this->m_size = size;
	this->m_tag_references = nullptr;
	this->m_classless_tag_references = nullptr;
	this->m_string_ids = nullptr;
	this->m_data_references = nullptr;
	this->m_tag_blocks = nullptr;

	// do a first pass of the current element to get the counts we need
	// for allocating the buffers
	tinyxml2::XMLElement* element = this->m_element->FirstChildElement();
	this->reset_counts();
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
			if (!element->BoolAttribute("withClass"))
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

	if (this->m_tag_reference_count)
		this->m_tag_references = (uint32*)malloc(sizeof(uint32) * this->m_tag_reference_count);

	if(this->m_classless_tag_reference_count)
		this->m_classless_tag_references = (uint32*)malloc(sizeof(uint32) * this->m_classless_tag_reference_count);

	if(this->m_string_id_count)
		this->m_string_ids = (uint32*)malloc(sizeof(uint32) * this->m_string_id_count);

	if(this->m_tag_reference_count)
		this->m_data_references = (uint32*)malloc(sizeof(uint32) * this->m_data_reference_count);

	if(this->m_tag_block_count)
		this->m_tag_blocks = (c_xml_definition_block*)malloc(sizeof(c_xml_definition_block) * this->m_tag_block_count);

	// counts are reset to be re-used to keep track of which index
	// inside the array the current item should be placed
	this->reset_counts();

	// do a second pass to actually grab and store the offsets
	element = this->m_element->FirstChildElement();
	while (element)
	{
		const char* element_name = element->Name();
		uint32 offset = UINT_MAX;
		if(element->Attribute("offset") != nullptr)
			offset = strtoul(element->Attribute("offset"), nullptr, 16);
		if (strcmp(element_name, "tagBlock") == 0 || strcmp(element_name, "reflexive") == 0)
		{
			uint32 size = UINT_MAX;
			if (element->Attribute("entrySize") != nullptr)
				size = strtoul(element->Attribute("entrySize"), nullptr, 16);
			else if (element->Attribute("elementSize") != nullptr)
				size = strtoul(element->Attribute("elementSize"), nullptr, 16);

			this->m_tag_blocks[this->m_tag_block_count] = c_xml_definition_block(element, offset, size);
			this->m_tag_block_count++;

			element = element->NextSiblingElement();
			continue;
		}
		if (strcmp(element_name, "tagRef") == 0 || strcmp(element_name, "tagref") == 0)
		{
			if (!element->BoolAttribute("withClass"))
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

c_xml_definition_block::~c_xml_definition_block()
{
	this->clear();
}

void c_xml_definition_block::reset_counts()
{
	this->m_tag_reference_count = 0;
	this->m_classless_tag_reference_count = 0;
	this->m_string_id_count = 0;
	this->m_data_reference_count = 0;
	this->m_tag_block_count = 0;
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

uint32 c_xml_definition_block::get_tag_references_count() const
{
	return this->m_tag_reference_count;
}

uint32 c_xml_definition_block::get_tag_reference_offset(uint32 index) const
{
	ASSERT(this->m_tag_reference_count < index)

	return this->m_tag_references[index];
}

uint32 c_xml_definition_block::get_classless_tag_references_count() const
{
	return this->m_classless_tag_reference_count;
}

uint32 c_xml_definition_block::get_classless_tag_reference_offset(uint32 index) const
{
	ASSERT(this->m_classless_tag_reference_count < index)

	return this->m_classless_tag_references[index];
}

uint32 c_xml_definition_block::get_data_references_count() const
{
	return this->m_data_reference_count;
}

uint32 c_xml_definition_block::get_data_reference_offset(uint32 index) const
{
	ASSERT(this->m_classless_tag_reference_count < index)

	return this->m_classless_tag_references[index];
}

uint32 c_xml_definition_block::get_string_id_count() const
{
	return this->m_string_id_count;
}

uint32 c_xml_definition_block::get_string_id_offset(uint32 index) const
{
	ASSERT(this->m_classless_tag_reference_count < index)

	return this->m_string_ids[index];
}

uint32 c_xml_definition_block::get_tag_block_count() const
{
	return this->m_tag_block_count;
}

c_xml_definition_block* c_xml_definition_block::get_tag_block(uint32 index) const
{
	ASSERT(this->m_classless_tag_reference_count < index)

	return &this->m_tag_blocks[index];
}

uint32 c_xml_definition_loader::resolve_cache_tag_data_offset(uint32 offset)
{
	// todo: tag_data + offset;
	return 0;
}

c_xml_definition_loader::c_xml_definition_loader(c_xml_definition_block* definition, FILE* file_handle,
                                                 datum cache_index)
{
	this->m_data = nullptr;
	this->m_definition = definition;
	this->m_file_handle = file_handle;

	fseek(this->m_file_handle, 0, SEEK_SET);
	fread(&this->m_cache_header, sizeof(s_cache_header), 1, this->m_file_handle);

	fseek(this->m_file_handle, this->m_cache_header.tag_offset, SEEK_SET);
	fread(&this->m_tags_header, sizeof(s_tags_header), 1, this->m_file_handle);

	// set the initial size of the data_loader
	this->m_total_data_size += definition->get_size();

	this->calculate_total_data_size(this->m_definition, this->m_file_offset);
}

void c_xml_definition_loader::calculate_total_data_size(const c_xml_definition_block* definition, uint32 base_offset)
{
	for(uint32 i = 0; i < definition->get_data_references_count(); i++)
	{
		data_reference reference;

		fseek(this->m_file_handle, base_offset + definition->get_data_reference_offset(i), SEEK_SET);
		fread(&reference, sizeof(data_reference), 1, this->m_file_handle);

		this->m_total_data_size += reference.size;
	}

	for (uint32 i = 0; i < definition->get_tag_block_count(); i++)
	{
		tag_block<> block;

		fseek(this->m_file_handle, base_offset + definition->get_tag_block(i)->get_offset(), SEEK_SET);
		fread(&block, sizeof(tag_block<>), 1, this->m_file_handle);

		this->m_total_data_size += definition->get_tag_block(i)->get_size() * block.count;

		this->calculate_total_data_size(definition->get_tag_block(i), resolve_cache_tag_data_offset(block.data));
	}
}

void c_xml_definition_loader::load_tag_data()
{

}


c_xml_definition_agent::c_xml_definition_agent(tag_group type, const char* plugin_path)
{
	this->m_type = type;
	this->m_plugin_path.set(plugin_path);
	tinyxml2::XMLError error = this->m_document.LoadFile(plugin_path);

	if (error != tinyxml2::XML_SUCCESS)
	{
		char err_buff[MAX_PATH + 40];
		sprintf(err_buff, "[xml_definition_agent] failed to load plugin %s for type %s", plugin_path, type.string);
		LOG_ERROR_GAME("{}", err_buff);
		throw err_buff;
	}

	tinyxml2::XMLElement* root = this->m_document.RootElement();
	uint32 root_size = strtoul(root->Attribute("baseSize"), nullptr, 16);

	this->m_definition = c_xml_definition_block(root, 0, root_size);
}

c_xml_definition_agent::~c_xml_definition_agent()
{
	this->m_definition.clear();
}
