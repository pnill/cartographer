#include "stdafx.h"
#include "tag_injection_table.h"

#define lazy_malloc_buffer(TYPE, COUNT)\
	(TYPE*)malloc(sizeof(TYPE) * COUNT)

constexpr uint32 entry_count_per_resize = 50;

c_tag_injection_table::c_tag_injection_table()
{
	this->m_entry_count = 0;
	this->m_table_size = 0;
	this->m_table = nullptr;
	this->resize_table();
}

c_tag_injection_table::~c_tag_injection_table()
{
	for(uint16 i = 0; i < this->m_entry_count; i++)
	{
		this->m_table[i].loaded_data.clear();
	}
	free(this->m_table);
}

uint16 c_tag_injection_table::get_entry_count() const
{
	return this->m_entry_count;
}

s_tag_injecting_table_entry* c_tag_injection_table::init_entry(datum cache_index, e_tag_group type)
{
	//tag_group temp;
	//temp.group = type;
	s_tag_injecting_table_entry* result = &this->m_table[this->m_entry_count];
	result->cache_index = cache_index;
	result->injected_index = k_first_injected_datum + this->m_entry_count;
	result->type = tag_group_from_enum(type);
	result->is_initialized = true;
	result->is_injected = false;
	this->m_entry_count++;
	

	if (this->m_entry_count == this->m_table_size)
	{
		this->resize_table();
		return &this->m_table[this->m_entry_count - 1];
	}

	return result;
}

s_tag_injecting_table_entry* c_tag_injection_table::get_entry(uint16 index) const
{
	return &this->m_table[index];
}

s_tag_injecting_table_entry* c_tag_injection_table::get_entry_by_cache_index(datum datum_index) const
{
	for (uint16 i = 0; i < this->m_entry_count; i++)
	{
		if (this->m_table[i].cache_index == datum_index)
			return &this->m_table[i];
	}
	return nullptr;
}

s_tag_injecting_table_entry* c_tag_injection_table::get_entry_by_injected_index(datum datum_index) const
{
	return this->get_entry(DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index) - k_first_injected_datum);
}

bool c_tag_injection_table::has_entry_by_cache_index(datum datum_index) const
{
	for (uint16 i = 0; i < this->m_entry_count; i++)
	{
		if (this->m_table[i].cache_index == datum_index)
			return &this->m_table[i].is_initialized;
	}
	return false;
}

void c_tag_injection_table::resize_table()
{
	// Allocate new larger buffer
	s_tag_injecting_table_entry* new_buffer = lazy_malloc_buffer(s_tag_injecting_table_entry, this->m_table_size + entry_count_per_resize);

	if (this->m_table)
	{
		// Copy old buffer into the new one
		memcpy(new_buffer, this->m_table, sizeof(s_tag_injecting_table_entry) * this->m_table_size);

		// Free the old buffer
		free(this->m_table);
	}
	// Increase the size of the table
	this->m_table_size += entry_count_per_resize;
	// Place the new buffer
	this->m_table = new_buffer;
}

#undef lazy_malloc_buffer