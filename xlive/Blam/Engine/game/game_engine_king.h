#pragma once
#include "game_engine_default.h"
#include "math/color_math.h"
#include "simulation/game_interface/simulation_game_entities.h"

class c_king_engine_globals
{
public:
	int32 m_netgame_flag_indices[16];
	int32 m_differences[8];
	int32 m_even_count;
	real_point2d m_hill_points[32];
	real_point3d m_center;
	real32 m_max_xy_distance;
	real32 m_min_z;
	real32 m_max_z;
	real32 m_total_perimeter;
	uint32 m_team_control_accumulator[k_game_multiplayer_team_count];
	int32 m_hill_id;
	uint32 m_ticks_till_hill_move;
	uint16 m_players_in_hill;
	uint16 m_pad;
	real_rgb_color m_color_1;
	real_rgb_color m_color_2;
	real_rgb_color m_color_3;
	real_rgb_color m_color_4;

	void setup();
	void setup_colors();
	int32 get_next_hill_index() const;
	void setup_points(uint32 hill_index);

	static uint32 get_hill_count();
	static void set_hill_count(uint32 count);
	static uint16* get_hill_indices();
	static uint16 get_teams_mask_from_players_mask(uint16 players_mask);
	static void alert_players_of_players_in_hill(uint16 old_players_in_hill, uint16 new_players_in_hill);

};
ASSERT_STRUCT_SIZE(c_king_engine_globals, 476)


class c_king_engine : public c_game_engine
{
public:
	virtual e_game_engine_type get_type() override;
	virtual bool setup() override;
	virtual bool function_4() override;
	virtual void send_game_start_event(datum player_index) override;
	virtual void function_13() override;
	virtual void function_14(datum player_index) override;
	virtual void render_game_engine_hud_elements(uint32 user_index) override;
	virtual void function_16(datum player_index) override;
	virtual void update() override;
	virtual void get_multiplayer_score_string(wchar_t* out_string) override;
	virtual void player_killed(datum killing_player, datum killed_player, bool suicide, int32 unk_index) override;
	virtual void function_33(datum player_index, void* unk) override;
	virtual bool function_35(int32 unk_index) override;
	virtual bool test_variant_engine_flag(datum player_index, e_game_engine_variant_flag_test_type type) override;
	virtual e_simulation_entity_type get_game_engine_entity_type() override;
	virtual void function_41() override;
	virtual void set_simulation_baseline_data(int32 unused, void* state_data) override;
	virtual void build_simulation_update(uint32* unk, int32 unused, void* state_data) override;
	virtual bool apply_simulation_update(uint32 flags, int32 unused, void* state_data) override;
};