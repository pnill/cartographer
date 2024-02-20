#pragma once
#include "Blam/Cache/DataTypes/TagBlock.h"
#include "tag_files/tag_reference.h"
#include "Blam/Engine/cseries/cseries_strings.h"

#define k_maximum_number_of_campaigns 4
#define k_maximum_number_of_campaign_maps 20
#define k_maximum_number_of_multiplayer_maps 50

enum e_multiplayer_ui_level_definition_flags : uint8
{
    _multiplayer_ui_level_definition_flag_unlockable = FLAG(0)
};


struct s_ui_level_descriptions
{
    c_static_wchar_string32 english_name;
    c_static_wchar_string32 japanese_name;
    c_static_wchar_string32 german_name;
    c_static_wchar_string32 french_name;
    c_static_wchar_string32 spanish_name;
    c_static_wchar_string32 italian_name;
    c_static_wchar_string32 korean_name;
    c_static_wchar_string32 chinese_name;
    c_static_wchar_string32 portuguese_name;
    c_static_wchar_string128 english_description;
    c_static_wchar_string128 japanese_description;
    c_static_wchar_string128 german_description;
    c_static_wchar_string128 french_description;
    c_static_wchar_string128 spanish_description;
    c_static_wchar_string128 italian_description;
    c_static_wchar_string128 korean_description;
    c_static_wchar_string128 chinese_description;
    c_static_wchar_string128 portuguese_description;
};
CHECK_STRUCT_SIZE(s_ui_level_descriptions, 2880);

// max count: k_maximum_number_of_campaign_maps 20
struct s_campaign_ui_level_definition
{
    int32 campaign_id;
    int32 map_id;
    tag_reference bitmap;   // bitm
    s_ui_level_descriptions level_descriptions;
};
TAG_BLOCK_SIZE_ASSERT(s_campaign_ui_level_definition, 2896);

// max count: k_maximum_number_of_multiplayer_maps 50
struct s_multiplayer_ui_level_definition
{
    int32 map_id;
    tag_reference bitmap;   // bitm
    s_ui_level_descriptions level_descriptions;
    static_string256 path;
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
TAG_BLOCK_SIZE_ASSERT(s_multiplayer_ui_level_definition, 3172);

// max count: k_maximum_number_of_campaign_maps
struct s_campaign_runtime_level_definition
{
    int32 campaign_id;
    int32 map_id;
    static_string256 path;
};
TAG_BLOCK_SIZE_ASSERT(s_campaign_runtime_level_definition, 264);

// max count: 1
struct s_runtime_levels_definition
{
    tag_block<s_campaign_runtime_level_definition> campaign_levels;
};
TAG_BLOCK_SIZE_ASSERT(s_runtime_levels_definition, 8);

// max count k_maximum_number_of_campaigns
struct s_campaign_definition
{
    int32 campaign_id;
    s_ui_level_descriptions level_descriptions;
};
TAG_BLOCK_SIZE_ASSERT(s_campaign_definition, 2884);

// max count: 1
struct s_ui_levels_definition
{
    tag_block<s_campaign_definition> campaigns;
    tag_block<s_campaign_ui_level_definition> campaign_levels;
    tag_block<s_multiplayer_ui_level_definition> multiplayer_levels;
};
TAG_BLOCK_SIZE_ASSERT(s_ui_levels_definition, 24);
