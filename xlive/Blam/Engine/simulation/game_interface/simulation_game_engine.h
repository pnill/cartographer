#pragma once
#include "simulation_game_entities.h"

enum e_game_engine_state_data_flags
{
	_game_engine_state_data_flag_team_mapping_exists,
	_game_engine_state_data_flag_current_state_exists,
	_game_engine_state_data_flag_game_finished_exists,
	_game_engine_state_data_flag_current_round_exists,
	_game_engine_state_data_flag_round_timer_exists,

	k_game_engine_state_data_flag_count,
	k_game_engine_state_data_initial_update_mask = MASK(k_game_engine_state_data_flag_count),
	k_game_engine_state_data_flags_mask = FLAG_RANGE(_game_engine_state_data_flag_team_mapping_exists, _game_engine_state_data_flag_round_timer_exists)
};

struct s_game_engine_state_data
{
	uint16 initial_teams;
	uint16 valid_designators;
	uint16 valid_teams;
	uint16 active_teams;
	uint16 ever_active_teams;
	int16 initial_team_count;
	int16 team_designator[9];
	uint8 current_state;
	bool game_finished;
	int16 current_round;
	int16 round_timer;
};

class c_simulation_game_engine_definition : public c_simulation_entity_definition
{
public:
	virtual uint32 creation_data_size() const override;
	virtual int8 entity_replication_required_for_view_activation(s_simulation_game_entity* entity) override;
	virtual int8 get_object_index(s_simulation_game_entity* entity) override;
	virtual int32 sub_A9004D(int a2, DWORD a3) override;
	virtual int8 creation_minimum_required_bits(s_simulation_game_entity* entity, void* a3, int32* minimum_required_bits) override;
	virtual void write_creation_description_to_string(s_simulation_game_entity* entity, void* tel_data, int32 buffer_size, char* buffer) override;
	virtual void entity_creation_encode(uint32 creation_data_size, void* creation_data, void* telemetry_data, c_bitstream* packet) override;
	virtual bool entity_creation_decode(uint32 creation_data_size, void* creation_data, c_bitstream* packet) override;
	virtual bool entity_update_encode(bool a1, uint32 update_mask, uint32* update_mask_written, uint32 state_data_size, void* state_data, void* telemetry_data, c_bitstream* packet, int32 required_leave_space_bits) override;
	virtual bool entity_update_decode(bool a1, uint32* out_update_mask, uint32 state_data_size, void* state_data, c_bitstream* packet) override;
	virtual uint32 build_creation_data(s_simulation_game_entity* entity, int32 creation_data_size, void* out_creation_data) override;
	virtual bool build_baseline_state_data(int32 creation_data_size, void* creation_data, int32 state_data_size, void* out_state_baseline_data) override;
	virtual bool build_updated_state_data(s_simulation_game_entity* entity, uint32* update_mask, int32 state_data_size, void* state_data) override;
	virtual uint32 rotate_entity_indices(s_simulation_game_entity* entity) override;
	virtual bool create_game_entity(s_simulation_game_entity* entity, int32 creation_data_size, void* creation_data, uint32 mask, int32 state_data_size, void* state_data) override;
	virtual bool update_game_entity(s_simulation_game_entity* entity, uint32 update_flags, int32 state_data_size, void* state_data) override;
	virtual bool delete_game_entity(s_simulation_game_entity* entity) override;
	virtual bool promote_game_entity_to_authority(s_simulation_game_entity* entity) override;
};