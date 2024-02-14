#include "stdafx.h"
#include "simulation_world.h"

#include "simulation_queue_events.h"
#include "simulation_queue_entities.h"
#include "simulation_queue_global_events.h"

#include "game/game.h"
#include "saved_games/game_state_procs.h"
#include "shell/shell_windows.h"


// TODO verify if these buffers get saturated quickly
// if that's the case, increse the buffer size
c_simulation_queue g_simulation_queues[k_simulation_queue_count];


void c_simulation_world::gamestate_flush(void) const
{
	if (!is_authority())
	{
		game_state_call_before_save_procs(0);
		game_state_call_after_save_procs(0);
	}
	return;
}

c_simulation_queue* c_simulation_world::queue_get(e_simulation_queue_type type)
{
	return &g_simulation_queues[type];
}

void c_simulation_world::simulation_queue_allocate(e_event_queue_type type, int32 size, s_simulation_queue_element** out_allocated_elem)
{
	*out_allocated_elem = NULL;

	if (TEST_FLAG(FLAG(type), _simulation_queue_element_type_1))
	{
		// player event, player update, gamestate clear
		queue_get(_simulation_queue_bookkeeping)->allocate(size, out_allocated_elem);
	}
	else
	{
		bool sim_entity_queue_full = false;

		if (TEST_FLAG(FLAG(type), _simulation_queue_element_type_2))
		{
			// entity_deletion, entity_promotion, game_global_event

			// ### TODO FIXME implement this
			// c_simulation_queue::get_allocation_status()
			// check the allocation status
			// this type of elements can be skupped from being queued, in case of overflow??
			// sim_entity_queue_full = true;
		}

		// event, creation, update, entity_deletion, entity_promotion, game_global_event
		if (!sim_entity_queue_full)
			queue_get(_simulation_queue)->allocate(size, out_allocated_elem);
	}

	if (*out_allocated_elem)
	{
		(*out_allocated_elem)->type = type;
	}
}

void c_simulation_world::simulation_queue_free(s_simulation_queue_element* element)
{
	if (TEST_FLAG(FLAG(element->type), _simulation_queue_element_type_1))
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
	if (TEST_FLAG(FLAG(element->type), _simulation_queue_element_type_1))
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
			g_simulation_queues[_simulation_queue_bookkeeping].queued_size());
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
			g_simulation_queues[_simulation_queue].queued_size());
	}
}

void c_simulation_world::simulation_apply_queued_elements(simulation_update* update)
{
	apply_simulation_queue(queue_get(_simulation_queue), update);
}

void c_simulation_world::simulation_apply_bookkeeping_queue(simulation_update* update)
{
	apply_simulation_queue(queue_get(_simulation_queue_bookkeeping), update);
}

void c_simulation_world::apply_simulation_queue(const c_simulation_queue* queue, simulation_update* update)
{
	if (queue->queued_count() > 0)
	{
		const s_simulation_queue_element* element = queue->get_first_element();

		while (element != NULL)
		{
			SIM_EVENT_QUEUE_DBG("appying element: %08X, type: %d to gamestate",
				element,
				element->type);

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
				simulation_queue_game_global_event_apply(element, update);
				break;
			case _simulation_queue_element_type_player_event:
				simulation_queue_player_event_apply(element);
				break;
			case _simulation_queue_element_type_player_update_event:
				if (!simulation_get_globals()->fatal_error)
					simulation_queue_player_update_apply(element);
				break;
			case _simulation_queue_element_type_gamestates_clear:
				break;
			//case _simulation_queue_element_type_sandbox_event:
				//break;
			default:
				// DEBUG unk event type
				break;
			}

			element = queue->get_next_element(element);
		}
	}
}

void c_simulation_world::destroy_update()
{
	for (int32 i = 0; i < k_simulation_queue_count; i++)
	{
		queue_get((e_simulation_queue_type)i)->clear();
	}
}

typedef void(__thiscall* t_c_simulation_world__initialize_world)(c_simulation_world*, int32, int32, int32);
t_c_simulation_world__initialize_world p_c_simulation_world__initialize_world;

void c_simulation_world::initialize_world(int32 a2, int32 a3, int32 a4)
{
	p_c_simulation_world__initialize_world(this, a2, a3, a4);
	if (!is_playback())
	{
		queues_initialize();
	}
}

void __declspec(naked) jmp_initialize_world() { __asm { jmp c_simulation_world::initialize_world } }

typedef void(__thiscall* t_c_simulation_world__reset)(c_simulation_world*);
t_c_simulation_world__reset p_c_simulation_world__reset;

void c_simulation_world::reset()
{
	p_c_simulation_world__reset(this);

	if (!is_playback())
	{
		destroy_update();
	}
}

__declspec(naked) void jmp_reset_world() { __asm { jmp c_simulation_world::reset } }

typedef void(__thiscall* t_c_simulation_world__destroy_world)(c_simulation_world*);
t_c_simulation_world__destroy_world p_c_simulation_world__destroy_world;

void c_simulation_world::destroy_world()
{
	// call orig
	p_c_simulation_world__destroy_world(this);

	if (!is_playback())
	{
		queues_dispose();
	}
}

void __declspec(naked) jmp_destroy_world() { __asm{ jmp c_simulation_world::destroy_world } }

void c_simulation_world::send_player_acknowledgements_not_during_simulation_reset_in_progress(bool a1)
{
	if (!simulation_reset_in_progress())
	{
		send_player_acknowledgements(a1);
	}
}

void __declspec(naked) jmp_send_player_acknowledgements_not_during_simulation_reset_in_progress() { 
	__asm { jmp c_simulation_world::send_player_acknowledgements_not_during_simulation_reset_in_progress } 
}

void c_simulation_world::queues_initialize()
{
	for (int32 i = 0; i < k_simulation_queue_count; i++)
	{
		queue_get((e_simulation_queue_type)i)->initialize();
	}
}

void c_simulation_world::queues_dispose()
{
	for (int32 i = 0; i < k_simulation_queue_count; i++)
	{
		queue_get((e_simulation_queue_type)i)->dispose();
	}
}

void simulation_world_apply_patches()
{
	DETOUR_ATTACH(p_c_simulation_world__initialize_world, Memory::GetAddress<t_c_simulation_world__initialize_world>(0x1DDB4E, 0x1C500E), jmp_initialize_world);
	DETOUR_ATTACH(p_c_simulation_world__destroy_world, Memory::GetAddress<t_c_simulation_world__destroy_world>(0x1DE0A9, 0x1C5569), jmp_destroy_world);
	DETOUR_ATTACH(p_c_simulation_world__reset, Memory::GetAddress<t_c_simulation_world__reset>(0x1DD0EA, 0x1C459E), jmp_reset_world);

	PatchCall(Memory::GetAddress(0x1DD9FB, 0x1C4EBB), jmp_send_player_acknowledgements_not_during_simulation_reset_in_progress);
	return;
}
