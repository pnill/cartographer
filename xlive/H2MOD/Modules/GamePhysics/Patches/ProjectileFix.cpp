#include "ProjectileFix.h"

#include "Blam/Math/BlamMath.h"
#include "Blam/Engine/Game/GameTimeGlobals.h"
#include "Blam/Engine/Objects/Objects.h"

#include "H2MOD/Tags/TagInterface.h"

#include "Util/Hooks/Hook.h"

#include <float.h>
#pragma fenv_access (on)

#define DEFAULT_PROJECTILE_OBJECT_DATA_SIZE 428

// h3 gets the projectile collision data from somewhere else
#define ENABLE_H3_COLLISION_DATA_SOURCE 0
#define INCREASED_PROJECTILE_OBJECT_DATA_SIZE (DEFAULT_PROJECTILE_OBJECT_DATA_SIZE + sizeof(int) + (ENABLE_H3_COLLISION_DATA_SOURCE ? sizeof(real_point3d) : 0))

// game logic is updated synchronously, this shouldn't cause any issues
static float tick_length;
float HitFix_Projectile_Tick_Rate = 30.f;

typedef char(__cdecl* projectile_new_def)(unsigned __int16 projectile_object_index, int a2);
projectile_new_def p_projectile_new;

typedef void(__cdecl* projectile_update_def)(datum projectile_object_index, real_point3d *a2);
projectile_update_def p_projectile_update;

// determines whether the projectile should be updated in a 30hz context or not
void projectile_set_tick_length_context(datum projectile_datum_index, bool projectile_instant_update)
{
	char* object_data = (char*)object_get_fast_unsafe(projectile_datum_index);
	char* proj_tag_data = tags::get_tag_fast<char>(*((datum*)object_data));

	if (*(DWORD*)(proj_tag_data + 0xBC) & FLAG(5) // check if travels instantaneously flag is set in the projectile flags
		&& (projectile_instant_update || *(DWORD*)(object_data + 428) == time_globals::get()->tick_count)) // also check if the projectile is updated twice in the same tick
	{
		//LOG_TRACE_GAME("{} - projectile: {:X} at 30 hz context", __FUNCTION__, projectile_datum_index);
		tick_length = time_globals::get_seconds_per_tick() * ((float)time_globals::get()->ticks_per_second / HitFix_Projectile_Tick_Rate);
	}
	else
	{
		//LOG_TRACE_GAME("{} - projectile: {:X} at {} hz context", __FUNCTION__, projectile_datum_index, time_globals::get()->ticks_per_second);
		tick_length = time_globals::get_seconds_per_tick();
	}
}

// sets the tick when the projectile has been created
inline void projectile_set_creation_tick(datum projectile_datum_index)
{
	char* object_data = (char*)object_get_fast_unsafe(projectile_datum_index);
	*(DWORD*)(object_data + 428) = time_globals::get()->tick_count; // store the projectile creation tick count
}

bool __cdecl projectile_new(unsigned __int16 projectile_object_index, int a2)
{
	char ret = p_projectile_new(projectile_object_index, a2);

	projectile_set_creation_tick(projectile_object_index);

	return ret;
}

void __cdecl projectile_update_instantaneous(datum projectile_object_index, real_point3d *a2)
{
	projectile_set_tick_length_context(projectile_object_index, true);
	//LOG_TRACE_GAME("projectile_update_instantaneous() - projectile obj index: {:X}, tick length: {}", projectile_object_index, tick_length);
	p_projectile_update(projectile_object_index, a2);
}

void __cdecl projectile_update_regular(datum projectile_object_index, real_point3d *a2)
{
	projectile_set_tick_length_context(projectile_object_index, false);
	//LOG_TRACE_GAME("projectile_update_regular() - projectile obj index: {:X}, tick length: {}", projectile_object_index, tick_length);
	p_projectile_update(projectile_object_index, a2);
}

float __cdecl get_tick_length_hook()
{
	return tick_length;
}

// we still keep this because the fix above doen't fully fix it
// object string, initial bullet speed, final bullet speed
std::vector<std::tuple<std::string, float, float>> weapon_projectiles =
{
	//std::make_tuple("objects\\weapons\\rifle\\battle_rifle\\projectiles\\battle_rifle_bullet", 400.f * 2, 400.f * 2),
	//std::make_tuple("objects\\weapons\\rifle\\covenant_carbine\\projectiles\\carbine_slug\\carbine_slug", 400.f * 2, 400.f * 2),
	std::make_tuple("objects\\weapons\\rifle\\sniper_rifle\\projectiles\\sniper_bullet", 1200.0f * 2.f, 1200.0f * 2.f),
	std::make_tuple("objects\\weapons\\rifle\\beam_rifle\\projectiles\\beam_rifle_beam", 1200.0f * 2.f, 1200.0f * 2.f),
	std::make_tuple("objects\\vehicles\\warthog\\turrets\\gauss\\weapon\\gauss_bullet", 90.f * 2.f, 90.f * 2.f),
	//std::make_tuple("objects\\weapons\\pistol\\magnum\\projectiles\\magnum_bullet", 400.f * 2, 400.f * 2),
	//std::make_tuple("objects\\vehicles\\warthog\\turrets\\chaingun\\weapon\\bullet", 2000.0f, 2000.0f)
};

datum trigger_projectile_datum_index = DATUM_NONE;

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

void __cdecl matrix4x3_transform_point(void* matrix, real_vector3d* v1, real_vector3d* out)
{
	auto p_matrix4x3_transform_point = Memory::GetAddressRelative<void(__cdecl*)(void*, real_vector3d*, real_vector3d*)>(0x47795A);

	BYTE* projectile = (BYTE*)object_get_fast_unsafe(trigger_projectile_datum_index);

	LOG_TRACE_GAME(L" projectile matrix4x3_transform_point() - original values v1: i: {}, j: {}, k: {}", v1->i, v1->j, v1->k);

	v1 = (real_vector3d*)(projectile + 432);

	LOG_TRACE_GAME(L" projectile matrix4x3_transform_point() - new values v1: i: {}, j: {}, k: {}", v1->i, v1->j, v1->k);

	p_matrix4x3_transform_point(matrix, v1, out);
}
#pragma endregion

void ProjectileFix::ApplyProjectileVelocity()
{
	for (auto& proj_tuple : weapon_projectiles)
	{
		auto proj_datum = tags::find_tag(blam_tag::tag_group_type::projectile, std::get<0>(proj_tuple));
		BYTE* projectile_tag_data = tags::get_tag<blam_tag::tag_group_type::projectile, BYTE>(proj_datum);
		if (projectile_tag_data != nullptr)
		{
			*(float*)(projectile_tag_data + 380) = std::get<1>(proj_tuple);
			*(float*)(projectile_tag_data + 384) = std::get<2>(proj_tuple);
		}
	}
}

void ProjectileFix::ApplyPatches()
{
	// increase projectile game object data size to store the elapsed tick time when created
	WriteValue<unsigned short>(Memory::GetAddress(0x41EE58, 0x3C2368) + 0x8, INCREASED_PROJECTILE_OBJECT_DATA_SIZE);

	// hook projectile_create
	p_projectile_new = (projectile_new_def)DetourFunc(Memory::GetAddress<BYTE*>(0x146ECE, 0x171E6B), (BYTE*)projectile_new, 5);

	// replace update_projectile
	p_projectile_update = (projectile_update_def)(Memory::GetAddress(0x1493F2, 0x174392));
	PatchCall(Memory::GetAddress(0x14A30D, 0x1752AD), projectile_update_regular);
	PatchCall(Memory::GetAddress(0x14A25B, 0x1751FB), projectile_update_instantaneous);

	PatchCall(Memory::GetAddress(0x149442, 0x1743E2), get_tick_length_hook);

#if	ENABLE_H3_COLLISION_DATA_SOURCE
	Codecave(Memory::GetAddressRelative(0x547BDA, 0x572B77), update_projectile_collision_data, 6);
	Codecave(Memory::GetAddressRelative(0x55D66E, 0x54192E), matrix4x3_transform_point_parameter_replacement, 2);
#endif

	//PatchCall(Memory::GetAddressRelative(0x55D653), object_get_origin);
	//PatchCall(Memory::GetAddressRelative(0x55D67E), matrix4x3_transform_point);
}
