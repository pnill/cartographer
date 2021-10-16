#pragma once
namespace EngineHooks
{
	typedef int(__thiscall* game_life_cycle_update)(BYTE* this_);
	typedef void(__cdecl main_game_reset_map)();
	typedef bool(__cdecl* verify_game_version_on_join)(int executable_version, int build_version, int build_version2);
	typedef bool(__cdecl* verify_executable_version)(int executable_version);
	typedef void(__cdecl *get_game_version)(DWORD *executable_version, DWORD *build_version, DWORD *build_version2);

	void ApplyHooks();
}