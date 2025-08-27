#pragma once
#include "game_engine_default.h"
#include "simulation/game_interface/simulation_game_entities.h"

class c_slayer_engine : public c_game_engine
{
public:
	virtual e_game_engine_type get_type() override;
	virtual bool setup() override;
	virtual void player_join(datum player_index) override;
	virtual void send_game_start_event(datum player_index) override;
	virtual void swap_player_indices(uint32 old_index, uint32 new_index) override;
	virtual void function_13() override;
	virtual void render_game_engine_hud_elements(uint32 user_index) override;
	virtual void function_16(datum player_index) override;
	virtual void player_killed(datum killing_player, datum killed_player, bool suicide, int32 unk_index) override;
	virtual bool function_35(int32 unk_index) override;
	virtual e_simulation_entity_type get_game_engine_entity_type() override;
	virtual void function_41() override;
	virtual void set_simulation_baseline_data(int32 unused, void* state_data) override;
	virtual void build_simulation_update(uint32* unk, int32 unused, void* state_data) override;
	virtual bool apply_simulation_update(uint32 flags, int32 unused, void* state_data) override;
};