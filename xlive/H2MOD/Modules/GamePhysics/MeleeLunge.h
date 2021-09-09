#pragma once

#include "Blam/Math/BlamMath.h"
#include "Blam/Engine/Game/GameTimeGlobals.h"

#include "H2MOD.h"

#define k_valid_real_epsilon 0.001f
#define k_acceleration_ticks_real 5.0f
#define k_acceleration_ticks ((int)k_acceleration_ticks_real)
#define k_deceleration_ticks_real 4.0f
#define k_deceleration_ticks ((int)k_deceleration_ticks_real)

enum melee_flags
{
	melee_deceleration_early_start = 4,
	melee_deceleration_finished,

	melee_flags_end
};

#define melee_flags_mask 0xF0 // use first 4 bits of the 1 byte variable as flags
#define melee_deceleration_tick_count_mask 0xFF000000

struct character_physics_output
{
	DWORD flags;
	BYTE gap_4[8];
	real_vector3d out_translational_velocity;
	real_point3d field_18;
	real_vector3d field_24;
	BYTE gap_30[52];
};
CHECK_STRUCT_SIZE(character_physics_output, 0x64);

struct __declspec(align(4)) c_character_physics_mode_melee_datum
{
	int m_time_to_target_in_ticks;
	int m_melee_tick;
	int m_maximum_counter;
	bool m_weapon_is_sword;
	bool m_has_target;
	BYTE field_E;
	BYTE field_F;
	real_point3d start_unit_origin;
	real_vector3d field_1C;
	float m_field_28;
	float m_distance;
	real_point3d m_target_point;
	real_vector3d m_aiming_direction;
	float m_maximum_distance;
	BYTE m_started_decelerating;
	BYTE field_4D;
	BYTE field_4E[2];
	float m_velocity_to_decelerate;
	float field_54;
	real_vector3d field_58;
	int field_64;
	int field_68;
	int field_6C;
	int field_70;
	char gap_74[20];

	bool pin_localized_velocity(real_vector3d* output, real_vector3d* localized_velocity);
	void melee_deceleration_fixup(character_physics_output* output, real_point3d* object_origin, real_vector3d* current_velocity, real_vector3d* aiming_vector, BYTE& current_flags, BYTE& deceleration_tick_count);

	void __thiscall update_internal(character_physics_output* a2, bool a3, float distance_world_units, real_vector3d *target_translational_velocity, real_point3d *target_origin, real_vector3d *aiming_vector, real_vector3d *translational_velocity, real_point3d *object_origin);
};
CHECK_STRUCT_SIZE(c_character_physics_mode_melee_datum, 0x88);

void call_character_melee_physics_input_update_internal();

void melee_force_decelerate_fixup();
void melee_limit_some_random_vector();
void __cdecl biped_dash_hook(datum object_index, datum target_player, char sword);