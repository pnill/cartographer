#pragma once

#include "game/game_engine.h"
#include "tag_files/string_id.h"

#define k_maximum_number_of_dynamic_spawn_zones 32
#define MAXIMUM_STATIC_SPAWN_ZONES_PER_SCENARIO 128

enum e_spawn_overload_type : short
{
    spawn_overload_type_enemy = 0,
    spawn_overload_type_friend = 1,
    spawn_overload_type_enemy_vehicle = 2,
    spawn_overload_type_friendly_vehicle = 3,
    spawn_overload_type_empty_vehicle = 4,
    spawn_overload_type_oddball_inclusion = 5,
    spawn_overload_type_oddball_exclusion = 6,
    spawn_overload_type_hill_inclusion = 7,
    spawn_overload_type_hill_exclusion = 8,
    spawn_overload_type_last_race_flag = 9,
    spawn_overload_type_dead_ally = 10,
    spawn_overload_type_controlled_territory = 11
};

enum e_static_spawn_flags : int
{
    static_spawn_flag_disabled_if_flag_home = FLAG(0),
    static_spawn_flag_disabled_if_flag_away = FLAG(1),
    static_spawn_flag_disabled_if_bomb_home = FLAG(2),
    static_spawn_flag_disabled_if_bomb_away = FLAG(3)
};

enum e_relevant_team : int
{
    relevant_team_red_alpha = 0,
    relevant_team_blue_bravo = 1,
    relevant_team_yellow_charlie = 2,
    relevant_team_green_delta = 3,
    relevant_team_purple_echo = 4,
    relevant_team_orange_foxtrot = 5,
    relevant_team_brown_golf = 6,
    relevant_team_pink_hotel = 7,
    relevant_team_neutral = 8
};

// max count: k_maximum_number_of_dynamic_spawn_zones 32
struct s_dynamic_spawn_zone_overload
{
    e_spawn_overload_type overload_type;
    short pad;
    float inner_radius;
    float outer_radius;
    float weight;
};
TAG_BLOCK_SIZE_ASSERT(s_dynamic_spawn_zone_overload, 16);

struct s_static_spawn_zone_data
{
    string_id name;
    e_relevant_team relevant_team;
    e_relevant_multiplayer_games releveant_games;
    e_static_spawn_flags flags;
};
CHECK_STRUCT_SIZE(s_static_spawn_zone_data, 16);

// max count: MAXIMUM_STATIC_SPAWN_ZONES_PER_SCENARIO 128
struct s_static_spawn_zone
{
    /* cflt */
    // Explaination("Static Spawn Zones", "Lower and upper heights can be left at 0, in which case they use defaults.  Leaving relevant teams empty means all teams; leaving all games empty means all games.")
    s_static_spawn_zone_data data;

    real_point3d position;
    float lower_height;
    float upper_height;
    float inner_radius;
    float outer_radius;
    float weight;
};
TAG_BLOCK_SIZE_ASSERT(s_static_spawn_zone, 48);
