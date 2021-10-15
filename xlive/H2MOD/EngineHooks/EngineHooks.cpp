#include "EngineHooks.h"
#include "H2MOD.h"
#include "H2MOD\Modules\EventHandler\EventHandler.hpp"
#include "Util\Hooks\Hook.h"

namespace EngineHooks
{
	static game_life_cycle previousGamestate = life_cycle_none;
	ChangeGameState p_EvaulateGameState;
	void EvaluateGameState()
	{
		p_EvaulateGameState(Memory::GetAddress<BYTE*>(0x420FC4, 0x3C40AC));
		game_life_cycle GameState = *Memory::GetAddress<game_life_cycle*>(0x420FC4, 0x3C40AC);
		if (previousGamestate != GameState) {
			previousGamestate = GameState;
			EventHandler::execute_callback<EventHandler::GameStateEvent>(gamestate_change, execute_after, GameState);
		}
	}

	p_main_game_reset_map* c_main_game_reset_map;
	void __cdecl main_game_reset_map_blue_screen_detection()
	{
		c_main_game_reset_map();
		EventHandler::execute_callback<EventHandler::BlueScreenEvent>(blue_screen, execute_after);
		LOG_TRACE_GAME("[{}] Bluescreen Detected.", __FUNCTION__);
	}

	#pragma region Game Version hooks
	verify_game_version_on_join p_verify_game_version_on_join;
	bool __cdecl VerifyGameVersionOnJoin(int executable_version, int build_version, int build_version2)
	{
		return executable_version == EXECUTABLE_VERSION && build_version >= GAME_BUILD && build_version2 <= GAME_BUILD;
	}

	verify_executable_version p_verify_executable_version;

	bool __cdecl VerifyExecutableVersion(int executable_version)
	{
		return executable_version == EXECUTABLE_VERSION; // will not display servers that don't match this in server list
	}

	get_game_version p_get_game_version;

	void __cdecl GetGameVersion(DWORD *executable_version, DWORD *build_version, DWORD *build_version2)
	{
		*executable_version = EXECUTABLE_VERSION;
		*build_version = GAME_BUILD;
		*build_version2 = GAME_BUILD;
	}
	#pragma endregion

	void ApplyHooks()
	{
		/*Game State Change Detection*/
		p_EvaulateGameState = Memory::GetAddress<ChangeGameState>(0x1d7738, 0x1BCDA8);
		PatchCall(Memory::GetAddress(0x1AD84D, 0x1A67CA), EvaluateGameState);

		/*Blue Screen detection*/
		c_main_game_reset_map = Memory::GetAddress<p_main_game_reset_map*>(0x9763, 0x1FA4E);
		PatchCall(Memory::GetAddress(0x1AE82F, 0x1A8A89), main_game_reset_map_blue_screen_detection);
		
		/*Game Version Hooks*/
		p_get_game_version = (get_game_version)DetourFunc(Memory::GetAddress<BYTE*>(0x1B4BF5, 0x1B0043), (BYTE*)GetGameVersion, 8);
		if (!Memory::isDedicatedServer()) {
			p_verify_game_version_on_join = (verify_game_version_on_join)DetourFunc(Memory::GetAddress<BYTE*>(0x1B4C14), (BYTE*)VerifyGameVersionOnJoin, 5);
			p_verify_executable_version = (verify_executable_version)DetourFunc(Memory::GetAddress<BYTE*>(0x1B4C32), (BYTE*)VerifyExecutableVersion, 8);
		}
	}
}
