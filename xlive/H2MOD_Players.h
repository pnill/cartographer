#pragma once

class Players {
public:
	int getPlayerCount();
	void reload();
	void logAllPlayersToConsole();
	void iteratePlayers(PlayerIterableHandler* playerHandler);
	int getPlayerOffset();
private:
	std::mutex playerLock;
	std::vector<Player*> players;
};