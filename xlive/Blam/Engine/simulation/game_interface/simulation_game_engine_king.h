#pragma once
#include "simulation_game_engine.h"

#define k_simulation_king_entity_name "king-engine-globals"

enum e_simulation_king_engine_state_data_flags
{
	_king_engine_state_data_flag_hill_id_exists = 5,
	_king_engine_state_data_flag_players_in_hill_exists = 6,


	k_king_engine_state_data_flags_count = 2,
	k_king_engine_state_data_flags_total_count = k_king_engine_state_data_flags_count + k_game_engine_state_data_flag_count,
	k_king_engine_state_data_initial_update_mask = MASK(k_king_engine_state_data_flags_total_count),
	k_king_engine_state_data_flags_mask = FLAG_RANGE(_king_engine_state_data_flag_hill_id_exists, _king_engine_state_data_flag_players_in_hill_exists)
};

struct s_king_engine_state_data : s_game_engine_state_data
{
	uint16 hill_id;
	uint16 players_in_hill;
};

class c_simulation_king_engine_globals_definition : public c_simulation_game_engine_definition
{
public:
	virtual e_simulation_entity_type entity_type() override;
	virtual const char* entity_type_name() override;
	virtual uint32 state_data_size() const override;
	virtual int32 update_flag_count() override;
	virtual uint32 initial_update_mask() override;
	virtual void calculate_update_relevance(int a1, void* update_data, int32* out_relevance) override;
	virtual bool entity_update_encode(bool a1, uint32 update_mask, uint32* update_mask_written, uint32 state_data_size, void* state_data, void* telemetry_data, c_bitstream* packet, int32 required_leave_space_bits) override;
	virtual bool entity_update_decode(bool a1, uint32* out_update_mask, uint32 state_data_size, void* state_data, c_bitstream* packet) override;
	virtual bool entity_state_lossy_compare(void* a1, void* a2, int32 a3) override;
	virtual bool entity_creation_lossy_compare(void* a1, void* a2, int32 a3) override;
	virtual void* build_object_creation_data(datum object_index, int32 creation_data_size, void* creation_data) override;
};