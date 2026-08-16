#pragma once
#include "tag_injection_table.h"

#include "cache/cache_files.h"
#include "xml/xml_agent.h"

/* constants */

enum
{
	// 20mb
	k_injectable_allocation_size = (1024 * 1024 * 20)
};

/* structures */

struct s_tag_injection_string_container
{
	struct s_language_pack_offsets* unic_str;
	struct s_string_reference* references;
	uint32 strings_count;
	uint32 first_string_offset;
	uint32 buffer_length;
	uint32 base_index;
};

/* classes */

class c_tag_injecting_manager
{
public:
	c_tag_injecting_manager(void);
	~c_tag_injecting_manager(void) = default;

	void init_directories(void);

	void set_base_map_tag_data_size(uint32 size);
	uint32 get_base_map_tag_data_size(void) const;
	void set_instance_table(cache_file_tag_instance* table);

	uint16 get_entry_count(void) const;
	c_tag_injection_table* get_table(void);

	bool find_map(const wchar_t* map_name, c_static_wchar_string<MAX_PATH>* out_string) const;
	void set_active_map(const wchar_t* map_name);
	
	bool get_active_map_verified(void) const;
	void close_active_map(void);
	void reset(void);
	datum get_tag_datum_by_name(tag_group group, const char* tag_name) const;
	void get_name_by_tag_datum(tag_group group, datum cache_datum, char* out_name) const;

	bool initialize_agent(tag_group group);
	c_xml_definition_agent* get_agent(tag_group type);

	datum load_tag(tag_group group, char const* tag_name, bool load_dependencies);
	datum load_tag(tag_group group, datum cache_datum, bool load_dependencies);
	static void load_tag_internal(c_tag_injecting_manager* manager, tag_group group, datum cache_datum, bool load_dependencies);
	static void load_dependencies(c_tag_injecting_manager* manager, const s_tag_injecting_table_entry* new_entry);

	void inject_tags(void);
	void* extend_tag_block(void* block, uint32 entry_size, uint32 count);
	void* reserve_space_in_cache_memory(uint32 size, uint32* out_data_offset);

	void load_unicode_strings() const;

private:
	c_tag_injection_table m_table;
	c_static_flags<k_tag_group_count> m_agents_initialized;
	c_xml_definition_agent m_agents[k_tag_group_count];

	c_static_wchar_string<MAX_PATH> m_base_map_directory;
	c_static_wchar_string<MAX_PATH> m_mods_map_directory;
	c_static_wchar_string<MAX_PATH> m_plugins_directory;

	uint32 m_base_tag_data_size;
	uint32 m_injectable_used_size;
	cache_file_tag_instance* m_instances;

	bool m_active_map_verified;
	c_static_wchar_string<MAX_PATH> m_active_map;
	cache_file_header m_active_map_cache_header;
	cache_file_tags_header m_active_map_tags_header;
	uint32 m_active_map_scenario_instance_offset;
	uint32 m_active_map_instance_table_offset;
	FILE* m_active_map_file_handle;

	cache_file_tag_instance get_tag_instance_from_cache(datum cache_datum) const;
	tag_group get_tag_group_by_datum(datum cache_datum) const;

	void load_raw_data_from_cache(datum injected_index) const;
	static void apply_definition_fixup(tag_group group, datum injected_index);
	static void initialize_shader_template(datum injected_datum);
};
