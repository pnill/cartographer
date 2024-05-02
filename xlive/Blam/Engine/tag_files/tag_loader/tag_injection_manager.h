#pragma once
#include "tag_injection_table.h"
#include "memory/static_arrays.h"
#include "tag_files/tag_groups.h"
#include "xml/xml_agent.h"
#include "xml/xml_definition_loader.h"

#define k_first_injected_datum 15268
#define k_max_tag_instance_count 65535

// 20mb
#define k_injectable_allocation_size (1024 * 1024 * 20)


class c_tag_injecting_manager
{
private:
	c_tag_injection_table m_table;
	c_static_flags<k_tag_group_count> m_agents_initialized;
	c_xml_definition_agent m_agents[k_tag_group_count];

	c_static_string260 m_base_map_directory;
	c_static_string260 m_mods_map_directory;
	c_static_string260 m_plugins_directory;

	uint32 m_base_tag_data_size;
	uint32 m_injectable_used_size;
	tags::tag_instance* m_instances;

	bool m_active_map_verified;
	c_static_string260 m_active_map;
	s_cache_header m_active_map_cache_header;
	s_tags_header m_active_map_tags_header;
	tags::tag_instance m_active_map_scenario_instance;
	uint32 m_active_map_instance_table_offset;
	FILE* m_active_map_file_handle;

	tags::tag_instance get_tag_instance_from_cache(datum cache_datum) const;
	tag_group get_tag_group_by_datum(datum cache_datum) const;

	void load_raw_data_from_cache(datum injected_index) const;
	static void setup_havok_vtables(e_tag_group group, datum injected_index);
	static void initialize_shader_template(datum injected_datum);
public:

	c_tag_injecting_manager();
	~c_tag_injecting_manager() = default;

	void init_directories();

	void set_base_map_tag_data_size(uint32 size);
	void set_instance_table(tags::tag_instance* table);

	uint16 get_entry_count() const;
	c_tag_injection_table* get_table();

	bool find_map(const char* map_name, c_static_string260* out_string);
	void set_active_map(const char* map_name);
	
	bool get_active_map_verified();
	void reset();
	datum get_tag_datum_by_name(e_tag_group group, const char* tag_name) const;
	void get_name_by_tag_datum(e_tag_group group, datum cache_datum, char* out_name) const;

	bool initialize_agent(tag_group group);
	c_xml_definition_agent* get_agent(tag_group type);

	datum load_tag(e_tag_group group, const char* tag_name, bool load_dependencies);
	datum load_tag(e_tag_group group, datum tag_name, bool load_dependencies);
	static void load_tag_internal(c_tag_injecting_manager* manager, tag_group group, datum cache_datum, bool load_dependencies);

	void inject_tags();
};