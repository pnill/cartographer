#include "stdafx.h"
#include "xml_definition_loader.h"

#include "xml_definition_block.h"

#include "cache/cache_files.h"
#include "filesys/pc_file_system.h"
#include "networking/network_event.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_loader/tag_injection.h"
#include "tag_files/tag_loader/tag_injection_define.h"

#define lazy_malloc_buffer(TYPE, COUNT)\
	(TYPE*)malloc(sizeof(TYPE) * (COUNT))

void c_xml_definition_loader::init(
	const c_xml_definition_block* definition,
	FILE* file_handle, 
	cache_file_header* cache_header,
	cache_file_tags_header* tags_header,
	uint32 scenario_instance_offset,
	datum cache_index
)
{
	m_tag_reference_offsets = nullptr;
	m_classless_tag_reference_offsets = nullptr;
	m_data_reference_offsets = nullptr;
	m_string_id_offsets = nullptr;
	m_tag_block_offsets = nullptr;
	m_data = nullptr;

	m_used_data = 0;
	m_definition = definition;
	m_file_handle = file_handle;
	m_cache_header = cache_header;
	m_tags_header = tags_header;
	m_scenario_instance_offset = scenario_instance_offset;

	m_cache_index = cache_index;
	m_total_data_size = m_definition->get_size();

	load_cache_info();

	reset_counts();
	// set the initial size of the data_loader
	calculate_total_data_size(m_definition, m_file_offset, 1);

	// seen some tags that have an instance size greater than the computed size? failsafe?
	if (m_instance.size > m_total_data_size)
	{
#if TAG_INJECTION_DEBUG
		event(_event_verbose, "tags:injection: [%s] computed size: %u instance size: %u", __FUNCTION__, m_total_data_size, m_instance.size);
#endif
		m_total_data_size = m_instance.size;
	}

	initialize_arrays();
	load_tag_data();

#if TAG_INJECTION_DEBUG
	event(_event_verbose, "tags:injection: [%s] %x - %x", __FUNCTION__, m_instance.data_offset, m_instance.data_offset + m_instance.size);
#endif
}

void c_xml_definition_loader::load_cache_info()
{
	uint32 instance_table_offset = m_cache_header->tag_offset + sizeof(s_tag_group_link) * m_tags_header->tag_group_link_set_count + sizeof(cache_file_tags_header);
	uint32 tag_data_start_offset = m_cache_header->tag_offset + m_cache_header->data_offset;

	uint32 tag_instance_offset = instance_table_offset + sizeof(cache_file_tag_instance) * DATUM_INDEX_TO_ABSOLUTE_INDEX(m_cache_index);

	cache_file_tag_instance instance;
	// read requested tag instance from file
	file_seek_and_read(m_file_handle, tag_instance_offset, sizeof(cache_file_tag_instance), 1, &instance);

	m_instance = instance;
	m_file_offset = tag_data_start_offset + m_instance.data_offset - m_scenario_instance_offset;

#if TAG_INJECTION_DEBUG
	char const string[5] = { m_instance.group_tag.string[3], m_instance.group_tag.string[2], m_instance.group_tag.string[1], m_instance.group_tag.string[0], '\0' };

	event(_event_verbose, "tags:injection: [%s] instance class %s data-offset %x file-offset %x", __FUNCTION__, string, m_instance.data_offset, m_file_offset);
#endif
}

uint32 c_xml_definition_loader::resolve_cache_tag_data_offset(uint32 offset) const
{
	return (m_cache_header->tag_offset + m_cache_header->data_offset) + (offset - m_scenario_instance_offset);
}

void c_xml_definition_loader::reset_counts(void)
{
	m_tag_reference_offset_count = 0;
	m_classless_tag_reference_offset_count = 0;
	m_data_reference_offset_count = 0;
	m_string_id_offset_count = 0;
	m_tag_block_offset_count = 0;
	m_tag_reference_count = 0;
	return;
}

void c_xml_definition_loader::clear(void)
{
	if (m_tag_reference_offset_count)
	{
		free(m_tag_reference_offsets);
	}
	if (m_classless_tag_reference_offset_count)
	{
		free(m_classless_tag_reference_offsets);
	}
	if (m_data_reference_offset_count)
	{
		free(m_data_reference_offsets);
	}
	if (m_string_id_offset_count)
	{
		free(m_string_id_offsets);
	}
	if (m_tag_block_offset_count)
	{
		free(m_tag_block_offsets);
	}
	if (m_tag_reference_count)
	{
		free(m_tag_references);
	}
	if (m_data)
	{
		free(m_data);
	}
	reset_counts();
	return;
}

tag_reference t_tag_reference;
datum t_classless_tag_reference;
tag_data t_data_reference;
string_id t_string_id;
tag_block<> t_tag_block;

void c_xml_definition_loader::initialize_arrays_internal(c_xml_definition_loader* loader, const c_xml_definition_block* definition, uint32 file_offset, uint32 block_count)
{
	for (uint32 block_index = 0; block_index < block_count; block_index++)
	{
		uint32 base_offset = file_offset + definition->get_size() * block_index;
		for (uint32 i = 0; i < definition->get_tag_references_count(); i++)
		{
			file_seek_and_read(loader->m_file_handle, base_offset + definition->get_tag_reference_offset(i), sizeof(tag_reference), 1, &t_tag_reference);
			if (t_tag_reference.group != _tag_group_none && t_tag_reference.index != NONE)
			{
				++loader->m_tag_reference_offset_count;
			}
		}

		for (uint32 i = 0; i < definition->get_classless_tag_references_count(); i++)
		{
			file_seek_and_read(loader->m_file_handle, base_offset + definition->get_classless_tag_reference_offset(i), sizeof(datum), 1, &t_classless_tag_reference);
			if (t_classless_tag_reference != NONE)
				loader->m_classless_tag_reference_offset_count++;
		}

		for (uint32 i = 0; i < definition->get_data_references_count(); i++)
		{
			file_seek_and_read(loader->m_file_handle, base_offset + definition->get_data_reference_offset(i), sizeof(tag_data), 1, &t_data_reference);
			if (t_data_reference.size != 0)
				loader->m_data_reference_offset_count++;
		}

		for (uint32 i = 0; i < definition->get_string_id_count(); i++)
		{
			file_seek_and_read(loader->m_file_handle, base_offset + definition->get_string_id_offset(i), sizeof(string_id), 1, &t_string_id);
			if (t_string_id != 0)
			{
				loader->m_string_id_offset_count++;
			}
		}

		for (uint32 i = 0; i < definition->get_tag_block_count(); i++)
		{
			file_seek_and_read(loader->m_file_handle, base_offset + definition->get_tag_block(i)->get_offset(), sizeof(tag_block<>), 1, &t_tag_block);
			if (t_tag_block.count > 0)
			{
				loader->m_tag_block_offset_count++;
				initialize_arrays_internal(loader, definition->get_tag_block(i), loader->resolve_cache_tag_data_offset(t_tag_block.data), t_tag_block.count);
			}
		}
	}
}

void c_xml_definition_loader::initialize_arrays(void)
{
	initialize_arrays_internal(this, m_definition, m_file_offset, 1);

	if (m_tag_reference_offset_count)
		m_tag_reference_offsets = lazy_malloc_buffer(s_offset_link, m_tag_reference_offset_count);

	if (m_classless_tag_reference_offset_count)
		m_classless_tag_reference_offsets = lazy_malloc_buffer(s_offset_link, m_classless_tag_reference_offset_count);

	if (m_data_reference_offset_count)
		m_data_reference_offsets = lazy_malloc_buffer(s_memory_link, m_data_reference_offset_count);

	if (m_string_id_offset_count)
		m_string_id_offsets = lazy_malloc_buffer(s_offset_link, m_string_id_offset_count);

	if (m_tag_block_offset_count)
		m_tag_block_offsets = lazy_malloc_buffer(s_memory_link, m_tag_block_offset_count);

	if (m_tag_reference_offset_count + m_classless_tag_reference_offset_count)
		m_tag_references = lazy_malloc_buffer(datum, (m_tag_reference_offset_count + m_classless_tag_reference_offset_count));

	m_data = (int8*)calloc(m_total_data_size, sizeof(int8));


#if TAG_INJECTION_DEBUG
	event(_event_verbose, "tags:injection: [%s] tag_r %u ctag_r %u dat_r %u str_id %u tblock %u",
		__FUNCTION__,
		m_tag_reference_offset_count,
		m_classless_tag_reference_offset_count,
		m_data_reference_offset_count,
		m_string_id_offset_count,
		m_tag_block_offset_count);
#endif
	return;
}

int8* c_xml_definition_loader::reserve_data(uint32 size)
{
	if (m_used_data + size > m_total_data_size)
	{
		DISPLAY_ASSERT("[c_xml_definition_loader::reserve_data]: RAN OUT OF SPACE");
	}
	ASSERT(m_data);

	const uint32 old_size = m_used_data;
	m_used_data += size;

#if TAG_INJECTION_DEBUG
	event(_event_verbose, "tags:injection: [%s] data: %x requested: %x used: %x total: %x", __FUNCTION__, (uint32)m_data, size, m_used_data, m_total_data_size);
#endif

	return &m_data[old_size];
}

void c_xml_definition_loader::load_tag_data_internal(c_xml_definition_loader* loader, const c_xml_definition_block* definition, uint32 file_offset, int8* buffer, uint32 block_count)
{
	ASSERT(loader);
	ASSERT(definition);
	ASSERT(buffer);

	int8* cache_data = (int8*)calloc(block_count, definition->get_size());

	ASSERT(cache_data);

	fseek(loader->m_file_handle, file_offset, SEEK_SET);
	fread(cache_data, definition->get_size() * block_count, 1, loader->m_file_handle);
	csmemcpy(buffer, cache_data, definition->get_size() * block_count);
	free(cache_data);

	for (uint32 block_index = 0; block_index < block_count; block_index++)
	{
		for (uint32 i = 0; i < definition->get_tag_references_count(); i++)
		{
			uint32 calc_offset = definition->get_tag_reference_offset(i) + definition->get_size() * block_index;

			file_seek_and_read(loader->m_file_handle, file_offset + calc_offset, sizeof(tag_reference), 1, &t_tag_reference);
			if (t_tag_reference.group != _tag_group_none && t_tag_reference.index != NONE)
			{
				s_offset_link* link = &loader->m_tag_reference_offsets[loader->m_tag_reference_offset_count];
				link->cache_offset = file_offset + calc_offset;
				link->memory_offset = (uintptr_t)buffer + calc_offset;

#if TAG_INJECTION_DEBUG
				csstrncpy(link->name, definition->get_tag_reference_name(i)->get_string(), sizeof(s_offset_link::name));
#endif

				loader->m_tag_references[loader->m_tag_reference_count] = t_tag_reference.index;
				loader->m_tag_reference_count++;
				loader->m_tag_reference_offset_count++;
			}
		}

		for (uint32 i = 0; i < definition->get_classless_tag_references_count(); i++)
		{
			uint32 calc_offset = definition->get_classless_tag_reference_offset(i) + definition->get_size() * block_index;
			
			file_seek_and_read(loader->m_file_handle, file_offset + calc_offset, sizeof(datum), 1, &t_classless_tag_reference);
			if (t_classless_tag_reference != NONE)
			{
				s_offset_link* link = &loader->m_classless_tag_reference_offsets[loader->m_classless_tag_reference_offset_count];

				link->cache_offset = file_offset + calc_offset;
				link->memory_offset = (uint32)buffer + calc_offset;

#if TAG_INJECTION_DEBUG
				csstrncpy(link->name, definition->get_classless_tag_reference_name(i)->get_string(), sizeof(s_offset_link::name));
#endif

				loader->m_tag_references[loader->m_tag_reference_count] = t_classless_tag_reference;
				loader->m_tag_reference_count++;
				loader->m_classless_tag_reference_offset_count++;
			}
		}

		for (uint32 i = 0; i < definition->get_data_references_count(); i++)
		{
			uint32 calc_offset = definition->get_data_reference_offset(i) + definition->get_size() * block_index;

			file_seek_and_read(loader->m_file_handle, file_offset + calc_offset, sizeof(tag_data), 1, &t_data_reference);
			if (t_data_reference.size != 0)
			{
				int8* data_cache = (int8*)malloc(t_data_reference.size);

				ASSERT(data_cache);

				int8* data_buffer = loader->reserve_data(t_data_reference.size);
				fseek(loader->m_file_handle, loader->resolve_cache_tag_data_offset(t_data_reference.data), SEEK_SET);
				fread(data_cache, t_data_reference.size, 1, loader->m_file_handle);
				csmemcpy(data_buffer, data_cache, t_data_reference.size);
				free(data_cache);

				s_memory_link* link = &loader->m_data_reference_offsets[loader->m_data_reference_offset_count];

				link->cache_offset = file_offset + calc_offset;
				link->memory_offset = (uint32)buffer + calc_offset;
				link->data = data_buffer;
				link->size = t_data_reference.size;

#if TAG_INJECTION_DEBUG
				event(_event_verbose, "tags:injection: [%s] data reference: %u-%x size: %x data: %x", __FUNCTION__, i, file_offset + definition->get_data_reference_offset(i), t_data_reference.size, t_data_reference.data);
				csstrncpy(link->name, definition->get_data_reference_name(i)->get_string(), sizeof(s_offset_link::name));
#endif

				loader->m_data_reference_offset_count++;
			}
		}

		for (uint32 i = 0; i < definition->get_string_id_count(); i++)
		{
			uint32 calc_offset = definition->get_string_id_offset(i) + definition->get_size() * block_index;

			file_seek_and_read(loader->m_file_handle, file_offset + calc_offset, sizeof(string_id), 1, &t_string_id);
			if (t_string_id != 0)
			{
				loader->m_string_id_offsets[loader->m_string_id_offset_count].cache_offset = file_offset + calc_offset;
				loader->m_string_id_offsets[loader->m_string_id_offset_count].memory_offset = (uint32)buffer + calc_offset;
				loader->m_string_id_offset_count++;
			}
		}

		for (uint32 i = 0; i < definition->get_tag_block_count(); i++)
		{
			const c_xml_definition_block* definition_block = definition->get_tag_block(i);
			uint32 calc_offset = definition_block->get_offset() + definition->get_size() * block_index;

			file_seek_and_read(loader->m_file_handle, file_offset + calc_offset, sizeof(tag_block<>), 1, &t_tag_block);
			if (t_tag_block.count > 0)
			{
				int8* block_buffer = loader->reserve_data(definition_block->get_size() * t_tag_block.count);

				s_memory_link* link = &loader->m_tag_block_offsets[loader->m_tag_block_offset_count];

				link->cache_offset = file_offset + calc_offset;
				link->memory_offset = (uint32)buffer + calc_offset;
				link->size = definition_block->get_size() * t_tag_block.count;
				link->data = block_buffer;

#if TAG_INJECTION_DEBUG
				event(_event_verbose, "tags:injection: [%s] tag_block: %u name: %s count: %d, size: %x", __FUNCTION__, i, definition->get_tag_block_name(i)->get_string(), t_tag_block.count, definition_block->get_size());
				csstrncpy(link->name, definition->get_tag_block_name(i)->get_string(), sizeof(s_offset_link::name));
#endif

				loader->m_tag_block_offset_count++;

				load_tag_data_internal(loader, definition_block, loader->resolve_cache_tag_data_offset(t_tag_block.data), block_buffer, t_tag_block.count);
			}
		}
	}
}

void c_xml_definition_loader::load_tag_data()
{
	reset_counts();

	int8* buffer = reserve_data(m_definition->get_size());

	load_tag_data_internal(this, m_definition, m_file_offset, buffer, 1);

#if TAG_INJECTION_DEBUG
	validate_data();
#endif
}

void c_xml_definition_loader::calculate_total_data_size(const c_xml_definition_block* definition, uint32 base_offset, uint32 block_count)
{

	for (uint32 block_index = 0; block_index < block_count; block_index++)
	{
		uint32 calc_offset = base_offset + block_index * definition->get_size();
		for (uint32 i = 0; i < definition->get_data_references_count(); i++)
		{
			tag_data reference;

			file_seek_and_read(m_file_handle, calc_offset + definition->get_data_reference_offset(i), sizeof(tag_data), 1, &reference);
			if (reference.size != 0)
			{
				m_total_data_size += reference.size;
#if TAG_INJECTION_DEBUG
				bool in_bounds = (m_instance.data_offset < (uint32)reference.data && m_instance.data_offset + m_instance.size > (uint32)reference.data);
				event(_event_verbose, "tags:injection: [%s] %x data_reference  %d %d %x %d", __FUNCTION__, base_offset, definition->get_data_reference_offset(i), reference.size, reference.data, in_bounds);
#endif
			}
		}


		for (uint32 i = 0; i < definition->get_tag_block_count(); i++)
		{
			tag_block<> block;

			const c_xml_definition_block* definition_block = definition->get_tag_block(i);

			file_seek_and_read(m_file_handle, calc_offset + definition_block->get_offset(), sizeof(tag_block<>), 1, &block);
			if (block.count != 0 && block.data != 0 && block.count != -1 && block.data != -1)
			{
				m_total_data_size += definition_block->get_size() * block.count;

#if TAG_INJECTION_DEBUG
				bool in_bounds = (m_instance.data_offset < (uint32)block.data && m_instance.data_offset + m_instance.size > (uint32)block.data);
				event(_event_verbose,
					"tags:injection: [%s] %x %s %x %d %x %d",
					__FUNCTION__,
					base_offset,
					definition_block->get_name(),
					definition_block->get_offset(),
					block.count,
					block.data,
					in_bounds);
#endif

				calculate_total_data_size(definition->get_tag_block(i), resolve_cache_tag_data_offset(block.data), block.count);
			}
		}
	}
}


uint32 c_xml_definition_loader::get_total_size() const
{
	return m_total_data_size;
}

int8* c_xml_definition_loader::get_data() const
{
	return m_data;
}

uint32 c_xml_definition_loader::get_tag_reference_count(void) const
{
	return m_tag_reference_count;
}

datum c_xml_definition_loader::get_tag_reference(uint32 index) const
{
	return m_tag_references[index];
}

void c_xml_definition_loader::copy_tag_data(int8* out_buffer, uint32 base_offset)
{
	// copy the data into the out buffer
	csmemcpy(out_buffer, m_data, m_total_data_size);

	// resolve and update tag references
	for (uint32 i = 0; i < m_tag_reference_offset_count; i++)
	{
		s_offset_link* link = &m_tag_reference_offsets[i];
		tag_reference* reference = (tag_reference*)(out_buffer + link->memory_offset - m_data);
		const datum resolved_index = tag_injection_resolve_cache_datum(reference->index);

#if TAG_INJECTION_DEBUG
		event(_event_verbose, "tags:injection: [%s]: updating tag reference: %s from: %x to %x", __FUNCTION__, link->name, reference->index, resolved_index);
#endif

		reference->index = resolved_index;
	}

	// resolve and update classless tag references
	for (uint32 i = 0; i < m_classless_tag_reference_offset_count; i++)
	{
		s_offset_link* link = &m_classless_tag_reference_offsets[i];
		datum* reference = (datum*)(out_buffer + link->memory_offset - m_data);
		datum resolved_index = tag_injection_resolve_cache_datum(*reference);

#if TAG_INJECTION_DEBUG
		event(_event_verbose, "tags:injection: [%s]: updating classless tag reference: %s from: %x to %x addr: %x",
			__FUNCTION__,
			link->name,
			*reference,
			resolved_index,
			link->memory_offset);
#endif

		*reference = resolved_index;
	}

	// update data references
	for (uint32 i = 0; i < m_data_reference_offset_count; i++)
	{
		s_memory_link* link = &m_data_reference_offsets[i];
		tag_data* reference = (tag_data*)(out_buffer + link->memory_offset - m_data);
		uint32 resolved_offset = ((uint32)link->data - (uint32)m_data);
		reference->data = (uint32)base_offset + resolved_offset;

#if TAG_INJECTION_DEBUG
		bool in_bounds = (resolved_offset + link->size <= m_total_data_size);
		event(_event_verbose, "tags:injection: [%s]: rebase tag_block: %s base_offset: %x block_offset %x block_size: %x total size: %x in bounds: %d",
			__FUNCTION__,
			link->name,
			base_offset,
			resolved_offset,
			link->size,
			m_total_data_size,
			in_bounds);
#endif
	}

	// update tag blocks
	for (uint32 i = 0; i < m_tag_block_offset_count; i++)
	{
		s_memory_link* link = &m_tag_block_offsets[i];
		tag_block<>* reference = (tag_block<>*)(out_buffer + link->memory_offset - m_data);
		uint32 resolved_offset = ((uint32)link->data - (uint32)m_data);
		reference->data = (uint32)base_offset + resolved_offset;

#if TAG_INJECTION_DEBUG
		bool in_bounds = (resolved_offset + link->size <= m_total_data_size);
		event(_event_verbose, "tags:injection: [%s]: rebase tag_block: %s base_offset: %x block_offset %x block_size: %x total size: %x in bounds: %d",
			__FUNCTION__,
			link->name,
			base_offset,
			resolved_offset,
			link->size,
			m_total_data_size,
			in_bounds);
#endif
	}
}

#if TAG_INJECTION_DEBUG
void c_xml_definition_loader::validate_data() const
{
	int8* cache_data = (int8*)malloc(m_definition->get_size());

	file_seek_and_read(m_file_handle, m_file_offset, m_definition->get_size(), 1, cache_data);

	int res = memcmp(cache_data, m_data, m_definition->get_size());

	if (res != 0)
	{
		error(_error_immediate, "tags:injection: [%s] Base definition block is invalid", __FUNCTION__);
	}
	for (uint32 i = 0; i < m_data_reference_offset_count; i++)
	{
		int8* data_data = (int8*)malloc(m_data_reference_offsets[i].size);

		file_seek_and_read(m_file_handle, m_data_reference_offsets[i].cache_offset, m_data_reference_offsets[i].size, 1, data_data);
		int _res = memcmp(data_data, (uint8*)m_data_reference_offsets[i].memory_offset, 1);
		if (_res != 0)
		{
			error(_error_immediate, "tags:injection: [%s] data_reference is invalid", __FUNCTION__);
		}
		free(data_data);
	}
	for (uint32 i = 0; i < m_tag_block_offset_count; i++)
	{
		int8* block_data = (int8*)malloc(m_tag_block_offsets[i].size);

		file_seek_and_read(m_file_handle, m_tag_block_offsets[i].cache_offset, m_tag_block_offsets[i].size, 1, block_data);
		int _res = memcmp(block_data, (uint8*)m_tag_block_offsets[i].memory_offset, 1);
		if (_res != 0)
		{
			error(_error_immediate, "tags:injection: [%s] tag_block is invalid", __FUNCTION__);
		}
		free(block_data);
	}
	free(cache_data);
}
#endif

#undef lazy_malloc_buffer