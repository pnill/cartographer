#include "stdafx.h"
#include "main_game.h"

#include "Blam/Engine/cache/cache_files.h"
#include "Blam/Engine/cseries/cseries_strings.h"
#include "Blam/Engine/game/game.h"
#include "Blam/Engine/Networking/Session/NetworkSession.h"
#include "Blam/Engine/saved_games/game_variant.h"

#include "Util/Hooks/Hook.h"

s_game_options g_main_game_launch_options;
DWORD g_main_game_launch_user_count;

// Setup default values for the options structure depending on the game mode set
void main_game_launch_setup_game_mode_details(void);

void main_game_initialize()
{
    game_options_new(&g_main_game_launch_options);
    return;
}

void main_game_launch_set_map_name(const char* map_name)
{
    MultiByteToWideChar(CP_UTF8, 0, map_name, -1, g_main_game_launch_options.scenario_path.get_string(), 260);
    return;
}

// TODO rewrite the obfuscated function
void main_game_change(const s_game_options* options)
{
    typedef void(__stdcall* main_game_change_t)(const s_game_options*);
    auto p_main_game_change = Memory::GetAddress<main_game_change_t>(0x89BA, 0x1E4EC);
    p_main_game_change(options);
    return;
}

void game_set_difficulty(short difficulty)
{
    if (difficulty < 4)
    {
        g_main_game_launch_options.difficulty = difficulty;
        g_main_game_launch_options.game_mode = _game_mode_campaign;
    }
    return;
}

void main_game_launch_set_coop_player_count(int player_count)
{
    if (player_count <= k_number_of_users)
    {
        g_main_game_launch_options.game_mode = _game_mode_campaign;
        g_main_game_launch_options.coop = player_count > 1;
        g_main_game_launch_user_count = player_count;
    }
    return;
}

void main_game_launch_set_multiplayer_splitscreen_count(int player_count)
{
    if (player_count <= k_number_of_users)
    {
        g_main_game_launch_options.game_mode = _game_mode_multiplayer;
        g_main_game_launch_user_count = player_count;
        if (g_main_game_launch_options.game_variant.variant_game_engine_index == _game_engine_campaign)
        {
            game_variant_build_default(&g_main_game_launch_options.game_variant, _game_variant_description_slayer);
            g_main_game_launch_options.game_variant.round_time_limit = 0;
        }
    }
    return;
}

void main_game_launch_set_multiplayer_variant(const char* variant_name)
{
    s_variant_description_map variants[7];
    variants[_game_variant_description_slayer] = { "slayer", _game_variant_description_slayer };
    variants[_game_variant_description_oddball] = { "oddball", _game_variant_description_oddball };
    variants[_game_variant_description_juggernaut] = { "juggernaut", _game_variant_description_juggernaut };
    variants[_game_variant_description_king] = { "king", _game_variant_description_king };
    variants[_game_variant_description_ctf] = { "ctf", _game_variant_description_ctf };
    variants[_game_variant_description_invasion] = { "invasion", _game_variant_description_invasion };
    variants[_game_variant_description_territories] = { "territories", _game_variant_description_territories };
    
    size_t i;
    for (i = 0; i < k_variant_count; ++i)
    {
        if (!csstricmp(variant_name, variants[i].name))
        {
            break;
        }
    }

    if (i == k_variant_count)
    {
        LOG_ERROR_GAME("invalid variant name [%s] provided, defaulting to slayer", variant_name);
        game_variant_build_default(&g_main_game_launch_options.game_variant, _game_variant_description_slayer);
    }
    else
    {
        game_variant_build_default(&g_main_game_launch_options.game_variant, variants[i].index);
    }

    g_main_game_launch_options.game_variant.round_time_limit = 0;
    g_main_game_launch_options.game_mode = _game_mode_multiplayer;
    return;
}

void main_game_launch_set_game_mode(int game_mode)
{
    if (game_mode < k_game_mode_count)
    {
        g_main_game_launch_options.game_mode = (e_game_mode)game_mode;
    }
    else
    {
        LOG_ERROR_GAME("invalid game mode [%d] provided", game_mode);
    }
    return;
}

void main_game_launch_setup_game_mode_details(void)
{
    switch (g_main_game_launch_options.game_mode)
    {
    case _game_mode_campaign:
    {
        if (g_main_game_launch_options.difficulty >= 0)
        {
            if (g_main_game_launch_options.difficulty > 3)
            {
                g_main_game_launch_options.difficulty = 3;
            }
        }
        else
        {
            g_main_game_launch_options.difficulty = 0;
        }

        if (!g_main_game_launch_options.coop)
        {
            g_main_game_launch_user_count = 1;
        }
        else if (g_main_game_launch_user_count < 1)
        {
            g_main_game_launch_user_count = 1;
        }
        else if (g_main_game_launch_user_count > 2)
        {
            g_main_game_launch_user_count = 2;
        }
        break;
    }
    case _game_mode_multiplayer:
    {
        if (g_main_game_launch_user_count >= 1)
        {
            if (g_main_game_launch_user_count > k_number_of_users)
            {
                g_main_game_launch_user_count = k_number_of_users;
            }
        }
        else
        {
            g_main_game_launch_user_count = 1;
        }
        break;
    }
    case _game_mode_ui_shell:
    {
        g_main_game_launch_user_count = 1;
        g_main_game_launch_options.menu_context = 7;
        break;
    }
    default:
    {
        LOG_ERROR_GAME("main_game_launch: unknown game mode %d!", g_main_game_launch_options.game_mode);
    }
    }
    
    return;
}

void main_game_launch(const char* map_name)
{
    cache_file_map_clear_all_failures();
    main_game_launch_set_map_name(map_name);
    main_game_launch_setup_game_mode_details();
    game_options_setup_default_players(g_main_game_launch_user_count, &g_main_game_launch_options);
    main_game_change(&g_main_game_launch_options);
    return;
}

void main_game_apply_patches(void)
{
    // Patch the empty function in the run_main_loop function with the proper function call
    PatchCall(Memory::GetAddress(0x39E38), main_game_initialize);
    return;
}
