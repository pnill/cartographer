#pragma once

class PlayerIterator {
public:
	virtual void onIteratePlayer(Player* player) = 0;
};

class PlayerLookup {
public:
	virtual void lookup(Player* player) = 0;
	BOOL foundPlayer = false;
};

class Players {
public:
	int getPlayerCount();
	void reload();
	void logAllPlayersToConsole();
	void iteratePlayers(PlayerIterator* playerHandler);
	void lookupPlayer(PlayerLookup* playerLookup);
	int getPlayerOffset();
private:
	std::mutex playerLock;
	std::vector<Player*> players;
};