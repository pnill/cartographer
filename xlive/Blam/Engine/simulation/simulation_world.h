#pragma once
#include "simulation_actors.h"
#include "simulation_entity_database.h"
#include "simulation_event_handler.h"
#include "simulation_players.h"
#include "simulation_queue.h"
#include "simulation_view.h"

#include "networking/replication/replication_event_manager.h"

/* constants */

enum
{
	k_simulation_world_maximum_views = k_maximum_players,
	k_network_maximum_actors_per_simulation = 16,
};

/* enums */

enum e_simulation_queue_type
{
	_simulation_queue_bookkeeping,
	_simulation_queue,

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

/* structure */

struct s_world_state_disconnected
{
	uint32 disconnected_timestamp;
};

struct s_world_state_data_joining
{
	uint32 join_start_timestamp;
	uint32 join_client_machine_mask;
};

struct s_world_state_data_active
{
	uint32 active_client_machine_mask;
};

union s_world_state_data
{
	s_world_state_disconnected disconnected;
	s_world_state_data_joining joining;
	s_world_state_data_active active;
};
ASSERT_STRUCT_SIZE(s_world_state_data, 0x8);

class c_simulation_distributed_world
{
public:
	c_replication_entity_manager m_entity_manager;
	c_replication_event_manager m_event_manager;
	c_simulation_entity_database m_entity_database;
	c_simulation_event_handler m_event_handler;	
};
ASSERT_STRUCT_SIZE(c_simulation_distributed_world, 45260);

class c_simulation_world
{
	class c_simulation_watcher* m_watcher;
	c_simulation_distributed_world* m_distributed_world;
	e_simulation_world_type m_world_type;
	bool m_local_machine_identifier_valid;
	s_machine_identifier m_local_machine_identifier;
	int32 m_local_machine_index;
	e_simulation_world_state m_world_state;
	s_world_state_data m_world_state_data;
	bool m_time_running;
	bool m_time_immediate_update;
	int m_next_update_number;
	bool m_out_of_sync;
	bool m_out_of_sync_determinism_failure;
	bool m_gamestate_flushed;
	bool m_attached_to_map;
	int32 m_unsuccessful_join_attempts;
	uint32 m_last_active_timestamp;
	int32 m_next_view_establishment_identifier;
	int32 m_joining_total_wait_msec;
	int32 m_view_count;
	c_simulation_view* m_views[k_simulation_world_maximum_views];
	int32 m_player_count; // guessed name for potential use, field is completely unused
	c_simulation_player m_players[k_maximum_players];
	c_simulation_actor m_actors[k_network_maximum_players_per_session];
	bool m_gamestate_flush_active;
	int32 m_synchronous_gamestate_write_progress;
	void* m_synchronous_gamestate_write_buffer;
	uint32 m_synchronous_catchup_initiation_failure_timestamp;
	int32 m_synchronous_client_next_update_number_to_dequeue;
	int32 m_synchronous_client_latest_update_number_received;
	int32 m_synchronous_client_queue_length;
	void* m_synchronous_client_queue_head;
	void* m_synchronous_client_queue_tail;
	int32 _pad_12AC;

public:
	void gamestate_flush_immediate(void);

	void simulation_queue_allocate(e_event_queue_type type, int32 encoded_size, s_simulation_queue_element** out_allocated_elem);
	void simulation_queue_free(s_simulation_queue_element* element);
	void simulation_queue_enqueue(s_simulation_queue_element* element);

	void queues_initialize(void);
	void apply_simulation_queue(c_simulation_queue const* queue);

	void attach_simulation_queues_to_update(c_simulation_queue* out_bookkeepin_queue, c_simulation_queue* out_game_simulation_queue);

	c_simulation_queue* queue_get(e_simulation_queue_type type) const;

	c_simulation_distributed_world* get_distributed_world(void) const { return m_distributed_world; }

	void initialize_world(c_simulation_type_collection* type_collection, class c_simulation_watcher* watcher, c_simulation_distributed_world* distributed_world);
	
	bool claim_authority_gameworld(void);

	void delete_all_actors(void);

	void update_queue_reset(void);

	// discard resources
	void reset_world(void);

	void destroy_world(void);
	void disconnect(void);

	void queues_dispose(void);

	void create_player(datum player_index);
	void delete_player(datum player_index);

	void queues_update_statistics(void)
	{
		for (int32 i = 0; i < k_simulation_queue_count; i++)
		{
			queue_get((e_simulation_queue_type)i)->build_statistics();
		}
	}

	bool queue_describe(e_simulation_queue_type type, const s_simulation_queue_stats** out_stats) const
	{
		return queue_get(type)->get_statistics(out_stats);
	}

	void queues_clear(void);

	bool is_playback(void) const
	{
		// todo: re-add once destroy_world function is re-written
		//ASSERT(exists());
		return false;
	}

	bool is_distributed(void) const
	{
		return m_world_type == _simulation_world_type_distributed_authority
			|| m_world_type == _simulation_world_type_distributed_client;
	}

	bool exists(void) const
	{
		return m_world_type != _simulation_world_type_none;
	}

	bool runs_simulation(void) const
	{
		ASSERT(exists());
		return m_world_type != _simulation_world_type_synchronous_client;
	}

	bool is_authority(void) const
	{
		ASSERT(exists());
		return m_world_type != _simulation_world_type_distributed_client && m_world_type != _simulation_world_type_synchronous_client;
	}

	bool is_active(void) const
	{
		ASSERT(exists());
		return m_world_state == _simulation_world_state_active;
	}

	bool simulation_queues_empty(void) const
	{
		return queue_get(_simulation_queue_bookkeeping)->queued_count() == 0 && queue_get(_simulation_queue)->queued_count() == 0;
	}

	void send_player_acknowledgements_not_during_simulation_reset_in_progress(bool a1);

	void send_player_acknowledgements(bool a1)
	{
		INVOKE_TYPE(0x1DD777, 0x1C4C37, void(__thiscall*)(c_simulation_world*, bool), this, a1);
		return;
	}

	bool time_running(void) const
	{
		ASSERT(exists());
		return m_time_running;
	}
};
ASSERT_STRUCT_SIZE(c_simulation_world, 0x12B0);

void simulation_world_apply_patches(void);