#pragma once
#include "Blam/Cache/DataTypes/BlamDataTypes.h"
#include "Blam/Cache/TagGroups.hpp"

#include "Blam/Common/Common.h"
#include "Blam/Math/real_math.h"
#include "Blam/Engine/animations/animation_channel.h"
#include "Blam/Engine/tag_files/string_id.h"

#include <wtypes.h>

#define k_max_nodes_per_animation = 255

struct s_additional_node_data
{
    string_id node_name;
    real_quaternion default_rotation;
    real_point3d default_translation;
    float default_scale;
    real_point3d min_bounds;
    real_point3d max_bounds;
};
TAG_BLOCK_SIZE_ASSERT(s_additional_node_data, 60);

struct inherited_animation_node_map_block
{
    short local_node;
};
TAG_BLOCK_SIZE_ASSERT(inherited_animation_node_map_block, 2);

struct inherited_animation_node_map_flag_block
{
    DWORD local_node_flags;
};
TAG_BLOCK_SIZE_ASSERT(inherited_animation_node_map_flag_block, 4);

enum e_inheritance_flags : byte
{
    inheritance_flag_inherit_root_translation_scale_only = FLAG(0),
    inheritance_flag_inherit_for_use_on_player = FLAG(1)
};

struct s_animation_inheritence
{
    tag_reference inherited_graph;
    tag_block<inherited_animation_node_map_block> node_map;
    tag_block<inherited_animation_node_map_flag_block> node_map_flags;

    float root_z_offset;
    e_inheritance_flags inheritance_flags;
    PAD(3);
};
TAG_BLOCK_SIZE_ASSERT(s_animation_inheritence, 32);

struct s_weapon_class_listing
{
    string_id weapon_name;
    string_id weapon_class;
};

class c_model_animation_runtime_data
{
    tag_block<s_animation_inheritence> inheritence_list;
    tag_block<s_weapon_class_listing> weapon_list;

    DWORD left_arm_nodes_1;
    DWORD left_arm_nodes_2;
    DWORD left_arm_nodes_3;
    DWORD left_arm_nodes_4;
    DWORD left_arm_nodes_5;
    DWORD left_arm_nodes_6;
    DWORD left_arm_nodes_7;
    DWORD left_arm_nodes_8;

    DWORD right_arm_nodes_1;
    DWORD right_arm_nodes_2;
    DWORD right_arm_nodes_3;
    DWORD right_arm_nodes_4;
    DWORD right_arm_nodes_5;
    DWORD right_arm_nodes_6;
    DWORD right_arm_nodes_7;
    DWORD right_arm_nodes_8;
};
TAG_BLOCK_SIZE_ASSERT(c_model_animation_runtime_data, 80);

enum e_node_model_flags : byte
{
    node_model_flag_primary_model = FLAG(0),
    node_model_flag_secondary_model = FLAG(1),
    node_model_flag_local_root = FLAG(2),
    node_model_flag_left_hand = FLAG(3),
    node_model_flag_right_hand = FLAG(4),
    node_model_flag_left_arm_member = FLAG(5),
};

enum e_node_joint_flags : byte
{
    node_joint_flag_ball_socket = FLAG(0),
    node_joint_flag_hinge = FLAG(1),
    node_joint_flag_no_movement = FLAG(2),
};

struct animation_graph_node
{
    string_id name;
    short next_sibling_node_index;
    short first_child_node_index;
    short parent_node_index;

    e_node_model_flags model_flags;
    e_node_joint_flags node_joint_flags;

    real_vector3d baseVector;
    float vector_range;
    float zpos;
};
TAG_BLOCK_SIZE_ASSERT(animation_graph_node, 32);

enum e_animation_graph_resources_flags : byte
{
    animation_graph_resources_prepared_for_cache = FLAG(0),
    animation_graph_resources_unused = FLAG(1),
    animation_graph_resources_imported_with_codec_compressors = FLAG(2),
    animation_graph_resources_unused_smelly_flag = FLAG(3),
    animation_graph_resources_written_to_cache = FLAG(4),
    animation_graph_resources_animation_data_reordered = FLAG(5),
};

enum e_animation_graph_reference_flags : short
{
    animation_graph_reference_flag_allow_on_player = FLAG(0),
    animation_graph_reference_flag_left_arm_only = FLAG(1),
    animation_graph_reference_flag_right_arm_only = FLAG(2),
    animation_graph_reference_flag_first_person_only = FLAG(3),
    animation_graph_reference_flag_forward_only = FLAG(4),
    animation_graph_reference_flag_reverse_only = FLAG(5),
};

struct animation_graph_sound_reference
{
    // TagReference("snd!")
    tag_reference sound;
    e_animation_graph_reference_flags flags;
    PAD16;
};
TAG_BLOCK_SIZE_ASSERT(animation_graph_sound_reference, 0xC);

struct animation_graph_effect_reference
{
    // TagReference("snd!")
    tag_reference effect;
    e_animation_graph_reference_flags flags;
    PAD16;
};
TAG_BLOCK_SIZE_ASSERT(animation_graph_effect_reference, 12);

struct animation_aiming_screen_bounds
{
    float right_yaw_per_frame;
    float left_yaw_per_frame;
    short right_frame_count;
    short left_frame_count;
    float down_pitch_per_frame;
    float up_pitch_per_frame;
    short down_pitch_frame_count;
    short up_pitch_frame_count;
};
TAG_BLOCK_SIZE_ASSERT(animation_aiming_screen_bounds, 24);

struct animation_blend_screen_block
{
    string_id label;
    animation_aiming_screen_bounds aimingScreen;
};
TAG_BLOCK_SIZE_ASSERT(animation_blend_screen_block, 28);

enum e_animation_type : byte
{
    animation_type_base = 0,
    animation_type_overlay = 1,
    animation_type_replacement = 2,
};

enum e_frame_info_type : byte
{
    frame_info_type_none = 0,
    frame_info_type_dxdy = 1,
    frame_info_type_dxdydyaw = 2,
    frame_info_type_dxdydzdyaw = 3,
};

enum e_model_animation_flags : byte
{
    model_animation_flag_unused0 = FLAG(0),
    model_animation_flag_world_relative = FLAG(1),
    model_animation_flag_unused1 = FLAG(2),
    model_animation_flag_unused2 = FLAG(3),
    model_animation_flag_unused3 = FLAG(4),
    model_animation_flag_compression_disabled = FLAG(5),
    model_animation_flag_old_production_checksum = FLAG(6),
    model_animation_flag_valid_production_checksum = FLAG(7),
};

enum e_production_flags : byte
{
    production_flag_do_not_monitor_changes = FLAG(0),
    production_flag_verify_sound_events = FLAG(1),
    production_flag_do_not_inherit_for_player_graphs = FLAG(2),
};

enum e_playback_flags : short
{
    playback_flag_disable_interpolation_in = FLAG(0),
    playback_flag_disable_interpolation_out = FLAG(1),
    playback_flag_disable_mode_ik = FLAG(2),
    playback_flag_disable_weapon_ik = FLAG(3),
    playback_flag_disable_weapon_aim_1st_person = FLAG(4),
    playback_flag_disable_look_screen = FLAG(5),
    playback_flag_disable_transition_adjustment = FLAG(6),
};

enum e_animation_compression_type : byte
{
    animation_compression_type_best_score = 0,
    animation_compression_type_best_compression = 1,
    animation_compression_type_best_accuracy = 2,
    animation_compression_type_best_fullframe = 3,
    animation_compression_type_best_small_keyframe = 4,
    animation_compression_type_best_large_keyframe = 5,
};

class c_animation_data_sizes
{
    byte field_0;
    byte field_1;
    short field_2;
    short field_4;
    short field_6;
    int field_8;
    int field_12;
};
TAG_BLOCK_SIZE_ASSERT(c_animation_data_sizes, 16);

enum e_frame_event_type : short
{
    frame_event_type_primary_keyframe = 0,
    frame_event_type_secondary_keyframe = 1,
    frame_event_type_left_foot = 2,
    frame_event_type_right_foot = 3,
    frame_event_type_allow_interruption = 4,
    frame_event_type_transition_a = 5,
    frame_event_type_transition_b = 6,
    frame_event_type_transition_c = 7,
    frame_event_type_transition_d = 8,
    frame_event_type_bothfeet_shuffle = 9,
    frame_event_type_body_impact = 10,
};

struct s_frame_event
{
    e_frame_event_type type;
    short frame;
};
TAG_BLOCK_SIZE_ASSERT(s_frame_event, 4);

struct s_sound_event
{
    short sound;
    short frame;
    string_id marker_name;
};
TAG_BLOCK_SIZE_ASSERT(s_sound_event, 8);

struct s_effect_event
{
    short effect;
    short frame;
};
TAG_BLOCK_SIZE_ASSERT(s_effect_event, 4);

enum e_component_flags : short
{
    component_flag_rotation = FLAG(0),
    component_flag_translation = FLAG(1),
    component_flag_scale = FLAG(2),
};

struct s_object_space_node_data
{
    short node_index;
    e_component_flags component_flags;
    c_quantized_orientation orientation;
};
TAG_BLOCK_SIZE_ASSERT(s_object_space_node_data, 28);

class c_model_animation
{
    string_id name;
    int node_list_checksum;
    int production_checksum;
    int import_checksum;

    e_animation_type type;
    e_frame_info_type frame_info_type;

    byte blend_screen;
    byte node_count;
    short frame_count;

    e_model_animation_flags internal_flags;
    e_production_flags production_flags;
    e_playback_flags playback_flags;
    e_animation_compression_type desired_compression;
    e_animation_compression_type current_compression;

    float weight;
    short loop_frame_index;
    short parent_animation_index;
    short next_animation_index;
    short animation_size;           // This field is set to 0 upon compile

    /****************************************
    * definition_name: aligned_animation_data_definition
    * flags: 0
    * alignment_bit: 16
    ****************************************/
    // DataSize(33724160)
    data_block resource;
    c_animation_data_sizes animation_data_sizes;

    tag_block<s_frame_event> frame_events;
    tag_block<s_sound_event> sound_events;
    tag_block<s_effect_event> effect_events;
    tag_block<s_object_space_node_data> objectspace_parent_nodes;

public:
    bool animation_is_world_relative() const;
    short find_first_key_of_type(const e_frame_event_type event_type) const;
    short find_first_sound_event(s_sound_event* sound_event) const;
    short find_next_key_of_type(const e_frame_event_type event_type, const int frame) const;
    e_animation_type get_animation_type() const;
    float get_authored_duration() const;
    size_t get_effect_events_size() const;
    short get_frame_count() const;
    double get_frame_count_minus_epsilon() const;
    size_t get_frame_events_size() const;
    e_frame_info_type get_frame_info_type() const;
    double get_frame_position_from_playback_ratio(float playback_ratio) const;
    short get_last_frame_index() const;
    short get_loop_frame_index() const;
    byte get_node_count() const;
    int get_node_list_checksum() const;
    short get_primary_key_frame_index() const;
    short get_secondary_key_frame_index() const;
    short get_left_foot_frame_index() const;
    short get_right_foot_frame_index() const;
    size_t get_sound_events_size() const;
    short get_sound_reference_index() const;
    string_id get_string_id() const;
};

class c_animation_graph_resources
{
    tag_reference parent_animation_graph;

    e_inheritance_flags inheritance_flags;

    e_animation_graph_resources_flags animation_graph_resources_flags;

    short animation_codec_pack;
    tag_block<animation_graph_node> skeleton_nodes;
    tag_block<animation_graph_sound_reference> sound_references;
    tag_block<animation_graph_effect_reference> effect_references;
    tag_block<animation_blend_screen_block> blend_screens;
    tag_block<c_model_animation> animations;

public:
    const animation_graph_node* get_node(byte node_index) const;
    size_t get_node_count() const;
};
TAG_BLOCK_SIZE_ASSERT(c_animation_graph_resources, 52);

struct s_animation_ik_point
{
    /// the marker name on the object being attached
    string_id marker;
    /// the marker name object (weapon, vehicle, etc.) the above marker is being attached to
    string_id attach_to_marker;
};
TAG_BLOCK_SIZE_ASSERT(s_animation_ik_point, 8);

struct s_animation_entry
{
    string_id label;
    c_animation_id animation;
};
TAG_BLOCK_SIZE_ASSERT(s_animation_entry, 8);

struct s_animation_damage_direction
{
    tag_block<c_animation_id> regions;
};
TAG_BLOCK_SIZE_ASSERT(s_animation_damage_direction, 8);

struct s_animation_damage_actions
{
    string_id label;
    tag_block<s_animation_damage_direction> directions;
};
TAG_BLOCK_SIZE_ASSERT(s_animation_damage_actions, 12);

enum e_frame_event_link : byte
{
    frame_event_link_nokeyframe = 0,
    frame_event_link_keyframetype_a = 1,
    frame_event_link_keyframetype_b = 2,
    frame_event_link_keyframetype_c = 3,
    frame_event_link_keyframetype_d = 4,
};

struct s_animation_transition_state
{
    /// name of the state
    string_id state_name;

    /// which frame event to link to
    e_frame_event_link frame_event_link;

    PAD8;
    /// first level sub-index into state
    byte index_a;
    /// second level sub-index into state
    byte index_b;
};
TAG_BLOCK_SIZE_ASSERT(s_animation_transition_state, 8);

struct s_animation_transition_destination
{
    /// name of the mode & state this transitions to
    string_id full_name;
    /// name of the mode
    string_id mode;
    s_animation_transition_state state_info;
    c_animation_id animation;
};
TAG_BLOCK_SIZE_ASSERT(s_animation_transition_destination, 20);

struct s_animation_transition_source
{
    /// name of the mode & state of the source
    string_id full_name;
    s_animation_transition_state state_info;
    tag_block<s_animation_transition_destination> destinations;
};
TAG_BLOCK_SIZE_ASSERT(s_animation_damage_actions, 12);

struct precache_list_block
{
    int cache_block_index;
};
TAG_BLOCK_SIZE_ASSERT(precache_list_block, 4);

struct c_weapon_type
{
    string_id label;
    tag_block<s_animation_entry> actions;
    tag_block<s_animation_entry> overlays;
    tag_block<s_animation_damage_actions> death_and_damage;
    tag_block<s_animation_transition_source> transitions;
    tag_block<precache_list_block> high_precache;
    tag_block<precache_list_block> low_precache;
};
TAG_BLOCK_SIZE_ASSERT(c_weapon_type, 52);

class c_weapon_class
{
    string_id label;
    tag_block<c_weapon_type> weapon_type;
    tag_block<s_animation_ik_point> weapon_ik;
};
TAG_BLOCK_SIZE_ASSERT(c_weapon_class, 20);

class c_animation_mode
{
    string_id label;
    tag_block<c_weapon_class> weapon_class;
    tag_block<s_animation_ik_point> mode_ik;
};
TAG_BLOCK_SIZE_ASSERT(c_animation_mode, 20);

struct c_vehicle_suspension
{
    string_id label;
    c_animation_id animation;

    string_id marker_name;
    float mass_point_offset;
    float full_extension_grounddepth;
    float full_compression_grounddepth;
    // Explaination("Destroyed Suspension", "Only Necessary for suspensions with a destroyed state")
    string_id region_name;
    float destroyed_mass_point_offset;
    float destroyed_full_extension_grounddepth;
    float destroyed_full_compression_grounddepth;
};
TAG_BLOCK_SIZE_ASSERT(c_vehicle_suspension, 40);

enum e_function_controls : short
{
    function_control_frame = 0,
    function_control_scale = 1,
};

struct s_object_overlay
{
    string_id label;
    c_animation_id animation;
    PAD16;
    e_function_controls function_controls;
    string_id function;
    PAD32;
};
TAG_BLOCK_SIZE_ASSERT(s_object_overlay, 20);

class c_model_animation_graph_contents
{
    // Explaination("MODE-n-STATE GRAPH", "")
    tag_block<c_animation_mode> modes;

    // Explaination("SPECIAL CASE ANIMS", "")
    tag_block<c_vehicle_suspension> vehicle_suspension;
    tag_block<s_object_overlay> object_overlays;
};
TAG_BLOCK_SIZE_ASSERT(c_model_animation_graph_contents, 24);


// TODO label and implement the rest of the member functions for this class
// There's a lot of them and they're not currently needed
class c_model_animation_graph : TagGroup<'jmad'>
{
    c_animation_graph_resources resources;
    c_model_animation_graph_contents content;
    c_model_animation_runtime_data runtime_data;

    /* Explaination("RESULTS OF THE LAST IMPORT", "")
    =============================================
    * definition_name: error_report_string_data
    * flags: 2
    * alignment_bit: 0
    =============================================
    DataSize(131072) */
    data_block last_import_results;
    tag_block<s_additional_node_data> additional_node_data;

public:
    byte find_node(const string_id string) const;
    byte find_node_with_flags(const e_node_model_flags flags) const;

    const c_model_animation_graph* get(const datum tag_datum_index) const;
    c_model_animation_graph* get_writable(const datum tag_datum_index) const;
    const animation_graph_node* get_node(const byte node_index) const;
    short get_node_count() const;
};
TAG_GROUP_SIZE_ASSERT(c_model_animation_graph, 172);