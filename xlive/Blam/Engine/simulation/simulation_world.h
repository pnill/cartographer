#pragma once
#include "simulation_queue_events.h"

enum e_simulation_queue_type
{
	_simulation_queue_high_priority,
	_simulation_queue_basic,

	k_simulation_queue_count
};

enum e_simulation_world_type
{
	_simulation_world_type_none = 0x0,
	_simulation_world_type_local = 0x1,
	_simulation_world_type_synchronous_authority = 0x2,
	_simulation_world_type_synchronous_client = 0x3,
	_simulation_world_type_distributed_authority = 0x4,
	_simulation_world_type_distributed_client = 0x5,
	k_simulation_world_type_count = 0x6,
};

enum e_simulation_world_state
{
	_simulation_world_state_none = 0x0,
	_simulation_world_state_dead = 0x1,
	_simulation_world_state_disconnected = 0x2,
	_simulation_world_state_joining = 0x3,
	_simulation_world_state_active = 0x4,
	_simulation_world_state_handoff = 0x5,
	_simulation_world_state_leaving = 0x6,
	k_simulation_world_state_count = 0x7,
};

struct s_simulation_queue_stats
{
	int32 allocated;
	int32 queued;
};

class c_simulation_world
{
public:
	uint8 gap_0[4];
	void* m_distributed_world;
	// ### TODO validate
	e_simulation_world_type m_simulation_world_mode;

	void simulation_queue_allocate(e_event_queue_type type, int32 encoded_size, s_simulation_queue_element** out_allocated_elem);
	void simulation_queue_enqueue(s_simulation_queue_element* element);

	void queues_initialize();

	void apply_simulation_queue(const c_simulation_queue* queue);
	void apply_basic_queue();
	void apply_high_priority_queue();

	c_simulation_queue* queue_get(e_simulation_queue_type type);

	void initialize_world(int32 a2, int32 a3, int32 a4);
	// discard resources
	void destroy_world();

	void queues_dispose();

	s_simulation_queue_stats queue_describe(const c_simulation_queue* queue)
	{
		return s_simulation_queue_stats{ queue->allocated_count(), queue->queued_count() };
	}

	void destroy_update();

	bool is_playback()
	{
		if (exists())
		{
			return false;
		}

		return false;
	}

	bool exists()
	{
		return m_simulation_world_mode != _simulation_world_type_none;
	}

private:
};

void simulation_world_apply_patches();