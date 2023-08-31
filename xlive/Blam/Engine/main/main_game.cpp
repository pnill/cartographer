#include "stdafx.h"
#include "main_game.h"

#include "Blam/Engine/cache/cache_files.h"
#include "Blam/Engine/cseries/cseries.h"
#include "Blam/Engine/cseries/cseries_strings.h"
#include "Blam/Engine/game/game.h"
#include "Blam/Engine/Networking/Session/NetworkSession.h"
#include "Blam/Engine/saved_games/game_variant.h"

#include "Util/Hooks/Hook.h"

s_game_options g_main_game_launch_options = {};
int g_main_game_launch_user_count = 1;

// Setup default values for the options structure depending on the game mode set
void main_game_launch_setup_game_mode_details(void);
// Perform validation on the game launch options for campaign
void main_game_launch_set_campaign_details(void);
// Perform validation on the game launch options for multiplayer
void main_game_launch_set_multiplayer_details(void);
// Set default details for the mainmenu
void main_game_launch_set_ui_shell_details(void);

void main_game_initialize(void)
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
bool main_game_change(const s_game_options* options)
{
    typedef bool(__cdecl* main_game_change_t)(const s_game_options*);
    auto p_main_game_change = Memory::GetAddress<main_game_change_t>(0x89BA, 0x1E4EC);
    return p_main_game_change(options);
}

void main_game_launch_set_difficulty(short difficulty)
{
    if (!VALID_INDEX(difficulty, k_campaign_difficulty_levels_count))
    {
        LOG_ERROR_GAME("main_game_launch_set_difficulty: invalid difficulty {} (must be from 0-{})", difficulty, k_campaign_difficulty_levels_count - 1);
    }
    else
    {
        g_main_game_launch_options.difficulty = difficulty;
        g_main_game_launch_options.game_mode = _game_mode_campaign;
    }
    return;
}

void main_game_launch_set_coop_player_count(int player_count)
{
    if (!IN_RANGE_INCLUSIVE(player_count, 1, k_number_of_users))
    {
        LOG_ERROR_GAME("main_game_launch_set_coop_player_count: invalid player count {} (must be from 1-{})", player_count, k_number_of_users);
    }
    else
    {
        g_main_game_launch_options.game_mode = _game_mode_campaign;
        g_main_game_launch_options.coop = player_count > 1;
        g_main_game_launch_user_count = player_count;
    }
    return;
}

void main_game_launch_set_multiplayer_splitscreen_count(int player_count)
{
    if (!IN_RANGE_INCLUSIVE(player_count, 1, k_number_of_users))
    {
        LOG_ERROR_GAME("main_game_launch_set_multiplayer_splitscreen_count: invalid player count {} (must be from 1-{})", player_count, k_number_of_users);
    }
    else
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
    s_variant_description_map variants[k_variant_count];
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
        LOG_ERROR_GAME("invalid variant name [{}] provided, defaulting to slayer", variant_name);
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
    if (!IN_RANGE_INCLUSIVE(game_mode, 1, (int)k_game_mode_count - 1))
    {
        LOG_ERROR_GAME("invalid game mode [{}] provided", game_mode);
    }
    else
    {
        g_main_game_launch_options.game_mode = (e_game_mode)game_mode;
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

void main_game_launch_setup_game_mode_details(void)
{
    switch (g_main_game_launch_options.game_mode)
    {
    case _game_mode_campaign:
    {
        main_game_launch_set_campaign_details();
        break;
    }
    case _game_mode_multiplayer:
    {
        main_game_launch_set_multiplayer_details();
        break;
    }
    case _game_mode_ui_shell:
    {
        main_game_launch_set_ui_shell_details();
        break;
    }
    default:
    {
        LOG_ERROR_GAME("main_game_launch: unknown game mode {}!", g_main_game_launch_options.game_mode);
    }
    }

    return;
}

void main_game_launch_set_campaign_details(void)
{
    if (!IN_RANGE_INCLUSIVE(g_main_game_launch_options.difficulty, 0, k_campaign_difficulty_levels_count))
    {
        // If higher than legendary set to legendary
        if (g_main_game_launch_options.difficulty >= k_campaign_difficulty_levels_count)
        {
            g_main_game_launch_options.difficulty = 3;
        }
        // If lower than easy set to easy
        else if (g_main_game_launch_options.difficulty < 0)
        {
            g_main_game_launch_options.difficulty = 0;
        }
    }


    if (!IN_RANGE_INCLUSIVE(g_main_game_launch_user_count, 1, k_number_of_users))
    {
        g_main_game_launch_user_count = 1;
    }
    else if (!g_main_game_launch_options.coop)
    {
        g_main_game_launch_user_count = 1;
    }

    /*
    // Removed this so we can campaign games with more than 4 local users
    // 4 Player doesn't currently work in most campaign maps
    else if (g_main_game_launch_user_count > 2)
    {
        g_main_game_launch_user_count = 2;
    }
    */
    return;
}

void main_game_launch_set_multiplayer_details(void)
{
    if (!IN_RANGE_INCLUSIVE(g_main_game_launch_user_count, 1, k_number_of_users))
    {
        if (g_main_game_launch_user_count > k_number_of_users)
        {
            g_main_game_launch_user_count = k_number_of_users;
        }
        else if (g_main_game_launch_user_count < 1)
        {
            g_main_game_launch_user_count = 1;
        }
    }
    return;
}

void main_game_launch_set_ui_shell_details(void)
{
    g_main_game_launch_user_count = 1;
    g_main_game_launch_options.menu_context = 7;
    return;
}
