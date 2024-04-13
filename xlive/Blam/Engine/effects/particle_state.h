#pragma once
#include "particle.h"
#include "particle_location.h"
#include "particle_system.h"

#define k_particle_state_values_count 17

enum e_particle_state_flags : uint32 
{
	_particle_update_bit_0 = 0,
	_particle_update_bit_1 = 1,
	_particle_update_bit_2 = 2,
	_particle_update_bit_3 = 3,
	_particle_update_bit_4 = 4,
	_particle_update_bit_5 = 5,
	_particle_update_bit_6 = 6,
	_particle_update_bit_7 = 7,
	_particle_update_bit_8 = 8,
	_particle_update_bit_9 = 9,
	_particle_update_bit_10 = 10,
	_particle_update_bit_11 = 11,
	_particle_update_bit_12 = 12,
	_particle_update_bit_13 = 13,
	_particle_update_bit_14 = 14,
	_particle_update_bit_15 = 15,
	_particle_update_bit_16 = 16,
	_particle_update_bit_17 = 17,
	_particle_update_bit_18 = 18,
	_particle_update_bit_19 = 19,
	_particle_update_bit_20 = 20,
	_particle_update_bit_21 = 21,
	_particle_update_bit_22 = 22,
	_particle_update_bit_23 = 23,
	_particle_update_bit_24 = 24,
	_particle_update_bit_25 = 25,
	_particle_update_bit_26 = 26,
	_particle_update_bit_27 = 27,
	_particle_update_bit_28 = 28,
	_particle_update_bit_29 = 29,
	_particle_update_bit_30 = 30,
	_particle_update_bit_31 = 31,
	k_particle_state_flags
};

void __cdecl particle_state_update(uint32 flags, c_particle_system* particle_system, c_particle_location* particle_location, c_particle* particle, s_particle_state* particle_state, int32 state_values_count);

struct s_particle_state
{
	real32 m_states[k_particle_state_values_count];
	struct
	{
		c_flags<e_particle_state_flags, uint32, k_particle_state_flags> flags;
		c_particle_system* particle_system;
		c_particle_location* particle_location;
		c_particle* particle;
	} m_system;

	s_particle_state()
	{
		csmemset(&m_system, 0, sizeof(m_system));
	}

	~s_particle_state() = default;

	void set_particle_system(c_particle_system* particle_system)
	{
		if (m_system.particle_system != particle_system)
		{
			m_system.flags.set(_particle_update_bit_4, false);
			m_system.flags.set(_particle_update_bit_5, false);
			m_system.flags.set(_particle_update_bit_6, false);
			m_system.flags.set(_particle_update_bit_9, false);
			m_system.flags.set(_particle_update_bit_10, false);
			m_system.particle_system = particle_system;
		}
	}

	void set_particle_location(c_particle_location* particle_location)
	{
		if (m_system.particle_location != particle_location)
		{
			m_system.flags.set(_particle_update_bit_7, false);
			m_system.flags.set(_particle_update_bit_12, false);
			m_system.flags.set(_particle_update_bit_13, false);
			m_system.flags.set(_particle_update_bit_16, false);
			m_system.particle_location = particle_location;
		}
	}

	void set_particle(c_particle* particle)
	{
		if (m_system.particle != particle)
		{
			m_system.flags.set(_particle_update_bit_0, false);
			m_system.flags.set(_particle_update_bit_1, false);
			m_system.flags.set(_particle_update_bit_2, false);
			m_system.flags.set(_particle_update_bit_3, false);
			m_system.flags.set(_particle_update_bit_11, false);
			m_system.flags.set(_particle_update_bit_12, false);
			m_system.flags.set(_particle_update_bit_13, false);
			m_system.flags.set(_particle_update_bit_14, false);
			m_system.flags.set(_particle_update_bit_15, false);
			m_system.particle = particle;
		}
	}

	void state_update(uint32 flags)
	{
		particle_state_update(
			~m_system.flags & flags,
			m_system.particle_system,
			m_system.particle_location,
			m_system.particle,
			this,
			k_particle_state_values_count
		);
		m_system.flags |= flags;
	}
};
CHECK_STRUCT_SIZE(s_particle_state, 84);
