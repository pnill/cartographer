
enum EventTypes
{
	player_leave,
	player_join,
	gamestate_change,
};
enum GameState : BYTE
{
	GS_MainMenu = 0,
	GS_Lobby = 1,
	GS_Loading = 2,
	GS_InGame = 3,
	GS_PostGame = 4,
	GS_MatchMaking = 5
};
//template <class T>
//struct EventCallback
//{
//	
//	EventTypes Type;
//	T Callback;
//	EventCallback(std::string name, EventTypes type, GameStateCallback callback)
//	{
//		this->Name = name;
//		this->Type = type;
//		this->Callback = callback;
//	}
//};
struct GameStateCallback
{
	std::string name;
	GameState gameState;
	std::function<void()> callback;
	/**
	 * \brief Callback that executes when the GameState reaches the given GameState
	 * \param name A name that is stored with this callback that can later be referenced for removal.
	 * \param gameState The GameState type in which this should execute
	 * \param callback a void that will be executed when this event happens.
	 */
	GameStateCallback(std::string name, GameState gameState, std::function<void()> callback)
	{
		this->name = name;
		this->gameState = gameState;
		this->callback = callback;
	}
};
struct NetworkPeerEventCallback
{
	
	std::string name;
	std::function<void(int)> callback;
	/**
	 * \brief Basic callback for when a peer action happens in the network session.
	 * \param name A name that is stored with this callback that can later be referenced for removal.
	 * \param callback a void that will be executed when this event happens the paramter in is the peer index.
	 */
	NetworkPeerEventCallback(std::string name, std::function<void(int)> callback){
		this->name = name;
		this->callback = callback;
	}
};

namespace EventHandler
{
	/**
	 * \brief Adds a callback to be executed when the GameState changes
	 * \param callback A GameStateCallback to be executed when the event happens.
	 * \param threaded Tells the EventHandler whether or not this callback should be ran in its own thread with other threaded callbacks.
	 */
	void registerGameStateCallback(GameStateCallback callback, bool threaded);
	/**
	 * \brief Removes a callback from the store
	 * \param name Name of the callback to remove.
	 */
	void removeGameStateCallback(std::string name);
	/**
	 * \brief Executes the callbacks that match the given GameState.
	 * \param gamestate
	 */
	void executeGameStateCallbacks(BYTE gamestate);

	/**
	 * \brief Adds a callback to be executed when the NetworkSession recieves a player_add packet
	 * \param callback A NetworkPeerEventCallback to be executed when the event happens.
	 * \param threaded Tells the EventHandler whether or not this callback should be ran in its own thread with other threaded callbacks.
	 */
	void registerNetworkPlayerAddCallback(NetworkPeerEventCallback callback, bool threaded);
	/**
	 * \brief Removes a callback from the store
	 * \param name Name of the callback to remove
	 */
	void removeNetworkPlayerAddCallback(std::string name);
	/**
	 * \brief Executes the callbacks passing the Peer Index to the function
	 * \param peerIndex 
	 */
	void executeNetworkPlayerAddCallbacks(int peerIndex);

	/**
	 * \brief Adds a callback to be executed when the NetworkSession recieves a player_remove packet
	 * \param callback A NetworkPeerEventCallback to be executed when the event happens.
	 * \param threaded Tells the EventHandler whether or not this callback should be ran in its own thread with other threaded callbacks.
	 */
	void registerNetworkPlayerRemoveCallback(NetworkPeerEventCallback callback, bool threaded);
	/**
	 * \brief Removes a callback from the store
	 * \param name Name of the callback to remove
	 */
	void removeNetworkPlayerRemoveCallback(std::string name);
	/**
	 * \brief Executes the callbacks passing the Peer Index to the function
	 * \param peerIndex 
	 */
	void executeNetworkPlayerRemoveCallbacks(int peerIndex);

	///////////////////////////////////////////////
	//Someone smarter than me can figure this out//
	//It's kinda shit though					 //
	///////////////////////////////////////////////
	//template<typename T>
	//extern void registerCallback(EventTypes type, bool threaded, T callback)
	//{
	//	switch(type)
	//	{
	//		case player_leave: 
	//			break;
	//		case player_join:
	//			/*if (threaded)
	//			{
	//				playerJoinCallbacks.push_back(EventCallback<PlayerJoinCallback>(
	//					name,
	//					type,
	//					callback
	//				));
	//			}
	//			else
	//			{
	//				playerJoinCallbacks.push_back(EventCallback<PlayerJoinCallback>(
	//					name,
	//					type,
	//					callback
	//				));
	//			}*/
	//			break;
	//		case gamestate_change:
	//			if(threaded)
	//			{
	//				gameStateCallbacksThreaded.push_back({
	//					"A",
	//					GS_InGame,
	//					[](){}
	//				});
	//			}
	//			else
	//			{
	//				//gameStateCallbacks.push_back(static_cast<GameStateCallback>(callback));
	//			}
	//			break;
	//		default: ;
	//	}
	//}
	//template <typename T1 = BYTE, typename T2 = BYTE>
	//void executeCallbacks(EventTypes type, T1 arg1 = NULL, T2 arg2 = NULL)
	//{
	//	switch (type)
	//	{
	//	case player_leave:
	//		break;
	//	case player_join:
	//		/*if(!playerJoinCallbacksThreaded.empty())
	//		{
	//			
	//		}*/
	//		break;
	//	case gamestate_change:
	//		/*if (!gameStateCallbacksThreaded.empty())
	//		{
	//			auto executeThreaded = [](BYTE GameState)
	//			{
	//				for(const auto &cb : gameStateCallbacksThreaded)
	//				{
	//					if(cb.gameState == GameState)
	//						cb.callback();
	//				}
	//			};
	//			std::thread(executeThreaded, arg1).detach();
	//		}
	//		if(!gameStateCallbacks.empty())
	//		{
	//			for(const auto &cb : gameStateCallbacks)
	//			{
	//				if (cb.gameState == arg1)
	//					cb.callback();
	//			}
	//		}*/
	//		break;
	//	}
	//}
}
