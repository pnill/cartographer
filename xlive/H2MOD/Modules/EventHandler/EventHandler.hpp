#pragma once

#include "Blam/Common/Common.h"
#include "Blam/Engine/Game/game/game.h"
#include "Blam/Cache/DataTypes/BlamPrimitiveType.h"
#include "H2MOD/Modules/Shell/ServerConsole.h"

/*
 * To Setup a new Event you need to add an enum to EventType above the none enum
 * Then inside EventHandler you need to create a function alias type use the current ones as an example
 *  - CountdownStartEvent
 *  - GameStateEvent
 *
 *  Then where ever the event needs to be triggered you can decide if you want the execution to be before or after
 *  the triggering circumstances
 *
 *  EventHandler::<RegisteredEvent>(EventExecutionType, Additional parameters for the alias type);
 *  EventHandler::PlayerControlEventExecute(EventExecutionType::execute_before, &yawChange, &pitchChange);
 *  EventHandler::PlayerControlEventExecute(EventExecutionType::execute_after, &yawChange, &pitchChange);
 *
 *  Registering a callback should only happen in a state where the game engine should not actively be firing any callbacks.
 *  I.E NOT IN GAME.
 */

#define REGISTER_EVENT_EXECUTE_METHOD(function_name, event_type, callback_definition) \
	template<typename ... Args> \
	static void function_name(EventExecutionType event_execution_type, Args&& ... args) \
	{ \
		execute_callback<callback_definition>(event_execution_type, event_type, std::forward<Args>(args) ...); \
	} \

enum class EventType
{
	none = -1,
	network_player,
	gamelifecycle_change,
	game_loop,
	server_command,
	map_load,
	countdown_start,
	player_control,
	blue_screen,
	player_spawn,
	object_damage
};

enum class EventExecutionType
{
	execute_before = 0,
	execute_after
};

class EventCallback
{
public:
	void* callback;
	EventType type;
	EventExecutionType executionType;
	bool runOnce;
	bool hasRun = false;

	EventCallback(void* _callback, EventType _type, EventExecutionType _executionType = EventExecutionType::execute_after, bool _runOnce = false) :
		callback(_callback),
		type(_type),
		executionType(_executionType),
		runOnce(_runOnce)
	{
	}
};

extern std::map<EventType, std::vector<EventCallback>> event_map;

namespace EventHandler
{
	enum class NetworkPlayerEventType
	{
		add,
		remove
	};

	using CountdownStartEventCallback = void(*)();
	using GameLifeCycleEventCallback = void(*)(e_game_life_cycle state);
	using NetworkPlayerEventCallback = void(*)(int peerIndex, NetworkPlayerEventType type);
	using GameLoopEventCallback = void(*)();
	using ServerCommandEventCallback = void(*)(ServerConsole::e_server_console_commands command);
	using PlayerControlEventCallback = void(*)(float* yaw, float* pitch);
	using MapLoadEventCallback = void(*)(e_engine_type type);
	using BlueScreenEventCallback = void(*)();
	using PlayerSpawnEventCallback = void(*)(datum PlayerDatum);
	using ObjectDamageEventCallback = void(*)(datum PlayerDatum, datum KillerDatum);

	static const char* get_event_name(EventType event_type)
	{
		switch (event_type)
		{
		case EventType::network_player:
			return STRINGIFY(EventType::network_player);
		case EventType::gamelifecycle_change:
			return STRINGIFY(EventType::gamelifecycle_change);
		case EventType::game_loop:
			return STRINGIFY(EventType::game_loop);
		case EventType::server_command:
			return STRINGIFY(EventType::server_command);
		case EventType::map_load:
			return STRINGIFY(EventType::map_load);
		case EventType::countdown_start:
			return STRINGIFY(EventType::countdown_start);
		case EventType::player_control:
			return STRINGIFY(EventType::player_control);
		case EventType::blue_screen:
			return STRINGIFY(EventType::blue_screen);
		case EventType::player_spawn:
			return STRINGIFY(EventType::player_spawn);
		case EventType::object_damage:
			return STRINGIFY(EventType::object_damage);
		case EventType::none:
			return STRINGIFY(EventType::none);
		default:
			return "<UNKNOWN EVENT>";
		}
	}

	static const char* get_event_execution_type(EventExecutionType event_execution_type)
	{
		switch (event_execution_type)
		{
		case EventExecutionType::execute_before:
			return STRINGIFY(EventExecutionType::execute_before);
		case EventExecutionType::execute_after:
			return STRINGIFY(EventExecutionType::execute_after);
		default:
			return "<UNKNOWN EVENT EXECUTION TYPE>";
		}
	}

	/**
	 * \brief Grabs the vector from the event map that relates to the corresponding event type
	 * \param type the event type
	 * \return std::vector<EventCallback<void*>>
	 */
	static std::vector<EventCallback>* get_vector(EventType type)
	{
		auto event = event_map.find(type);
		if (event == event_map.end())
		{
			//If the map doesn't contain a vector for the type create one
			event_map.emplace(std::make_pair(type, std::vector<EventCallback>()));
			return &event_map[type];
		}

		return &event->second;
	}

	/**
	 * \brief This function will remove a callback with the matching properties
	 * \tparam T The type alias for the callback to be removed
	 * \param callback pointer to the callback to be removed
	 * \param execution_type the execution type of the method to be removed
	 */
	static void remove_callback(void* callback, EventType event_type, EventExecutionType execution_type)
	{
		auto events = get_vector(event_type);
		for (auto it = events->begin(); it != events->end(); ++it)
		{
			if (it->callback == callback && it->type == event_type && it->executionType == execution_type)
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
	static void cleanup_callbacks(EventType event_type, EventExecutionType execution_type)
	{
		std::vector<EventCallback>* events = get_vector(event_type);
		auto it = events->begin();
		while (it != events->end())
		{
			if (it->type == event_type && it->runOnce && it->hasRun && it->executionType == execution_type)
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
	 * \param run_once flags the callback to only be ran once and then erased afterwards.
	 */
	static void register_callback(void* callback, EventType event_type, EventExecutionType execution_type = EventExecutionType::execute_after, bool run_once = false)
	{
		//Prevent duplicate events
		remove_callback(callback, event_type, execution_type);

		get_vector(event_type)->emplace_back(callback, event_type, execution_type, run_once);
	}
	/**
	 * \brief Executes the callbacks based off the given parameters
	 * \tparam T The Event Alias type of callback's to execute
	 * \tparam Args Do not pass anything to this template
	 * \param execution_type
	 * \param args Any arguments that need to be forwarded to the callback's
	 */
	template<typename T, typename ... Args>
	static void execute_callback(EventExecutionType event_execution_type, EventType event_type, Args&& ... args)
	{
		auto execute_callbacks_internal = [&](EventExecutionType execution_type)
		{
			std::vector<EventCallback>* events = get_vector(event_type);
			for (auto it = events->begin(); it != events->end(); ++it) {
				if (it->type == event_type && it->executionType == execution_type)
				{
					((T)it->callback)(std::forward<Args>(args) ...);
					it->hasRun = true;
				}
			}
			cleanup_callbacks(event_type, execution_type);
		};

		/*LOG_TRACE_GAME("{} executing callback {} at execution time of: {}", 
			__FUNCTION__, 
			get_event_name(event_type),
			get_event_execution_type(event_execution_type));*/

		execute_callbacks_internal(event_execution_type);
		//When trying to debug issues in threaded callbacks switch out the two lines, helps trace call stacks
		//execute_internal(execution_type, true);
		//std::thread(execute_internal, execution_type, true).detach();
	}

	REGISTER_EVENT_EXECUTE_METHOD(NetworkPlayerEventExecute, EventType::network_player, NetworkPlayerEventCallback);
	REGISTER_EVENT_EXECUTE_METHOD(GameLifeCycleEventExecute, EventType::gamelifecycle_change, GameLifeCycleEventCallback);
	REGISTER_EVENT_EXECUTE_METHOD(GameLoopEventExecute, EventType::game_loop, GameLoopEventCallback);
	REGISTER_EVENT_EXECUTE_METHOD(ServerCommandEventExecute, EventType::server_command, ServerCommandEventCallback);
	REGISTER_EVENT_EXECUTE_METHOD(MapLoadEventExecute, EventType::map_load, MapLoadEventCallback);
	REGISTER_EVENT_EXECUTE_METHOD(CountdownStartEventExecute, EventType::countdown_start, CountdownStartEventCallback);
	REGISTER_EVENT_EXECUTE_METHOD(PlayerControlEventExecute, EventType::player_control, PlayerControlEventCallback);
	REGISTER_EVENT_EXECUTE_METHOD(BlueScreenEventExecute, EventType::blue_screen, BlueScreenEventCallback);
	REGISTER_EVENT_EXECUTE_METHOD(PlayerSpawnEventExecute, EventType::player_spawn, PlayerSpawnEventCallback);
	REGISTER_EVENT_EXECUTE_METHOD(ObjectDamageEventExecute, EventType::object_damage, ObjectDamageEventCallback);
}