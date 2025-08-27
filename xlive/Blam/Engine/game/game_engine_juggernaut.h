#pragma once
#include "game_engine_default.h"
#include "simulation/game_interface/simulation_game_entities.h"


class c_juggernaut_engine : c_game_engine
{
public:
	virtual e_game_engine_type get_type() override;
	virtual bool setup() override;
	virtual void player_join(datum player_index) override;
	virtual void send_game_start_event(datum player_index) override;
	virtual void player_leave(datum player_index) override;
	virtual void swap_player_indices(uint32 old_index, uint32 new_index) override;
	virtual void function_13() override;
	virtual void function_14(datum player_index) override;
	virtual void render_game_engine_hud_elements(uint32 user_index) override;
	virtual void function_16(datum player_index) override;
	virtual void update() override;
	virtual real32 get_player_speed_modifier(datum player_index) override;
	virtual bool is_team_enemy(e_game_team a, e_game_team b) override;
	virtual void player_killed(datum killing_player, datum killed_player, bool suicide, int32 unk_index) override;
	virtual bool function_35(int32 unk_index) override;
	virtual bool test_variant_engine_flag(datum player_index, e_game_engine_variant_flag_test_type type) override;
	virtual void get_player_state_index(datum player_index, bool* always_returned_true) override;
	virtual e_simulation_entity_type get_game_engine_entity_type() override;
	virtual void function_41() override;
	virtual void set_simulation_baseline_data(int32 unused, void* state_data) override;
	virtual void build_simulation_update(uint32* unk, int32 unused, void* state_data) override;
	virtual bool apply_simulation_update(uint32 flags, int32 unused, void* state_data) override;
};