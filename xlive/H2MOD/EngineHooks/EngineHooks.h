#pragma once
namespace EngineHooks
{
	typedef int(__thiscall* game_life_cycle_update)(BYTE* this_);
	typedef void(__cdecl main_game_reset_map)();

	typedef bool(__cdecl* verify_game_version_on_join)(BYTE executable_type, unsigned short executable_version, unsigned short compatible_version);
	typedef bool(__cdecl* verify_executable_type)(BYTE executable_type);
	typedef void(__cdecl* get_game_version)(BYTE* executable_type, DWORD* executable_version, DWORD* compatible_version);

	void ApplyHooks();
}