#include "stdafx.h"
#include "simulation_world.h"

#include "game/game.h"
#include "shell/shell_windows.h"

// TODO verify if these buffers get saturated quickly
// if that's the case, increse the buffer size
c_simulation_queue g_entity_update_queue;
c_simulation_queue g_event_update_queue;

void c_simulation_world::simulation_queue_allocate(e_event_queue_type type, int32 size, s_simulation_queue_element** out_allocated_elem)
{
	if (TEST_FLAG(FLAG(type), _simulation_queue_element_type_1))
	{
		// player event, player update, gamestate clear
		g_event_update_queue.allocate(size, out_allocated_elem);
	}
	else
	{
		bool sim_entity_queue_full = false;

		if (TEST_FLAG(FLAG(type), _simulation_queue_element_type_2))
		{
			// entity_deletion, entity_promotion, game_global_event

			// TODO
			// c_simulation_queue::get_allocation_status()
			// check the allocation status
			// this type of elements can be skupped from being queued, in case of overflow??
			// sim_entity_queue_full = true;
		}

		// event, creation, update, entity_deletion, entity_promotion, game_global_event
		if (!sim_entity_queue_full)
			g_entity_update_queue.allocate(size, out_allocated_elem);
	}

	if (*out_allocated_elem)
	{
		(*out_allocated_elem)->type = type;
	}
}

void c_simulation_world::simulation_queue_enqueue(s_simulation_queue_element* element)
{
	if (TEST_FLAG(FLAG(element->type), _simulation_queue_element_type_1))
	{
		// player event, player update, gamestate clear
		g_event_update_queue.enqueue(element);

		SIM_QUEUE_DBG("queue 0x%08X allocated count: %d, size: %d",
			&g_event_update_queue, 
			g_event_update_queue.allocated_count(), 
			g_event_update_queue.allocated_size_in_bytes());

		SIM_QUEUE_DBG("queue 0x%08X queued count: %d, size: %d", 
			&g_event_update_queue, 
			g_event_update_queue.queued_count(), 
			g_event_update_queue.queued_size());
	}
	else
	{
		// event, creation, update, entity_deletion, entity_promotion, game_global_event
		g_entity_update_queue.enqueue(element);

		SIM_QUEUE_DBG("queue 0x%08X allocated count: %d, size: %d",
			&g_entity_update_queue,
			g_entity_update_queue.allocated_count(),
			g_entity_update_queue.allocated_size_in_bytes());

		SIM_QUEUE_DBG("queue 0x%08X queued count: %d, size: %d", &g_entity_update_queue, g_entity_update_queue.queued_count(), g_entity_update_queue.queued_size());
	}
}

void c_simulation_world::apply_entity_update_queue()
{
	apply_simulation_queue(&g_entity_update_queue);
}

void c_simulation_world::apply_event_update_queue()
{
	apply_simulation_queue(&g_event_update_queue);
}

void c_simulation_world::apply_simulation_queue(c_simulation_queue* queue)
{
	static long long last_time_msec;
	static void* last_queue;

	if (shell_time_now_msec() - last_time_msec > 1000)
	{
		/*<SIM_QUEUE_DBG(
			"queue 0x%08X, initialized: %d queued count: %d, size: %d",
			queue,
			queue->initialized(),
			queue->queued_count(),
			queue->queued_size());*/
	}

	if (queue->queued_count() > 0)
	{
		const s_simulation_queue_element* element = queue->get_first_element();

		while (element != NULL)
		{
			SIM_QUEUE_DBG("appying element: %08X, type: %d to gamestate",
				element,
				element->type);

			switch (element->type)
			{
			case _simulation_queue_element_type_event:
				simulation_event_queue_apply(element);
				break;
			case _simulation_queue_element_type_entity_creation:
				break;
			case _simulation_queue_element_type_entity_update:
				break;
			case _simulation_queue_element_type_entity_deletion:
				break;
			case _simulation_queue_element_type_entity_promotion:
				break;
			case _simulation_queue_element_type_game_global_event:
				break;
			case _simulation_queue_element_type_player_event:
				break;
			case _simulation_queue_element_type_player_update_event:
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

	if (shell_time_now_msec() - last_time_msec > 1000)
	{
		last_time_msec = shell_time_now_msec();
	}
}

void c_simulation_world::destroy_update()
{
	c_simulation_queue::dispose(&g_event_update_queue);
	c_simulation_queue::dispose(&g_entity_update_queue);
}

void c_simulation_world::queues_initialize()
{
	g_event_update_queue.initialize();
	g_entity_update_queue.initialize();
}
