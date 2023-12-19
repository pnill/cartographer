#pragma once
#include "Blam/Engine/models/render_models.h"
#include "Blam/Engine/Networking/Session/NetworkSession.h"

#define k_interpolation_first_person_weapon_slot_count 4
#define k_interpolation_positions_count 2
#define k_interpolation_distance_cutoff 900.0f

struct s_object_interpolation_data
{
	datum object_index;
	real_point3d position;
	real_vector3d forward;
	real_vector3d up;
	real_point3d center_of_mass;
	real_matrix4x3 node_matrices[MAXIMUM_NODES_PER_MODEL];
};
CHECK_STRUCT_SIZE(s_object_interpolation_data, 0x3400);

struct s_weapon_interpolation_data
{
	datum animation_index;
	DWORD node_count;
	real_matrix4x3 nodes[255];
};
CHECK_STRUCT_SIZE(s_weapon_interpolation_data, 0x33D4);

struct s_position_interpolation_data
{
	bool initialized;
	char pad[3];
	real_matrix4x3 node;
};
CHECK_STRUCT_SIZE(s_position_interpolation_data, 56);

struct s_interpolation_data
{
	bool initialized;
	char pad[3];
	s_object_interpolation_data object_data[k_maximum_objects_per_map];
	s_weapon_interpolation_data weapon_data[k_number_of_users][k_interpolation_first_person_weapon_slot_count];
	s_position_interpolation_data position_data[k_number_of_users][k_interpolation_positions_count];
};
CHECK_STRUCT_SIZE(s_interpolation_data, 0x1A33F04);

struct s_frame_data_storage
{
	s_interpolation_data previous_data;
	s_interpolation_data target_data;
};
CHECK_STRUCT_SIZE(s_frame_data_storage, 0x3467E08);

extern s_frame_data_storage* g_frame_data_storage;
extern s_interpolation_data* g_frame_data_intermediate;
extern s_interpolation_data* g_previous_interpolation_frame_data;
extern s_interpolation_data* g_target_interpolation_frame_data;

bool is_interpolation_enabled(void);
void set_interpolation_enabled(bool enabled);

void halo_interpolator_clear_buffers(void);

void halo_interpolator_update_begin(void);
void halo_interpolator_update_end(void);

bool halo_interpolator_interpolate_center_of_mass(datum object_datum, real_point3d* center_of_mass);

bool halo_interpolator_interpolate_object_node_matrices(datum object_index, real_matrix4x3** node_matrices, int32* out_node_count);

void halo_interpolator_setup_new_object(datum object_datum);

void halo_interpolator_setup_weapon_data(int32 user_index, datum animation_index, int32 weapon_slot, real_matrix4x3* node_matrices, int32 nodes_count);

void halo_interpolator_set_target_position_data(int32 user_index, int32 position_index, real_matrix4x3* matrix);

void halo_interpolator_object_populate_interpolation_data(
	datum object_index,
	const real_matrix4x3* node_matrices,
	int32 nodes_count,
	const real_point3d* position,
	const real_vector3d* forward,
	const real_vector3d* up,
	const real_point3d* center_of_mass);

bool halo_interpolator_get_interpolated_matrix_from_user_index(int32 user_index, int32 position_index, real_matrix4x3* out);

bool halo_interpolator_interpolate_weapon(datum user_index, datum animation_index, int32 weapon_slot, real_matrix4x3** nodes, int32* node_matrices_count);

bool halo_interpolator_interpolate_object_node_matrix(datum object_index, int16 node_index, real_matrix4x3* out_matrix);

bool halo_interpolator_interpolate_object_position(datum object_index, real_point3d* point);

void halo_interpolator_interpolate_position_data(int32 user_index, int32 position_index, real_point3d* position);

bool halo_interpolator_interpolate_position_backwards(int32 user_index, int32 position_index, real_point3d* position);
