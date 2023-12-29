#pragma once
#include "Blam/Engine/animations/animation_manager.h"
#include "Blam/Engine/game/players.h"
#include "Blam/Engine/math/matrix_math.h"

#define MAXIMUM_NODES_PER_FIRST_PERSON_MODEL 64
#define k_first_person_max_weapons 2

enum e_first_person_weapon_data_flags : uint32
{
	_weapon_node_table_valid_bit = 1,
	_arm_node_table_valid_bit = 2,
};

struct s_first_person_model_data
{
	datum render_model_index;
	datum object_index;
	uint32 flags;
	real_matrix4x3 nodes[MAXIMUM_NODES_PER_FIRST_PERSON_MODEL];
};

struct s_first_person_weapon_data
{
	e_first_person_weapon_data_flags flags;
	datum weapon_index;
	c_animation_manager animation_manager;
	int8 gap_90[12];	// unused? maybe a real_point3d?
	c_animation_channel move_animation_channel;
	c_animation_channel jitter_animation_channel;
	c_animation_id pitch_turn_animation_id;
	c_animation_id overlay_animation_id;
	c_animation_id ammo_animation_id;
	real32 field_E4;
	real32 field_E8;
	int16 field_EC;
	int16 maybe_framerate;
	int16 field_F0;
	int16 field_F2;
	int32 weapon_node_remapping_table_count;
	int32 hands_node_remapping_table_count;
	int32 weapon_node_remapping_table[MAXIMUM_NODES_PER_FIRST_PERSON_MODEL];
	int32 hands_node_remapping_table[MAXIMUM_NODES_PER_FIRST_PERSON_MODEL];
	int16 child_node_index;
	int16 alternate_parent_node_index;
	int32 node_orientations_count;
	int32 node_matrices_count;
	real_matrix4x3 nodes[MAXIMUM_NODES_PER_FIRST_PERSON_MODEL];
	datum sound_source_index;
	int16 field_1004;
	int16 pad;
};
CHECK_STRUCT_SIZE(s_first_person_weapon_data, 4112);

struct s_first_person_weapon
{
	uint32 flags;
	datum unit_index;
	e_character_type character_type;
	int8 pad[3];
	s_first_person_weapon_data weapons[k_first_person_max_weapons];
	c_interpolator_control rate_interpolator_control;
	real_euler_angles2d pos;
	real_euler_angles2d field_2038;
	real_euler_angles2d turn;
	real_euler_angles2d field_2048;
	real_euler_angles2d player_facing;
	real_euler_angles2d field_2058;
	real_matrix4x3 identity_matrix;
	int32 adjustment_matrix_index;
	real_matrix4x3 adjustment_matrix;
};
CHECK_STRUCT_SIZE(s_first_person_weapon, 8396);

struct s_first_person_orientations
{
	real_orientation weapon_orientations[MAXIMUM_NODES_PER_FIRST_PERSON_MODEL];
	real_orientation hand_orientations[MAXIMUM_NODES_PER_FIRST_PERSON_MODEL];
};
CHECK_STRUCT_SIZE(s_first_person_orientations, 4096);

s_first_person_weapon* first_person_weapon_get_global(void);
s_first_person_weapon* first_person_weapons_get(uint32 user_index);
s_first_person_orientations* first_person_orientations_get_global(void);
s_first_person_orientations* first_person_orientations_get(uint32 user_index, uint32 weapon_slot);
s_first_person_model_data* first_person_model_data_get_global(void);
s_first_person_model_data* first_person_model_data_get(uint32 user_index);

void toggle_first_person(bool state);

void first_person_weapon_apply_camera_effect(int32 user_index, real_matrix4x3* effect_matrix);

real_matrix4x3* first_person_weapon_get_relative_node_matrix(int32 user_index, datum weapon_index, int16 node_index);

void __cdecl first_person_weapon_get_worldspace_node_matrix(int32 user_index, datum weapon_index, int16 node_index, real_matrix4x3* out_matrix);

void first_person_weapons_apply_patches(void);