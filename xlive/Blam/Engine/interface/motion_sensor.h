#pragma once
#include "game/players.h"

enum e_motion_sensor_blip_size : int16
{
    _motion_sensor_blip_medium = 0,
    _motion_sensor_blip_small = 1,
    _motion_sensor_blip_large = 2
};

struct s_local_player_motion_sensor_sample
{
    byte gap[68];
};
ASSERT_STRUCT_SIZE(s_local_player_motion_sensor_sample, 0x44);

class c_local_player_motion_sensor_data
{
public:
    s_local_player_motion_sensor_sample samples[9];
    byte gap_264[130];
    int32 field_2E8;
    int32 field_2EC;

    void sort();
};
ASSERT_STRUCT_SIZE(c_local_player_motion_sensor_data, 0x2F0);

struct s_motion_sensor_globals
{
    c_local_player_motion_sensor_data local_players[k_number_of_users];
    int32 current_sample_index;
    int32 field_BC4;
};
ASSERT_STRUCT_SIZE(s_motion_sensor_globals, 0xBC8);

s_motion_sensor_globals* get_motion_sensor_globals();

void motion_sensor_update_with_delta(real32 delta);
void motion_sensor_apply_patches();
void motion_sensor_fix_size();