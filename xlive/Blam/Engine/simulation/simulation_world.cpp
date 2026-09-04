#include "stdafx.h"
#include "simulation_world.h"

#include "simulation_queue_events.h"
#include "simulation_queue_entities.h"
#include "simulation_queue_global_events.h"

#include "simulation.h"

#include "networking/network_event.h"
#include "saved_games/game_state_procs.h"

#include "H2MOD/GUI/imgui_integration/Console/ImGui_ConsoleImpl.h"

// TODO verify if these buffers get saturated quickly
// if that's the case, increse the buffer size
c_simulation_queue g_simulation_queues[k_simulation_queue_count];

void c_simulation_world::gamestate_flush_immediate(void)
{
	if (!is_authority())
	{
		game_state_call_before_save_procs(0);
		game_state_call_after_save_procs(0);
	}
	return;
}

c_simulation_queue* c_simulation_world::queue_get(e_simulation_queue_type type) const
{
	return &g_simulation_queues[type];
}

void c_simulation_world::simulation_queue_allocate(e_event_queue_type type, int32 data_size, s_simulation_queue_element** out_allocated_elem)
{
	ASSERT(type != _simulation_queue_element_type_none);
	ASSERT(data_size > 0);
	ASSERT(out_allocated_elem != NULL);

	*out_allocated_elem = NULL;
	if (TEST_FLAG(FLAG(type), _simulation_queue_element_type_bookkeeping))
	{
		// player event, player update, gamestate clear
		queue_get(_simulation_queue_bookkeeping)->allocate(data_size, out_allocated_elem);
	}
	else
	{
		bool sim_queue_restrict_allocations = false;
		c_simulation_queue* simulation_queue = queue_get(_simulation_queue);

		if (!TEST_FLAG(FLAG(type), _simulation_queue_element_important_update))
		{
			real32 allocated_percentage;
			real32 allocated_in_bytes_percentage;
			simulation_queue->get_allocation_status(&allocated_percentage, &allocated_in_bytes_percentage);

			// if we allocated more than 90% of the buffer
			// skip some updates to aleviate some of the stress on the queue
			// especially if the game froze for multiple seconds
			// and allow the allocation for important updates only
			// entity deletion, entity promotion, and global game events
			if (allocated_percentage > 90.f / 100.f
				|| allocated_in_bytes_percentage > 90.f / 100.f)
			{
				sim_queue_restrict_allocations = true;
			}
		}

		// event, creation, update, entity_deletion, entity_promotion, game_global_event
		if (!sim_queue_restrict_allocations)
			simulation_queue->allocate(data_size, out_allocated_elem);
	}

	if (*out_allocated_elem)
	{
		(*out_allocated_elem)->type = type;
	}
}

void c_simulation_world::simulation_queue_free(s_simulation_queue_element* element)
{
	if (TEST_FLAG(FLAG(element->type), _simulation_queue_element_type_bookkeeping))
	{
		// player event, player update, gamestate clear
		queue_get(_simulation_queue_bookkeeping)->deallocate(element);
	}
	else
	{
		queue_get(_simulation_queue)->deallocate(element);
	}
}

void c_simulation_world::simulation_queue_enqueue(s_simulation_queue_element* element)
{
	if (TEST_FLAG(FLAG(element->type), _simulation_queue_element_type_bookkeeping))
	{
		// player event, player update, gamestate clear
		queue_get(_simulation_queue_bookkeeping)->enqueue(element);

		SIM_EVENT_QUEUE_DBG("queue 0x%08X allocated count: %d, size: %d",
			&g_simulation_queues[_simulation_queue_bookkeeping],
			g_simulation_queues[_simulation_queue_bookkeeping].allocated_count(),
			g_simulation_queues[_simulation_queue_bookkeeping].allocated_size_in_bytes());

		SIM_EVENT_QUEUE_DBG("queue 0x%08X queued count: %d, size: %d", 
			&g_simulation_queues[_simulation_queue_bookkeeping],
			g_simulation_queues[_simulation_queue_bookkeeping].queued_count(),
			g_simulation_queues[_simulation_queue_bookkeeping].queued_size_in_bytes());
	}
	else
	{
		// event, creation, update, entity_deletion, entity_promotion, game_global_event

		queue_get(_simulation_queue)->enqueue(element);

		SIM_EVENT_QUEUE_DBG("queue 0x%08X allocated count: %d, size: %d",
			&g_simulation_queues[_simulation_queue],
			g_simulation_queues[_simulation_queue].allocated_count(),
			g_simulation_queues[_simulation_queue].allocated_size_in_bytes());

		SIM_EVENT_QUEUE_DBG("queue 0x%08X queued count: %d, size: %d", 
			&g_simulation_queues[_simulation_queue], 
			g_simulation_queues[_simulation_queue].queued_count(), 
			g_simulation_queues[_simulation_queue].queued_size_in_bytes());
	}
}

void c_simulation_world::apply_simulation_queue(
	const c_simulation_queue* simulation_queue)
{
	ASSERT(simulation_queue != NULL);

	if (simulation_queue->queued_count() > 0)
	{
		const s_simulation_queue_element* element = simulation_queue->get_first_element();
		int32 update_count = 0;
		int32 total_size = 0;

		// Added logic so we don't apply player updates if one of them fails
		bool apply_player_updates = true;

		while (element != NULL)
		{
			switch (element->type)
			{
			case _simulation_queue_element_type_event:
				simulation_queue_event_apply(element);
				break;
			case _simulation_queue_element_type_entity_creation:
				simulation_queue_entity_creation_apply(element);
				break;
			case _simulation_queue_element_type_entity_update:
				simulation_queue_entity_update_apply(element);
				break;
			case _simulation_queue_element_type_entity_deletion:
				simulation_queue_entity_deletion_apply(element);
				break;
			case _simulation_queue_element_type_entity_promotion:
				simulation_queue_entity_promotion_apply(element);
				break;
			case _simulation_queue_element_type_game_global_event:
				simulation_queue_game_global_event_apply(element);
				break;
			case _simulation_queue_element_type_player_event:
				simulation_queue_player_event_apply(element);
				break;
			case _simulation_queue_element_type_player_update_event:
				if (apply_player_updates && !simulation_queue_player_update_apply(element))
				{
					simulation_fatal_error();
					apply_player_updates = false;
				}
				break;
			case _simulation_queue_element_type_gamestates_clear:
				break;
			case _simulation_queue_element_type_sandbox_event:
				ASSERT(false);
				break;
			default:
				event(
					_event_error,
					"networking:simulation:world: apply_simulation_queue() unknown/invalid element type %d",
					element->type
				);
				break;
			}

			++update_count;
			total_size += simulation_queue->get_element_size_in_bytes(element);
			element = simulation_queue->get_next_element(element);
		}

		if (update_count != simulation_queue->queued_count())
		{
			event(
				_event_error,
				"networking:simulation:world: simulation queue from simulation update count mismatch [%d != %d]",
				update_count,
				simulation_queue->queued_count()
			);
		}

		if (total_size != simulation_queue->queued_size_in_bytes())
		{
			event(
				_event_error,
				"networking:simulation:world: simulation queue from simulation update size mismatch [%d != %d]",
				total_size,
				simulation_queue->queued_size_in_bytes()
			);
		}
	}
	return;
}

void c_simulation_world::attach_simulation_queues_to_update(
	c_simulation_queue* out_bookkeepin_queue,
	c_simulation_queue* out_game_simulation_queue)
{
	// FIXME: return the function to it's original state
	out_bookkeepin_queue->transfer_elements(queue_get(_simulation_queue_bookkeeping));
	out_game_simulation_queue->transfer_elements(queue_get(_simulation_queue));
	return;
}

void c_simulation_world::queues_clear(void)
{
	for (int32 i = 0; i < k_simulation_queue_count; i++)
	{
		queue_get((e_simulation_queue_type)i)->clear();
	}
}

typedef void(__thiscall* t_c_simulation_world__initialize_world)(c_simulation_world*, c_simulation_type_collection*, c_simulation_watcher*, c_simulation_distributed_world*);
t_c_simulation_world__initialize_world p_c_simulation_world__initialize_world;

CLASS_HOOK_DECLARE_LABEL(c_simulation_world__initialize_world, c_simulation_world::initialize_world);
void c_simulation_world::initialize_world(c_simulation_type_collection* type_collection, c_simulation_watcher* watcher, c_simulation_distributed_world* distributed_world)
{
	ASSERT(type_collection);

	p_c_simulation_world__initialize_world(this, type_collection, watcher, distributed_world);
	if (!is_playback())
	{
		queues_initialize();
	}
	return;
}

bool c_simulation_world::claim_authority_gameworld(
	void)
{
	return INVOKE_TYPE(0x1DE3D0, 0x1C5890, bool(__thiscall*)(c_simulation_world*), this);
}

void __declspec(naked) jmp_initialize_world(void)
{
	CLASS_HOOK_JMP(c_simulation_world__initialize_world, c_simulation_world::initialize_world);
}

void c_simulation_world::delete_all_actors(void)
{
	for (uint32 i = 0; i < NUMBEROF(m_actors); i++)
	{
		c_simulation_actor* actor = &m_actors[i];
		if (actor->m_actor_index != NONE)
		{
			actor->destroy();
		}
	}
	return;
}

void c_simulation_world::update_queue_reset(void)
{
	typedef void(__thiscall* update_queue_reset_t)(c_simulation_world*);
	INVOKE_TYPE(0x1DCDC3, 0x1C4277, update_queue_reset_t, this);
	return;
}

CLASS_HOOK_DECLARE_LABEL(c_simulation_world__reset_world, c_simulation_world::reset_world);
void c_simulation_world::reset_world(void)
{
	ASSERT(!is_authority());

	m_time_immediate_update = false;
	m_out_of_sync = false;
	m_gamestate_flushed = false;
	if (this->is_distributed())
	{
		ASSERT(m_distributed_world);

		m_distributed_world->m_entity_manager.reset();
		m_distributed_world->m_event_manager.reset();
		m_distributed_world->m_entity_database.reset();
		m_distributed_world->m_event_handler.reset();
		this->delete_all_actors();
	}

	if (!is_playback())
	{
		// during reset, discard just simulation updates
		// not bookkeeping updates
		queue_get(_simulation_queue)->clear();
	}

	if (!runs_simulation())
	{
		this->update_queue_reset();
	}

	return;
}

__declspec(naked) void jmp_reset_world(void)
{
	CLASS_HOOK_JMP(c_simulation_world__reset_world, c_simulation_world::reset_world);
}

typedef void(__thiscall* t_c_simulation_world__destroy_world)(c_simulation_world*);
t_c_simulation_world__destroy_world p_c_simulation_world__destroy_world;

CLASS_HOOK_DECLARE_LABEL(c_simulation_world__destroy_world, c_simulation_world::destroy_world);
void c_simulation_world::destroy_world(void)
{
	// call orig
	p_c_simulation_world__destroy_world(this);

	if (!is_playback())
	{
		queues_dispose();
	}
}

void __declspec(naked) jmp_destroy_world(void)
{
	CLASS_HOOK_JMP(c_simulation_world__destroy_world, c_simulation_world::destroy_world);
}

void c_simulation_world::disconnect(void)
{
	return;
}

CLASS_HOOK_DECLARE_LABEL(c_simulation_world__send_player_acknowledgements_not_during_simulation_reset_in_progress, c_simulation_world::send_player_acknowledgements_not_during_simulation_reset_in_progress);
void c_simulation_world::send_player_acknowledgements_not_during_simulation_reset_in_progress(bool a1)
{
	if (!simulation_reset_in_progress())
	{
		send_player_acknowledgements(a1);
	}
}

void __declspec(naked) jmp_send_player_acknowledgements_not_during_simulation_reset_in_progress()
{
	CLASS_HOOK_JMP(c_simulation_world__send_player_acknowledgements_not_during_simulation_reset_in_progress, c_simulation_world::send_player_acknowledgements_not_during_simulation_reset_in_progress);
}

void c_simulation_world::queues_initialize(void)
{
	for (int32 i = 0; i < k_simulation_queue_count; i++)
	{
		queue_get((e_simulation_queue_type)i)->initialize();
	}
}

void c_simulation_world::queues_dispose(void)
{
	for (int32 i = 0; i < k_simulation_queue_count; i++)
	{
		queue_get((e_simulation_queue_type)i)->dispose();
	}
}

void c_simulation_world::create_player(datum player_index)
{
	event(_event_verbose, "simulation:players: create player 0x%08X", player_index);
	typedef void(__thiscall* create_player_t)(c_simulation_world*, datum);
	INVOKE_TYPE(0x1DC05C, 0x1C3511, create_player_t, this, player_index);
	return;
}

void c_simulation_world::delete_player(datum player_index)
{
	typedef void(__thiscall* delete_player_t)(c_simulation_world*, datum);
	INVOKE_TYPE(0x1DC124, 0x1C35D8, delete_player_t, this, player_index);
	return;
}


void simulation_world_apply_patches(void)
{
	DETOUR_ATTACH(p_c_simulation_world__initialize_world, Memory::GetAddress<t_c_simulation_world__initialize_world>(0x1DDB4E, 0x1C500E), jmp_initialize_world);
	DETOUR_ATTACH(p_c_simulation_world__destroy_world, Memory::GetAddress<t_c_simulation_world__destroy_world>(0x1DE0A9, 0x1C5569), jmp_destroy_world);

	PatchCall(Memory::GetAddress(0x1AE82A, 0x1A8A84), jmp_reset_world);
	PatchCall(Memory::GetAddress(0x1DD9FB, 0x1C4EBB), jmp_send_player_acknowledgements_not_during_simulation_reset_in_progress);
	return;
}
