#pragma once
#include "game_allegiance.h"
#include "saved_games/game_variant.h"
#include "simulation/game_interface/simulation_game_entities.h"

enum e_simulation_game_engine_update : uint32
{

	k_simulation_game_engine_update_flag_count = 31
};

enum e_game_engine_variant_flag_test_type : uint32
{
	_game_engine_variant_flag_test_extra_damage,
	_game_engine_variant_flag_test_damage_resistant,
	_game_engine_variant_flag_test_always_invisible
};

class c_game_engine
{
public:
	virtual e_game_engine_type get_type();
	virtual bool setup();
	virtual bool cleanup();
	virtual bool function_4();
	virtual bool verify_netpoint(uint32 netpoint_index);
	virtual void player_join(datum player_index);
	virtual void send_game_start_event(datum player_index) = 0;
	virtual void player_leave(datum player_index);
	virtual void player_rejoin(datum player_index);
	virtual void swap_player_indices(uint32 old_index, uint32 new_index);
	virtual void player_team_change(datum player_index);
	virtual void game_start();
	virtual void function_13() = 0;
	virtual void function_14(datum player_index);
	virtual void render_game_engine_hud_elements(uint32 user_index) = 0;
	virtual void function_16(datum player_index) = 0;
	virtual bool player_can_interact_with_weapon(datum player_index, datum weapon_index);
	virtual void handle_player_objective_touch_interaction(datum player_index, datum object_index);
	virtual void update();
	virtual real32 get_player_speed_modifier(datum player_index);
	virtual uint32 function_21(datum object_index);
	virtual void update_object_color_change(datum object_index);
	virtual void function_23(datum index);
	virtual void handle_object_taken_event(datum weapon_index, datum biped_index);
	virtual void handle_object_dropped_event(datum weapon_index, datum biped_index);
	virtual int32 get_sudden_death_timer(int32 time_remaining_in_ticks, bool unk, bool unk_2);
	virtual e_game_team get_primary_team_index();
	virtual bool is_team_enemy(e_game_team a, e_game_team b);
	virtual void get_multiplayer_score_string(wchar_t* out_string);
	virtual void function_30(int32 a1, int32 a2, int32 a3);
	virtual void player_killed(datum killing_player, datum killed_player, bool suicide, int32 unk_index) = 0;
	virtual int32 get_player_killed_event_id(datum player_index, datum player_index_2, bool a3);
	virtual void function_33(datum player_index, void* unk);
	virtual bool function_34(datum player_index, void* unk);
	virtual bool function_35(int32 unk_index) = 0;
	virtual bool test_variant_engine_flag(datum player_index, e_game_engine_variant_flag_test_type type);
	virtual void function_37(int32 unk_always_1);
	virtual void get_player_state_index(datum player_index, bool* always_returned_true);
	virtual bool should_garbage_collect(datum object_index);
	virtual e_simulation_entity_type get_game_engine_entity_type();
	virtual void function_41() = 0;
	virtual void function_42(void* unk);
	virtual void function_43(uint32 flags, void* unk, void* unk_2);
	virtual bool function_44(uint32 flags, void* unk);
	virtual void set_simulation_baseline_data(int32 unused, void* state_data) = 0;
	virtual void build_simulation_update(uint32* update_mask, int32 unused, void* state_data) = 0;
	virtual bool apply_simulation_update(uint32 update_mask, int32 unused, void* state_data) = 0;
	virtual void set_player_simulation_baseline_data(int32 unused, int32 unused_2, void* unk);
	virtual void build_player_simulation_update(int16 abs_player_index, int32* update_mask, int32 unused, void* unk_2);
	virtual bool apply_player_simulation_update(int16 abs_player_index, int32* update_mask, int32 unused, void* unk);
	virtual uint32 get_territory_name(wchar_t* a1, int32 a2, int32 a3, wchar_t* a4, wchar_t* a5);
};