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
	}
	void registerGameStateCallback(GameStateCallback callback, bool threaded)
	{
		if(threaded)
			gameStateCallbacksThreaded.push_back(callback);
		else
			gameStateCallbacks.push_back(callback);
	}

	void removeGameStateCallback(std::string name)
	{
		auto it = std::find_if(gameStateCallbacks.begin(), gameStateCallbacks.end(),
			[&name](const GameStateCallback &obj) {return obj.name == name;});
		if(it != gameStateCallbacks.end())
			gameStateCallbacks.erase(gameStateCallbacks.begin() + 
				std::distance(gameStateCallbacks.begin(), it));

		it = std::find_if(gameStateCallbacksThreaded.begin(), gameStateCallbacksThreaded.end(),
			[&name](const GameStateCallback &obj) {return obj.name == name;});
		if (it != gameStateCallbacks.end())
			gameStateCallbacksThreaded.erase(gameStateCallbacksThreaded.begin() +
				std::distance(gameStateCallbacksThreaded.begin(), it));
	}

	void executeGameStateCallbacks(BYTE gamestate)
	{
		if(!gameStateCallbacksThreaded.empty())
		{
			auto executeThreaded = [](BYTE gamestate)
			{
				for (const auto &cb : gameStateCallbacksThreaded) {
					if (cb.gameState == gamestate) {
						cb.callback();
					}
				}
			};
			std::thread(executeThreaded, gamestate).detach();
		}
		if (!gameStateCallbacks.empty()) {
			for (const auto &cb : gameStateCallbacks) {
				if (cb.gameState == gamestate) {
					cb.callback();
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
			while(it != gameStateCallbacksThreaded.end())
			{
				if(it->runOnce)
					it = gameStateCallbacksThreaded.erase(it);
				else
					++it;
			}
		}
		if(!gameStateCallbacks.empty())
		{
			auto it = gameStateCallbacks.begin();
			while (it != gameStateCallbacks.end())
			{
				if (it->runOnce)
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
			playerAddCallbacks.erase(playerAddCallbacks.begin() +
				std::distance(playerAddCallbacks.begin(), it));

		it = std::find_if(playerAddCallbacksThreaded.begin(), playerAddCallbacksThreaded.end(),
			[&name](const NetworkPeerEventCallback &obj) {return obj.name == name;});
		if (it != playerAddCallbacksThreaded.end())
			playerAddCallbacksThreaded.erase(playerAddCallbacksThreaded.begin() +
				std::distance(playerAddCallbacksThreaded.begin(), it));
	}

	void executeNetworkPlayerAddCallbacks(int peerIndex)
	{
		if(!playerAddCallbacksThreaded.empty())
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
			playerRemoveCallbacks.erase(playerRemoveCallbacks.begin() +
				std::distance(playerRemoveCallbacks.begin(), it));

		it = std::find_if(playerRemoveCallbacksThreaded.begin(), playerRemoveCallbacksThreaded.end(),
			[&name](const NetworkPeerEventCallback &obj) {return obj.name == name;});
		if (it != playerRemoveCallbacksThreaded.end())
			playerRemoveCallbacksThreaded.erase(playerRemoveCallbacksThreaded.begin() +
				std::distance(playerRemoveCallbacksThreaded.begin(), it));
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
}
