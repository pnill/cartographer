#include "stdafx.h"
#include "simulation_queue.h"

#include "simulation_queue_global_events.h"

#include "networking/memory/networking_memory.h"

void c_simulation_queue::allocate(int32 data_size, s_simulation_queue_element** out_allocated_elem)
{
	*out_allocated_elem = NULL;

	if (initialized())
	{
		uint32 required_data_size = sizeof(s_simulation_queue_element) + data_size;

		if (allocated_count() + 1 < k_simulation_queue_count_max)
		{
			if (allocated_size_in_bytes() + required_data_size < k_simulation_queue_size_max)
			{
				if (data_size < k_simulation_queue_element_data_size_max)
				{
					if (allocated_new_encoded_size_bytes(data_size) < k_simulation_queue_max_encoded_size)
					{
						uint8* net_heap_block = network_heap_allocate_block(required_data_size);
						if (net_heap_block)
						{
							csmemset(net_heap_block, 0, required_data_size);
							s_simulation_queue_element* allocated_elem = (s_simulation_queue_element*)net_heap_block;
							allocated_elem->type = _simulation_queue_element_type_none;
							allocated_elem->data = net_heap_block + sizeof(s_simulation_queue_element);
							allocated_elem->data_size = data_size;
							allocated_elem->next = NULL;

							m_allocated_count++;
							m_allocated_size_in_bytes += required_data_size;
							*out_allocated_elem = allocated_elem;
						}
						else
						{
							// DEBUG
						}
					}
				}
			}
		}
	}
}

void c_simulation_queue::transfer_elements(c_simulation_queue* source_queue)
{
	// transefrs data from the source into ours

	int32 queued_count = source_queue->queued_count();
	for (int32 i = 0; i < queued_count; i++)
	{
		s_simulation_queue_element* element = NULL;
		source_queue->dequeue(&element);
		source_queue->m_allocated_count--;
		source_queue->m_allocated_size_in_bytes -= get_element_size_in_bytes(element);
		this->m_allocated_count++;
		this->m_allocated_size_in_bytes += get_element_size_in_bytes(element);
		this->enqueue(element);
		// if the element type is a global event
		// check if we need to cut the update here
		if (element->type == _simulation_queue_element_type_game_global_event
			&& simulation_queue_game_global_event_requires_cutoff(element))
		{
			// cut the queue here, keep the rest for the next update
			break;
		}
	}
}

void c_simulation_queue::deallocate(s_simulation_queue_element* element)
{
	if (initialized())
	{
		m_allocated_size_in_bytes -= get_element_size_in_bytes(element);
		m_allocated_count--;
		network_heap_free_block((uint8*)element);
	}
}

void c_simulation_queue::enqueue(s_simulation_queue_element* element)
{
	if (initialized())
	{
		if (m_tail)
		{
			m_tail->next = element;
		}
		else
		{
			m_head = element;
		}

		m_tail = element;
		m_queued_count++;
		m_queued_size += get_element_size_in_bytes(element);
	}
}

void c_simulation_queue::dequeue(s_simulation_queue_element** out_deq_elem)
{
	*out_deq_elem = NULL;
	if (initialized())
	{
		if (m_head)
		{
			m_queued_count--;
			m_queued_size -= get_element_size_in_bytes(m_head);
			*out_deq_elem = m_head;
			m_head = m_head->next;
			(*out_deq_elem)->next = NULL;
		}
		if (m_head == NULL)
		{
			m_tail = NULL;
		}
	}
}

void c_simulation_queue::clear()
{
	if (initialized())
	{
		while (get_head() != NULL)
		{
			s_simulation_queue_element* element_to_deque = NULL;
			dequeue(&element_to_deque);
			deallocate(element_to_deque);
			// ### TODO clear allocated but not queued too??
		}
	}
}

void c_simulation_queue::dispose()
{
	if (initialized())
	{
		clear();
		m_initialized = false;
	}
}
