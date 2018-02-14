#pragma once

class Players {
public:
	int getPlayerCount();
	void reload();
	void logAllPlayersToConsole();
	void iteratePlayers(PlayerIterableHandler* playerHandler);
private:
	std::mutex playerLock;
	std::vector<Player*> players;
};