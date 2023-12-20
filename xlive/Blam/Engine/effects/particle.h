#pragma once

#define k_max_particles 1024
#include "Blam/Engine/math/real_math.h"
#include "Blam/Engine/memory/data.h"

enum e_particle_flags : int16
{
	_particle_flag_bit_0 = 0x0,
	_particle_flag_bit_1 = 0x1,
	_particle_flag_bit_2 = 0x2,
	_particle_flag_bit_3 = 0x3,
	_particle_flag_spawned_bit = 0x4,
	_particle_flag_bit_5 = 0x5,
	_particle_flag_bit_6 = 0x6,
	_particle_flag_bit_7 = 0x7,
	_particle_flag_bit_8 = 0x8,
	_particle_flag_bit_9 = 0x9,
	_particle_flag_bit_10 = 0xA,
	_particle_flag_bit_11 = 0xB,
	_particle_flag_bit_12 = 0xC,
	_particle_flag_bit_13 = 0xD,
	_particle_flag_bit_14 = 0xE,
	_particle_flag_bit_15 = 0xF,
};

struct c_particle
{
	int8 gap_0[2];
	e_particle_flags flags;
	int32 next_particle;
	real32 time_accumulator;
	real32 effect_delay_time;
	int8 gap_10[12];
	real_point3d m_position;
	real_vector3d m_velocity;
	int8 gap_34[12];
};

struct s_particle_interpolation_data
{
	datum particle_index;
	real_point3d position;
	real_vector3d velocity;
};


struct s_particle_interpolation_data_storage
{
	bool initialized;
	char pad[3];
	s_particle_interpolation_data particle_data[k_max_particles];
};
struct s_particle_frame_data_storage
{
	s_particle_interpolation_data_storage previous_data;
	s_particle_interpolation_data_storage target_data;
};

extern real32 g_particle_delta_temp;
extern real32 g_particle_interpolator_delta;
extern real32 g_particle_interpolator_delta_remainder;
extern s_particle_frame_data_storage* g_particle_frame_storage;
extern s_particle_interpolation_data_storage* g_previous_particle_interpolation_frame_data;
extern s_particle_interpolation_data_storage* g_target_particle_interpolation_frame_data;

s_data_array* get_particle_table();

bool halo_particle_interpolator_enabled();
void halo_particle_interpolator_set_state(bool state);
void halo_particle_interpolator_clear_buffers();
void halo_particle_interpolator_update_end();
real_point3d* halo_particle_interpolator_get_interpolated_position(datum particle_index, real_point3d* out_point);
real_vector3d* halo_particle_interpolator_get_interpolated_velocity(datum particle_index, real_vector3d* out_vector);

void particle_update(real32 delta);

void apply_particle_patches();