#include "stdafx.h"
#include "simulation_players.h"

#include "simulation.h"

#include "cartographer/discord/discord_interface.h"

/* prototypes */

void simulation_player_joined_game_patch_calls(void);
void simulation_player_left_game_patch_calls(void);

/* public code */

void simulation_players_apply_patches(void)
{
    simulation_player_joined_game_patch_calls();
    simulation_player_left_game_patch_calls();
    return;
}

bool __cdecl simulation_players_apply_update(simulation_player_update* player_update)
{
	return INVOKE(0x1E22E2, 0x1C930E, simulation_players_apply_update, player_update);
}

void __cdecl simulation_player_joined_game(datum player_index)
{
    s_simulation_globals* g_simulation_globals = simulation_get_globals();
    ASSERT(g_simulation_globals->world);

    if (g_simulation_globals->engine_initialized && !g_simulation_globals->field_B)
    {
        g_simulation_globals->world->create_player(player_index);
        if (!Memory::IsDedicatedServer())
        {
            // Update discord player counts
            discord_interface_set_player_counts();
        }
    }
    return;
}

void __cdecl simulation_player_left_game(datum player_index)
{
    s_simulation_globals* g_simulation_globals = simulation_get_globals();
    ASSERT(g_simulation_globals->world);

    if (g_simulation_globals->engine_initialized && !g_simulation_globals->field_B)
    {
        g_simulation_globals->world->delete_player(player_index);
        if (!Memory::IsDedicatedServer())
        {
            // Update discord player counts
            discord_interface_set_player_counts();
        }
    }
    return;
}

/* private code */

void simulation_player_joined_game_patch_calls(void)
{
    PatchCall(Memory::GetAddress(0x56447, 0x5E93F), simulation_player_joined_game);
    PatchCall(Memory::GetAddress(0x5647F, 0x5E977), simulation_player_joined_game);
    PatchCall(Memory::GetAddress(0x57E85, 0x6037D), simulation_player_joined_game);
    return;
}

void simulation_player_left_game_patch_calls(void)
{
    PatchCall(Memory::GetAddress(0x5633A, 0x5E832), simulation_player_left_game);
    return;
}
