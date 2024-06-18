#pragma once
#include "game_engine.h"
#include "game_options.h"
#include "materials.h"

#include "bitmaps/bitmap_group.h"
#include "interface/interface.h"
#include "main/game_preferences.h"
#include "main/level_definitions.h"
#include "objects/damage.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"
#include "text/text_group.h"


#include "H2MOD/Tags/TagInterface.h"

#define NUMBER_OF_GLOBAL_SOUNDS 2
#define k_unit_grenade_types_count 2
#define k_global_vertex_shader_count 32
#define k_maximum_material_types 256
#define k_game_globals_maximum_multiplayer_colors 32

enum e_game_globals_rasterizer_flags : int16
{
    _game_globals_rasterizer_flag_tint_edge_density = FLAG(0)
};

// max count: 1
struct s_game_globals_havok_cleanup_resources
{
    tag_reference object_cleanup_effect;
};
ASSERT_STRUCT_SIZE(s_game_globals_havok_cleanup_resources, 8);

// max count: 1
struct s_game_globals_collision_damage
{
    tag_reference collision_damage; // jpt!
    real32 min_game_acc_default;    // 0-oo
    real32 max_game_acc_default;    // 0-oo
    real32 min_game_scale_default;  // 0-1
    real32 max_game_scale_default;  // 0-1
    real32 min_abs_acc_default;     // 0-oo
    real32 max_abs_acc_default;     // 0-oo
    real32 min_abs_scale_default;   // 0-1
    real32 max_abs_scale_default;   // 0-1

    int32 pad[8];
};
ASSERT_STRUCT_SIZE(s_game_globals_collision_damage, 72);

// max count: 1
struct s_sound_globals_definition
{
    tag_reference sound_classes;                    // sncl
    tag_reference sound_effects;                    // sfx+
    tag_reference sound_mix;                        // snmx
    tag_reference sound_combat_dialogue_constants;  // spk!
    datum sound_gesalt;                             // ugh!
};
ASSERT_STRUCT_SIZE(s_sound_globals_definition, 36);

// max count: 1
struct s_ai_globals_gravemind_definition
{
    real32 min_retreat_time_secs;
    real32 ideal_retreat_time_secs;
    real32 max_retreat_time_Secs;
};
ASSERT_STRUCT_SIZE(s_ai_globals_gravemind_definition, 12);

// max count: 1
struct s_ai_globals_definition
{
    real32 danger_broadly_facing;
    int32 pad;
    real32 danger_shooting_near;
    int32 pad1;
    real32 danger_shooting_at;
    int32 pad2;
    real32 danger_extremely_close;
    int32 pad3;
    real32 danger_shield_damage;
    real32 danger_extended_shield_damage;
    real32 danger_body_damage;
    real32 danger_extended_body_damage;
    int32 pad4[12];
    tag_reference global_dialogue_tag;                  // adlg
    string_id default_mission_dialogue_sound_effect;
    int32 pad5[5];
    real32 jump_down_wu_tick;
    real32 jump_step_wu_tick;
    real32 jump_crouch_wu_tick;
    real32 jump_stand_wu_tick;
    real32 jump_storey_wu_tick;
    real32 jump_tower_wu_tick;
    real32 max_jump_down_height_down_wu;
    real32 max_jump_down_height_step_wu;
    real32 max_jump_down_height_crouch_wu;
    real32 max_jump_down_height_stand_wu;
    real32 max_jump_down_height_storey_wu;
    real32 max_jump_down_height_tower_wu;
    real_vector2d hoist_step_wus;
    real_vector2d hoist_crouch_wus;
    real_vector2d hoist_stand_wus;
    int32 pad6[6];
    real_vector2d vault_step_wus;
    real_vector2d vault_crouch_wus;
    int32 pad7[12];
    tag_block<s_ai_globals_gravemind_definition> gravemind_properties;

    int32 pad8[12];
    real32 scary_target_threhold;       // A target of this scariness is offically considered scary (by combat dialogue, etc.)
    real32 scary_weapon_threhold;       // A weapon of this scariness is offically considered scary (by combat dialogue, etc.)
    real32 player_scariness;
    real32 berserking_actor_scariness;
};
ASSERT_STRUCT_SIZE(s_ai_globals_definition, 360);

// max count: 1
struct s_damage_globals_definition
{
    tag_block<s_damage_group_definition> damage_groups;
};
ASSERT_STRUCT_SIZE(s_damage_globals_definition, 8);

// max count: 1
struct s_game_globals_camera
{
    tag_reference default_unit_camera_track;
    real32 default_change_pause;
    real32 first_person_change_pause;
    real32 following_camera_change_pause;
};
ASSERT_STRUCT_SIZE(s_game_globals_camera, 20);

// max count: 1
struct s_game_globals_player_control
{
    real32 magnetism_friction;              // how much the crosshair slows over enemies
    real32 magnetism_adhesion;              // how much the crosshair sticks to enemies
    real32 inconsequential_target_scale;    // scales magnetism level for inconsequential targets like infection forms

    int32 pad[3];
    
    // Explaination("crosshair", "")
    // -1..1, 0 is middle of the screen
    real_point2d crosshair_location;

    // Explaination("sprinting", "")

    real32 seconds_to_start;         // how long you must be pegged before you start sprinting
    real32 seconds_to_full_speed;    // how long you must sprint before you reach top speed
    real32 decay_rate;               // how fast being unpegged decays the timer (seconds per second)
    real32 full_speed_multiplier;    // how much faster we actually go when at full sprint
    real32 pegged_magnitude;         // how far the stick needs to be pressed before being considered pegged
    real32 pegged_angular_threshold; // how far off straight up (in degrees) we consider pegged

    int32 pad1[2];
    // Explaination("looking", "")

    real32 look_default_pitch_rate_degrees;
    real32 look_default_yaw_rate_degrees;

    real32 look_peg_threshold;                      // magnitude of yaw for pegged acceleration to kick in
    real32 look_yaw_acceleration_time_seconds;      // time for a pegged look to reach maximum effect
    real32 look_yaw_acceleration_scale;             // maximum effect of a pegged look (scales last value in the look function below)
    real32 look_pitch_acceleration_time_seconds;    // time for a pegged look to reach maximum effect
    real32 look_pitch_acceleration_scale;           // maximum effect of a pegged look (scales last value in the look function below)
    real32 look_autolevelling_scale;                // 1 is fast, 0 is none, >1 will probably be really fast
    int32 pad2[2];
    real32 gravity_scale;
    int16 pad3;
    int16 minimum_autolevelling_ticks;              // amount of time player needs to move and not look up or down for autolevelling to kick in
    real32 minimum_angle_for_vehicle_flipping;      // 0 means the vehicle's up vector is along the ground, 90 means the up vector is pointing straight up:degrees
    tag_block<real32> look_function;                // max count: 16
    real32 minimum_action_hold_time_seconds;        // time that player needs to press ACTION to register as a HOLD
};
ASSERT_STRUCT_SIZE(s_game_globals_player_control, 128);

// max count: 1
struct s_game_globals_difficulty_information
{
    // Explaination("health", "scale values for enemy health and damage settings")

    real32 easy_enemy_damage;           // enemy damage multiplier on easy difficulty
    real32 normal_enemy_damage;         // enemy damage multiplier on normal difficulty
    real32 hard_enemy_damage;           // enemy damage multiplier on hard difficulty
    real32 imposs_enemy_damage;         // enemy damage multiplier on impossible difficulty

    real32 easy_enemy_vitality;         // enemy maximum body vitality scale on easy difficulty
    real32 normal_enemy_vitality;       // enemy maximum body vitality scale on normal difficulty
    real32 hard_enemy_vitality;         // enemy maximum body vitality scale on hard difficulty
    real32 imposs_enemy_vitality;       // enemy maximum body vitality scale on impossible difficulty

    real32 easy_enemy_shield;           // enemy maximum shield vitality scale on easy difficulty
    real32 normal_enemy_shield;         // enemy maximum shield vitality scale on normal difficulty
    real32 hard_enemy_shield;           // enemy maximum shield vitality scale on hard difficulty
    real32 imposs_enemy_shield;         // enemy maximum shield vitality scale on impossible difficulty

    real32 easy_enemy_recharge;         // enemy shield recharge scale on easy difficulty
    real32 normal_enemy_recharge;       // enemy shield recharge scale on normal difficulty
    real32 hard_enemy_recharge;         // enemy shield recharge scale on hard difficulty
    real32 imposs_enemy_recharge;       // enemy shield recharge scale on impossible difficulty

    real32 easy_friend_damage;          // friend damage multiplier on easy difficulty
    real32 normal_friend_damage;        // friend damage multiplier on normal difficulty
    real32 hard_friend_damage;          // friend damage multiplier on hard difficulty
    real32 imposs_friend_damage;        // friend damage multiplier on impossible difficulty

    real32 easy_friend_vitality;        // friend maximum body vitality scale on easy difficulty
    real32 normal_friend_vitality;      // friend maximum body vitality scale on normal difficulty
    real32 hard_friend_vitality;        // friend maximum body vitality scale on hard difficulty
    real32 imposs_friend_vitality;      // friend maximum body vitality scale on impossible difficulty

    real32 easy_friend_shield;          // friend maximum shield vitality scale on easy difficulty
    real32 normal_friend_shield;        // friend maximum shield vitality scale on normal difficulty
    real32 hard_friend_shield;          // friend maximum shield vitality scale on hard difficulty
    real32 imposs_friend_shield;        // friend maximum shield vitality scale on impossible difficulty

    real32 easy_friend_recharge;        // friend shield recharge scale on easy difficulty
    real32 normal_friend_recharge;      // friend shield recharge scale on normal difficulty
    real32 hard_friend_recharge;        // friend shield recharge scale on hard difficulty
    real32 imposs_friend_recharge;      // friend shield recharge scale on impossible difficulty

    real32 easy_infection_forms;        // toughness of infection forms (may be negative) on easy difficulty
    real32 normal_infection_forms;      // toughness of infection forms (may be negative) on normal difficulty
    real32 hard_infection_forms;        // toughness of infection forms (may be negative) on hard difficulty
    real32 imposs_infection_forms;      // toughness of infection forms (may be negative) on impossible difficulty

    int32 pad[4];
    
    // Explaination("ranged fire", "difficulty-affecting values for enemy ranged combat settings")

    real32 easy_rate_of_fire;           // enemy rate of fire scale on easy difficulty
    real32 normal_rate_of_fire;         // enemy rate of fire scale on normal difficulty
    real32 hard_rate_of_fire;           // enemy rate of fire scale on hard difficulty
    real32 imposs_rate_of_fire;         // enemy rate of fire scale on impossible difficulty

    real32 easy_projectile_error;       // enemy projectile error scale, as a fraction of their base firing error. on easy difficulty
    real32 normal_projectile_error;     // enemy projectile error scale, as a fraction of their base firing error. on normal difficulty
    real32 hard_projectile_error;       // enemy projectile error scale, as a fraction of their base firing error. on hard difficulty
    real32 imposs_projectile_error;     // enemy projectile error scale, as a fraction of their base firing error. on impossible difficulty

    real32 easy_burst_error;            // enemy burst error scale; reduces intra-burst shot distance. on easy difficulty
    real32 normal_burst_error;          // enemy burst error scale; reduces intra-burst shot distance. on normal difficulty
    real32 hard_burst_error;            // enemy burst error scale; reduces intra-burst shot distance. on hard difficulty
    real32 imposs_burst_error;          // enemy burst error scale; reduces intra-burst shot distance. on impossible difficulty

    real32 easyNew_target_delay;        // enemy new-target delay scale factor. on easy difficulty
    real32 normalNew_target_delay;      // enemy new-target delay scale factor. on normal difficulty
    real32 hardNew_target_delay;        // enemy new-target delay scale factor. on hard difficulty
    real32 impossNew_target_delay;      // enemy new-target delay scale factor. on impossible difficulty

    real32 easy_burst_separation;       // delay time between bursts scale factor for enemies. on easy difficulty
    real32 normal_burst_separation;     // delay time between bursts scale factor for enemies. on normal difficulty
    real32 hard_burst_separation;       // delay time between bursts scale factor for enemies. on hard difficulty
    real32 imposs_burst_separation;     // delay time between bursts scale factor for enemies. on impossible difficulty

    real32 easy_target_tracking;        // additional target tracking fraction for enemies. on easy difficulty
    real32 normal_target_tracking;      // additional target tracking fraction for enemies. on normal difficulty
    real32 hard_target_tracking;        // additional target tracking fraction for enemies. on hard difficulty
    real32 imposs_target_tracking;      // additional target tracking fraction for enemies. on impossible difficulty

    real32 easy_target_leading;         // additional target leading fraction for enemies. on easy difficulty
    real32 normal_target_leading;       // additional target leading fraction for enemies. on normal difficulty
    real32 hard_target_leading;         // additional target leading fraction for enemies. on hard difficulty
    real32 imposs_target_leading;       // additional target leading fraction for enemies. on impossible difficulty

    real32 easy_overcharge_chance;      // overcharge chance scale factor for enemies. on easy difficulty
    real32 normal_overcharge_chance;    // overcharge chance scale factor for enemies. on normal difficulty
    real32 hard_overcharge_chance;      // overcharge chance scale factor for enemies. on hard difficulty
    real32 imposs_overcharge_chance;    // overcharge chance scale factor for enemies. on impossible difficulty

    real32 easy_special_fire_delay;     // delay between special-fire shots (overcharge, banshee bombs) scale factor for enemies. on easy difficulty
    real32 normal_special_fire_delay;   // delay between special-fire shots (overcharge, banshee bombs) scale factor for enemies. on normal difficulty
    real32 hard_special_fire_delay;     // delay between special-fire shots (overcharge, banshee bombs) scale factor for enemies. on hard difficulty
    real32 imposs_special_fire_delay;   // delay between special-fire shots (overcharge, banshee bombs) scale factor for enemies. on impossible difficulty

    real32 easyGuidance_vs_player;      // guidance velocity scale factor for all projectiles targeted on a player. on easy difficulty
    real32 normalGuidance_vs_player;    // guidance velocity scale factor for all projectiles targeted on a player. on normal difficulty
    real32 hardGuidance_vs_player;      // guidance velocity scale factor for all projectiles targeted on a player. on hard difficulty
    real32 impossGuidance_vs_player;    // guidance velocity scale factor for all projectiles targeted on a player. on impossible difficulty

    real32 easy_melee_delay_base;       // delay period added to all melee attacks, even when berserk. on easy difficulty
    real32 normal_melee_delay_base;     // delay period added to all melee attacks, even when berserk. on normal difficulty
    real32 hard_melee_delay_base;       // delay period added to all melee attacks, even when berserk. on hard difficulty
    real32 imposs_melee_delay_base;     // delay period added to all melee attacks, even when berserk. on impossible difficulty

    real32 easy_melee_delay_scale;      // multiplier for all existing non-berserk melee delay times. on easy difficulty
    real32 normal_melee_delay_scale;    // multiplier for all existing non-berserk melee delay times. on normal difficulty
    real32 hard_melee_delay_scale;      // multiplier for all existing non-berserk melee delay times. on hard difficulty
    real32 imposs_melee_delay_scale;    // multiplier for all existing non-berserk melee delay times. on impossible difficulty

    int32 pad1[4];

    // Explaination("grenades", "difficulty-affecting values for enemy grenade behavior")

    real32 easy_grenade_chance_scale;   // scale factor affecting the desicions to throw a grenade. on easy difficulty
    real32 normal_grenade_chance_scale; // scale factor affecting the desicions to throw a grenade. on normal difficulty
    real32 hard_grenade_chance_scale;   // scale factor affecting the desicions to throw a grenade. on hard difficulty
    real32 imposs_grenade_chance_scale; // scale factor affecting the desicions to throw a grenade. on impossible difficulty

    real32 easy_grenade_timer_scale;    // scale factor affecting the delay period between grenades thrown from the same encounter (lower is more often). on easy difficulty
    real32 normal_grenade_timer_scale;  // scale factor affecting the delay period between grenades thrown from the same encounter (lower is more often). on normal difficulty
    real32 hard_grenade_timer_scale;    // scale factor affecting the delay period between grenades thrown from the same encounter (lower is more often). on hard difficulty
    real32 imposs_grenade_timer_scale;  // scale factor affecting the delay period between grenades thrown from the same encounter (lower is more often). on impossible difficulty
    
    int32 pad2[4];
    int32 pad3[4];
    int32 pad4[4];

    // Explaination("placement", "difficulty-affecting values for enemy placement")

    real32 easy_major_upgrade_normal;   // fraction of actors upgraded to their major variant. on easy difficulty
    real32 normal_major_upgrade_normal; // fraction of actors upgraded to their major variant. on normal difficulty
    real32 hard_major_upgrade_normal;   // fraction of actors upgraded to their major variant. on hard difficulty
    real32 imposs_major_upgrade_normal; // fraction of actors upgraded to their major variant. on impossible difficulty

    real32 easy_major_upgrade_few;      // fraction of actors upgraded to their major variant when mix = normal. on easy difficulty
    real32 normal_major_upgrade_few;    // fraction of actors upgraded to their major variant when mix = normal. on normal difficulty
    real32 hard_major_upgrade_few;      // fraction of actors upgraded to their major variant when mix = normal. on hard difficulty
    real32 imposs_major_upgrade_few;    // fraction of actors upgraded to their major variant when mix = normal. on impossible difficulty

    real32 easy_major_upgrade_many;     // fraction of actors upgraded to their major variant when mix = many. on easy difficulty
    real32 normal_major_upgrade_many;   // fraction of actors upgraded to their major variant when mix = many. on normal difficulty
    real32 hard_major_upgrade_many;     // fraction of actors upgraded to their major variant when mix = many. on hard difficulty
    real32 imposs_major_upgrade_many;   // fraction of actors upgraded to their major variant when mix = many. on impossible difficulty


    // Explaination("vehicles", "difficulty-affecting values for vehicle driving/combat")
    
    real32 easy_player_vehicle_ram_chance;      // Chance of deciding to ram the player in a vehicle on easy difficulty
    real32 normal_player_vehicle_ram_chance;    // Chance of deciding to ram the player in a vehicle on normal difficulty
    real32 hard_player_vehicle_ram_chance;      // Chance of deciding to ram the player in a vehicle on hard difficulty
    real32 imposs_player_vehicle_ram_chance;    // Chance of deciding to ram the player in a vehicle on impossible difficulty

    int32 pad5[4];
    int32 pad6[4];
    int32 pad7[4];
    int32 pad8[21];
};
ASSERT_STRUCT_SIZE(s_game_globals_difficulty_information, 644);

// max count: k_unit_grenade_types_count
struct s_game_globals_grenade
{
    int16 maximum_count;
    int16 pad;
    tag_reference throwing_effect;  // effe
    int32 pad1[4];
    tag_reference equipment;        // eqip
    tag_reference projectile;       // proj
};
ASSERT_STRUCT_SIZE(s_game_globals_grenade, 44);

// max count: k_global_vertex_shader_count
struct s_vertex_shader_reference
{
    tag_reference vertex_shader;  // vrtx
};
ASSERT_STRUCT_SIZE(s_vertex_shader_reference, 8);

// max count: 1
struct s_game_globals_rasterizer_data
{
    // Explaination("function textures", "Used internally by the rasterizer. (Do not change unless you know what you're doing!)")
    tag_reference distance_attenuation;     // bitm
    tag_reference vector_normalization;     // bitm
    tag_reference gradients;                // bitm
    tag_reference loading_screen;           // bitm
    tag_reference loading_screen_sweep;     // bitm
    tag_reference loading_spinners;         // bitm
    tag_reference glow;                     // bitm
    tag_reference loading_screen_logos;     // bitm
    tag_reference loading_screen_tickers;   // bitm

    int32 pad[4];
    tag_block<s_vertex_shader_reference> global_vertex_shaders;

    // Explaination("default textures", "Used internally by the rasterizer - additive, multiplicative, detail, vector. (Do not change ever, period.)")
    tag_reference default_textures[k_bitmap_type_count];    // bitm

    // Explaination("experimental textures", "Used internally by the rasterizer. (Used by Bernie's experimental shaders.)")
    tag_reference experimental_bitmap_0;    // bitm
    tag_reference experimental_bitmap_1;    // bitm
    tag_reference experimental_bitmap_2;    // bitm
    tag_reference experimental_bitmap_3;    // bitm

    // Explaination("video effect textures", "Used in cinematics.")
    tag_reference video_effect_bitmap_0;    // bitm
    tag_reference video_effect_bitmap_1;    // bitm

    int32 pad1[9];
    
    // Explaination("global shader", "Used for layers that need to do something for other layers to work correctly if the layer is disabled, also used for active-camo, etc.")
    tag_reference global_shader;    // shad

    // Explaination("active camouflage", "")

    e_game_globals_rasterizer_flags flags;
    int16 pad2;
    real32 refraction_amount_pixels;
    real32 distance_falloff;
    real_rgb_color tint_color;
    real32 hyper_stealth_refraction_pixels;
    real32 hyper_stealth_distance_falloff;
    real_rgb_color hyper_stealth_tint_color;

    // Explaination("PC textures", "The PC can't use 3D textures, so we use this instead.")
    tag_reference pc_3d_bitmap;     // bitm
};
ASSERT_STRUCT_SIZE(s_game_globals_rasterizer_data, 264);

// max count: 1
struct s_game_globals_interface_tag_references
{
    tag_reference interface_tag_references[NUMBER_OF_INTERFACE_TAGS];
    tag_reference mainmenu_ui_globals;          // wgtz
    tag_reference singleplayer_ui_globals;      // wgtz
    tag_reference multiplayer_ui_globals;       // wgtz
};
ASSERT_STRUCT_SIZE(s_game_globals_interface_tag_references, 152);

struct s_game_globals_tag_reference
{
    tag_reference reference;
};
ASSERT_STRUCT_SIZE(s_game_globals_tag_reference, 8);

// max count: 1
struct s_game_globals_multiplayer_information
{
    tag_reference flag;         // item
    tag_reference unit;         // unit
    tag_block<s_game_globals_tag_reference> vehicles;   // max count: 20

    tag_reference hill_shader;  // shad
    tag_reference flag_shader;  // shad
    tag_reference ball;         // item
    tag_block<s_game_globals_tag_reference> sounds;     // max count: 60

    tag_reference in_game_text; // unic
    int32 pad[10];
    tag_block<s_multiplayer_event_response_definition> general_events;
    tag_block<s_multiplayer_event_response_definition> slayer_events;
    tag_block<s_multiplayer_event_response_definition> ctf_events;
    tag_block<s_multiplayer_event_response_definition> oddball_events;
    tag_block<> unk_block;
    tag_block<s_multiplayer_event_response_definition> king_events;
};
ASSERT_STRUCT_SIZE(s_game_globals_multiplayer_information, 152);

// max count: 1
struct s_game_globals_player_information
{
    tag_reference unused;           // unit
    int32 pad[7];
    real32 walking_speed;           // world units per second
    int32 pad1;
    real32 run_forward;             // world units per second
    real32 run_backward;            // world units per second
    real32 run_sideways;            // world units per second
    real32 run_acceleration;        // world units per second
    real32 sneak_forward;           // world units per second
    real32 sneak_backward;          // world units per second
    real32 sneak_sideways;          // world units per second
    real32 sneak_acceleration;      // world units per second
    real32 airborne_acceleration;   // world units per second
    int32 pad2[4];
    real_point3d grenade_origin;
    int32 pad3[3];
    real32 stun_movement_penalty;       // 1.0 prevents moving while stunned
    real32 stun_turning_penalty;        // 1.0 prevents turning while stunned
    real32 stun_jumping;                // 1.0 prevents jumping while stunned
    real32 minimum_stun_time_seconds;   // all stunning damage will last for at least this long
    real32 maximum_stun_time_seconds;   // no stunning damage will last for longer than this
    int32 pad4[2];
    real_vector2d first_person_idle_time_seconds;
    real32 first_person_skip_fraction;
    int32 pad5[4];
    tag_reference coop_respawn_effect;          // effe
    int32 binoculars_zoom_count;
    real_vector2d binoculars_zoom_range;
    tag_reference binoculars_zoom_in_sound;     // snd!
    tag_reference binoculars_zoom_out_sound;    // snd!
    int32 pad6[4];
    tag_reference active_camouflage_on;     // snd!
    tag_reference active_camouflage_off;    // snd!
    tag_reference active_camouflage_error;  // snd!
    tag_reference active_camouflage_ready;  // snd!
    tag_reference flashlight_on;    // snd!
    tag_reference flashlight_off;   // snd!
    tag_reference ice_cream;        // snd!
};
ASSERT_STRUCT_SIZE(s_game_globals_player_information, 284);

// max count: k_player_character_type_count
struct s_game_globals_player_representation
{
    tag_reference first_person_hands;   // mode
    tag_reference first_person_body;    // mode
    int32 pad[10];
    int32 pad1[30];
    tag_reference third_person_unit;    // unit
    string_id third_person_variant;
};
ASSERT_STRUCT_SIZE(s_game_globals_player_representation, 188);

// max count: 1
struct s_game_globals_falling_damage
{
    int32 pad[2];
    real_vector2d harmful_falling_distance; // world units
    tag_reference falling_damage;           // jpt!
    int32 pad1[2];
    real32 maximum_falling_distance;         // world units
    tag_reference distance_damage;          // jpt!
    tag_reference vehicle_environment_collision_damage_effect;  // jpt!
    tag_reference vehicle_killed_unit_damage_effect;            // jpt!
    tag_reference vehicle_collision_damage; // jpt!
    tag_reference flaming_death_damage;     // jpt!
    int32 pad2[4];
    real32 field_5C;
    real32 field_60;
    real32 unk_gravity_64;
};
ASSERT_STRUCT_SIZE(s_game_globals_falling_damage, 104);

// max count: 1
struct s_game_globals_multiplayer_ui
{
    tag_reference random_player_names;    // unic
    tag_block<real_rgb_color> obsolete_profile_colors;  // max count: k_game_globals_maximum_multiplayer_colors
    tag_block<real_rgb_color> team_colors;              // max count: k_game_globals_maximum_multiplayer_colors
    tag_reference team_names;           // unic
};
ASSERT_STRUCT_SIZE(s_game_globals_multiplayer_ui, 32);

struct s_game_globals
{
    uint8 pad0[172];
    e_language language;

    tag_block<s_game_globals_havok_cleanup_resources> havok_cleanup_resources;
    tag_block<s_game_globals_collision_damage> collision_damage;
    tag_block<s_sound_globals_definition> sound_globals;
    tag_block<s_ai_globals_definition> ai_globals;
    tag_block<s_damage_globals_definition> damage_table;
    tag_block<> null_tagblock;                  // Might be used as a runtime tagblock?
    tag_block<tag_reference> sounds;            // According to guerilla this is unused (max count: NUMBER_OF_GLOBAL_SOUNDS) 
    tag_block<s_game_globals_camera> camera;
    tag_block<s_game_globals_player_control> player_control;
    tag_block<s_game_globals_difficulty_information> difficulty;
    tag_block<s_game_globals_grenade> grenades;
    tag_block<s_game_globals_rasterizer_data> rasterizer_data;
    tag_block<s_game_globals_interface_tag_references> interface_tags;
    tag_block<s_game_globals_tag_reference> weapon_list;    // max count: 20
    tag_block<s_game_globals_tag_reference> cheat_powerups; // max count: 20
    tag_block<s_game_globals_multiplayer_information> multiplayer_information;
    tag_block<s_game_globals_player_information> player_information;
    tag_block<s_game_globals_player_representation> player_representation;
    tag_block<s_game_globals_falling_damage> falling_damage;
    tag_block<material_definition> old_materials;
    tag_block<s_global_material_definition> materials;
    tag_block<s_game_globals_multiplayer_ui> multiplayer_ui;
    tag_block<real_rgb_color> profile_colors;               // max count: k_game_globals_maximum_multiplayer_colors

    tag_reference multiplayer_globals;      // mulg

    tag_block<s_runtime_levels_definition> runtime_level_data;
    tag_block<s_ui_levels_definition> ui_level_data;

    // Explaination("Default global lighting", "")
    tag_reference default_global_lighting;  // gldf

    // Populated during game startup
    c_language_pack language_pack[k_language_count];
};
ASSERT_STRUCT_SIZE(s_game_globals, 644);

/* public code */

s_game_globals* scenario_get_game_globals(void);
s_ui_levels_definition* game_globals_get_ui_levels(void);
s_game_globals_player_representation* game_globals_get_representation(e_character_type type);

void game_globals_apply_tag_patches(s_game_options* options);

static tag_reference game_globals_get_interface_tag_reference(e_interface_tag interface_tag_index)
{
    return scenario_get_game_globals()->interface_tags[0]->interface_tag_references[interface_tag_index];
}
