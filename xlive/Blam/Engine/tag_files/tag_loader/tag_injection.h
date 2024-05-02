#pragma once
#include "tag_files/tag_groups.h"


bool tag_injection_check_map_exists(const char* map_name);
/**
 * \brief Sets the map that will be accessed for loading tag data
 * \param map_name the filename of the map file without .map
 */
void tag_injection_set_active_map(const char* map_name);
bool tag_injection_active_map_verified();

datum tag_injection_load(e_tag_group group, const char* tag_name, bool load_dependencies);
datum tag_injection_load(e_tag_group group, datum cache_datum, bool load_dependencies);

void tag_injection_inject();

datum tag_injection_resolve_cache_datum(datum cache_datum);

void tag_injection_apply_hooks();
void tag_injection_initialize();