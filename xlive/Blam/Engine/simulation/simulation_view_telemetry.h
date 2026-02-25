#pragma once
#include "game/player_constants.h"

/* structures */

struct s_simulation_view_player_telemetry_data
{
	int32 absolute_player_index;
	datum controlled_entity_index;
	real_point3d position;
	real_vector3d desired_aiming_vector;
	int8 desired_zoom_level;
};
ASSERT_STRUCT_SIZE(s_simulation_view_player_telemetry_data, 36);

struct s_simulation_view_telemetry_data
{
	void* provider;
	bool joining;
	uint32 player_acknowledged_mask;
	int32 number_of_players;
	s_simulation_view_player_telemetry_data players[k_number_of_users];
};
ASSERT_STRUCT_SIZE(s_simulation_view_telemetry_data, 160);

struct c_simulation_view_telemetry_provider
{
	int m_field_0;
	class c_simulation_view* m_view;
	s_simulation_view_telemetry_data m_telemetry_data;
};
ASSERT_STRUCT_SIZE(c_simulation_view_telemetry_provider, 168);
