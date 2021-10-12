#pragma once
#include "Blam\Common\Common.h"
#include "H2MOD\Modules\ServerConsole\ServerConsole.h"
#include "Blam\Cache\DataTypes\BlamPrimitiveType.h"

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
	bool threaded;
	bool runOnce;
	bool hasRun = false;
	EventCallback(const T callback, EventType type, EventExecutionType execution_type = execute_after, bool threaded = false, bool runOnce = false)
	{
		this->callback = callback;
		this->type = type;
		this->execution_type = execution_type;
		this->threaded = threaded;
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
	static std::vector<EventCallback<void*>>* get_vector(EventType type)
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
	template<typename T> static void remove_callback(const T callback, EventExecutionType execution_type)
	{
		auto type = get_type<T>();
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
	/**
	 * \brief Erases callbacks that are flagged with run once that have ran
	 * \tparam T event alias type
	 * \param execution_type 
	 */
	template<typename T> static void cleanup_callbacks(EventExecutionType execution_type)
	{
		auto type = get_type<T>();
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
	 * \brief Registers a new callback that will execute off the given parameters
	 * \tparam T Event Alias type
	 * \param callback point to the callback
	 * \param execution_type determines if the callback will be ran before or after the execution of the triggering function
	 * \param threaded tells the executor to only run this callback in a threaded manner
	 * \param run_once flags the callback to only be ran once and then erased afterwards.
	 */
	template<typename T> static void register_callback(const T callback, EventExecutionType execution_type = execute_after, bool threaded = false, bool run_once = false)
	{
		auto type = get_type<T>();
		if(type != EventType::none)
		{
			//Prevent duplicate events
			remove_callback<T>(callback, execution_type);
			
			get_vector(type)->emplace_back(callback, type, execution_type, threaded, run_once);
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
	static void execute_callback(EventExecutionType execution_type, Args&& ... args)
	{
		auto execute_internal = [&](EventExecutionType execution_type, bool threaded)
		{
			auto type = get_type<T>();
			std::vector<EventCallback<void*>>* events = get_vector(type);
			for (std::vector<EventCallback<void*>>::iterator it = events->begin(); it != events->end(); ++it) {
				if (it->type == type && it->execution_type == execution_type && it->threaded == threaded)
				{
					((T)it->callback)(std::forward<Args>(args) ...);
					it->hasRun = true;
				}
			}
			cleanup_callbacks<T>(execution_type);
		};
		//uncomment this for debugging.
		//LOG_TRACE_GAME("[{}]: {} ", __FUNCSIG__, execution_type);
		execute_internal(execution_type, false);
		//When trying to debug issues in threaded callbacks switch out the two lines, helps trace call stacks
		//execute_internal(execution_type, true);
		std::thread(execute_internal, execution_type, true).detach();
		
	}
}
