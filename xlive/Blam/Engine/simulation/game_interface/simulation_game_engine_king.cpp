#include "stdafx.h"
#include "simulation_game_engine_king.h"

e_simulation_entity_type c_simulation_king_engine_globals_definition::entity_type()
{
	return _simulation_entity_type_king_engine_globals;
}

const char* c_simulation_king_engine_globals_definition::entity_type_name()
{
	return k_simulation_king_entity_name;
}

uint32 c_simulation_king_engine_globals_definition::state_data_size() const
{
	return sizeof(s_king_engine_state_data);
}

int32 c_simulation_king_engine_globals_definition::update_flag_count()
{
	return k_king_engine_state_data_flags_total_count;
}

uint32 c_simulation_king_engine_globals_definition::initial_update_mask()
{
	return k_king_engine_state_data_initial_update_mask;
}

void c_simulation_king_engine_globals_definition::calculate_update_relevance(int a1, void* update_data, int32* out_relevance)
{
	INVOKE_TYPE(0x200E39, 0x1EABA1, int32(__thiscall*)(c_simulation_king_engine_globals_definition*, int, void*, int32*), this, a1, update_data, out_relevance);
}

bool c_simulation_king_engine_globals_definition::entity_update_encode(bool a1, uint32 update_mask, uint32* update_mask_written, uint32 state_data_size, void* state_data, void* telemetry_data, c_bitstream* packet, int32 required_leave_space_bits)
{
	return INVOKE_TYPE(0x200F1E, 0x1EAC86, bool(__thiscall*)(c_simulation_king_engine_globals_definition*, bool, uint32, uint32*, uint32, void*, void*, c_bitstream*, int32),
		this, a1, update_mask, update_mask_written, state_data_size, state_data, telemetry_data, packet, required_leave_space_bits);
}

bool c_simulation_king_engine_globals_definition::entity_update_decode(bool a1, uint32* out_update_mask, uint32 state_data_size, void* state_data, c_bitstream* packet)
{
	return INVOKE_TYPE(0x200FFB, 0x1EAD63, bool(__thiscall*)(c_simulation_king_engine_globals_definition*, bool, uint32*, uint32, void*, c_bitstream*),
		this, a1, out_update_mask, state_data_size, state_data, packet);
}

bool c_simulation_king_engine_globals_definition::entity_state_lossy_compare(void* a1, void* a2, int32 a3)
{
	return true;
}

bool c_simulation_king_engine_globals_definition::entity_creation_lossy_compare(void* a1, void* a2, int32 a3)
{
	return true;
}

void* c_simulation_king_engine_globals_definition::build_object_creation_data(datum object_index, int32 creation_data_size, void* creation_data)
{
	return INVOKE_TYPE(0x200E7A, 0x1EABE2, void* (__thiscall*)(c_simulation_king_engine_globals_definition*, datum, int32, void*),
		this, object_index, creation_data_size, creation_data);
}