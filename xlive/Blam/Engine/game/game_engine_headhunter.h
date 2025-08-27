#pragma once
#include "game_engine_default.h"
#include "game_engine_king.h"
#include "players.h"

class c_headhunter_engine_globals : public c_king_engine_globals
{
public:
	uint8 m_player_skull_count[k_maximum_players];
};

class c_headhunter_engine : public c_king_engine
{
public:
	virtual e_game_engine_type get_type() override;
	virtual bool setup() override;
	virtual bool function_4();
	virtual void send_game_start_event(datum player_index) override;
	virtual void swap_player_indices(uint32 old_index, uint32 new_index) override;
	virtual void render_game_engine_hud_elements(uint32 user_index) override;
	virtual real32 get_player_speed_modifier(datum player_index) override;
	virtual void player_killed(datum killing_player, datum killed_player, bool suicide, int32 unk_index) override;
	virtual bool player_can_interact_with_weapon(datum player_index, datum weapon_index) override;
	virtual bool function_34(datum player_index, void* unk) override;
	virtual void update() override;
	virtual void set_simulation_baseline_data(int32 unused, void* state_data) override;
	virtual void build_simulation_update(uint32* update_mask, int32 unused, void* state_data) override;
	virtual bool apply_simulation_update(uint32 update_mask, int32 unused, void* state_data) override;
	virtual e_simulation_entity_type get_game_engine_entity_type() override;
private:
	static void draw_skulls_carried_string(uint32 user_index);
	static uint8 variant_get_max_heads_carried();
	static void update_scores();
};

c_headhunter_engine* get_global_headhunter_engine_instance();