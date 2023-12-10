#pragma once

#define GAME_FRAME_INTERPOLATOR_ENABLED false

namespace Interpolation
{
	extern bool enabled;
	extern bool game_state_updating;

	void Reset();
	void ApplyPatches();
	void PreGameTick();
	void PostGameTick();
	float GetInterpolateTime();
	bool ShouldInterpolate();
} 