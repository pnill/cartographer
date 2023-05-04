#pragma once
#include "Blam/Common/Common.h"
#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Engine/tag_files/string_id.h"

enum e_new_hud_dashlight_flags : short
{
    new_hud_dashlight_flag_dont_scale_when_pulsing = FLAG(0),
};

#define k_hud_dashlight_count 9
struct s_new_hud_dashlight_definition
{
    // bitm
    tag_reference bitmap;
    // shad
    tag_reference shader;
    short sequence_index;

    e_new_hud_dashlight_flags flags;
    // snd!
    tag_reference sound;
};
TAG_BLOCK_SIZE_ASSERT(s_new_hud_dashlight_definition, 28);

#define k_hud_waypoint_arrow_type_count 4
struct s_new_hud_waypoint_arrow_definition
{
    // bitm
    tag_reference bitmap;
    // shad
    tag_reference shader;
    short sequence_index;
    short pad;
    float smallest_size;
    float smallest_distance;
    // bitm
    tag_reference border_bitmap;
};
TAG_BLOCK_SIZE_ASSERT(s_new_hud_waypoint_arrow_definition, 36);

#define k_hud_waypoint_type_count 8
struct s_new_hud_waypoint_definition
{
    // bitm
    tag_reference bitmap;
    // shad
    tag_reference shader;
    short onscreen_sequence_index;
    short occluded_sequence_index;
    short offscreen_sequence_index;
    short pad;
};
TAG_BLOCK_SIZE_ASSERT(s_new_hud_waypoint_definition, 24);

enum e_sound_latched_to : int
{
    sound_latched_to_shield_recharging = FLAG(0),
    sound_latched_shield_damaged = FLAG(1),
    sound_latched_shield_low = FLAG(2),
    sound_latched_shield_empty = FLAG(3),
    sound_latched_health_low = FLAG(4),
    sound_latched_health_empty = FLAG(5),
    sound_latched_health_minor_damage = FLAG(6),
    sound_latched_health_major_damage = FLAG(7),
    sound_latched_rocket_locking = FLAG(8),
    sound_latched_rocket_locked = FLAG(9)
};

#define k_maximum_hud_sounds 6
struct new_hud_sound_element_definition
{
    tag_reference chief_sound;
    e_sound_latched_to latched_to;
    float scale;
    tag_reference dervish_sound;
};

enum e_player_training_flags : short
{
    player_training_flag_not_in_multiplayer = FLAG(0),
};

#define k_player_training_count 32
struct s_player_training_entry_data
{
    /// comes out of the HUD text globals
    string_id display_string;
    /// comes out of the HUD text globals, used for grouped prompt
    string_id display_string2;
    /// comes out of the HUD text globals, used for ungrouped prompt
    string_id display_string3;
    /// how long the message can be on screen before being hidden
    short max_display_time;
    /// how many times a training message will get displayed (0-3 only!)
    short display_count;
    /// how long a displayed but untriggered message stays up
    short dissapear_delay;
    /// how long after display this message will stay hidden
    short redisplay_delay;
    /// how long the event can be triggered before it's displayed
    float display_delays;

    e_player_training_flags flags;
    short pad;
};
TAG_BLOCK_SIZE_ASSERT(s_player_training_entry_data, 28);


struct s_new_hud_globals_constants
{
    tag_reference primary_message_sound;
    tag_reference secondary_message_sound;
    string_id boot_griefer_string;
    string_id cannot_boot_griefer_string;
    // shad
    tag_reference training_shader;
    // bitm
    tag_reference human_training_top_right;
    // bitm
    tag_reference human_training_top_center;
    // bitm
    tag_reference human_training_top_left;
    // bitm
    tag_reference human_training_middle;
    // bitm
    tag_reference elite_training_top_right;
    // bitm
    tag_reference elite_training_top_center;
    // bitm
    tag_reference elite_training_top_left;
    // bitm
    tag_reference elite_training_middle;
};
TAG_BLOCK_SIZE_ASSERT(s_new_hud_globals_constants, 96);

struct s_new_hud_globals  
{
    // unic
    tag_reference hud_text;

    tag_block<s_new_hud_dashlight_definition> dashlights;
    
    tag_block<s_new_hud_waypoint_arrow_definition> waypoint_arrows;
    
    tag_block<s_new_hud_waypoint_definition> waypoints;
    
    tag_block<new_hud_sound_element_definition> hud_sounds;

    tag_block<s_player_training_entry_data> player_training_data;

    s_new_hud_globals_constants constants;
};
TAG_BLOCK_SIZE_ASSERT(s_new_hud_globals, 144);
