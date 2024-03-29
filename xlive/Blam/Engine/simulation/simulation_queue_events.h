#pragma once

#include "simulation/simulation_queue.h"
#include "simulation/simulation_event_handler.h"
#include "simulation/simulation_type_collection.h"

struct s_simulation_queue_events_apply
{
	e_simulation_event_type event_type;
	int32 reference_count;
	int32 entity_references[k_entity_reference_indices_count_max];
	uint8 data[k_simulation_event_maximum_payload_size];
	int32 data_size;
};

bool simulation_queue_event_apply(const s_simulation_queue_element* update);
void simulation_queue_event_insert(e_simulation_event_type type, int32 reference_count, int32* references, int32 block_size, uint8* block);