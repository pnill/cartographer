#include "stdafx.h"
#include "xml_definition_loader.h"
#include "xml_definition_block.h"
#include "cache/cache_files.h"
#include "H2MOD/Tags/TagInterface.h"
#include "tag_files/data_reference.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"

#define lazy_fread(FILE, OFFSET, OUT, SIZE, COUNT)\
	fseek(FILE, OFFSET, SEEK_SET);\
	fread(OUT, SIZE, COUNT, FILE)

#define lazy_malloc_buffer(TYPE, COUNT)\
	(TYPE*)malloc(sizeof(TYPE) * COUNT)

c_xml_definition_loader::c_xml_definition_loader(c_xml_definition_block* definition, FILE* file_handle, datum cache_index)
{
	this->m_tag_reference_offsets = nullptr;
	this->m_classless_tag_reference_offsets = nullptr;
	this->m_data_reference_offsets = nullptr;
	this->m_string_id_offsets = nullptr;
	this->m_tag_block_offsets = nullptr;
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

uint32 c_xml_definition_loader::resolve_cache_tag_data_offset(uint32 offset) const
{
	return (this->m_cache_header.tag_offset + this->m_cache_header.data_offset) + (offset - this->m_scenario_instance.data_offset);
}

void c_xml_definition_loader::reset_counts()
{
	this->m_tag_reference_offset_count = 0;
	this->m_classless_tag_reference_offset_count = 0;
	this->m_data_reference_offset_count = 0;
	this->m_string_id_offset_count = 0;
	this->m_tag_block_offset_count = 0;
	this->m_tag_reference_count = 0;
}

void c_xml_definition_loader::clear() const
{
	if (this->m_tag_reference_offset_count)
		free(this->m_tag_reference_offsets);

	if (this->m_classless_tag_reference_offset_count)
		free(this->m_classless_tag_reference_offsets);

	if (this->m_data_reference_offset_count)
		free(this->m_data_reference_offsets);

	if (this->m_string_id_offset_count)
		free(this->m_string_id_offsets);

	if (this->m_tag_block_offset_count)
		free(this->m_tag_block_offsets);

	if (this->m_tag_reference_count)
		free(this->m_tag_references);

	if (this->m_data)
		free(this->m_data);
}

tag_reference t_tag_reference;
datum t_classless_tag_reference;
data_reference t_data_reference;
string_id t_string_id;
tag_block<> t_tag_block;

void c_xml_definition_loader::initialize_arrays_internal(c_xml_definition_loader* loader, const c_xml_definition_block* definition, uint32 file_offset)
{
	for (uint32 i = 0; i < definition->get_tag_references_count(); i++)
	{
		lazy_fread(loader->m_file_handle, file_offset + definition->get_tag_reference_offset(i), &t_tag_reference, sizeof(tag_reference), 1);
		if (t_tag_reference.group.group != _tag_group_none && t_tag_reference.index != NONE)
			loader->m_tag_reference_offset_count++;
	}

	for (uint32 i = 0; i < definition->get_classless_tag_references_count(); i++)
	{
		lazy_fread(loader->m_file_handle, file_offset + definition->get_classless_tag_reference_offset(i), &t_classless_tag_reference, sizeof(datum), 1);
		if (t_classless_tag_reference != NONE)
			loader->m_classless_tag_reference_offset_count++;
	}

	for (uint32 i = 0; i < definition->get_data_references_count(); i++)
	{
		lazy_fread(loader->m_file_handle, file_offset + definition->get_data_reference_offset(i), &t_data_reference, sizeof(data_reference), 1);
		if (t_data_reference.size != 0)
			loader->m_data_reference_offset_count++;
	}

	for (uint32 i = 0; i < definition->get_string_id_count(); i++)
	{
		lazy_fread(loader->m_file_handle, file_offset + definition->get_string_id_offset(i), &t_string_id, sizeof(string_id), 1);
		if (t_string_id.is_valid())
			loader->m_string_id_offset_count++;
	}

	for (uint32 i = 0; i < definition->get_tag_block_count(); i++)
	{
		lazy_fread(loader->m_file_handle, file_offset + definition->get_tag_block(i)->get_offset(), &t_tag_block, sizeof(tag_block<>), 1);
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

	if(this->m_tag_reference_offset_count)
		this->m_tag_reference_offsets = lazy_malloc_buffer(s_offset_link, this->m_tag_reference_offset_count);

	if(this->m_classless_tag_reference_offset_count)
		this->m_classless_tag_reference_offsets = lazy_malloc_buffer(s_offset_link, this->m_classless_tag_reference_offset_count);

	if(this->m_data_reference_offset_count)
		this->m_data_reference_offsets = lazy_malloc_buffer(s_memory_link, this->m_data_reference_offset_count);

	if(this->m_string_id_offset_count)
		this->m_string_id_offsets = lazy_malloc_buffer(s_offset_link, this->m_string_id_offset_count);

	if(this->m_tag_block_offset_count)
		this->m_tag_block_offsets = lazy_malloc_buffer(s_memory_link, this->m_tag_block_offset_count);

	if(this->m_tag_reference_offset_count + this->m_classless_tag_reference_offset_count)
		this->m_tag_references = lazy_malloc_buffer(datum, (this->m_tag_reference_offset_count + this->m_classless_tag_reference_offset_count));

	this->m_data = (int8*)calloc(this->m_total_data_size, sizeof(int8));
}

int8* c_xml_definition_loader::reserve_data(uint32 size)
{
	if (this->m_used_data + size > this->m_total_data_size)
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
	int8* cache_data = (int8*)malloc(definition->get_size() * block_count);
	fseek(loader->m_file_handle, file_offset, SEEK_SET);
	fread(cache_data, definition->get_size() * block_count, 1, loader->m_file_handle);
	memcpy(buffer, cache_data, definition->get_size() * block_count);
	free(cache_data);

	for (uint32 i = 0; i < definition->get_tag_references_count(); i++)
	{
		lazy_fread(loader->m_file_handle, file_offset + definition->get_tag_reference_offset(i), &t_tag_reference, sizeof(tag_reference), 1);
		if (t_tag_reference.group.group != _tag_group_none && t_tag_reference.index != NONE)
		{
			loader->m_tag_reference_offsets[loader->m_tag_reference_offset_count].cache_offset = file_offset + definition->get_tag_reference_offset(i);
			loader->m_tag_reference_offsets[loader->m_tag_reference_offset_count].memory_offset = (uint32)buffer + definition->get_tag_reference_offset(i);
			loader->m_tag_references[loader->m_tag_reference_count] = t_tag_reference.index;
			loader->m_tag_reference_count++;
			loader->m_tag_reference_offset_count++;
		}
	}

	for (uint32 i = 0; i < definition->get_classless_tag_references_count(); i++)
	{
		lazy_fread(loader->m_file_handle, file_offset + definition->get_classless_tag_reference_offset(i), &t_classless_tag_reference, sizeof(datum), 1);
		if (t_classless_tag_reference != NONE)
		{
			loader->m_classless_tag_reference_offsets[loader->m_classless_tag_reference_offset_count].cache_offset = file_offset + definition->get_classless_tag_reference_offset(i);
			loader->m_classless_tag_reference_offsets[loader->m_classless_tag_reference_offset_count].memory_offset = (uint32)buffer + definition->get_classless_tag_reference_offset(i);
			loader->m_tag_references[loader->m_tag_reference_count] = t_classless_tag_reference;
			loader->m_tag_reference_count++;
			loader->m_classless_tag_reference_offset_count++;
		}
	}

	for (uint32 i = 0; i < definition->get_data_references_count(); i++)
	{
		lazy_fread(loader->m_file_handle, file_offset + definition->get_data_reference_offset(i), &t_data_reference, sizeof(data_reference), 1);
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
			loader->m_data_reference_offsets[loader->m_data_reference_offset_count].data = data_buffer;
			loader->m_data_reference_offsets[loader->m_data_reference_offset_count].size = t_data_reference.size;

			loader->m_data_reference_offset_count++;
		}
	}

	for (uint32 i = 0; i < definition->get_string_id_count(); i++)
	{
		lazy_fread(loader->m_file_handle, file_offset + definition->get_string_id_offset(i), &t_string_id, sizeof(string_id), 1);
		if (t_string_id.is_valid())
		{
			loader->m_string_id_offsets[loader->m_string_id_offset_count].cache_offset = file_offset + definition->get_string_id_offset(i);
			loader->m_string_id_offsets[loader->m_string_id_offset_count].memory_offset = (uint32)buffer + definition->get_string_id_offset(i);
			loader->m_string_id_offset_count++;
		}
	}

	for (uint32 i = 0; i < definition->get_tag_block_count(); i++)
	{
		lazy_fread(loader->m_file_handle, file_offset + definition->get_tag_block(i)->get_offset(), &t_tag_block, sizeof(tag_block<>), 1);
		if (t_tag_block.count > 0)
		{
			int8* block_buffer = loader->reserve_data(definition->get_tag_block(i)->get_size() * t_tag_block.count);

			loader->m_tag_block_offsets[loader->m_tag_block_offset_count].cache_offset = file_offset + definition->get_tag_block(i)->get_offset();
			loader->m_tag_block_offsets[loader->m_tag_block_offset_count].memory_offset = (uint32)buffer + definition->get_tag_block(i)->get_offset();
			loader->m_tag_block_offsets[loader->m_tag_block_offset_count].size = definition->get_tag_block(i)->get_size();
			loader->m_tag_block_offsets[loader->m_tag_block_offset_count].data = block_buffer;
			loader->m_tag_block_offset_count++;

			load_tag_data_internal(loader, definition->get_tag_block(i), loader->resolve_cache_tag_data_offset(t_tag_block.data), block_buffer, t_tag_block.count);
		}
	}
}

void c_xml_definition_loader::load_tag_data()
{
	this->reset_counts();

	int8* buffer = this->reserve_data(this->m_definition->get_size());

	this->load_tag_data_internal(this, this->m_definition, this->m_file_offset, buffer, 1);
}

void c_xml_definition_loader::calculate_total_data_size(const c_xml_definition_block* definition, uint32 base_offset)
{
	for (uint32 i = 0; i < definition->get_data_references_count(); i++)
	{
		data_reference reference;

		lazy_fread(this->m_file_handle, base_offset + definition->get_data_reference_offset(i), &reference, sizeof(data_reference), 1);
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

		lazy_fread(this->m_file_handle, base_offset + definition->get_offset(), &block, sizeof(tag_block<>), 1);
		if (block.count > 0)
		{
			this->m_total_data_size += definition_block->get_size() * block.count;
			bool in_bounds = (this->m_instance.data_offset < block.data && this->m_instance.data_offset + this->m_instance.size > block.data);
			LOG_DEBUG_GAME("[c_xml_definition_loader::calc_data_size] {:x} {} {} {} {:x} {}", base_offset, definition_block->get_name()->get_string(), definition_block->get_offset(), block.count, block.data, in_bounds);
			this->calculate_total_data_size(definition->get_tag_block(i), resolve_cache_tag_data_offset(block.data));
		}
	}
}

void c_xml_definition_loader::validate_data() const
{
	int8* cache_data = (int8*)malloc(this->m_definition->get_size());
	lazy_fread(this->m_file_handle, this->m_file_offset, cache_data,this->m_definition->get_size(), 1);

	int res = memcmp(cache_data, this->m_data, this->m_definition->get_size());
	if(res != 0)
	{
		LOG_ERROR_GAME("[c_xml_definition_loader::validate_data] Base definition block is invalid");
	}
	for(uint32 i = 0; i < this->m_data_reference_offset_count; i++)
	{
		int8* data_data = (int8*)malloc(this->m_data_reference_offsets[i].size);
		lazy_fread(this->m_file_handle, this->m_data_reference_offsets[i].cache_offset, data_data, this->m_data_reference_offsets[i].size, 1);
		int res = memcmp(data_data, (uint8*)this->m_data_reference_offsets[i].memory_offset, 1);
		if(res != 0)
		{
			LOG_ERROR_GAME("[c_xml_definition_loader::validate_data] data_reference is invalid");
		}
		free(data_data);
	}
	for(uint32 i = 0; i < this->m_tag_block_offset_count; i++)
	{
		int8* block_data = (int8*)malloc(this->m_tag_block_offsets[i].size);
		lazy_fread(this->m_file_handle, this->m_tag_block_offsets[i].cache_offset, block_data, this->m_tag_block_offsets[i].size, 1);
		int res = memcmp(block_data, (uint8*)this->m_tag_block_offsets[i].memory_offset, 1);
		if (res != 0)
		{
			LOG_ERROR_GAME("[c_xml_definition_loader::validate_data] tag_block is invalid");
		}
	}
}

bool c_xml_definition_loader::is_persisted() const
{
	return this->m_persist;
}

void c_xml_definition_loader::set_persistence(bool state)
{
	this->m_persist = true;
}

#undef lazy_fread
#undef lazy_malloc_buffer