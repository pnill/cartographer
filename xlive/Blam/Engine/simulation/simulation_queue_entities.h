#pragma once
#include "simulation.h"
#include "simulation_queue.h"

struct s_simulation_queue_entity_data
{
	int32 entity_index;
	e_simulation_entity_type entity_type;
	int32 creation_data_size;
	uint8* creation_data;
	int32 state_data_size;
	uint8* state_data;
};

struct s_simulation_queue_decoded_creation_data
{
	int32 entity_index;
	e_simulation_entity_type entity_type;
	datum gamestate_index;
	uint32 initial_update_mask;
	uint8 creation_data[k_simulation_payload_size_max];
	int32 creation_data_size;
	uint8 state_data[k_simulation_payload_size_max];
	int32 state_data_size;
};

struct s_simulation_queue_entity_update_apply
{
};

void simulation_queue_entity_creation_insert(s_simulation_queue_element* element);
bool simulation_queue_entity_creation_allocate(s_simulation_queue_entity_data* simulation_queue_entity_data, uint32 update_mask, s_simulation_queue_element** element, int32* gamestate_index);
void simulation_queue_entity_creation_apply(const s_simulation_queue_element* element);

