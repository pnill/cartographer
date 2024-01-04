#include "stdafx.h"
#include "particle_system.h"

#include "effects_definitions.h"
#include "particle.h"
#include "particle_emitter.h"
#include "particle_location.h"
#include "particle_model_definitions.h"
#include "particle_system_definition.h"
#include "Blam/Engine/game/game_time.h"
#include "Blam/Engine/physics/breakable_surface_definitions.h"
#include "Blam/Engine/tag_files/global_string_ids.h"
#include "H2MOD/Tags/TagInterface.h"
#include "Util/Hooks/Hook.h"

int particle_system_game_time[k_max_particle_systems];
real32 particle_system_accumulated_time[k_max_particle_systems];

s_data_array* get_particle_system_table()
{
	return *Memory::GetAddress<s_data_array**>(0x4D83D4, 0x4FFEA8);
}

void particle_system_update_update_game_time(datum datum_index)
{
	particle_system_game_time[DATUM_INDEX_TO_ABSOLUTE_INDEX(datum_index)] = time_globals::get_game_time();
}

typedef bool(__stdcall* c_particle_system_update_t)(c_particle_system* thisx, real32 dt);
c_particle_system_update_t p_c_particle_system_update;

const real32 tolerance = 1e-5f;
bool __stdcall c_particle_system::update(c_particle_system* thisx, real32 delta_time)
{
	bool result = p_c_particle_system_update(thisx, delta_time);

	datum particle_system_datum_index = ((char*)thisx - get_particle_system_table()->data) / sizeof(c_particle_system);
	particle_system_datum_index |= (thisx->datum_salt << 16);
	if(!result)
	{
		real32* accum = &particle_system_accumulated_time[DATUM_INDEX_TO_ABSOLUTE_INDEX(particle_system_datum_index)];
		real32 max_accum = ((thisx->duration != 0.0f) ? 1.0f / thisx->duration : 0.0f) + time_globals::get_seconds_per_tick();
		real32 difference = max_accum - *accum;
		int32 ticks = time_globals::get_game_time() - particle_system_game_time[DATUM_INDEX_TO_ABSOLUTE_INDEX(particle_system_datum_index)];
		
		if (difference <= tolerance && ticks >= 1)
		{
			particle_system_game_time[DATUM_INDEX_TO_ABSOLUTE_INDEX(particle_system_datum_index)] = 0;
			*accum = 0.f;
			return false;
		}
		else
		{
			particle_system_accumulated_time[DATUM_INDEX_TO_ABSOLUTE_INDEX(particle_system_datum_index)] += delta_time;
			return true;
		}
	}
	particle_system_accumulated_time[DATUM_INDEX_TO_ABSOLUTE_INDEX(particle_system_datum_index)] += delta_time;
	return  result;
}

typedef void(__stdcall* t_c_particle_system__update_location_time)(c_particle_system*, s_particle_system_update_timings*, real_matrix4x3*, int );
t_c_particle_system__update_location_time p_c_particle_system__update_location_time;

void c_particle_system::update_location_time(c_particle_system* thisx, s_particle_system_update_timings* timings, real_matrix4x3* matrix, int unused)
{
	datum particle_system_datum_index = ((char*)thisx - get_particle_system_table()->data) / sizeof(c_particle_system);
	particle_system_datum_index |= (thisx->datum_salt << 16);
	particle_system_update_update_game_time(particle_system_datum_index);
	p_c_particle_system__update_location_time(thisx, timings, matrix, unused);
}

void __cdecl particle_system_remove_from_effects_cache(datum effect_index, datum particle_system_index)
{
	INVOKE(0xA69EB, 0x98A6B, particle_system_remove_from_effects_cache, effect_index, particle_system_index);
}

void __cdecl c_particle_system::destroy(datum particle_system_index)
{
	INVOKE(0xC3C49, 0xB0B34, c_particle_system::destroy, particle_system_index);
}

void apply_particle_system_patches()
{
	p_c_particle_system_update = (c_particle_system_update_t)DetourClassFunc((uint8*)Memory::GetAddressRelative(0x4C43F9), (uint8*)c_particle_system::update, 10);
	p_c_particle_system__update_location_time = (t_c_particle_system__update_location_time)DetourClassFunc((uint8*)Memory::GetAddress(0xC3E32), (uint8*)c_particle_system::update_location_time, 11);
}
