#pragma once
#include "game_engine_default.h"
#include "simulation/game_interface/simulation_game_entities.h"


#define k_ctf_flag_count 9

class c_ctf_engine : public c_game_engine
{
public:
	virtual e_game_engine_type get_type() override;
	virtual bool setup() override;
	virtual bool verify_netpoint(uint32 netpoint_index) override;
	virtual void send_game_start_event(datum player_index) override;
	virtual void function_13() override;
	virtual void function_14(datum player_index) override;
	virtual void render_game_engine_hud_elements(uint32 user_index) override;
	virtual void function_16(datum player_index) override;
	virtual bool player_can_interact_with_weapon(datum player_index, datum weapon_index) override;
	virtual void handle_player_objective_touch_interaction(datum player_index, datum object_index) override;
	virtual void update() override;
	virtual real32 get_player_speed_modifier(datum player_index) override;
	virtual uint32 function_21(datum object_index) override;
	virtual void update_object_color_change(datum object_index) override;
	virtual void handle_object_taken_event(datum weapon_index, datum biped_index) override;
	virtual void handle_object_dropped_event(datum weapon_index, datum biped_index) override;
	virtual int32 get_sudden_death_timer(int32 time_remaining_in_ticks, bool unk, bool unk_2) override;
	virtual e_game_team get_primary_team_index() override;
	virtual void player_killed(datum killing_player, datum killed_player, bool suicide, int32 unk_index) override;
	virtual int32 get_player_killed_event_id(datum player_index, datum player_index_2, bool a3) override;
	virtual bool function_34(datum player_index, void* unk) override;
	virtual bool function_35(int32 unk_index) override;
	virtual bool test_variant_engine_flag(datum player_index, e_game_engine_variant_flag_test_type type) override;
	virtual void function_37(int32 unk_always_1) override;
	virtual void get_player_state_index(datum player_index, bool* always_returned_true) override;
	virtual e_simulation_entity_type get_game_engine_entity_type() override;
	virtual void function_41() override;
	virtual void set_simulation_baseline_data(int32 unused, void* state_data) override;
	virtual void build_simulation_update(uint32* unk, int32 unused, void* state_data) override;
	virtual bool apply_simulation_update(uint32 flags, int32 unused, void* state_data) override;
};