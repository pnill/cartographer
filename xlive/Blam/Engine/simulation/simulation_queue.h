#pragma once

#include "Networking/memory/networking_memory.h"

#include "H2MOD/GUI/ImGui_Integration/Console/ImGui_ConsoleImpl.h"

#define k_simulation_queue_count_max 4096
#define k_simulation_queue_element_data_size_max 1024
#define k_simulation_queue_avg_payload_size 32
#define k_simulation_queue_size_max (k_simulation_queue_count_max * k_simulation_queue_avg_payload_size)

#define k_simulation_queue_type_encoded_size_in_bits 4
#define k_simulation_queue_payload_encoded_size_in_bits 10
#define k_simulation_queue_header_encoded_size_in_bits (k_simulation_queue_type_encoded_size_in_bits + k_simulation_queue_payload_encoded_size_in_bits)

#define k_simulation_queue_max_encoded_size 59392

enum e_event_queue_type : int16
{
	_simulation_queue_element_type_none,
	_simulation_queue_element_type_event = 1,
	_simulation_queue_element_type_entity_creation = 2,
	_simulation_queue_element_type_entity_update = 3,
	_simulation_queue_element_type_entity_deletion = 4,
	_simulation_queue_element_type_entity_promotion = 5,
	_simulation_queue_element_type_game_global_event = 6,
	_simulation_queue_element_type_player_event = 7,
	_simulation_queue_element_type_player_update_event = 8,
	_simulation_queue_element_type_gamestates_clear = 9,

	// probably unused, forge related
	_simulation_queue_element_type_sandbox_event = 10,

	k_simulation_queue_element_type_count,

	_simulation_queue_element_type_1 = FLAG(_simulation_queue_element_type_player_event) | FLAG(_simulation_queue_element_type_player_update_event) | FLAG(_simulation_queue_element_type_gamestates_clear),
	_simulation_queue_element_type_2 = FLAG(_simulation_queue_element_type_entity_deletion) | FLAG(_simulation_queue_element_type_entity_promotion) | FLAG(_simulation_queue_element_type_game_global_event),
};

struct s_simulation_queue_element
{
	e_event_queue_type type;
	s_simulation_queue_element* next;
	uint8* data;
	uint32 data_size;
};

class c_simulation_queue
{
	bool	m_initialized;
	int32	m_allocated_count;
	int32	m_allocated_size_in_bytes;

	int32	m_queued_count;
	int32	m_queued_size;

	s_simulation_queue_element* m_head;
	s_simulation_queue_element* m_tail;

	void dequeue(s_simulation_queue_element** out_deq_elem);

public:
	c_simulation_queue()
	{
		initialize();
	}

	~c_simulation_queue()
	{
		dispose();
	}

	void initialize()
	{
		m_allocated_count = 0;
		m_allocated_size_in_bytes = 0;
		m_queued_count = 0;
		m_queued_size = 0;
		m_head = NULL;
		m_tail = NULL;
		m_initialized = true;
	}

	bool initialized() const
	{
		return m_initialized;
	}

	const s_simulation_queue_element* get_head() const
	{
		if (initialized())
		{
			return m_head;
		}
		return NULL;
	}

	const s_simulation_queue_element* get_first_element() const
	{
		if (initialized())
		{
			return get_head();
		}
		return NULL;
	}

	const s_simulation_queue_element* get_next_element(const s_simulation_queue_element* element) const
	{
		if (initialized())
		{
			return element->next;
		}
		return NULL;
	}

	int32 allocated_count() const
	{
		if (initialized())
		{
			return m_allocated_count;
		}

		return 0;
	}

	int32 allocated_size_in_bytes() const
	{
		if (initialized())
		{
			return m_allocated_size_in_bytes;
		}

		return 0;
	}

	int32 get_element_size_in_bytes(const s_simulation_queue_element* element) const
	{
		return element->data_size + sizeof(s_simulation_queue_element);
	}

	int32 allocated_encoded_size_bytes() const
	{
		// ### FIXME figure out + 36 in bits
		int32 size = (k_simulation_queue_header_encoded_size_in_bits * allocated_count() + 36) / 8;
		return size + allocated_size_in_bytes() - (sizeof(s_simulation_queue_element) * allocated_count());
	}

	int32 allocated_new_encoded_size_bytes(int32 size) const
	{
		return size + 2 + allocated_encoded_size_bytes();
	}

	int32 queued_count() const
	{
		if (initialized())
		{
			return m_queued_count;
		}

		return 0;
	}
	
	int32 queued_size() const
	{
		if (initialized())
		{
			return m_queued_size;
		}

		return 0;
	}

	void allocate(int32 data_size, s_simulation_queue_element** out_allocated_elem);
	void transfer_elements(c_simulation_queue* source_queue);
	void deallocate(s_simulation_queue_element* element);
	void enqueue(s_simulation_queue_element* element);

	void clear();

	void dispose();
};