#pragma once
#include "Blam/Engine/Networking/Session/NetworkSession.h"



enum e_game_life_cycle : int
{
	_life_cycle_none,
	_life_cycle_pre_game,
	_life_cycle_start_game,
	_life_cycle_in_game,
	_life_cycle_post_game,
	_life_cycle_joining,
	_life_cycle_matchmaking,
	k_life_cycle_count
};

e_game_life_cycle get_game_life_cycle();
typedef bool (*life_cycle_update)(void* life_cycle_handler);
typedef void (*life_cycle_initialize)(void* life_cycle_handler_requested, void* life_cycle_handler_current, int32 unk_1, void* unk_2);
typedef void (*life_cycle_dispose)(void* life_cycle_handler_current, void* life_cycle_handler_requested);


class c_game_life_cycle_handler_functions
{
public:
	life_cycle_update update;
	life_cycle_initialize initialize;
	life_cycle_dispose dispose;
};

class c_game_life_cycle_handler
{
public:
	c_game_life_cycle_handler_functions* functions;
	e_game_life_cycle life_cycle;
	void* life_cycle_manager;
	bool unk_bool;
	void initialize(void* life_cycle_manager, e_game_life_cycle life_cycle, bool unk_bool);
};

class __declspec(align(4)) c_game_life_cycle_manager
{
public:
	e_game_life_cycle life_cycle_state;
	c_game_life_cycle_handler* life_cycle_handlers[e_game_life_cycle::k_life_cycle_count];
	void* network_session_manager;
	void** network_sessions;
	void* text_chat_manager_maybe;
	void* network_message_gateway;
	void* network_observer;
	bool life_cycle_changing;
	bool life_cycle_updating;
	bool update_requested;
	e_game_life_cycle requested_life_cycle;
	int32 field_3C;
	void* field_40;
	c_game_life_cycle_manager(
		void* network_message_gateway, 
		void* network_observer, 
		void* network_session_manager, 
		void** network_session, 
		void* text_chat_manager_maybe)
	{
		this->life_cycle_state = _life_cycle_none;
		this->network_message_gateway = network_message_gateway;
		this->network_observer = network_observer;
		this->network_session_manager = network_session_manager;
		this->network_sessions = network_session;
		this->text_chat_manager_maybe = text_chat_manager_maybe;
		this->life_cycle_updating = 0;
		this->life_cycle_changing = false;
		this->update_requested = false;
		this->requested_life_cycle = _life_cycle_none;
		this->field_3C = 0;
		this->field_40 = 0;
	}

	static bool game_life_cycle_initialized();
	static c_game_life_cycle_manager* get();
	e_game_life_cycle get_life_cycle() const;
	bool state_is_joining() const;
	bool state_is_in_game() const;
	void request_state_change(e_game_life_cycle requested_state, int unk_int, void* unk_ptr);
	void update();
};
CHECK_STRUCT_SIZE(c_game_life_cycle_manager, 0x44);

namespace game_life_cycle
{
	void apply_patches();
}