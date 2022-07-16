#pragma once

namespace Interpolation
{
	extern bool enabled;
	extern bool game_state_updating;

	void Reset();
	void ApplyPatches();
	void PreGameTick();
	void PostGameTick();
	float GetInterpolateTime();
}