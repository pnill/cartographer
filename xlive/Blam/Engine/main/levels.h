#pragma once

/* public code */

struct s_multiplayer_ui_level_definition* __cdecl levels_get_multiplayer_ui_level(int32 map_id);

struct s_campaign_ui_level_definition* __cdecl levels_get_campaign_ui_level(int campaign_id, int map_id);

bool __cdecl levels_get_custom_map_ui_level_definition(int32 custom_map_id, void* out_level_definition);

struct s_campaign_runtime_level_definition* __cdecl levels_get_runtime_campaign_map(int32 campaign_id, int32 map_id);

// Reads the first multiplayer element of the scenario level data straight from a cache file
// Returns false when the file is not a cache file or the scenario has no multiplayer level data
bool levels_read_multiplayer_level_data_from_map_file(struct _iobuf* map_handle, struct s_multiplayer_ui_level_definition* out_level_data);

void levels_apply_patches(void);
