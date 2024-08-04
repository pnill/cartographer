#pragma once
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"
#include "cseries/cseries_strings.h"

#define k_maximum_number_of_campaigns 4
#define k_maximum_number_of_campaign_maps 20
#define k_maximum_number_of_multiplayer_maps 50

enum e_multiplayer_ui_level_definition_flags : uint8
{
    _multiplayer_ui_level_definition_flag_unlockable = FLAG(0)
};

struct s_ui_level_descriptions
{
    wchar_t name[k_language_count][32];
    wchar_t description[k_language_count][128];
};
ASSERT_STRUCT_SIZE(s_ui_level_descriptions, 2880);

// max count: k_maximum_number_of_campaign_maps 20
struct s_campaign_ui_level_definition
{
    int32 campaign_id;
    int32 map_id;
    tag_reference bitmap;   // bitm
    s_ui_level_descriptions level_descriptions;
};
ASSERT_STRUCT_SIZE(s_campaign_ui_level_definition, 2896);

// max count: k_maximum_number_of_multiplayer_maps 50
struct s_multiplayer_ui_level_definition
{
    int32 map_id;
    tag_reference bitmap;   // bitm
    s_ui_level_descriptions level_descriptions;
    char path[256];
    int32 sort_order;
    e_multiplayer_ui_level_definition_flags flags;
    int8 pad[3];
    uint8 max_teams_none;
    uint8 max_teams_ctf;
    uint8 max_teams_slayer;
    uint8 max_teams_oddball;
    uint8 max_teams_koth;
    uint8 max_teams_race;
    uint8 max_teams_headhunter;
    uint8 max_teams_juggernaut;
    uint8 max_teams_territories;
    uint8 max_teams_assault;
    uint8 max_teams_stub_10;
    uint8 max_teams_stub_11;
    uint8 max_teams_stub_12;
    uint8 max_teams_stub_13;
    uint8 max_teams_stub_14;
    uint8 max_teams_stub_15;
};
ASSERT_STRUCT_SIZE(s_multiplayer_ui_level_definition, 3172);

// max count: k_maximum_number_of_campaign_maps
struct s_campaign_runtime_level_definition
{
    int32 campaign_id;
    int32 map_id;
    static_string256 path;
};
ASSERT_STRUCT_SIZE(s_campaign_runtime_level_definition, 264);

// max count: 1
struct s_runtime_levels_definition
{
    tag_block<s_campaign_runtime_level_definition> campaign_levels;
};
ASSERT_STRUCT_SIZE(s_runtime_levels_definition, 8);

// max count k_maximum_number_of_campaigns
struct s_campaign_definition
{
    int32 campaign_id;
    s_ui_level_descriptions level_descriptions;
};
ASSERT_STRUCT_SIZE(s_campaign_definition, 2884);

// max count: 1
struct s_ui_levels_definition
{
    tag_block<s_campaign_definition> campaigns;
    tag_block<s_campaign_ui_level_definition> campaign_levels;
    tag_block<s_multiplayer_ui_level_definition> multiplayer_levels;
};
ASSERT_STRUCT_SIZE(s_ui_levels_definition, 24);
