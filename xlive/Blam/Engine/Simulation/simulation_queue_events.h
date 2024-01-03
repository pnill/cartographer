#pragma once

#include "Simulation/simulation_queue.h"
#include "Simulation/simulation_type_collection.h"

void simulation_event_queue_apply(const s_simulation_queue_element* update);
void simulation_queue_event_insert(e_simulation_event_type type, int32 reference_count, int32* references, int32 block_size, uint8* block);