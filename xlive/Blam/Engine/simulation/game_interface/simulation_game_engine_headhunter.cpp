#include "stdafx.h"
#include "simulation_game_engine_headhunter.h"

#include "simulation/simulation_entity_update_encode_helper.h"

static c_simulation_headhunter_engine_globals_definition g_simulation_headhunter_engine_globals_definition;

e_simulation_entity_type c_simulation_headhunter_engine_globals_definition::entity_type()
{
	return _simulation_entity_type_headhunter_engine_globals;
}

const char* c_simulation_headhunter_engine_globals_definition::entity_type_name()
{
	return k_simulation_headhunter_entity_name;
}

uint32 c_simulation_headhunter_engine_globals_definition::state_data_size() const
{
	return sizeof(s_headhunter_engine_state_data);
}

int32 c_simulation_headhunter_engine_globals_definition::update_flag_count()
{
	return k_headhunter_engine_state_data_flags_total_count;
}

uint32 c_simulation_headhunter_engine_globals_definition::initial_update_mask()
{
	return k_headhunter_engine_state_data_initial_update_mask;
}

//void c_simulation_headhunter_engine_globals_definition::calculate_update_relevance(int a1, void* update_data,
//	int32* out_relevance)
//{
//	*out_relevance = INVOKE_TYPE(0x2017AD, 0, int32(__thiscall*)(c_simulation_headhunter_engine_globals_definition*, int, void*, int32*), this, a1, update_data, out_relevance);
//}
//
bool c_simulation_headhunter_engine_globals_definition::entity_update_encode(bool a1, uint32 update_mask,
	uint32* update_mask_written, uint32 state_data_size, void* state_data, void* telemetry_data, c_bitstream* packet,
	int32 required_leave_space_bits)
{
	////if (!c_simulation_game_engine_definition::entity_update_encode(
	////	a1,
	////	update_mask & k_game_engine_state_data_flags_mask,
	////	update_mask_written,
	////	state_data_size,
	////	state_data,
	////	telemetry_data,
	////	packet,
	////	required_leave_space_bits))
	////{
	////	return false;
	////}

	if (!c_simulation_king_engine_globals_definition::entity_update_encode(
		a1,
		update_mask,
		update_mask_written,
		state_data_size,
		state_data,
		telemetry_data,
		packet,
		required_leave_space_bits))
	{
		return false;
	}

	c_entity_update_encode_helper helper{};
	s_headhunter_engine_state_data* game_state_data = (s_headhunter_engine_state_data*)state_data;

	if (!helper.setup(
		packet,
		required_leave_space_bits,
		_headhunter_engine_state_flag_player_skull_count,
		k_headhunter_engine_state_data_flags_count,
		update_mask & k_headhunter_engine_state_data_flags_mask))
	{
		return false;
	}

	if (helper.set_component_flag(_headhunter_engine_state_flag_player_skull_count, "players-skull-count-exists"))
	{
		packet->write_raw_data("players-skull-count", game_state_data->player_skull_count, sizeof(game_state_data->player_skull_count));
	}
	helper.commit();

	return true;
	//return INVOKE_TYPE(0x201864, 0, bool(__thiscall*)(c_simulation_headhunter_engine_globals_definition*, bool, uint32, uint32*, uint32, void*, void*, c_bitstream*, int32),
	//	this, a1, update_mask, update_mask_written, state_data_size, state_data, telemetry_data, packet, required_leave_space_bits);
}

bool c_simulation_headhunter_engine_globals_definition::entity_update_decode(bool a1, uint32* out_update_mask,
	uint32 state_data_size, void* state_data, c_bitstream* packet)
{
	//if(!c_simulation_game_engine_definition::entity_update_decode(
	//	a1, 
	//	out_update_mask, 
	//	state_data_size, 
	//	state_data, 
	//	packet))
	//{
	//	return false;
	//}

	if (!c_simulation_king_engine_globals_definition::entity_update_decode(
		a1,
		out_update_mask,
		state_data_size,
		state_data,
		packet))
	{
		return false;
	}

	s_headhunter_engine_state_data* game_state_data = (s_headhunter_engine_state_data*)state_data;

	if (packet->read_bool("player-skull-count-exists"))
	{
		packet->read_raw_data("players-skull-count", game_state_data->player_skull_count, sizeof(game_state_data->player_skull_count));
		*out_update_mask |= FLAG(_headhunter_engine_state_flag_player_skull_count);
	}

	return true;
	//return INVOKE_TYPE(0x201885, 0, bool(__thiscall*)(c_simulation_headhunter_engine_globals_definition*, bool, uint32*, uint32, void*, c_bitstream*),
	//	this, a1, out_update_mask, state_data_size, state_data, packet);
}
//
//bool c_simulation_headhunter_engine_globals_definition::entity_state_lossy_compare(void* a1, void* a2, int32 a3)
//{
//	return true;
//}
//
//bool c_simulation_headhunter_engine_globals_definition::entity_creation_lossy_compare(void* a1, void* a2, int32 a3)
//{
//	return true;
//}
//
//void* c_simulation_headhunter_engine_globals_definition::build_object_creation_data(datum object_index,
//	int32 creation_data_size, void* creation_data)
//{
//	return INVOKE_TYPE(0x2017C0, 0, void* (__thiscall*)(c_simulation_headhunter_engine_globals_definition*, datum, int32, void*),
//		this, object_index, creation_data_size, creation_data);
//}

c_simulation_headhunter_engine_globals_definition* get_global_headhunter_engine_globals_definition()
{
	return &g_simulation_headhunter_engine_globals_definition;
}