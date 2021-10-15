#pragma once
#include "Blam\Common\Common.h"
#include "H2MOD\Modules\ServerConsole\ServerConsole.h"
#include "Blam\Cache\DataTypes\BlamPrimitiveType.h"


#define EVENT_HANDLER_ENABLE_TEST_EVENTS 0

/*
 * To Setup a new Event you need to add an enum to EventType above the none enum
 * Then inside EventHandler you need to create a function alias type use the current ones as an example
 *  - CountdownStartEvent
 *  - GameStateEvent
 *  
 *  Then where ever the event needs to be triggered you can decide if you want the execution to be before or after
 *  the triggering circumstances
 *  
 *  EventHandler::execute_callback<Alias Type>(EventExecutionType, Additional parameters for the alias type);
 *  EventHandler::execute_callback<EventHandler::PlayerControlEvent>(execute_before, &yawChange, &pitchChange);
 *  EventHandler::execute_callback<EventHandler::PlayerControlEvent>(execute_after, &yawChange, &pitchChange);
 *  
 *  Registering a callback should only happen in a state where the game engine should not actively be firing any callbacks.
 *  I.E NOT IN GAME.
 */



enum EventType
{
	network_player,
	gamestate_change,
	game_loop,
	server_command,
	map_load,
	countdown_start,
	player_control,
	blue_screen,
	player_spawn_event,
	player_death_event,
	none = -1
};
enum EventExecutionType
{
	execute_before,
	execute_after
};

template<typename T> class EventCallback
{
public:
	T callback;
	EventType type;
	EventExecutionType execution_type;
	bool runOnce;
	bool hasRun = false;
	EventCallback(const T callback, EventType type, EventExecutionType execution_type = execute_after, bool runOnce = false)
	{
		this->callback = callback;
		this->type = type;
		this->execution_type = execution_type;
		this->runOnce = runOnce;
	}
};

extern std::map<EventType, std::vector<EventCallback<void*>>> event_map;

namespace EventHandler
{
	enum NetworkPlayerEventType
	{
		add,
		remove
	};

	using CountdownStartEvent = void(*)();
	using GameStateEvent = void(*)(game_life_cycle state);
	using NetworkPlayerEvent = void(*)(int peerIndex, NetworkPlayerEventType type);
	using GameLoopEvent = void(*)();
	using ServerCommandEvent = void(*)(ServerConsole::ServerConsoleCommands command);
	using PlayerControlEvent = void(*)(float* yaw, float* pitch);
	using MapLoadEvent = void(*)(e_engine_type type);
	using BlueScreenEvent = void(*)();
	using PlayerSpawnEvent = void(*)(datum PlayerDatum);
	using PlayerDeathEvent = void(*)(datum PlayerDatum, datum KillerDatum);
	/**
	 * \brief Takes the alias type and returns the corresponding EventType
	 * \tparam T alias event type
	 * \return EventType::
	 */
	template<typename T> static EventType get_type()
	{
		if (std::is_same<T, GameStateEvent>::value)
			return EventType::gamestate_change;
		if (std::is_same<T, CountdownStartEvent>::value)
			return EventType::countdown_start;
		if (std::is_same<T, NetworkPlayerEvent>::value)
			return EventType::network_player;
		if (std::is_same<T, GameLoopEvent>::value)
			return EventType::game_loop;
		if (std::is_same<T, ServerCommandEvent>::value)
			return EventType::server_command;
		if (std::is_same<T, PlayerControlEvent>::value)
			return EventType::player_control;
		if (std::is_same<T, MapLoadEvent>::value)
			return EventType::map_load;
		if (std::is_same<T, BlueScreenEvent>::value)
			return EventType::blue_screen;
		if (std::is_same<T, PlayerSpawnEvent>::value)
			return EventType::player_spawn_event;
		if (std::is_same<T, PlayerDeathEvent>::value)
			return EventType::player_death_event;
		return EventType::none;
	}
	
	/**
	 * \brief Grabs the vector from the event map that relates to the corresponding event type
	 * \param type the event type
	 * \return std::vector<EventCallback<void*>>
	 */
	__declspec(noinline) static std::vector<EventCallback<void*>>* get_vector(EventType type)
	{
		//If the map doesn't contain a vector for the type create one
		if (event_map.count(type) == 0)
			event_map[type] = std::vector<EventCallback<void*>>();

		return &event_map.at(type);
	}

	/**
	 * \brief This function will remove a callback with the matching properties
	 * \tparam T The type alias for the callback to be removed
	 * \param callback pointer to the callback to be removed
	 * \param execution_type the execution type of the method to be removed
	 */
	template<typename T> static void remove_callback(const T callback, EventType type, EventExecutionType execution_type)
	{
		std::vector<EventCallback<void*>>* events = get_vector(type);
		for (std::vector<EventCallback<void*>>::iterator it = events->begin(); it != events->end(); ++it)
		{
			if((T)it->callback == callback && it->type == type && it->execution_type == execution_type)
			{
				events->erase(it);
				return;
			}
		}
	}
	template<typename T> static void remove_callback2(void* callback, EventExecutionType execution_type)
	{
		auto type = get_type<T>();
		std::vector<EventCallback<void*>>* events = get_vector(type);
		for (std::vector<EventCallback<void*>>::iterator it = events->begin(); it != events->end(); ++it)
		{
			if ((T)it->callback == callback && it->type == type && it->execution_type == execution_type)
			{
				events->erase(it);
				return;
			}
		}
	}

	/**
	 * \brief Erases callbacks that are flagged with run once that have ran
	 * \tparam T event alias type
	 * \param execution_type 
	 */
	template<typename T> __declspec(noinline) static void cleanup_callbacks(EventType type, EventExecutionType execution_type)
	{
		std::vector<EventCallback<void*>>* events = get_vector(type);
		auto it = events->begin();
		while(it != events->end())
		{
			if (it->runOnce && it->hasRun && it->type == type && it->execution_type == execution_type)
				it = events->erase(it);
			else
				++it;
		}
	}
	
	/**
	 * \brief Executes the callbacks based off the given parameters
	 * \tparam T The Event Alias type of callback's to execute
	 * \tparam Args Do not pass anything to this template
	 * \param execution_type 
	 * \param args Any arguments that need to be forwarded to the callback's
	 */
	template<typename T, typename ... Args>
	static void execute_callback(EventType type,EventExecutionType execution_type, Args&& ... args)
	{
		std::vector<EventCallback<void*>>* events = get_vector(type);
		for (std::vector<EventCallback<void*>>::iterator it = events->begin(); it != events->end(); ++it) {
			if (it->type == type && it->execution_type == execution_type)
			{
				((T)it->callback)(std::forward<Args>(args) ...);
				it->hasRun = true;
			}
		}
		cleanup_callbacks<T>(type, execution_type);
		//uncomment this for debugging.
		//LOG_TRACE_GAME("[{}]: {} ", __FUNCSIG__, execution_type);
	}



	template<EventType E> static void register_callback(void* callback, EventExecutionType execution_type = execute_after, bool run_once = false)
	{
		if (E != EventType::none)
		{
			switch(E)
			{
				case network_player:
					remove_callback2<NetworkPlayerEvent>(callback, execution_type);
					break;
				case gamestate_change:
					remove_callback2<GameStateEvent>(callback, execution_type);
					break;
				case game_loop:
					remove_callback2<GameLoopEvent>(callback, execution_type);
					break;
				case server_command: 
					remove_callback2<ServerCommandEvent>(callback, execution_type);
					break;
				case map_load: 
					remove_callback2<MapLoadEvent>(callback, execution_type);
					break;
				case countdown_start: 
					remove_callback2<CountdownStartEvent>(callback, execution_type);
					break;
				case player_control: 
					remove_callback2<PlayerControlEvent>(callback, execution_type);
					break;
				case blue_screen: 
					remove_callback2<BlueScreenEvent>(callback, execution_type);
					break;
				case player_spawn_event: 
					remove_callback2<PlayerSpawnEvent>(callback, execution_type);
					break;
				case player_death_event: 
					remove_callback2<PlayerDeathEvent>(callback, execution_type);
					break;
			}
			get_vector(E)->emplace_back(callback, E, execution_type, run_once);
		}
	}


#if EVENT_HANDLER_ENABLE_TEST_EVENTS
	static void callback_network(int peer_index, NetworkPlayerEventType type)
	{
		LOG_DEBUG_GAME("[{}] {} {}", __FUNCSIG__, peer_index, type);
	}
	static void callback_gamestate(game_life_cycle state)
	{
		LOG_DEBUG_GAME("[{}] {}", __FUNCSIG__, state);
	}
	static void callback_loop()
	{
		LOG_DEBUG_GAME("[{}]", __FUNCSIG__);
	}
	static void callback_server(ServerConsole::ServerConsoleCommands command)
	{
		LOG_DEBUG_GAME("[{}] {}", __FUNCSIG__, command);
	}
	static void callback_map(e_engine_type type)
	{
		LOG_DEBUG_GAME("[{}] {}", __FUNCSIG__, type);
	}
	static void callback_countdown()
	{
		LOG_DEBUG_GAME("[{}]", __FUNCSIG__);
	}
	static void callback_player_control(float* yaw, float* pitch)
	{
		LOG_DEBUG_GAME("[{}] {} {}", __FUNCSIG__, *yaw, *pitch);
	}
	static void callback_bluscreen()
	{
		LOG_DEBUG_GAME("[{}]", __FUNCSIG__);
	}
	static void callback_player_spawn(datum player_datum)
	{
		LOG_DEBUG_GAME("[{}] {:x}", __FUNCSIG__, player_datum);
	}
	static void callback_player_death(datum player_datum, datum killer_datum)
	{
		LOG_DEBUG_GAME("[{}] {:x} {:x}", __FUNCSIG__, player_datum, killer_datum);
	}

	static void TestEvents()
	{

		register_callback<network_player>(callback_network);
		register_callback<gamestate_change>(callback_gamestate);
		//register_callback<game_loop>(callback_loop);
		register_callback<server_command>(callback_server);
		register_callback<map_load>(callback_map);
		register_callback<player_control>(callback_player_control);
		register_callback<blue_screen>(callback_bluscreen);
		register_callback<player_spawn_event>(callback_player_spawn);
		register_callback<player_death_event>(callback_player_death);
	}
#endif
}
