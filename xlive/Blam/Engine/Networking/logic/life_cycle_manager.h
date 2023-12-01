#pragma once
#include "Blam/Engine/Networking/Session/NetworkSession.h"

enum e_game_life_cycle : int32
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

e_game_life_cycle get_game_life_cycle(void);
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

struct c_game_life_cycle_handler_none : c_game_life_cycle_handler
{
	int8 gap[15];
};
CHECK_STRUCT_SIZE(c_game_life_cycle_handler_none, 32);

struct c_game_life_cycle_handler_pre_game : c_game_life_cycle_handler
{
	int32 count_down_network_time;
	int32 count_down_network_session;
	int32 field_18;
	int8 gap_1C[15];
};
CHECK_STRUCT_SIZE(c_game_life_cycle_handler_pre_game, 44);

struct c_game_life_cycle_handler_start_game : c_game_life_cycle_handler
{
	int8 field_10;
	int8 field_11;
	int8 gap_12[25];
};
CHECK_STRUCT_SIZE(c_game_life_cycle_handler_start_game, 44);

struct c_game_life_cycle_handler_in_game : c_game_life_cycle_handler
{
	int64 rand_bytes;
	int8 gap_18[15];
};
CHECK_STRUCT_SIZE(c_game_life_cycle_handler_in_game, 40);

struct c_game_life_cycle_handler_post_game : c_game_life_cycle_handler
{
	int8 gap[15];
};
CHECK_STRUCT_SIZE(c_game_life_cycle_handler_post_game, 32);

class c_game_life_cycle_handler_joining : c_game_life_cycle_handler
{
public:
	bool field_10;
	int8 field_11;
	bool field_12;
	int32 field_14;
	XNKID joining_xnkid;
	XNKEY joining_xnkey;
	XNADDR joining_xnaddr;
	int32 field_54;
	int32 joining_user_count;
	s_player_identifier player_identifiers[4];
	wchar_t player_names[4][16];
	int8 gapFC[128];
	int32 field_17C;
	bool field_180;
	int8 field_181;
	int8 gap_182[2];
	int32 field_184;
	int32 field_188;
	int32 field_18C;
	bool field_190;
	int8 field_191;
	bool field_192;
	int8 gap_193[5];
	int32 field_198;
	int8 gap_19C[3804];
	int32 field_1078;
	int8 gap_107C[4];
	int32 join_attempt_result_code;
	static void __cdecl check_joining_capability(void);
};
CHECK_STRUCT_SIZE(c_game_life_cycle_handler_joining, 4228);

struct  c_game_life_cycle_handler_match_making : c_game_life_cycle_handler
{
	void* some_function_pointer;
	int32 field_14;
};
CHECK_STRUCT_SIZE(c_game_life_cycle_handler_match_making, 24);

class c_game_life_cycle_manager
{
public:
	e_game_life_cycle life_cycle_state;
	c_game_life_cycle_handler* life_cycle_handlers[e_game_life_cycle::k_life_cycle_count];
	void* network_session_manager;
	s_network_session* network_session;
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
		s_network_session* network_session,
		void* text_chat_manager_maybe)
	{
		this->life_cycle_state = _life_cycle_none;
		this->network_message_gateway = network_message_gateway;
		this->network_observer = network_observer;
		this->network_session_manager = network_session_manager;
		this->network_session = network_session;
		this->text_chat_manager_maybe = text_chat_manager_maybe;
		this->life_cycle_updating = false;
		this->life_cycle_changing = false;
		this->update_requested = false;
		this->requested_life_cycle = _life_cycle_none;
		this->field_3C = 0;
		this->field_40 = NULL;
	}

	static bool game_life_cycle_initialized(void);
	static c_game_life_cycle_manager* get(void);
	e_game_life_cycle get_life_cycle(void) const;
	bool state_is_joining(void) const;
	bool state_is_in_game(void) const;
	void request_state_change(e_game_life_cycle requested_state, int unk_int, void* unk_ptr);
	
};
CHECK_STRUCT_SIZE(c_game_life_cycle_manager, 68);
