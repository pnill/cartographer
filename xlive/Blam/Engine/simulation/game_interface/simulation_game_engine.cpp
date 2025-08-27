#include "stdafx.h"
#include "simulation_game_engine.h"

#include "game/game_allegiance.h"
#include "saved_games/game_variant.h"
#include "simulation/simulation_entity_update_encode_helper.h"

uint32 c_simulation_game_engine_definition::creation_data_size() const
{
	return 0;
}

int8 c_simulation_game_engine_definition::entity_replication_required_for_view_activation(s_simulation_game_entity* entity)
{
	return 1;
}

int8 c_simulation_game_engine_definition::get_object_index(s_simulation_game_entity* entity)
{
	return 0;
}

int32 c_simulation_game_engine_definition::sub_A9004D(int a2, DWORD a3)
{
	return 1;
}

int8 c_simulation_game_engine_definition::creation_minimum_required_bits(s_simulation_game_entity* entity, void* a3, int32* minimum_required_bits)
{
	*minimum_required_bits = 0;
	return 1;
}

void c_simulation_game_engine_definition::write_creation_description_to_string(s_simulation_game_entity* entity, void* tel_data, int32 buffer_size, char* buffer)
{
	INVOKE_TYPE(0x1F7512, 0x1E1278, void(__thiscall*)(c_simulation_game_engine_definition*, s_simulation_game_entity*, void*, int32, char*),
		this, entity, tel_data, buffer_size, buffer);
}

void c_simulation_game_engine_definition::entity_creation_encode(uint32 creation_data_size, void* creation_data, void* telemetry_data, c_bitstream* packet)
{
	return;
}

bool c_simulation_game_engine_definition::entity_creation_decode(uint32 creation_data_size, void* creation_data, c_bitstream* packet)
{
	return true;
}

bool c_simulation_game_engine_definition::entity_update_encode(bool a1, uint32 update_mask, uint32* update_mask_written,
	uint32 state_data_size, void* state_data, void* telemetry_data, c_bitstream* packet,
	int32 required_leave_space_bits)
{
	return INVOKE_TYPE(0x1F76B7, 0x1E141D, bool(__thiscall*)(c_simulation_game_engine_definition*, uint32, uint32*, void*, c_bitstream*, int32),
		this, update_mask, update_mask_written, state_data, packet, required_leave_space_bits);


	/*c_entity_update_encode_helper helper {};
	s_game_engine_state_data* game_state_data = (s_game_engine_state_data*)state_data;

	if (!helper.setup(packet, required_leave_space_bits, _game_engine_state_data_flag_team_mapping_exists, k_game_engine_state_data_flag_count, update_mask))
		return false;

	if(helper.set_component_flag(_game_engine_state_data_flag_team_mapping_exists, "team-mapping-exists"))
	{
		packet->write_integer("initial-teams", game_state_data->initial_teams, 8);
		packet->write_integer("valid-designators", game_state_data->valid_designators, 9);
		packet->write_integer("valid-teams", game_state_data->valid_teams, 8);
		packet->write_integer("active-teams", game_state_data->active_teams, 8);
		packet->write_integer("ever-active-teams", game_state_data->ever_active_teams, 8);
		packet->write_integer("initial-team-count", game_state_data->initial_team_count, 5);

		for (uint32 index = 0; index < k_game_simulation_team_count; index++)
		{
			if (TEST_BIT(game_state_data->valid_designators, index))
				packet->write_integer("team-designator-to-team-index", game_state_data->team_designator[index], bits_required_for(k_game_simulation_team_count));
		}
	}
	helper.commit();

	if (helper.set_component_flag(_game_engine_state_data_flag_current_state_exists, "current-state-exists"))
		packet->write_integer("current-state", game_state_data->current_state, 2);
	helper.commit();

	if (helper.set_component_flag(_game_engine_state_data_flag_game_finished_exists, "game-finished-exists"))
		packet->write_bool("game-finished", game_state_data->game_finished);
	helper.commit();

	if (helper.set_component_flag(_game_engine_state_data_flag_current_round_exists, "current-round-exists"))
		packet->write_integer("current-round", game_state_data->current_round, 5);
	helper.commit();

	if (helper.set_component_flag(_game_engine_state_data_flag_round_timer_exists, "round-timer-exists"))
		packet->write_integer("round-timer", game_state_data->round_timer + 1, SHORT_BITS);
	helper.commit();

	helper.set_update_mask(update_mask_written);

	return true;*/
}

bool c_simulation_game_engine_definition::entity_update_decode(bool a1, uint32* out_update_mask, uint32 state_data_size,
	void* state_data, c_bitstream* packet)
{
	return INVOKE_TYPE(0x1F789E, 0x1E1504, bool(__thiscall*)(c_simulation_game_engine_definition*, uint32*, void*, c_bitstream*),
		this, out_update_mask, state_data, packet);

	//s_game_engine_state_data* game_state_data = (s_game_engine_state_data*)state_data;

	//if(packet->read_bool("team-mapping-exists"))
	//{
	//	game_state_data->initial_teams = packet->read_integer("initial-teams", 8);
	//	game_state_data->valid_designators = packet->read_integer("valid-designators", 9);
	//	game_state_data->valid_teams = packet->read_integer("valid-teams", 8);
	//	game_state_data->active_teams = packet->read_integer("active-teams", 8);
	//	game_state_data->ever_active_teams = packet->read_integer("ever-active-teams", 8);
	//	game_state_data->initial_team_count = packet->read_integer("initial-team-count", 5);

	//	bool valid_teams = true;

	//	if ((game_state_data->valid_teams & ~game_state_data->initial_teams) != 0 ||
	//		(game_state_data->ever_active_teams & ~game_state_data->valid_teams) != 0 ||
	//		(game_state_data->active_teams & ~game_state_data->ever_active_teams) != 0 ||
	//		(game_state_data->initial_team_count >= 17))
	//	{
	//		valid_teams = false;
	//	}

	//	if(valid_teams)
	//	{
	//		for(uint32 index = 0; index < k_game_simulation_team_count; index++)
	//		{
	//			if(TEST_BIT(game_state_data->valid_designators, index))
	//			{
	//				uint16 team_designator = packet->read_integer("team-designator-to-team-index", bits_required_for(k_game_simulation_team_count));
	//				game_state_data->team_designator[index] = team_designator;

	//				//if (team_designator <= 7)
	//				//	return false;
	//			}
	//			else
	//			{
	//				game_state_data->team_designator[index] = NONE;
	//			}
	//		}
	//	}

	//	*out_update_mask |= FLAG(_game_engine_state_data_flag_team_mapping_exists);
	//}

	//if(packet->read_bool("current-state-exists"))
	//{
	//	game_state_data->current_state = packet->read_integer("current-state", 2);
	//	*out_update_mask |= FLAG(_game_engine_state_data_flag_current_state_exists);
	//}

	//if(packet->read_bool("game-finished-exists"))
	//{
	//	game_state_data->game_finished = packet->read_bool("game-finished");
	//	*out_update_mask |= FLAG(_game_engine_state_data_flag_game_finished_exists);
	//}

	//if(packet->read_bool("current-round-exists"))
	//{
	//	game_state_data->current_round = packet->read_integer("current-round", 5);
	//	*out_update_mask |= FLAG(_game_engine_state_data_flag_current_round_exists);
	//}

	//if(packet->read_bool("round-timer-exists"))
	//{
	//	game_state_data->round_timer = packet->read_integer("round-timer", SHORT_BITS) - 1;
	//	*out_update_mask |= FLAG(_game_engine_state_data_flag_round_timer_exists);
	//}

	//return true;
}

uint32 c_simulation_game_engine_definition::build_creation_data(s_simulation_game_entity* entity, int32 creation_data_size, void* out_creation_data)
{
	return 0;
}

bool c_simulation_game_engine_definition::build_baseline_state_data(int32 creation_data_size, void* creation_data, int32 state_data_size, void* out_state_baseline_data)
{
	return INVOKE_TYPE(0x1F73CD, 0x1E1133, bool(__thiscall*)(c_simulation_game_engine_definition*, int32, void*, int32, void*),
		this, creation_data_size, creation_data, state_data_size, out_state_baseline_data);
}

bool c_simulation_game_engine_definition::build_updated_state_data(s_simulation_game_entity* entity, uint32* update_mask, int32 state_data_size, void* state_data)
{
	return INVOKE_TYPE(0x1F7402, 0x1E1168, bool(__thiscall*)(c_simulation_game_engine_definition*, s_simulation_game_entity*, uint32*, int32, void*),
		this, entity, update_mask, state_data_size, state_data);
}

uint32 c_simulation_game_engine_definition::rotate_entity_indices(s_simulation_game_entity* entity)
{
	return INVOKE_TYPE(0x1F7443, 0x1E11A9, uint32(__thiscall*)(c_simulation_game_engine_definition*, s_simulation_game_entity*), this, entity);
}

bool c_simulation_game_engine_definition::create_game_entity(s_simulation_game_entity* entity, int32 creation_data_size, void* creation_data, uint32 mask, int32 state_data_size, void* state_data)
{
	return INVOKE_TYPE(0x1F755F, 0x1E12C5, bool(__thiscall*)(c_simulation_game_engine_definition*, s_simulation_game_entity*, int32, void*, uint32, int32, void*),
		this, entity, creation_data_size, creation_data, mask, state_data_size, state_data);
}

bool c_simulation_game_engine_definition::update_game_entity(s_simulation_game_entity* entity, uint32 update_flags, int32 state_data_size, void* state_data)
{
	return INVOKE_TYPE(0x1F7482, 0x1E11E8, bool(__thiscall*)(c_simulation_game_engine_definition*, s_simulation_game_entity*, uint32, int32, void*),
		this, entity, update_flags, state_data_size, state_data);
}

bool c_simulation_game_engine_definition::delete_game_entity(s_simulation_game_entity* entity)
{
	return INVOKE_TYPE(0x1F75BE, 0x1E1324, bool(__thiscall*)(c_simulation_game_engine_definition*, s_simulation_game_entity*), this, entity);
}

bool c_simulation_game_engine_definition::promote_game_entity_to_authority(s_simulation_game_entity* entity)
{
	return INVOKE_TYPE(0x1F74C1, 0x1E1227, bool(__thiscall*)(c_simulation_game_engine_definition*, s_simulation_game_entity*), this, entity);
}