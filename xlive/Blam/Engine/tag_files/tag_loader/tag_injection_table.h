#pragma once

/* constants */

enum
{
	k_first_injected_datum = 15268,
	k_max_tag_instance_count = k_unsigned_short_max,
};

/* structures */

struct s_tag_injecting_table_entry
{
	bool is_initialized;
	bool is_injected;
	bool is_preloaded;

	// a flag that can be used for flagging added processing to a tag
	bool is_post_processed;
	datum cache_index;
	datum injected_index;
	tag_group type;
	class c_xml_definition_loader* loaded_data;
	uint8* injected_data;
};

/* classes */

class c_tag_injection_table
{
public:
	c_tag_injection_table(void);
	~c_tag_injection_table(void);

	void clear(void);

	uint16 get_entry_count(void) const;
	uint16 get_entry_count_by_type(tag_group type) const;

	s_tag_injecting_table_entry* init_entry(datum cache_index, tag_group type);

	s_tag_injecting_table_entry* get_entry(uint16 index) const;
	s_tag_injecting_table_entry* get_entry_by_cache_index(datum datum_index) const;
	s_tag_injecting_table_entry* get_entry_by_injected_index(datum datum_index) const;
	void get_entries_by_type(tag_group type, s_tag_injecting_table_entry* out_results);

	bool has_entry_by_cache_index(datum datum_index) const;

private:
	s_tag_injecting_table_entry* m_table;
	uint16 m_entry_count;
	uint16 m_table_size;

	void resize_table(void);
};
