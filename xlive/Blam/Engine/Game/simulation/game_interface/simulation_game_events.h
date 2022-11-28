#pragma once
#include "Blam/Engine/Game/memory/bitstream.h"
#include <wtypes.h>

enum e_simulation_event_type
{
	_simulation_event_type_damage_aftermath_event = 0x0,
	_simulation_event_type_damage_section_response = 0x1,
	_simulation_event_type_breakable_surface_damage_event = 0x2,
	_simulation_event_type_projectile_attached_event = 0x3,
	_simulation_event_type_projectile_effect_event = 0x4,
	_simulation_event_type_projectile_impact_effect_event = 0x5,
	_simulation_event_type_projectile_object_impact_effect_event = 0x6,
	_simulation_event_type_game_engine_event = 0x7,
	_simulation_event_type_unit_board_vehicle_event = 0x8,
	_simulation_event_type_unit_pickup_event = 0x9,
	_simulation_event_type_weapon_effect_event = 0xA,
	_simulation_event_type_unit_exit_vehicle_event = 0xB,
	_simulation_event_type_weapon_fire_event = 0xC,
	_simulation_event_type_weapon_reload_event = 0xD,
	_simulation_event_type_unit_grenade_initiate_event = 0xE,
	_simulation_event_type_unit_melee_initiate_event = 0xF,
	_simulation_event_type_weapon_pickup_event = 0x10,
	_simulation_event_type_weapon_put_away_event = 0x11,
	_simulation_event_type_weapon_drop_event = 0x12,
	_simulation_event_type_vehicle_flip_event = 0x13,
	_simulation_event_type_vehicle_trick_event = 0x14,
	_simulation_event_type_device_touch_event = 0x15,
	_simulation_event_type_unit_grenade_release_event = 0x16,
	_simulation_event_type_unit_melee_damage_event = 0x17,
	_simulation_event_type_unit_enter_vehicle_event = 0x18,
	_simulation_event_type_request_boot_player_event = 0x19,
	k_simulation_event_count = 0x1A,
};
class c_simulation_event_definition
{
public:
	virtual e_simulation_event_type event_type();
	virtual const char* event_type_name();
	virtual signed int payload_size();
	virtual signed int number_of_entity_references();
	virtual bool reference_delays_entity_deletion();
	virtual char sub_A894C7(int a1, int a2);
	virtual DWORD minimum_required_bits(int a1, int a2, DWORD* a3);
	virtual void get_relevance(int a2, int a3, float a4);
	virtual int get_relevance_string(int a2, int a3, DWORD a4, int a5, char* a6);
	virtual bool encode(DWORD payload_size, void* data, bitstream* packet);
	virtual bool decode(DWORD payload_size, void* data, bitstream* packet);
	virtual bool perform(DWORD entity_reference_count, void* entity_refernces, DWORD payload_size, void* data);
};
