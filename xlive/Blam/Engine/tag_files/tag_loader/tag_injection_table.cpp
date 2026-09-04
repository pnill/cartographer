#include "stdafx.h"
#include "tag_injection_table.h"

#include "xml/xml_definition_loader.h"

/* constants */

enum
{
	k_injection_table_entry_count_per_resize = 200
};

/* public code */

c_tag_injection_table::c_tag_injection_table(void)
{
	m_entry_count = 0;
	m_table_size = 0;
	m_table = nullptr;
	resize_table();
	return;
}

c_tag_injection_table::~c_tag_injection_table(void)
{
	for(uint16 i = 0; i < m_entry_count; ++i)
	{
		ASSERT(m_table[i].loaded_data);
		m_table[i].loaded_data->clear();
		delete m_table[i].loaded_data;
	}

	ASSERT(m_table);
	free(m_table);

	return;
}

void c_tag_injection_table::clear(void)
{
	for (uint16 i = 0; i < m_entry_count; ++i)
	{
		ASSERT(m_table[i].loaded_data);
		m_table[i].loaded_data->clear();
		delete m_table[i].loaded_data;
	}

	ASSERT(m_table);
	free(m_table);

	m_entry_count = 0;
	m_table_size = 0;
	m_table = nullptr;
	resize_table();
}

uint16 c_tag_injection_table::get_entry_count(void) const
{
	return m_entry_count;
}

uint16 c_tag_injection_table::get_entry_count_by_type(
	tag_group type) const
{
	uint16 result = 0;

	for (uint16 i = 0; i < m_entry_count; ++i)
	{
		if (m_table[i].type == type)
		{
			++result;
		}
	}

	return result;
}

s_tag_injecting_table_entry* c_tag_injection_table::init_entry(
	datum cache_index,
	tag_group type)
{
	if (m_entry_count >= m_table_size)
	{
		resize_table();
	}

	s_tag_injecting_table_entry* result = &m_table[m_entry_count];
	result->cache_index = cache_index;
	result->injected_index = k_first_injected_datum + m_entry_count;
	result->type = type;
	result->is_initialized = true;
	result->is_injected = false;
	result->loaded_data = new c_xml_definition_loader();
	m_entry_count++;

	return result;
}

s_tag_injecting_table_entry* c_tag_injection_table::get_entry(uint16 index) const
{
	return &m_table[index];
}

s_tag_injecting_table_entry* c_tag_injection_table::get_entry_by_cache_index(datum datum_index) const
{
	for (uint16 i = 0; i < m_entry_count; i++)
	{
		if (m_table[i].cache_index == datum_index)
			return &m_table[i];
	}
	return nullptr;
}

s_tag_injecting_table_entry* c_tag_injection_table::get_entry_by_injected_index(datum datum_index) const
{
	return get_entry(DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index) - k_first_injected_datum);
}

void c_tag_injection_table::get_entries_by_type(
	tag_group type,
	s_tag_injecting_table_entry* out_results)
{
	int32 out_index = 0;

	for (uint16 i = 0; i < m_entry_count; ++i)
	{
		if (m_table[i].type == type)
		{
			out_results[out_index++] = m_table[i];
		}
	}

	return;
}

bool c_tag_injection_table::has_entry_by_cache_index(datum datum_index) const
{
	bool result = false;
	for (uint16 i = 0; i < m_entry_count; ++i)
	{
		if (m_table[i].cache_index == datum_index)
		{
			result = &m_table[i].is_initialized;
			break;
		}
	}
	return result;
}

/* private code */

// todo: this breaks when resizing mid injection because there will be pointers in the stack to the old table
// need to figure out a better way to do this maybe, for now just increased the size of the injection table..
void c_tag_injection_table::resize_table(void)
{
	// Allocate new larger buffer
	s_tag_injecting_table_entry* new_buffer = (s_tag_injecting_table_entry*)calloc(
		m_table_size + k_injection_table_entry_count_per_resize,
		sizeof(s_tag_injecting_table_entry));

	if (m_table)
	{
		// Copy old buffer into the new one
		csmemcpy(new_buffer, m_table, sizeof(s_tag_injecting_table_entry) * m_table_size);

		// Free the old buffer
		free(m_table);
	}
	// Increase the size of the table
	m_table_size += k_injection_table_entry_count_per_resize;
	// Place the new buffer
	m_table = new_buffer;
	return;
}
