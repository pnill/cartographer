#include "stdafx.h"

#include "Interpolation.h"

#include "Util\Hooks\Hook.h"

#include "Blam\Math\BlamMath.h"
#include "Blam\Engine\Game\GameTimeGlobals.h"
#include "Blam\Engine\Players\LocalPlayers.h"

#include "CameraInterpolate.h"

namespace Interpolation
{
	bool enabled = true;
	bool game_state_updating = false;
	bool ticks_executed = false;
	float effect_update_accumulator = 0.0f;

	void PreGameTick()
	{
		game_state_updating = true;
		CameraInterpolate::PreGameTickLocalPlayersUpdate();
	}

	void PostGameTick()
	{
		game_state_updating = false;
		CameraInterpolate::PostGameTickLocalPlayersUpdate();
	}

	float GetInterpolateTime()
	{
		return time_globals::get_ticks_fraction_leftover();
	}

	void Reset()
	{
		CameraInterpolate::Reset();
	}

	void __cdecl game_update_hook(int target_ticks, float* a2)
	{
		typedef void(__cdecl* game_update_t)(int, float*);
		auto p_game_update = Memory::GetAddressRelative<game_update_t>(0x44A5D0);

		ticks_executed = target_ticks > 0;

		return p_game_update(target_ticks, a2);
	}

	void __cdecl effects_update_hook(float dt)
	{
		typedef void(__cdecl* effects_update_t)(float);
		auto p_effects_update = Memory::GetAddressRelative<effects_update_t>(0x4AA5D0);

		effect_update_accumulator += dt;
		if (ticks_executed)
		{
			p_effects_update(effect_update_accumulator);
			effect_update_accumulator = 0.0f;
		}
	}

	void __cdecl game_tick_hook()
	{
		typedef void(__cdecl* game_tick_t)();
		auto p_game_tick = Memory::GetAddressRelative<game_tick_t>(0x44A4AF);

		PreGameTick();
		p_game_tick();
		PostGameTick();
	}

	void ApplyPatches()
	{
		if (Memory::IsDedicatedServer())
			return;

		CameraInterpolate::ApplyPatches();

		// fix effects at uncapped FPS
		PatchCall(Memory::GetAddressRelative(0x439D2A), game_update_hook);
		PatchCall(Memory::GetAddressRelative(0x448D5C), effects_update_hook);
		PatchCall(Memory::GetAddressRelative(0x44A5E8), game_tick_hook);
	}
}