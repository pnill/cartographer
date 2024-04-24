#include "stdafx.h"
#include "xml_loader.h"

#include "tag_files/data_reference.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"
#include "tinyxml/tinyxml2.h"



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

	if (this->m_element->Attribute("name"))
		this->m_name.set(this->m_element->Attribute("name"));

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

	if (this->m_tag_reference_count)
		this->m_tag_references = (uint32*)malloc(sizeof(uint32) * this->m_tag_reference_count);

	if(this->m_classless_tag_reference_count)
		this->m_classless_tag_references = (uint32*)malloc(sizeof(uint32) * this->m_classless_tag_reference_count);

	if(this->m_string_id_count)
		this->m_string_ids = (uint32*)malloc(sizeof(uint32) * this->m_string_id_count);

	if(this->m_data_reference_count)
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
	//ASSERT(this->m_tag_reference_count < index)

	return this->m_tag_references[index];
}

uint32 c_xml_definition_block::get_classless_tag_references_count() const
{
	return this->m_classless_tag_reference_count;
}

uint32 c_xml_definition_block::get_classless_tag_reference_offset(uint32 index) const
{
	//ASSERT(this->m_classless_tag_reference_count < index)

	return this->m_classless_tag_references[index];
}

uint32 c_xml_definition_block::get_data_references_count() const
{
	return this->m_data_reference_count;
}

uint32 c_xml_definition_block::get_data_reference_offset(uint32 index) const
{
	//ASSERT(this->m_classless_tag_reference_count < index)

	return this->m_data_references[index];
}

uint32 c_xml_definition_block::get_string_id_count() const
{
	return this->m_string_id_count;
}

uint32 c_xml_definition_block::get_string_id_offset(uint32 index) const
{
	//ASSERT(this->m_classless_tag_reference_count < index)

	return this->m_string_ids[index];
}

uint32 c_xml_definition_block::get_tag_block_count() const
{
	return this->m_tag_block_count;
}

c_xml_definition_block* c_xml_definition_block::get_tag_block(uint32 index) const
{
	//ASSERT(this->m_classless_tag_reference_count < index)

	return &this->m_tag_blocks[index];
}

c_xml_definition_loader::c_xml_definition_loader(c_xml_definition_block* definition, FILE* file_handle,
	datum cache_index)
{
	this->m_data = nullptr;
	this->m_used_data = 0;
	this->m_definition = definition;
	this->m_file_handle = file_handle;
	this->m_cache_index = cache_index;
	this->m_total_data_size = this->m_definition->get_size();

	this->load_cache_info();
	this->reset_counts();
	// set the initial size of the data_loader
	this->calculate_total_data_size(this->m_definition, this->m_file_offset);

	// seen some tags that have an instance size greater than the computed size? failsafe?
	if (this->m_instance.size > this->m_total_data_size)
		this->m_total_data_size = this->m_instance.size;

	this->initialize_arrays();
	this->load_tag_data();

	LOG_INFO_GAME("[uhh] {:x} - {:x}", this->m_instance.data_offset, this->m_instance.data_offset + this->m_instance.size);

	// because breakpoints are too hard.
	ASSERT(true);
}

void c_xml_definition_loader::load_cache_info()
{
	// read cache header from file
	fseek(this->m_file_handle, 0, SEEK_SET);
	fread(&this->m_cache_header, sizeof(s_cache_header), 1, this->m_file_handle);

	// read tags header from file
	fseek(this->m_file_handle, this->m_cache_header.tag_offset, SEEK_SET);
	fread(&this->m_tags_header, sizeof(s_tags_header), 1, this->m_file_handle);

	uint32 instance_table_offset = this->m_cache_header.tag_offset + 0xC * this->m_tags_header.tag_parent_info_count + 0x20;
	uint32 tag_data_start_offset = this->m_cache_header.tag_offset + this->m_cache_header.data_offset;

	// read scenario instance from file
	fseek(this->m_file_handle, instance_table_offset, SEEK_SET);
	fread(&this->m_scenario_instance, sizeof(tags::tag_instance), 1, this->m_file_handle);

	uint32 tag_instance_offset = instance_table_offset + sizeof(tags::tag_instance) * DATUM_INDEX_TO_ABSOLUTE_INDEX(this->m_cache_index);

	// read requested tag instance from file
	fseek(this->m_file_handle, tag_instance_offset, SEEK_SET);
	fread(&this->m_instance, sizeof(tags::tag_instance), 1, this->m_file_handle);

	this->m_file_offset = tag_data_start_offset + this->m_instance.data_offset - this->m_scenario_instance.data_offset;
}

void c_xml_definition_loader::reset_counts()
{
	this->m_tag_reference_offset_count = 0;
	this->m_classless_tag_reference_offset_count = 0;
	this->m_data_reference_offset_count = 0;
	this->m_string_id_offset_count = 0;
	this->m_tag_block_offset_count = 0;
}

void c_xml_definition_loader::initialize_arrays_internal(c_xml_definition_loader* loader, const c_xml_definition_block* definition, uint32 file_offset)
{
	tag_reference t_tag_reference;
	datum t_classless_tag_reference;
	data_reference t_data_reference;
	string_id t_string_id;
	tag_block<> t_tag_block;

	for(uint32 i = 0; i < definition->get_tag_references_count(); i++)
	{
		fseek(loader->m_file_handle, file_offset + definition->get_tag_reference_offset(i), SEEK_SET);
		fread(&t_tag_reference, sizeof(tag_reference), 1, loader->m_file_handle);
		if(t_tag_reference.group.group != _tag_group_none && t_tag_reference.index != NONE)
			loader->m_tag_reference_offset_count++;
	}

	for(uint32 i = 0; i < definition->get_classless_tag_references_count(); i++)
	{
		fseek(loader->m_file_handle, file_offset + definition->get_classless_tag_reference_offset(i), SEEK_SET);
		fread(&t_classless_tag_reference, sizeof(datum), 1, loader->m_file_handle);
		if (t_classless_tag_reference != NONE)
			loader->m_classless_tag_reference_offset_count++;
	}

	for(uint32 i = 0; i < definition->get_data_references_count(); i++)
	{
		fseek(loader->m_file_handle, file_offset + definition->get_data_reference_offset(i), SEEK_SET);
		fread(&t_data_reference, sizeof(data_reference), 1, loader->m_file_handle);
		if (t_data_reference.size != 0)
			loader->m_data_reference_offset_count++;
	}

	for(uint32 i = 0; i < definition->get_string_id_count(); i++)
	{
		fseek(loader->m_file_handle, file_offset + definition->get_string_id_offset(i), SEEK_SET);
		fread(&t_string_id, sizeof(string_id), 1, loader->m_file_handle);
		if (t_string_id.is_valid())
			loader->m_string_id_offset_count++;
	}

	for(uint32 i = 0; i < definition->get_tag_block_count(); i++)
	{
		fseek(loader->m_file_handle, file_offset + definition->get_tag_block(i)->get_offset(), SEEK_SET);
		fread(&t_tag_block, sizeof(tag_block<>), 1, loader->m_file_handle);
		if (t_tag_block.count > 0)
		{
			loader->m_tag_block_offset_count++;
			initialize_arrays_internal(loader, definition->get_tag_block(i), loader->resolve_cache_tag_data_offset(t_tag_block.data));
		}
	}
}

void c_xml_definition_loader::initialize_arrays()
{
	this->initialize_arrays_internal(this, this->m_definition, this->m_file_offset);
}

int8* c_xml_definition_loader::reserve_data(uint32 size)
{
	if(this->m_used_data + size > this->m_total_data_size)
	{
		LOG_ERROR_GAME("[c_xml_definition_loader]: RAN OUT OF SPACE");
		throw;
	}
	if (!this->m_data)
		return nullptr;
	const uint32 old_size = this->m_used_data;
	this->m_used_data += size;

	LOG_DEBUG_GAME("[c_xml_definition_loader::reserve_data] data: {:x} requested: {:x} used: {:x} total: {:x}", (uint32)this->m_data, size, this->m_used_data, this->m_total_data_size);

	return &this->m_data[old_size];
}

void c_xml_definition_loader::load_tag_data_internal(c_xml_definition_loader* loader, c_xml_definition_block* definition, uint32 file_offset, int8* buffer, uint32 block_count)
{
	tag_reference t_tag_reference;
	datum t_classless_tag_reference;
	data_reference t_data_reference;
	string_id t_string_id;
	tag_block<> t_tag_block;


	int8* cache_data = (int8*)malloc(definition->get_size() * block_count);
	fseek(loader->m_file_handle, file_offset, SEEK_SET);
	fread(cache_data, definition->get_size() * block_count, 1, loader->m_file_handle);
	memcpy(buffer, cache_data, definition->get_size() * block_count);
	free(cache_data);

	for (uint32 i = 0; i < definition->get_tag_references_count(); i++)
	{
		fseek(loader->m_file_handle, file_offset + definition->get_tag_reference_offset(i), SEEK_SET);
		fread(&t_tag_reference, sizeof(tag_reference), 1, loader->m_file_handle);
		if (t_tag_reference.group.group != _tag_group_none && t_tag_reference.index != NONE)
		{
			loader->m_tag_reference_offsets[loader->m_tag_reference_offset_count].cache_offset = file_offset + definition->get_tag_reference_offset(i);
			loader->m_tag_reference_offsets[loader->m_tag_reference_offset_count].memory_offset = (uint32)buffer + definition->get_tag_reference_offset(i);
			loader->m_tag_reference_offset_count++;
		}
	}

	for (uint32 i = 0; i < definition->get_classless_tag_references_count(); i++)
	{
		fseek(loader->m_file_handle, file_offset + definition->get_classless_tag_reference_offset(i), SEEK_SET);
		fread(&t_classless_tag_reference, sizeof(datum), 1, loader->m_file_handle);
		if (t_classless_tag_reference != NONE)
		{
			loader->m_classless_tag_reference_offsets[loader->m_classless_tag_reference_offset_count].cache_offset = file_offset + definition->get_classless_tag_reference_offset(i);
			loader->m_classless_tag_reference_offsets[loader->m_classless_tag_reference_offset_count].memory_offset = (uint32)buffer + definition->get_classless_tag_reference_offset(i);
			loader->m_classless_tag_reference_offset_count++;
		}
	}

	for (uint32 i = 0; i < definition->get_data_references_count(); i++)
	{
		fseek(loader->m_file_handle, file_offset + definition->get_data_reference_offset(i), SEEK_SET);
		fread(&t_data_reference, sizeof(data_reference), 1, loader->m_file_handle);
		if (t_data_reference.size != 0)
		{
			LOG_DEBUG_GAME("[c_xml_definition_lodaer::load_tag_internal] data reference: {}-{:x} size: {:x} data: {:x}", i, file_offset + definition->get_data_reference_offset(i), t_data_reference.size, t_data_reference.data);

			int8* data_cache = (int8*)malloc(t_data_reference.size);
			int8* data_buffer = loader->reserve_data(t_data_reference.size);
			fseek(loader->m_file_handle, loader->resolve_cache_tag_data_offset(t_data_reference.data), SEEK_SET);
			fread(data_cache, t_data_reference.size, 1, loader->m_file_handle);
			memcpy(data_buffer, data_cache, t_data_reference.size);
			free(data_cache);

			loader->m_data_reference_offsets[loader->m_data_reference_offset_count].cache_offset = file_offset + definition->get_data_reference_offset(i);
			loader->m_data_reference_offsets[loader->m_data_reference_offset_count].memory_offset = (uint32)buffer + definition->get_data_reference_offset(i);
			loader->m_data_reference_offsets[loader->m_data_reference_offset_count].data_reference = data_buffer;

			loader->m_data_reference_offset_count++;
		}
	}

	for (uint32 i = 0; i < definition->get_string_id_count(); i++)
	{
		fseek(loader->m_file_handle, file_offset + definition->get_string_id_offset(i), SEEK_SET);
		fread(&t_string_id, sizeof(string_id), 1, loader->m_file_handle);
		if (t_string_id.is_valid())
		{
			loader->m_string_id_offsets[loader->m_string_id_offset_count].cache_offset = file_offset + definition->get_string_id_offset(i);
			loader->m_string_id_offsets[loader->m_string_id_offset_count].memory_offset = (uint32)buffer + definition->get_string_id_offset(i);
			loader->m_string_id_offset_count++;
		}
	}

	for (uint32 i = 0; i < definition->get_tag_block_count(); i++)
	{
		fseek(loader->m_file_handle, file_offset + definition->get_tag_block(i)->get_offset(), SEEK_SET);
		fread(&t_tag_block, sizeof(tag_block<>), 1, loader->m_file_handle);
		if (t_tag_block.count > 0)
		{
			int8* block_buffer = loader->reserve_data(definition->get_tag_block(i)->get_size() * t_tag_block.count);

			loader->m_tag_block_offsets[loader->m_tag_block_offset_count].cache_offset = file_offset + definition->get_tag_block(i)->get_offset();
			loader->m_tag_block_offsets[loader->m_tag_block_offset_count].memory_offset = (uint32)buffer + definition->get_tag_block(i)->get_offset();
			loader->m_tag_block_offset_count++;
			
			load_tag_data_internal(loader, definition->get_tag_block(i), loader->resolve_cache_tag_data_offset(t_tag_block.data), block_buffer, t_tag_block.count);
		}
	}
}

void c_xml_definition_loader::load_tag_data()
{
	this->m_tag_reference_offsets = (s_offset_link*)malloc(sizeof(uint32) * this->m_tag_reference_offset_count);
	this->m_classless_tag_reference_offsets = (s_offset_link*)malloc(sizeof(uint32) * this->m_classless_tag_reference_offset_count);
	this->m_data_reference_offsets = (s_data_reference_link*)malloc(sizeof(uint32) * this->m_data_reference_offset_count);
	this->m_string_id_offsets = (s_offset_link*)malloc(sizeof(uint32) * this->m_string_id_offset_count);
	this->m_tag_block_offsets = (s_offset_link*)malloc(sizeof(uint32) * this->m_tag_block_offset_count);

	this->m_data = (int8*)calloc(this->m_total_data_size, sizeof(int8));

	this->reset_counts();

	int8* buffer = this->reserve_data(this->m_definition->get_size());

	this->load_tag_data_internal(this, this->m_definition, this->m_file_offset, buffer, 1);
}


uint32 c_xml_definition_loader::resolve_cache_tag_data_offset(uint32 offset) const
{
	return (this->m_cache_header.tag_offset + this->m_cache_header.data_offset) + (offset - this->m_scenario_instance.data_offset);
}

void c_xml_definition_loader::calculate_total_data_size(const c_xml_definition_block* definition, uint32 base_offset)
{
	for(uint32 i = 0; i < definition->get_data_references_count(); i++)
	{
		data_reference reference;

		fseek(this->m_file_handle, base_offset + definition->get_data_reference_offset(i), SEEK_SET);
		fread(&reference, sizeof(data_reference), 1, this->m_file_handle);

		if (reference.size != 0)
		{
			this->m_total_data_size += reference.size;
			bool in_bounds = (this->m_instance.data_offset < reference.data && this->m_instance.data_offset + this->m_instance.size > reference.data);
			LOG_DEBUG_GAME("[c_xml_definition_loader::calc_data_size] {:x} data_reference  {} {} {:x} {}", base_offset, definition->get_data_reference_offset(i), reference.size, reference.data, in_bounds);
		}
	}

	for (uint32 i = 0; i < definition->get_tag_block_count(); i++)
	{
		tag_block<> block;

		c_xml_definition_block* definition_block = definition->get_tag_block(i);

		fseek(this->m_file_handle, base_offset + definition_block->get_offset(), SEEK_SET);
		fread(&block, sizeof(tag_block<>), 1, this->m_file_handle);

		if (block.count > 0)
		{
			this->m_total_data_size += definition_block->get_size() * block.count;
			bool in_bounds = (this->m_instance.data_offset < block.data && this->m_instance.data_offset + this->m_instance.size > block.data);
			LOG_DEBUG_GAME("[c_xml_definition_loader::calc_data_size] {:x} {} {} {} {:x} {}", base_offset, definition_block->get_name()->get_string(), definition_block->get_offset(), block.count, block.data, in_bounds);
			this->calculate_total_data_size(definition->get_tag_block(i), resolve_cache_tag_data_offset(block.data));
		}
	}
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

c_xml_definition_block* c_xml_definition_agent::get_definition()
{
	return &this->m_definition;
}
