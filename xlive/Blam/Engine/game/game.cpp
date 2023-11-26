#include "stdafx.h"
#include "game.h"

#include "Blam/Engine/Networking/logic/life_cycle_manager.h"

s_main_game_globals* get_main_game_globals(void)
{
	return *Memory::GetAddress<s_main_game_globals**>(0x482D3C, 0x4CB520);
}

bool map_initialized(void)
{
	return get_main_game_globals() && get_main_game_globals()->map_active && get_main_game_globals()->active_structure_bsp_index != -1;
}

s_game_options* game_options_get(void)
{
	return &get_main_game_globals()->options;
}

s_game_variant* current_game_variant(void)
{
	return &game_options_get()->game_variant;
}

e_game_mode game_mode_get(void)
{
	return get_main_game_globals()->options.game_mode;
}

bool game_is_campaign(void)
{
	return game_options_get()->game_mode == _game_mode_campaign;
}

bool game_is_multiplayer(void)
{
	return game_options_get()->game_mode == _game_mode_multiplayer;
}

bool game_is_ui_shell(void)
{
	return game_options_get()->game_mode == _game_mode_ui_shell;
}

void __cdecl game_shell_set_in_progress()
{
	INVOKE(0x242E5B, 0, game_shell_set_in_progress);
}

bool game_is_predicted(void)
{
	return game_options_get()->simulation_type == _game_simulation_distributed_client;
}

bool game_in_progress(void)
{
	return get_main_game_globals() && get_main_game_globals()->game_in_progress;
}

bool game_is_authoritative(void)
{
	return game_options_get()->simulation_type != _game_simulation_distributed_client;
}

s_game_cluster_bit_vectors* game_get_cluster_activation(void)
{
	return get_main_game_globals()->cluster_activation;
}

void game_options_setup_default_players(int player_count, s_game_options* game_options)
{
	typedef void(__cdecl* cache_file_map_clear_all_failures_t)(int, s_game_options*);
	auto p_game_options_setup_default_players = Memory::GetAddress<cache_file_map_clear_all_failures_t>(0x49650, 0x428ED);
	p_game_options_setup_default_players(player_count, game_options);
	return;
}

void __cdecl reset_global_player_counts()
{
	INVOKE(0xB8B9, 0, reset_global_player_counts);
}

void game_direct_connect_to_session(XNKID kid, XNKEY key, XNADDR addr, int8 exe_type, int32 exe_version,
	int32 comp_version)
{
    auto handler = (c_game_life_cycle_handler_joining*)c_game_life_cycle_manager::get()->life_cycle_handlers[e_game_life_cycle::_life_cycle_joining];
    handler->joining_xnkid = kid;
    handler->joining_xnkey = key;
    handler->joining_xnaddr = addr;
    if (exe_type != EXECUTABLE_TYPE || exe_version != EXECUTABLE_VERSION || comp_version != COMPATIBLE_VERSION)
    {
        handler->join_attempt_result_code = 9;
    }
    else
    {
        c_game_life_cycle_handler_joining::check_joining_capability();
        wchar_t local_usernames[4][16];
        s_player_identifier local_identifiers[4];
        int valid_local_player_count = 0;
        for (auto i = 0; i < 4; i++)
        {
            s_player_identifier temp_identifier;
            s_player_properties temp_properties;
            if (network_session_interface_get_local_user_identifier(i, &temp_identifier) || network_session_interface_get_local_user_properties(i, 0, &temp_properties, 0, 0))
            {
                memcpy(local_usernames[valid_local_player_count], temp_properties.player_name, 16);
                local_identifiers[valid_local_player_count].unk1 = temp_identifier.unk1;
                local_identifiers[valid_local_player_count].unk2 = temp_identifier.unk2;
                valid_local_player_count++;
            }
        }
        reset_global_player_counts();
        network_session_init_session(2, 1);
        memset(&handler->player_identifiers, 0, sizeof(handler->player_identifiers));
        memcpy(&handler->player_identifiers, local_identifiers, sizeof(s_player_identifier) * valid_local_player_count);
        memcpy(&handler->player_names, local_usernames, sizeof(wchar_t) * 16 * valid_local_player_count);
        handler->field_11 = 0; //Always 0 in the original function
        handler->field_12 = 0; //Always 0 in the original function
        handler->field_14 = 1;
        handler->joining_user_count = valid_local_player_count;
        handler->field_54 = 2; //Always 2 in original function
        handler->field_10 = true; //Always 1 in original function

        handler->join_attempt_result_code = 0; //Force valid result code, leave the denying the connection up to the host.
    }
    c_game_life_cycle_manager::get()->request_state_change(_life_cycle_joining, 0, 0);
    game_shell_set_in_progress();
}

