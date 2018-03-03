#pragma once

class GameClientServerHandler {
public:
	virtual void onPeerHost() = 0;
	virtual void onDedi() = 0;
	virtual void onClient() = 0;
	void execute();
};

template <class GameClientServerHandler>
class GameType {
public:
	GameClientServerHandler* initializer;
	GameClientServerHandler* deinitializer;
	GameClientServerHandler* preSpawnPlayer;
	GameClientServerHandler* spawnPlayer;
	GameClientServerHandler* playerDeath;
	GameClientServerHandler* playerKill;
};