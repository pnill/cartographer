#pragma once

/* structures */

struct ai_player_state
{
    datum player_index;
    datum recent_vehicle_index;
    int16 recent_seat_index;
    int16 recent_vehicle_ticks;
    uint16 ticks_since_shooting;
    uint16 ticks_since_player_threatening;
    int32 last_friendly_vehicle_shoot_time;
    datum last_friendly_vehicle_shoot_index;
    int8 last_vehicle_airborne_ticks;
    bool crazy_vehicle_vocalized;
    int8 pad_1A[2];
};
ASSERT_STRUCT_SIZE(ai_player_state, 28);

/* prototypes */

void ai_player_apply_patches(void);

void __cdecl ai_players_initialize_for_new_map(void);
