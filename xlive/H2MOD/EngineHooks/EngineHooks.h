#pragma once
namespace EngineHooks
{
	typedef int(__thiscall* game_life_cycle_update)(BYTE* this_);
	typedef void(__cdecl main_game_reset_map)();

	void ApplyHooks();
}