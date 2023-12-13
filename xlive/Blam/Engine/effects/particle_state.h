#pragma once
#include "particle.h"
#include "particle_location.h"
#include "particle_system.h"


enum e_particle_state_flags : uint32 {
	_particle_update_flag_0 = FLAG(0),
	_particle_update_flag_1 = FLAG(1),
	_particle_update_flag_2 = FLAG(2),
	_particle_update_flag_3 = FLAG(3),
	_particle_update_flag_4 = FLAG(4),
	_particle_update_flag_5 = FLAG(5),
	_particle_update_flag_6 = FLAG(6),
	_particle_update_flag_7 = FLAG(7),
	_particle_update_flag_8 = FLAG(8),
	_particle_update_flag_9 = FLAG(9),
	_particle_update_flag_10 = FLAG(10),
	_particle_update_flag_11 = FLAG(11),
	_particle_update_flag_12 = FLAG(12),
	_particle_update_flag_13 = FLAG(13),
	_particle_update_flag_14 = FLAG(14),
	_particle_update_flag_15 = FLAG(15),
	_particle_update_flag_16 = FLAG(16),
	_particle_update_flag_17 = FLAG(17),
	_particle_update_flag_18 = FLAG(18),
	_particle_update_flag_19 = FLAG(19),
	_particle_update_flag_20 = FLAG(20),
	_particle_update_flag_21 = FLAG(21),
	_particle_update_flag_22 = FLAG(22),
	_particle_update_flag_23 = FLAG(23),
	_particle_update_flag_24 = FLAG(24),
	_particle_update_flag_25 = FLAG(25),
	_particle_update_flag_26 = FLAG(26),
	_particle_update_flag_27 = FLAG(27),
	_particle_update_flag_28 = FLAG(28),
	_particle_update_flag_29 = FLAG(29),
	_particle_update_flag_30 = FLAG(30),
	_particle_update_flag_31 = FLAG(31)
};

struct s_particle_state
{
	int8 data[68];
	e_particle_state_flags flags;
	c_particle_system* particle_system;
	c_particle_location* particle_location;
	c_particle* particle;
};

void __cdecl particle_state_update(uint32 flags, c_particle_system* particle_system, c_particle_location* particle_location, c_particle* a4, s_particle_state* particle_state);
