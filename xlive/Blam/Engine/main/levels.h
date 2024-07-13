#pragma once

/* forward declations*/

struct s_campaign_ui_level_definition;
struct s_multiplayer_ui_level_definition;
struct s_campaign_runtime_level_definition;

/* public methods */

s_multiplayer_ui_level_definition* __cdecl levels_get_multiplayer_ui_level(int32 map_id);
s_campaign_ui_level_definition* __cdecl levels_get_campaign_ui_level(int campaign_id, int map_id);
bool __cdecl levels_get_custom_map_ui_level_definition(int32 custom_map_id, void* out_level_definition);
s_campaign_runtime_level_definition* __cdecl levels_get_runtime_campaign_map(int32 campaign_id, int32 map_id);



void levels_apply_patches(void);
