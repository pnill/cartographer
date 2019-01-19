#pragma once
#include <string>

class DiscordInterface
{
public:
	static void Init();
	static void SetPlayerCountInfo(int current, int max);
	static void SetDetails(const std::string &map_name);
	static void SetGameMode(const std::string &gamemode_id);
	static void SetGameState(
		std::string map_id,
		const std::string &game_state,
		const std::string &map_name = "",
		const std::string &gamemode_id = "",
		const std::string &gamemode_name ="",
		const bool hide_player_count = false
	);
	static void HidePlayerCount(bool hide);
	static void SetGameState(
		std::string map_id,	
		const std::string &game_state,
		const bool hide_player_count)
	{
		SetGameState(map_id, game_state, "", "", "", hide_player_count);
	}
};

