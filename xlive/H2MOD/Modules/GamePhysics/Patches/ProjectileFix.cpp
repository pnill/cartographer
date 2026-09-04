#include "stdafx.h"
#include "ProjectileFix.h"

#include "cache/cache_files.h"
#include "game/game_time.h"
#include "objects/objects.h"

#include "items/projectile_definition.h"

#include "math/matrix_math.h"

#define PROJECTILE_DEFAULT_OBJECT_DATA_SIZE 428

// h3 gets the projectile collision data from somewhere else
#define ENABLE_H3_COLLISION_DATA_SOURCE false
#define INCREASED_PROJECTILE_OBJECT_DATA_SIZE (PROJECTILE_DEFAULT_OBJECT_DATA_SIZE + sizeof(int32) + (ENABLE_H3_COLLISION_DATA_SOURCE ? sizeof(real_point3d) : 0))

typedef bool(__cdecl* projectile_new_t)(datum projectile_object_index, uint8* parent_object);
projectile_new_t p_projectile_new;

// tick_length parrameter is added extra
// and accessed by get_tick_length_hook()
typedef void(__cdecl* projectile_update_t)(datum projectile_object_index, real_point3d *a2, real32 tick_length);
projectile_update_t p_projectile_update;

// determines whether the projectile should be updated in a 30hz context or not
real32 projectile_get_update_tick_length(datum projectile_index, bool projectile_instant_update)
{
	object_datum const* object = object_get(projectile_index);
	struct projectile_definition const* projectile_definition = projectile_definition_get(object->definition_index);

	if (TEST_BIT(projectile_definition->projectile.flags, _projectile_travels_instantaneously_bit) // check if travels instantaneously flag is set in the projectile flags
		&& (projectile_instant_update || *(uint32*)(((uint8*)object) + 428) == game_time_get())) // also check if the projectile is updated twice in the same tick
	{
		//LIMITED_LOG(128, LOG_TRACE_GAME, "{} - projectile: {:X} at 30 hz context", __FUNCTION__, projectile_index);
		return game_tick_length() * ((real32)game_tick_rate() / 30.f);
	}
	else
	{
		//LIMITED_LOG(128, LOG_TRACE_GAME, "{} - projectile: {:X} at {} hz context", __FUNCTION__, projectile_datum_index, s_time_globals::get_tickrate());
		return game_tick_length();
	}
}

// sets the tick when the projectile has been created
inline void projectile_set_creation_tick(datum projectile_datum_index)
{
	uint8* object_data = (uint8*)object_get(projectile_datum_index);
	*(uint32*)(object_data + 428) = game_time_get(); // store the projectile creation tick count
}

bool __cdecl projectile_new_hook(datum projectile_index, uint8* parent_object)
{
	bool ret = p_projectile_new(projectile_index, parent_object);

	projectile_set_creation_tick(projectile_index);

	return ret;
}

void __cdecl projectile_update_instantaneous(datum projectile_index, real_point3d *a2)
{
	real32 tick_length = projectile_get_update_tick_length(projectile_index, true);
	// LOG_TRACE_GAME("projectile_update_instantaneous() - projectile obj index: {:X}, tick length: {}", projectile_index, tick_length);
	p_projectile_update(projectile_index, a2, tick_length);
}

void __cdecl projectile_update_regular(datum projectile_index, real_point3d *a2)
{
	real32 tick_length = projectile_get_update_tick_length(projectile_index, false);
	// LOG_TRACE_GAME("projectile_update_regular() - projectile obj index: {:X}, tick length: {}", projectile_index, tick_length);
	p_projectile_update(projectile_index, a2, tick_length);
}

static real32 __declspec(naked) get_tick_length_hook()
{
#define _stack_pointer_offset 120h + 4h
#define _last_param_offset 4h + 8h
	__asm
	{
		fld dword ptr [esp + _stack_pointer_offset + _last_param_offset]
		ret
	}
#undef _stack_pointer_offset
#undef _last_param_offset
}

// we still keep this because the fix above doen't fully fix it

const char* tag_names[]
{
	"objects\\weapons\\rifle\\sniper_rifle\\projectiles\\sniper_bullet",
	"objects\\weapons\\rifle\\beam_rifle\\projectiles\\beam_rifle_beam",
	"objects\\vehicles\\warthog\\turrets\\gauss\\weapon\\gauss_bullet"
};

datum trigger_projectile_datum_index = NONE;

#pragma region H3 collision data research
__declspec(naked) void update_projectile_collision_data()
{
	__asm
	{
		mov ax, [esi + 0x46]
		mov [ebx + 0x154], ax
		mov eax, [esi + 8]
		mov [ebx + 432], eax
		mov eax, [esi + 12]
		mov [ebx + 436], eax
		mov eax, [esi + 16]
		mov [ebx + 440], eax

		retn
	}
}

__declspec(naked) void matrix4x3_transform_point_parameter_replacement()
{
	__asm
	{
		lea edx, dword ptr[esi + 432]
		retn
	}
}

void __cdecl object_get_origin(unsigned __int16 unit_index, real_point3d *a3)
{
	auto p_object_get_origin = Memory::GetAddressRelative<void(__cdecl*)(unsigned __int16, real_point3d*)>(0x5329B8);

	trigger_projectile_datum_index = unit_index;

	return p_object_get_origin(unit_index, a3);
}

void __cdecl matrix4x3_transform_point_projectile_hook(real_matrix4x3* matrix, real_point3d* p1, real_point3d* out)
{
	uint8* projectile = (uint8*)object_get(trigger_projectile_datum_index);

	LOG_TRACE_GAME(L" projectile matrix4x3_transform_point() - original point p1: i: {}, j: {}, k: {}", p1->x, p1->y, p1->z);

	p1 = (real_point3d*)(projectile + 432);

	LOG_TRACE_GAME(L" projectile matrix4x3_transform_point() - new vector     p1: i: {}, j: {}, k: {}", p1->x, p1->y, p1->z);

	matrix4x3_transform_point(matrix, p1, out);
}
#pragma endregion

void ProjectileFix::ApplyProjectileVelocity()
{
	for (uint32 i = 0; i < ARRAYSIZE(tag_names); i++)
	{
		datum proj_index = tag_loaded(_tag_group_projectile, tag_names[i]);

		if (proj_index != NONE)
		{
			struct projectile_definition* projectile_definition = projectile_definition_get(proj_index);

			projectile_definition->projectile.initial_velocity *= 2;
			projectile_definition->projectile.final_velocity *= 2;
		}
	}
	return;
}

void ProjectileFix::ApplyPatches()
{
	// increase projectile game object data size to store the elapsed tick time when created
	WriteValue<uint16>(Memory::GetAddress(0x41EE58, 0x3C2368) + 0x8, INCREASED_PROJECTILE_OBJECT_DATA_SIZE);

	// hook projectile_create
	DETOUR_ATTACH(p_projectile_new, Memory::GetAddress<projectile_new_t>(0x146ECE, 0x171E6B), projectile_new_hook);

	// replace update_projectile
	p_projectile_update = (projectile_update_t)(Memory::GetAddress(0x1493F2, 0x174392));
	PatchCall(Memory::GetAddress(0x14A30D, 0x1752AD), projectile_update_regular);
	PatchCall(Memory::GetAddress(0x14A25B, 0x1751FB), projectile_update_instantaneous);

	PatchCall(Memory::GetAddress(0x149442, 0x1743E2), get_tick_length_hook);

#if	ENABLE_H3_COLLISION_DATA_SOURCE
	Codecave(Memory::GetAddressRelative(0x547BDA, 0x572B77), update_projectile_collision_data, 6);
	Codecave(Memory::GetAddressRelative(0x55D66E, 0x54192E), matrix4x3_transform_point_parameter_replacement, 2);
	PatchCall(Memory::GetAddressRelative(0x55D653), object_get_origin);
	PatchCall(Memory::GetAddressRelative(0x55D67E), matrix4x3_transform_point_projectile_hook);
#endif
}
