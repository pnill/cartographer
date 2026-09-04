#pragma once
#include "items.h"
#include "units/unit_messages.h"

/* constants */

enum
{
	MAXIMUM_NUMBER_OF_MAGAZINES_PER_WEAPON = 2,
	MAXIMUM_NUMBER_OF_MAGAZINE_OBJECTS_PER_MAGAZINE = 8,
	k_weapon_trigger_count = 2,
	k_weapon_barrel_count = 2,
	k_weapon_barrel_effect_count = 3
};

/* macros */

#define weapon_get(index) ((struct weapon_datum*)(object_get_and_verify_type((index), _object_mask_weapon)))
#define weapon_try_and_get(index) ((struct weapon_datum*)(object_try_and_get_and_verify_type((index), _object_mask_weapon)))

/* enums */

enum e_weapon_barrel_state : uint8
{
	_weapon_barrel_state_idle = 0,
	_weapon_barrel_state_firing,
	_weapon_barrel_state_locked_recovering,
	_weapon_barrel_state_locked_recovering_empty,
	_weapon_barrel_state_recovering,
	k_barrel_state_count,
};

enum e_weapon_barrel_flags : uint16
{
	_weapon_barrel_fire_bit = 0,
	_weapon_barrel_create_projectiles_bit,
	_weapon_barrel_destroyed_bit,
	_weapon_barrel_blurred_bit,
	_weapon_barrel_fired_before_charging_bit,
	_weapon_barrel_damaged_bit,
	_weapon_barrel_did_empty_click_bit,
	_weapon_barrel_did_firing_effect_this_burst_bit,
	k_weapon_barrel_flag_count
};
typedef c_flags_no_init<e_weapon_barrel_flags, uint16, k_weapon_barrel_flag_count> c_weapon_barrel_flags;

enum e_weapon_trigger_state : uint8
{
	_weapon_trigger_state_idle = 0,
	_weapon_trigger_state_charging,
	_weapon_trigger_state_charged,
	_weapon_trigger_state_tracking,
	_weapon_trigger_state_releasing,
	_weapon_trigger_state_locking,
	_weapon_trigger_state_locked,
	_weapon_trigger_state_overcharged,
	k_weapon_trigger_state_count,
};

enum e_weapon_trigger_flags : uint16
{
	_weapon_trigger_released_since_last_shot_bit = 0,
	_weapon_trigger_destroyed_bit,
	_weapon_trigger_fired_before_charging_bit,
	_weapon_trigger_did_primary_autofire_action_bit,
	_weapon_trigger_did_secondary_autofire_action_bit,
	_weapon_trigger_spewing_bit,
	_weapon_trigger_charge_from_predicted_trigger,
	_weapon_trigger_held_from_script_bit,
	_weapon_trigger_trying_to_track,
	_weapon_trigger_began_tracking_cycle,
	k_weapon_trigger_flag_count,
};
typedef c_flags_no_init<e_weapon_trigger_flags, uint16, k_weapon_trigger_flag_count> c_weapon_trigger_flags;

enum e_weapon_magazine_state : uint16
{
	_magazine_idle = 0,
	_magazine_reloading_single,
	_magazine_reloading_continuous_starting,
	_magazine_reloading_continuous_underway,
	_magazine_reloading_continuous_ending,
	_magazine_unchambered,
	_magazine_chambering,
	_magazine_busy,
	NUMBER_OF_WEAPON_MAGAZINE_STATES,
};


/* structures */

struct weapon_barrel
{
	int8_t firing_idle_ticks;
	e_weapon_barrel_state state;
	int16_t state_timer;
	c_weapon_barrel_flags flags;
	uint16_t fire_count;
	uint16_t firing_effects_used_flags;
	int16_t firing_effect_index;
	int16_t firing_effect_shots_remaining;
	int16_t sequential_non_tracer_rounds;
	float rate_of_fire;
	float ejection_port_position;
	float illumination;
	float current_error;
	float angle_change_scale;
	float bonus_shot_fraction;
	float recovery_overflow;
	uint16_t bonus_shot_count;
	uint8_t staggered_marker_offset;
	uint8_t _pad_2F;
	int32_t effect_index;
};


struct weapon_trigger
{
	e_weapon_trigger_state state;
	uint8 utility_timer;
	int16 state_timer;
	c_weapon_trigger_flags flags;
	int16 pad;
	int32 charging_effect_index;
};

struct weapon_magazine
{
	e_weapon_magazine_state state;
	int16 state_timer;
	int16 original_time;
	int16 rounds_inventory;
	int16 rounds_loaded;
	int16 rounds_loaded_delayed;
	int16 reload_timer;
	int16 reload_weapon_disable_duration;
};

struct weapon_first_person_emulation
{
	datum graph_index;
	int32 animation_id;
	real32 animation_time;
};

struct _weapon_datum
{
	uint16 weapon_flags;
	uint16 control_flags;
	uint8 primary_trigger;
	uint8 last_primary_trigger;
	uint8 last_hill_or_valley;
	int8 primary_trigger_direction;
	int8 primary_trigger_down_ticks;
	uint8 barrel_spin;
	uint8 _pad_0A;
	int8 tracked_model_target_index;
	int16 state;
	int16 state_timer;
	int16 weapon_disabled_by_reload_timer;
	int16 multiplayer_weapon_identifier;
	real32 heat;
	real32 age;
	real32 field_1C;
	real32 delayed_age;
	real32 overcharged;
	int32 tracked_object_index;
	real32 recoil_angular_velocity;
	int16 recoil_recovery_time;
	int16 shots_until_demotion;
	int16 alternate_shots_loaded;
	int16 _pad_36;
	weapon_barrel barrels[k_weapon_barrel_count];
	weapon_trigger triggers[k_weapon_trigger_count];
	weapon_magazine magazines[MAXIMUM_NUMBER_OF_MAGAZINES_PER_WEAPON];
	int32 overheated_effect_index;
	int32 game_time_last_fired;
	int32 tracked_object_last_target_acquisition_time;
	weapon_first_person_emulation first_person_emulation;
};

struct weapon_datum
{
	datum definition_index;
	_object_datum object;
	_item_datum item;
	_weapon_datum weapon;
};
ASSERT_STRUCT_SIZE(weapon_datum, 604);

/* prototypes */


void weapons_apply_patches(void);

void __cdecl weapon_send_message_to_unit(int32 weapon_index, e_unit_messages unit_message);

void __cdecl weapon_send_message_to_unit_deterministic(int32 weapon_index, e_unit_messages unit_message);

int32 __cdecl weapon_get_rounds_available(datum weapon_index, int32 magazine_index, bool a3);

int32 __cdecl weapon_get_rounds_total(datum object_index, int32 magazine_index, bool a3);

void __cdecl weapons_fire_barrels(void);
