#pragma once
#include "game/aim_assist.h"
#include "game/game_allegiance.h"
#include "objects/objects.h"

/* constants */

enum
{
	MAXIMUM_NUMBER_OF_UNIT_CAMERA_TRACKS = 2,
};

/* macros */

#define unit_get(index)			((struct unit_datum*)object_get_and_verify_type((index), _object_mask_unit))
#define unit_try_and_get(index) ((struct unit_datum*)object_try_and_get_and_verify_type((index), _object_mask_unit))

/* enums */

enum
{
	_powered_seat_driver = 0,
	_powered_seat_gunner,
	k_powered_seats_count,
};

enum e_weapon_addition_method : int16
{
	_weapon_addition_method_zero = 0,
	_weapon_addition_method_one = 1,
	_weapon_addition_method_two = 2,
	_weapon_addition_method_three = 3,
	_weapon_addition_method_four = 4,
	_weapon_addition_method_five = 5,
	_weapon_addition_method_six = 6,
	_weapon_addition_method_seven = 7,
};

enum e_unit_grenade_type : int16
{
	_unit_grenade_human_fragmentation = 0,
	_unit_grenade_covenant_plasma = 1,
	k_unit_grenade_types_count,
	k_unit_grenade_types_bits = 1
};

enum e_unit_weapons
{
	unit_weapons_primary_weapon,
	unit_weapons_secondary_weapon,
	unit_weapons_dual_weild_weapon
};

enum e_unit_data_flags : uint32
{
	_unit_is_actively_controlled = FLAG(1),
	_unit_is_alive = FLAG(2),
	_unit_active_camo_active = FLAG(3),
	_unit_is_enterable_by_player = FLAG(12),
	_unit_desires_tight_camera_track = FLAG(17),
	_unit_is_impervious = FLAG(19),
	_unit_suspended = FLAG(20),
	_unit_enable_eye_tracking = FLAG(24),
	_unit_only_takes_damage_from_players_team = FLAG(31)
};

enum e_unit_estimate_mode : int16
{
	_unit_estimate_none = 0,
	_unit_estimate_1 = 1,
	_unit_estimate_2 = 2,
	_unit_estimate_gun_position = 3,
	k_unit_estimate_position_modes_count
};

enum e_unit_flags : uint32
{
	_unit_is_crouching_bit = 23
};

enum e_unit_control_flags : uint32
{
	_unit_control_bit_1 = 1,
	_unit_control_bit_2 = 2,

	k_unit_control_flags_count
};

/* structures */

struct s_unit_304
{
	int8 gap_0[16];
};

struct _unit_datum
{
	uint32 unit_cached_state_flags;
	datum actor_index;
	datum simulation_actor_index;
	uint32 unit_flags;
	e_game_team unit_team;
	int16 last_unit_effect_type;
	datum player_index;
	datum last_player_index;
	int32 game_time_at_last_unit_effect;
	int32 desired_animation_seat;
	c_flags_no_init<e_unit_control_flags, uint32, k_unit_control_flags_count> control_flags;
	uint32 control_flags_2;
	string_id animation_state;
	real_vector3d desired_facing_vector;
	real_vector3d desired_aiming_vector;
	real_vector3d aiming_vector;
	real_vector3d aiming_velocity;
	real_vector3d desired_looking_vector;
	real_vector3d looking_vector;
	real_vector3d looking_velocity;
	real_vector3d throttle;
	real_vector3d animation_throttle;
	int8 aiming_speed;
	int8 gap_1C9[3];
	real32 primary_trigger;
	real32 secondary_trigger;
	s_aim_assist_targeting_result target_info;
	int32 persistent_control_timer;
	int32 persistent_control_flags;
	uint32 persistent_control_mask;
	uint32 persistent_control_mask_2;
	uint8 flaming_death_effect_timer;
	bool flaming_death_flag;
	uint8 left_eye_node_index;
	uint8 right_eye_node_index;
	uint8 horizontal_aiming_change;
	int8 weapon_drop_delay_ticks;
	int8 primary_fire_timer;
	int8 secondary_fire_timer;
	int16 parent_seat_index;
	int8 weapon_index;
	int8 weapon_type_index;
	real32 ambient_illumination;
	real32 self_illumination;
	real32 mouth_aperture;
	int32 mouth_time;
	uint16 current_weapon_set_identifier;
	int8 weapon_indices[2];
	uint16 weapon_set_identifier;
	int8 weapon_slots[2];
	datum weapon_object_indices[4];
	int32 weapon_last_used_at_game_time[4];
	datum equipment_object_index;
	int8 current_grenade_index;
	int8 desired_grenade_index;
	int8 grenade_counts[k_unit_grenade_types_count];
	int8 current_zoom_level;
	int8 desired_zoom_level;
	int8 seat_acceleration_update_countdown;
	uint8 aiming_change;
	int32 weapon_control_last_active_time;
	datum motion_control_unit_index;
	datum weapon_control_unit_index;
	datum last_vehicle_index;
	datum game_time_at_last_vehicle_exit;
	bool impulse_vehicle_from_seat_ticks;
	int8 seat_power_valid_flags;
	uint16 fake_squad_index;
	real32 driver_seat_power;
	real32 gunner_seat_power;
	real32 integrated_light_power;
	real32 integrated_light_battery;
	uint32 integrated_night_vision_power;
	real_point3d seat_acceleration_origin;
	real_point3d seat_acceleration_velocity;
	real_vector3d seat_acceleration;
	real_vector3d seat_acceleration_raw;
	datum predicted_vehicle_index;
	uint16 predicted_seat_index;
	datum predicted_player_index;
	datum predicted_simulation_actor_index;
	real32 active_camouflage;
	real32 active_camouflage_maximum;
	real32 active_camouflage_regrowth;
	int8 active_camo_mode;
	int8 field_2D1;
	uint16 active_camo_time_ticks;
	int8 gap_2D2[4];
	real32 crouch;
	int16 delayed_damage_category;
	int16 delayed_damage_timer;
	int32 delayed_damage_peak;
	datum delayed_damage_owner_object_index;
	datum flaming_death_attacker_object_index;
	real32 run_blindly_angle;
	real32 run_blindly_angular_velocity;
	int32 time_of_death;
	real32 movement_stun;
	uint16 movement_stun_ticks;
	uint16 killing_spree_count;
	int32 killing_spree_last_time;
	s_unit_304 attackers[4];
	int32 attacker_damage_reporting_infos;
	real32 boost_time;
	real32 boost_dead_time;
	object_header_block_reference animation_storage;
	object_header_block_reference speech_storage;
	object_header_block_reference motor_state_storage;
	int32 field_35C;
};


struct unit_datum
{
	datum definition_index;
	_object_datum object;
	_unit_datum unit;
};
ASSERT_STRUCT_SIZE(unit_datum, 864);

/* prototypes */

void unit_apply_patches(void);

void __cdecl unit_delete_all_weapons(datum unit_datum_index);

datum __cdecl unit_inventory_next_weapon(datum unit_datum_index);

bool __cdecl unit_add_weapon_to_inventory(datum unit_datum_index, datum weapon_datum_index, e_weapon_addition_method weapon_addition_method);

float __cdecl unit_get_field_of_view(datum unit_datum_index, real32 unit_camera_field_of_view, int16 zoom_level);

bool unit_is_dual_wielding(datum unit_index);

bool unit_in_vehicle(datum unit_index);

datum __cdecl unit_inventory_get_weapon(datum unit_index, int16 weapon_slot);

datum player_index_from_unit_index(datum unit_index);

void __cdecl unit_get_head_position_interpolated(datum unit_index, real_point3d* position);

void __cdecl unit_get_camera_position(datum unit_index, real_point3d* out_point);

void unit_get_aiming_vector(datum unit_index, real_vector3d* out_vector);

void __cdecl unit_control(datum unit_index, const struct unit_control_data* control_data);

e_game_team unit_get_team_index(datum unit_index);

bool __cdecl unit_desires_tight_camera_track(datum unit_index);

bool unit_does_not_show_readied_weapon(datum unit_index);

void unit_set_desired_grenade_type(datum unit_index, int16 grenade_type);

void unit_add_grenade_type_to_inventory(datum unit_index, int16 grenade_type, int16 grenade_count);
