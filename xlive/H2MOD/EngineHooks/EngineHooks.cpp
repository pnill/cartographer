#include "stdafx.h"

#include "EngineHooks.h"
#include "H2MOD.h"
#include "H2MOD\Modules\EventHandler\EventHandler.hpp"
#include "Util\Hooks\Hook.h"

#ifndef _CARTOGRAPHER_DLL_CONF
// DO NOT USE VERSIONS ABOVE USHORT_MAX - 1 (above 65534) OR BELLOW 0 
// REPLACE THE DEFAULTS FOR BOTH EXECUTABLE_VERSION AND COMPATIBLE_VERSION WITH THE SAME VALUE FOR THE MOST PART
// OR YOU CAN SET THE COMPATIBLE VERSION HIGHER IF YOU WANT TO ALLOW OLDER VERSIONS TO JOIN (EXECUTABLE_VERSION SHOULD NOT BE ABOVE IT)
// ALSO THE VERSION SHOULD BE MODIFIED INCREMENTALLY, NOT RANDOMLY
#define EXECUTABLE_VERSION 11122
#define COMPATIBLE_VERSION 11122

static_assert(EXECUTABLE_VERSION < 65535 && EXECUTABLE_VERSION > 0, "EXECUTABLE_VERSION VALUE EXCEEDS 65534");
static_assert(COMPATIBLE_VERSION < 65535 && COMPATIBLE_VERSION > 0, "COMPATIBLE_VERSION VALUE EXCEEDS 65534");

// DO NOT CHANGE, DO NOT USE TYPES ABOVE 7 OR BELLOW 0 (3 bits values max)
#define EXECUTABLE_TYPE 4
static_assert(EXECUTABLE_TYPE <= 7 && EXECUTABLE_TYPE >= 0, "EXECUTABLE_TYPE VALUE EXCEEDS 7 OR BELLOW 0");
#endif

namespace EngineHooks
{
	static e_game_life_cycle previousGamestate = _life_cycle_none;

	game_life_cycle_update p_game_life_cycle_update;
	void GameLifeCycleUpdate()
	{
		auto gameLifeCycleData = Memory::GetAddress<BYTE*>(0x420FC4, 0x3C40AC);
		p_game_life_cycle_update(gameLifeCycleData);

		e_game_life_cycle currentLifeCycle = *(e_game_life_cycle*)(gameLifeCycleData);
		if (previousGamestate != currentLifeCycle) {
			previousGamestate = currentLifeCycle;
			EventHandler::GameLifeCycleEventExecute(EventExecutionType::execute_after, currentLifeCycle);
		}
	}

	main_game_reset_map* p_main_game_reset_map;
	void __cdecl main_game_reset_map_blue_screen_detection()
	{
		p_main_game_reset_map();
		EventHandler::BlueScreenEventExecute(EventExecutionType::execute_after);
		LOG_TRACE_GAME("[{}] Bluescreen Detected.", __FUNCTION__);
	}

#pragma region Game Version hooks
	verify_game_version_on_join p_verify_game_version_on_join;
	bool __cdecl verify_game_version_on_join_hook(BYTE executable_type, unsigned short executable_version, unsigned short compatible_version)
	{
		return executable_type == EXECUTABLE_TYPE && executable_version >= EXECUTABLE_VERSION && compatible_version <= COMPATIBLE_VERSION;
	}

	verify_executable_type p_verify_executable_version;
	bool __cdecl verify_executable_type_hook(BYTE executable_type)
	{
		// will not display servers that don't match this in server list
		return executable_type == EXECUTABLE_TYPE;
	}

	get_game_version p_get_game_version;

	void __cdecl get_game_version_hook(BYTE* executable_type, unsigned short* executable_version, unsigned short* compatible_version)
	{
		*executable_type = EXECUTABLE_TYPE;
		*executable_version = EXECUTABLE_VERSION;
		*compatible_version = COMPATIBLE_VERSION;
	}
#pragma endregion

	void ApplyHooks()
	{
		/*Game Life Cycle Change Detection*/
		p_game_life_cycle_update = Memory::GetAddress<game_life_cycle_update>(0x1d7738, 0x1BCDA8);
		PatchCall(Memory::GetAddress(0x1AD84D, 0x1A67CA), GameLifeCycleUpdate);

		/*Blue Screen detection*/
		p_main_game_reset_map = Memory::GetAddress<main_game_reset_map*>(0x9763, 0x1FA4E);
		PatchCall(Memory::GetAddress(0x1AE82F, 0x1A8A89), main_game_reset_map_blue_screen_detection);

		/*Game Version Hooks*/
		p_get_game_version = (get_game_version)DetourFunc(Memory::GetAddress<BYTE*>(0x1B4BF5, 0x1B0043), (BYTE*)get_game_version_hook, 8);
		if (!Memory::IsDedicatedServer()) {
			p_verify_game_version_on_join = (verify_game_version_on_join)DetourFunc(Memory::GetAddress<BYTE*>(0x1B4C14), (BYTE*)verify_game_version_on_join_hook, 5);
			p_verify_executable_version = (verify_executable_type)DetourFunc(Memory::GetAddress<BYTE*>(0x1B4C32), (BYTE*)verify_executable_type_hook, 8);
		}
	}
}
