#include "EventHandler.h"
namespace EventHandler
{
	namespace
	{
		std::vector<GameStateCallback> gameStateCallbacks;
		std::vector<GameStateCallback> gameStateCallbacksThreaded;

		std::vector<NetworkPeerEventCallback> playerAddCallbacks;
		std::vector<NetworkPeerEventCallback> playerAddCallbacksThreaded;

		std::vector<NetworkPeerEventCallback> playerRemoveCallbacks;
		std::vector<NetworkPeerEventCallback> playerRemoveCallbacksThreaded;

		std::vector<GameLoopEventCallback> gameLoopCallbacks;
		std::vector<GameLoopEventCallback> gameLoopCallbacksThreaded;

		std::vector<ServerCommandEventCallback> serverCommandCallbacks;
		std::vector<ServerCommandEventCallback> serverCommandCallbacksThreaded;

		std::vector< PlayerControlChangeEventCallback> playerControlCallbacks;
		std::vector< PlayerControlChangeEventCallback> playerControlCallbacksThreaded;

		std::vector<MapLoadEventCallback> mapLoadCallbacks;
		std::vector<MapLoadEventCallback> mapLoadCallbacksThreaded;
	}
	void registerGameStateCallback(GameStateCallback callback, bool threaded)
	{
		if (threaded)
			gameStateCallbacksThreaded.push_back(callback);
		else
			gameStateCallbacks.push_back(callback);
	}

	void removeGameStateCallback(std::string name)
	{
		auto it = std::find_if(gameStateCallbacks.begin(), gameStateCallbacks.end(),
			[&name](const GameStateCallback &obj) {return obj.name == name;});
		if (it != gameStateCallbacks.end())
			gameStateCallbacks.erase(it);

		it = std::find_if(gameStateCallbacksThreaded.begin(), gameStateCallbacksThreaded.end(),
			[&name](const GameStateCallback &obj) {return obj.name == name;});
		if (it != gameStateCallbacks.end())
			gameStateCallbacksThreaded.erase(it);
	}

	void executeGameStateCallbacks(BYTE gamestate)
	{
		if (!gameStateCallbacksThreaded.empty())
		{
			auto executeThreaded = [](BYTE gamestate)
			{
				for (auto &cb : gameStateCallbacksThreaded) {
					if (cb.gameState == gamestate) {
						cb.callback();
						cb.hasRun = true;
					}
				}
			};
			std::thread(executeThreaded, gamestate).detach();
		}
		if (!gameStateCallbacks.empty()) {
			for (auto &cb : gameStateCallbacks) {
				if (cb.gameState == gamestate) {
					cb.callback();
					cb.hasRun = true;
				}
			}
		}
		cleanupGameStateCallbacks(gamestate);
	}

	void cleanupGameStateCallbacks(BYTE gamestate)
	{
		if (!gameStateCallbacksThreaded.empty())
		{
			auto it = gameStateCallbacksThreaded.begin();
			while (it != gameStateCallbacksThreaded.end())
			{
				if (it->runOnce && it->hasRun)
					it = gameStateCallbacksThreaded.erase(it);
				else
					++it;
			}
		}
		if (!gameStateCallbacks.empty())
		{
			auto it = gameStateCallbacks.begin();
			while (it != gameStateCallbacks.end())
			{
				if (it->runOnce && it->hasRun)
					it = gameStateCallbacks.erase(it);
				else
					++it;
			}
		}
	}


	void registerNetworkPlayerAddCallback(NetworkPeerEventCallback callback, bool threaded)
	{
		if (threaded)
			playerAddCallbacksThreaded.push_back(callback);
		else
			playerAddCallbacks.push_back(callback);
	}

	void removeNetworkPlayerAddCallback(std::string name)
	{
		auto it = std::find_if(playerAddCallbacks.begin(), playerAddCallbacks.end(),
			[&name](const NetworkPeerEventCallback &obj) {return obj.name == name;});
		if (it != playerAddCallbacks.end())
			playerAddCallbacks.erase(it);

		it = std::find_if(playerAddCallbacksThreaded.begin(), playerAddCallbacksThreaded.end(),
			[&name](const NetworkPeerEventCallback &obj) {return obj.name == name;});
		if (it != playerAddCallbacksThreaded.end())
			playerAddCallbacksThreaded.erase(it);
	}

	void executeNetworkPlayerAddCallbacks(int peerIndex)
	{
		if (!playerAddCallbacksThreaded.empty())
		{
			auto executeCallbacks = [](int peerIndex)
			{
				for (const auto &cb : playerAddCallbacksThreaded)
					cb.callback(peerIndex);
			};
			std::thread(executeCallbacks, peerIndex).detach();
		}
		if (!playerAddCallbacks.empty())
			for (const auto &cb : playerAddCallbacks)
				cb.callback(peerIndex);
	}

	void registerNetworkPlayerRemoveCallback(NetworkPeerEventCallback callback, bool threaded)
	{
		if (threaded)
			playerRemoveCallbacksThreaded.push_back(callback);
		else
			playerRemoveCallbacks.push_back(callback);
	}

	void removeNetworkPlayerRemoveCallback(std::string name)
	{
		auto it = std::find_if(playerRemoveCallbacks.begin(), playerRemoveCallbacks.end(),
			[&name](const NetworkPeerEventCallback &obj) {return obj.name == name;});
		if (it != playerRemoveCallbacks.end())
			playerRemoveCallbacks.erase(it);

		it = std::find_if(playerRemoveCallbacksThreaded.begin(), playerRemoveCallbacksThreaded.end(),
			[&name](const NetworkPeerEventCallback &obj) {return obj.name == name;});
		if (it != playerRemoveCallbacksThreaded.end())
			playerRemoveCallbacksThreaded.erase(it);
	}

	void executeNetworkPlayerRemoveCallbacks(int peerIndex)
	{
		if (!playerRemoveCallbacksThreaded.empty())
		{
			auto executeCallbacks = [](int peerIndex)
			{
				for (const auto &cb : playerRemoveCallbacksThreaded)
					cb.callback(peerIndex);
			};
			std::thread(executeCallbacks, peerIndex).detach();
		}
		if (!playerRemoveCallbacks.empty())
			for (const auto &cb : playerRemoveCallbacks)
				cb.callback(peerIndex);
	}

	void registerGameLoopCallback(GameLoopEventCallback callback, bool threaded)
	{
		if (threaded) {
			auto it = std::find_if(gameLoopCallbacksThreaded.begin(), gameLoopCallbacksThreaded.end(),
				[&callback](const GameLoopEventCallback &obj) {return obj.name == callback.name;});
			if (it == gameLoopCallbacksThreaded.end())
				gameLoopCallbacksThreaded.push_back(callback);
		}
		else {
			auto it = std::find_if(gameLoopCallbacks.begin(), gameLoopCallbacks.end(),
				[&callback](const GameLoopEventCallback &obj) {return obj.name == callback.name;});
			if (it == gameLoopCallbacks.end())
				gameLoopCallbacks.push_back(callback);
		}
	}

	void removeGameLoopCallback(std::string name)
	{
		auto it = std::find_if(gameLoopCallbacks.begin(), gameLoopCallbacks.end(),
			[&name](const GameLoopEventCallback &obj) {return obj.name == name;});
		if (it != gameLoopCallbacks.end())
			gameLoopCallbacks.erase(it);

		it = std::find_if(gameLoopCallbacksThreaded.begin(), gameLoopCallbacksThreaded.end(),
			[&name](const GameLoopEventCallback &obj) {return obj.name == name;});
		if (it != gameLoopCallbacksThreaded.end())
			gameLoopCallbacksThreaded.erase(it);
	}

	void executeGameLoopCallbacks()
	{
		if (!gameLoopCallbacksThreaded.empty())
		{
			auto executeCallbacks = []()
			{
				for (const auto &cb : gameLoopCallbacksThreaded)
					cb.callback();
			};
			std::thread(executeCallbacks).detach();
		}
		if (!gameLoopCallbacks.empty())
			for (const auto &cb : gameLoopCallbacks)
				cb.callback();
	}

	void registerServerCommandCallback(ServerCommandEventCallback callback, bool threaded)
	{
		if (threaded) {
			auto it = std::find_if(serverCommandCallbacksThreaded.begin(), serverCommandCallbacksThreaded.end(),
				[&callback](const ServerCommandEventCallback &obj) {return obj.name == callback.name;});
			if (it == serverCommandCallbacksThreaded.end())
				serverCommandCallbacksThreaded.push_back(callback);
		}
		else {
			auto it = std::find_if(serverCommandCallbacks.begin(), serverCommandCallbacks.end(),
				[&callback](const ServerCommandEventCallback &obj) {return obj.name == callback.name;});
			if (it == serverCommandCallbacks.end())
				serverCommandCallbacks.push_back(callback);
		}
	}

	void removeServerCommandCallback(std::string name)
	{
		auto it = std::find_if(serverCommandCallbacksThreaded.begin(), serverCommandCallbacksThreaded.end(),
			[&name](const ServerCommandEventCallback &obj) {return obj.name == name;});
		if (it != serverCommandCallbacksThreaded.end())
			serverCommandCallbacksThreaded.erase(it);

		it = std::find_if(serverCommandCallbacks.begin(), serverCommandCallbacks.end(),
			[&name](const ServerCommandEventCallback &obj) {return obj.name == name;});
		if (it != serverCommandCallbacks.end())
			serverCommandCallbacks.erase(it);
	}

	void executeServerCommandCallback(ServerConsole::ServerConsoleCommands command)
	{
		if (!serverCommandCallbacksThreaded.empty())
		{
			auto executeThreaded = [](ServerConsole::ServerConsoleCommands command)
			{
				for (auto &cb : serverCommandCallbacksThreaded) {
					if (cb.command == command || command == ServerConsole::any) {
						cb.callback();
					}
				}
			};
			std::thread(executeThreaded, command).detach();
		}
		if (!serverCommandCallbacks.empty()) {
			for (auto &cb : serverCommandCallbacks) {
				if (cb.command == command || command == ServerConsole::any) {
					cb.callback();
				}
			}
		}
	}

	void registerPlayerControlCallback(PlayerControlChangeEventCallback callback, bool threaded)
	{
		if (threaded) {
			auto it = std::find_if(playerControlCallbacksThreaded.begin(), playerControlCallbacksThreaded.end(),
				[&callback](const PlayerControlChangeEventCallback &obj) {return obj.name == callback.name;});
			if (it == playerControlCallbacksThreaded.end())
				playerControlCallbacksThreaded.push_back(callback);
		}
		else {
			auto it = std::find_if(playerControlCallbacks.begin(), playerControlCallbacks.end(),
				[&callback](const PlayerControlChangeEventCallback &obj) {return obj.name == callback.name;});
			if (it == playerControlCallbacks.end())
				playerControlCallbacks.push_back(callback);
		}
	}

	void removePlayerControlCallback(std::string name)
	{
		auto it = std::find_if(playerControlCallbacksThreaded.begin(), playerControlCallbacksThreaded.end(),
			[&name](const PlayerControlChangeEventCallback &obj) {return obj.name == name;});
		if (it != playerControlCallbacksThreaded.end())
			playerControlCallbacksThreaded.erase(it);

		it = std::find_if(playerControlCallbacks.begin(), playerControlCallbacks.end(),
			[&name](const PlayerControlChangeEventCallback &obj) {return obj.name == name;});
		if (it != playerControlCallbacks.end())
			playerControlCallbacks.erase(it);
	}

	void executePlayerControlCallback(float yaw, float pitch)
	{
		if (!playerControlCallbacksThreaded.empty())
		{
			auto executeThreaded = [](float yaw, float pitch)
			{
				for (auto &cb : playerControlCallbacksThreaded) {
					
						cb.callback(yaw, pitch);
				}
			};
			std::thread(executeThreaded, yaw, pitch).detach();
		}
		if (!playerControlCallbacks.empty()) {
			for (auto &cb : playerControlCallbacks) {
					cb.callback(yaw, pitch);
			}
		}
	}

	void registerMapLoadCallback(MapLoadEventCallback callback, bool threaded)
	{
		if (threaded) {
			auto it = std::find_if(mapLoadCallbacksThreaded.begin(), mapLoadCallbacksThreaded.end(),
				[&callback](const MapLoadEventCallback &obj) {return obj.name == callback.name;});
			if (it == mapLoadCallbacksThreaded.end())
				mapLoadCallbacksThreaded.push_back(callback);
		}
		else {
			auto it = std::find_if(mapLoadCallbacks.begin(), mapLoadCallbacks.end(),
				[&callback](const MapLoadEventCallback &obj) {return obj.name == callback.name;});
			if (it == mapLoadCallbacks.end())
				mapLoadCallbacks.push_back(callback);
		}
	}

	void removeMapLoadCallback(std::string name)
	{
		auto it = std::find_if(mapLoadCallbacksThreaded.begin(), mapLoadCallbacksThreaded.end(),
			[&name](const MapLoadEventCallback &obj) {return obj.name == name;});
		if (it != mapLoadCallbacksThreaded.end())
			mapLoadCallbacksThreaded.erase(it);

		it = std::find_if(mapLoadCallbacks.begin(), mapLoadCallbacks.end(),
			[&name](const MapLoadEventCallback &obj) {return obj.name == name;});
		if (it != mapLoadCallbacks.end())
			mapLoadCallbacks.erase(it);
	}

	void executeMapLoadCallback(e_engine_type engine_type)
	{
		if (!mapLoadCallbacksThreaded.empty())
		{
			auto executeThreaded = [](e_engine_type engine_type)
			{
				for (auto &cb : mapLoadCallbacksThreaded) {
					if(engine_type == cb.Type)
						cb.callback();
				}
			};
			std::thread(executeThreaded, engine_type).detach();
		}
		if (!mapLoadCallbacks.empty()) {
			for (auto &cb : mapLoadCallbacks) {
				if(engine_type == cb.Type)
					cb.callback();
			}
		}
	}
}
