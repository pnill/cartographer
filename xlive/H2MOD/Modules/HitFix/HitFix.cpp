#include "Hitfix.h"

#include "H2MOD.h"
#include "H2MOD\Tags\TagInterface.h"

#include "Util/Hooks/Hook.h"

#include "Globals.h"

// TODO: move the struct time_globals in here somewhere else
#include "H2MOD/Modules/MainLoopPatches/UncappedFPS/UncappedFPS.h"

// game logic is updated synchronously, this shouldn't cause any issues
// TODO: find if there is any way to add function arguments in a compiled function lol
datum projectileToBeUpdated;
bool updateInstantaneousProjectile = false;

typedef void(__cdecl* projectile_update_def)(datum projectile_object_index, real_point3d *a2);
projectile_update_def p_projectile_update;

typedef char(__cdecl* projectile_new_def)(unsigned __int16 projectile_object_index, int a2);
projectile_new_def p_projectile_new;

char __cdecl projectile_new(unsigned __int16 projectile_object_index, int a2)
{
	char ret = p_projectile_new(projectile_object_index, a2);

	ObjectHeader* objects_header = (ObjectHeader*)game_state_objects_header->datum;
	char* object_data = objects_header[projectile_object_index].object;
	*(DWORD*)(object_data + 428) = time_globals::get_game_time_globals()->tick_count; // store the projectile creation tick count

	return ret;
}

void __cdecl projectile_update_instantaneous(datum projectile_object_index, real_point3d *a2)
{
	updateInstantaneousProjectile = true;
	projectileToBeUpdated = projectile_object_index;
	LOG_TRACE_GAME("projectile_update_instantaneous() - projectile obj index: {}, just making sure: {}", projectileToBeUpdated.ToAbsoluteIndex(), *(int*)&projectile_object_index & 0xFFFF);
	p_projectile_update(projectile_object_index, a2);
}

void __cdecl projectile_update_regular(datum projectile_object_index, real_point3d *a2)
{
	updateInstantaneousProjectile = false;
	projectileToBeUpdated = projectile_object_index;
	LOG_TRACE_GAME("projectile_update_regular() - projectile obj index: {}, just making sure: {}", projectileToBeUpdated.ToAbsoluteIndex(), *(int*)&projectile_object_index & 0xFFFF);
	p_projectile_update(projectile_object_index, a2);
}

// this will get executed when p_projectile_update is called
void get_seconds_per_tick_internal_patch()
{
	ObjectHeader* objects_header = (ObjectHeader*)game_state_objects_header->datum;
	char* object_data = objects_header[projectileToBeUpdated.ToAbsoluteIndex()].object;
	char* proj_tag_data = (char*)tags::get_tag(*((datum*)object_data));
	time_globals* p_time_globals = time_globals::get_game_time_globals();

	float timeDelta = p_time_globals->seconds_per_tick;
	if (*(DWORD*)(proj_tag_data + 0xBC) & FLAG(5) // check if travels instantaneously flag is set in the projectile flags
		&& (updateInstantaneousProjectile || *(DWORD*)(object_data + 428) == p_time_globals->tick_count))
	{
		LOG_TRACE_GAME("get_time_delta_internal() - projectile obj index: {}, projectile creation tick: {}, current tick count {}", projectileToBeUpdated.ToAbsoluteIndex(), *(DWORD*)(object_data + 428), p_time_globals->tick_count);
		timeDelta = timeDelta * (float)((float)p_time_globals->ticks_per_second / 30.0f);
	}
	__asm
	{
		fld timeDelta
	}
}

__declspec(naked) void get_seconds_per_tick()
{
	__asm
	{
		PUSHAD
		PUSHFD

		call get_seconds_per_tick_internal_patch

		POPFD
		POPAD
		ret
	}
}

void HitFix::ApplyPatches()
{
	// increase projectile game object data size to store the elapsed tick time when created
	WriteValue<unsigned short>(h2mod->GetAddress(0x41EE58, 0x3C2368) + 0x8, 428 + 4);

	// hook projectile_create
	p_projectile_new = (projectile_new_def)DetourFunc(h2mod->GetAddress<BYTE*>(0x146ECE, 0x171E6B), (BYTE*)projectile_new, 5);

	// replace update_projectile
	p_projectile_update = (projectile_update_def)(h2mod->GetAddress(0x1493F2, 0x174392));
	PatchCall(h2mod->GetAddress(0x14A30D, 0x1752AD), projectile_update_regular);
	PatchCall(h2mod->GetAddress(0x14A25B, 0x1751FB), projectile_update_instantaneous);

	PatchCall(h2mod->GetAddress(0x149442, 0x1743E2), get_seconds_per_tick);
}
