#pragma once
#include "animation_id.h"

#include "cache/cache_files.h"
#include "tag_files/data_reference.h"
#include "tag_files/string_id.h"

#include "Blam/Cache/DataTypes/TagBlock.h"
#include "tag_files/tag_reference.h"



#define k_max_nodes_per_animation = 255

struct s_additional_node_data
{
    string_id node_name;
    real_quaternion default_rotation;
    real_point3d default_translation;
    real32 default_scale;
    real_point3d min_bounds;
    real_point3d max_bounds;
};
TAG_BLOCK_SIZE_ASSERT(s_additional_node_data, 60);

struct inherited_animation_node_map_block
{
    int16 local_node;
};
TAG_BLOCK_SIZE_ASSERT(inherited_animation_node_map_block, 2);

struct inherited_animation_node_map_flag_block
{
    uint32 local_node_flags;
};
TAG_BLOCK_SIZE_ASSERT(inherited_animation_node_map_flag_block, 4);

enum e_inheritance_flags : uint8
{
    _inheritance_flag_inherit_root_translation_scale_only = FLAG(0),
    _inheritance_flag_inherit_for_use_on_player = FLAG(1)
};

struct s_animation_inheritence
{
    tag_reference inherited_graph;
    tag_block<inherited_animation_node_map_block> node_map;
    tag_block<inherited_animation_node_map_flag_block> node_map_flags;

    real32 root_z_offset;
    e_inheritance_flags inheritance_flags;
    int8 pad[3];
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

    uint32 left_arm_nodes[8];
    uint32 right_arm_nodes[8];

public:
    s_animation_inheritence* get_animation_inhertence(uint8 index) const;
    s_weapon_class_listing* get_weapon_list(uint8 index) const;
    const uint32* get_left_arm_nodes(void) const;
    const uint32* get_right_arm_nodes(void) const;
};
TAG_BLOCK_SIZE_ASSERT(c_model_animation_runtime_data, 80);

enum e_node_model_flags : uint8
{
    _node_model_flag_primary_model = FLAG(0),
    _node_model_flag_secondary_model = FLAG(1),
    _node_model_flag_local_root = FLAG(2),
    _node_model_flag_left_hand = FLAG(3),
    _node_model_flag_right_hand = FLAG(4),
    _node_model_flag_left_arm_member = FLAG(5)
};

enum e_node_joint_flags : uint8
{
    _node_joint_flag_ball_socket = FLAG(0),
    _node_joint_flag_hinge = FLAG(1),
    _node_joint_flag_no_movement = FLAG(2)
};

struct animation_graph_node
{
    string_id name;
    int16 next_sibling_node_index;
    int16 first_child_node_index;
    int16 parent_node_index;

    e_node_model_flags model_flags;
    e_node_joint_flags node_joint_flags;

    real_vector3d base_vector;
    real32 vector_range;
    real32 zpos;
};
TAG_BLOCK_SIZE_ASSERT(animation_graph_node, 32);

enum e_animation_graph_resources_flags : uint8
{
    _animation_graph_resources_prepared_for_cache = FLAG(0),
    _animation_graph_resources_unused = FLAG(1),
    _animation_graph_resources_imported_with_codec_compressors = FLAG(2),
    _animation_graph_resources_unused_smelly_flag = FLAG(3),
    _animation_graph_resources_written_to_cache = FLAG(4),
    _animation_graph_resources_animation_data_reordered = FLAG(5)
};

enum e_animation_graph_reference_flags : int16
{
    _animation_graph_reference_flag_allow_on_player = FLAG(0),
    _animation_graph_reference_flag_left_arm_only = FLAG(1),
    _animation_graph_reference_flag_right_arm_only = FLAG(2),
    _animation_graph_reference_flag_first_person_only = FLAG(3),
    _animation_graph_reference_flag_forward_only = FLAG(4),
    _animation_graph_reference_flag_reverse_only = FLAG(5)
};

struct animation_graph_sound_reference
{
    tag_reference sound;    // snd!
    e_animation_graph_reference_flags flags;
    int16 pad;
};
TAG_BLOCK_SIZE_ASSERT(animation_graph_sound_reference, 0xC);

struct animation_graph_effect_reference
{
    tag_reference effect;   // effe
    e_animation_graph_reference_flags flags;
    int16 pad;
};
TAG_BLOCK_SIZE_ASSERT(animation_graph_effect_reference, 12);

struct animation_aiming_screen_bounds
{
    real32 right_yaw_per_frame;
    real32 left_yaw_per_frame;
    int16 right_frame_count;
    int16 left_frame_count;
    real32 down_pitch_per_frame;
    real32 up_pitch_per_frame;
    int16 down_pitch_frame_count;
    int16 up_pitch_frame_count;
};
TAG_BLOCK_SIZE_ASSERT(animation_aiming_screen_bounds, 24);

struct s_animation_blend_screen
{
    string_id label;
    animation_aiming_screen_bounds aiming_screen;
};
TAG_BLOCK_SIZE_ASSERT(s_animation_blend_screen, 28);

enum e_animation_type : uint8
{
    _animation_type_base = 0,
    _animation_type_overlay = 1,
    _animation_type_replacement = 2
};

enum e_frame_info_type : uint8
{
    _frame_info_type_none = 0,
    _frame_info_type_dxdy = 1,
    _frame_info_type_dxdydyaw = 2,
    _frame_info_type_dxdydzdyaw = 3,
};

enum e_model_animation_flags : uint8
{
    _model_animation_flag_unused0 = FLAG(0),
    _model_animation_flag_world_relative = FLAG(1),
    _model_animation_flag_unused1 = FLAG(2),
    _model_animation_flag_unused2 = FLAG(3),
    _model_animation_flag_unused3 = FLAG(4),
    _model_animation_flag_compression_disabled = FLAG(5),
    _model_animation_flag_old_production_checksum = FLAG(6),
    _model_animation_flag_valid_production_checksum = FLAG(7)
};

enum e_production_flags : uint8
{
    _production_flag_do_not_monitor_changes = FLAG(0),
    _production_flag_verify_sound_events = FLAG(1),
    _production_flag_do_not_inherit_for_player_graphs = FLAG(2),
};

enum e_playback_flags : int16
{
    playback_flag_disable_interpolation_in = FLAG(0),
    playback_flag_disable_interpolation_out = FLAG(1),
    playback_flag_disable_mode_ik = FLAG(2),
    playback_flag_disable_weapon_ik = FLAG(3),
    playback_flag_disable_weapon_aim_1st_person = FLAG(4),
    playback_flag_disable_look_screen = FLAG(5),
    playback_flag_disable_transition_adjustment = FLAG(6),
};

enum e_animation_compression_type : int8
{
    _animation_compression_type_best_score = 0,
    _animation_compression_type_best_compression = 1,
    _animation_compression_type_best_accuracy = 2,
    _animation_compression_type_best_fullframe = 3,
    _animation_compression_type_best_small_keyframe = 4,
    _animation_compression_type_best_large_keyframe = 5,
};

class c_animation_data_sizes
{
    uint8 field_0;
    uint8 field_1;
    int16 field_2;
    int16 field_4;
    int16 field_6;
    int32 field_8;
    int32 field_12;

public:
    uint8 get_field0(void) const { return field_0; }
    uint8 get_field1(void) const { return field_1; }
    int16 get_field2(void) const { return field_2; }
    int16 get_field4(void) const { return field_4; }
    int16 get_field6(void) const { return field_6; }
    int32 get_field8(void) const { return field_8; }
    int32 get_field12(void) const { return field_12; }
};
TAG_BLOCK_SIZE_ASSERT(c_animation_data_sizes, 16);

enum e_frame_event_type : int16
{
    _frame_event_type_primary_keyframe = 0,
    _frame_event_type_secondary_keyframe = 1,
    _frame_event_type_left_foot = 2,
    _frame_event_type_right_foot = 3,
    _frame_event_type_allow_interruption = 4,
    _frame_event_type_transition_a = 5,
    _frame_event_type_transition_b = 6,
    _frame_event_type_transition_c = 7,
    _frame_event_type_transition_d = 8,
    _frame_event_type_bothfeet_shuffle = 9,
    _frame_event_type_body_impact = 10
};

struct s_frame_event
{
    e_frame_event_type type;
    int16 frame;
};
TAG_BLOCK_SIZE_ASSERT(s_frame_event, 4);

struct s_sound_event
{
    int16 sound;
    int16 frame;
    string_id marker_name;
};
TAG_BLOCK_SIZE_ASSERT(s_sound_event, 8);

struct s_effect_event
{
    int16 effect;
    int16 frame;
};
TAG_BLOCK_SIZE_ASSERT(s_effect_event, 4);

enum e_component_flags : int16
{
    _component_flag_rotation = FLAG(0),
    _component_flag_translation = FLAG(1),
    _component_flag_scale = FLAG(2),
};

struct s_object_space_node_data
{
    int16 node_index;
    e_component_flags component_flags;
    c_quantized_orientation orientation;
};
TAG_BLOCK_SIZE_ASSERT(s_object_space_node_data, 28);

class c_model_animation
{
    string_id name;
    int32 node_list_checksum;
    int32 production_checksum;
    int32 import_checksum;

    e_animation_type type;
    e_frame_info_type frame_info_type;

    uint8 blend_screen;
    uint8 node_count;
    int16 frame_count;

    e_model_animation_flags internal_flags;
    e_production_flags production_flags;
    e_playback_flags playback_flags;
    e_animation_compression_type desired_compression;
    e_animation_compression_type current_compression;

    real32 weight;
    int16 loop_frame_index;
    int16 parent_animation_index;
    int16 next_animation_index;
    int16 animation_size;           // This field is set to 0 upon compile

    /****************************************
    * definition_name: aligned_animation_data_definition
    * flags: 0
    * alignment_bit: 16
    ****************************************/
    // DataSize(33724160)
    data_reference resource;
    c_animation_data_sizes animation_data_sizes;

    tag_block<s_frame_event> frame_events;
    tag_block<s_sound_event> sound_events;
    tag_block<s_effect_event> effect_events;
    tag_block<s_object_space_node_data> objectspace_parent_nodes;

public:
    bool animation_is_world_relative(void) const;
    int16 find_first_key_of_type(const e_frame_event_type event_type) const;
    int16 find_first_sound_event(s_sound_event* sound_event) const;
    int16 find_next_key_of_type(const e_frame_event_type event_type, const int32 frame) const;
    e_animation_type get_animation_type(void) const;
    real32 get_authored_duration(void) const;
    size_t get_effect_events_size(void) const;
    int16 get_frame_count(void) const;
    double get_frame_count_minus_epsilon(void) const;
    size_t get_frame_events_size(void) const;
    e_frame_info_type get_frame_info_type(void) const;
    real64 get_frame_position_from_playback_ratio(real32 playback_ratio) const;
    int16 get_last_frame_index(void) const;
    int16 get_loop_frame_index(void) const;
    uint8 get_node_count(void) const;
    int32 get_node_list_checksum(void) const;
    e_playback_flags get_playback_flags(void) const;
    int16 get_primary_key_frame_index(void) const;
    int16 get_secondary_key_frame_index(void) const;
    int16 get_left_foot_frame_index(void) const;
    int16 get_right_foot_frame_index(void) const;
    size_t get_sound_events_size(void) const;
    int16 get_sound_reference_index(void) const;
    string_id get_string_id(void) const;
};

class c_animation_graph_resources
{
    tag_reference parent_animation_graph;

    e_inheritance_flags inheritance_flags;

    e_animation_graph_resources_flags animation_graph_resources_flags;

    int16 animation_codec_pack;
    tag_block<animation_graph_node> skeleton_nodes;
    tag_block<animation_graph_sound_reference> sound_references;
    tag_block<animation_graph_effect_reference> effect_references;
    tag_block<s_animation_blend_screen> blend_screens;
    tag_block<c_model_animation> animations;

public:
    const c_model_animation* get_animation(c_animation_id animation_id) const;
    size_t get_animation_count(void) const;
    e_animation_graph_resources_flags get_animation_graph_resources_flags(void) const;
    const s_animation_blend_screen* get_blend_screen(uint32 index) const;
    size_t get_blend_screen_count(void) const;
    int16 get_codec_pack(void) const;
    const animation_graph_effect_reference* get_effect_reference(uint8 node_index) const;
    size_t get_effect_reference_count(void) const;
    e_inheritance_flags get_inheritance_flags(void) const;
    const animation_graph_node* get_node(uint8 node_index) const;
    int16 get_node_count(void) const;
    const animation_graph_sound_reference* get_sound_reference(uint8 node_index) const;
    size_t get_sound_reference_count(void) const;
    tag_reference get_parent_graph(void) const;
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

enum e_frame_event_link : uint8
{
    _frame_event_link_nokeyframe = 0,
    _frame_event_link_keyframetype_a = 1,
    _frame_event_link_keyframetype_b = 2,
    _frame_event_link_keyframetype_c = 3,
    _frame_event_link_keyframetype_d = 4
};

struct s_animation_transition_state
{
    string_id state_name;                   // name of the state
    e_frame_event_link frame_event_link;    // which frame event to link to

    int8 pad;

    uint8 index_a;  // first level sub-index into state
    uint8 index_b;  // second level sub-index into state
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
    int32 cache_block_index;
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
    real32 mass_point_offset;
    real32 full_extension_grounddepth;
    real32 full_compression_grounddepth;

    // Explaination("Destroyed Suspension", "Only Necessary for suspensions with a destroyed state")
    string_id region_name;
    real32 destroyed_mass_point_offset;
    real32 destroyed_full_extension_grounddepth;
    real32 destroyed_full_compression_grounddepth;
};
TAG_BLOCK_SIZE_ASSERT(c_vehicle_suspension, 40);

enum e_function_controls : int16
{
    _function_control_frame = 0,
    _function_control_scale = 1,
};

struct s_object_overlay
{
    string_id label;
    c_animation_id animation;
    int16 pad;
    e_function_controls function_controls;
    string_id function;
    int32 pad1;
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
class c_model_animation_graph
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
    data_reference last_import_results;
    tag_block<s_additional_node_data> additional_node_data;

public:
    static const c_model_animation_graph* get(datum tag_index)
    {
        return (const c_model_animation_graph*)tag_get_fast(tag_index);
    }

    int32 find_node(const string_id string) const;
    int32 find_node_with_flags(const e_node_model_flags flags) const;

    c_model_animation_graph* get_writable(datum tag_datum_index) const;
    const animation_graph_node* get_node(uint8 node_index) const;
    int16 get_node_count(void) const;
};
TAG_GROUP_SIZE_ASSERT(c_model_animation_graph, 172);