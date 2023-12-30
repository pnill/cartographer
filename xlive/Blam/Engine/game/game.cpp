#include "stdafx.h"
#include "game.h"

#include "game_engine.h"
#include "game_time.h"

#include "Blam/Engine/cseries/cseries.h"
#include "Blam/Engine/effects/particle.h"
#include "Blam/Engine/main/interpolator.h"
#include "Blam/Engine/main/main.h"
#include "Blam/Engine/Networking/logic/life_cycle_manager.h"
#include "Blam/Engine/saved_games/game_state.h"
#include "Blam/Engine/shell/shell.h"
#include "Util/Hooks/Hook.h"

s_game_systems* get_game_systems()
{
	return Memory::GetAddress<s_game_systems*>(0x3A0468, 0x35D198);
}

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
	INVOKE(0x242E5B, 0x22054B, game_shell_set_in_progress);
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
	INVOKE(0xB8B9, 0x219B9, reset_global_player_counts);
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
            if (network_session_interface_get_local_user_identifier(i, &temp_identifier) || network_session_interface_get_local_user_properties_out(i, 0, &temp_properties, 0, 0))
            {
                memcpy(local_usernames[valid_local_player_count], temp_properties.player_name, sizeof(temp_properties.player_name));
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


void __cdecl shell_initialize(void)
{
    game_state_shell_initialize();
    s_main_game_globals* p_game_globals = (s_main_game_globals*)game_state_malloc("game globals", NULL, sizeof(s_main_game_globals));
    csmemset(p_game_globals, 0, sizeof(s_main_game_globals));
    p_game_globals->active_structure_bsp_index = NONE;
    *Memory::GetAddress<s_main_game_globals**>(0x482D3C, 0x4CB520) = p_game_globals;    // Write allocated globals back to the original exe

    real_math_reset_precision();

    s_game_systems* g_game_systems = get_game_systems();
    for (size_t i = 0; i < 70; ++i)
    {
        g_game_systems[i].initialize_proc();
    }

    // Interpolation allocation
    if (shell_tool_type() || !Memory::IsDedicatedServer())
    {
        g_frame_data_storage = (s_frame_data_storage*)VirtualAlloc(0, sizeof(s_frame_data_storage), MEM_COMMIT, PAGE_READWRITE);
        g_frame_data_intermediate = (s_interpolation_data*)VirtualAlloc(0, sizeof(s_interpolation_data), MEM_COMMIT, PAGE_READWRITE);
        g_previous_interpolation_frame_data = &g_frame_data_storage->previous_data;
        g_target_interpolation_frame_data = &g_frame_data_storage->target_data;
        halo_interpolator_set_interpolation_enabled(true);

        //g_particle_frame_storage = (s_particle_frame_data_storage*)VirtualAlloc(0, sizeof(s_particle_frame_data_storage), MEM_COMMIT, PAGE_READWRITE);
        //g_previous_particle_interpolation_frame_data = &g_particle_frame_storage->previous_data;
        //g_target_particle_interpolation_frame_data = &g_particle_frame_storage->target_data;
        halo_particle_interpolator_set_state(false);
    }
    return;
}

bool __cdecl main_events_pending(void)
{
    return INVOKE(0x396B1, 0x411D0, main_events_pending);
}

void __cdecl game_tick(void)
{
    INVOKE(0x4A4AF, 0x4372D, game_tick);
    return;
}

void __cdecl game_update(int32 desired_ticks, real32* elapsed_game_dt)
{
    int32 actual_ticks = 0;
    if (desired_ticks > 0)
    {
        while (!main_events_pending())
        {
            halo_interpolator_update_begin();
            game_tick();
            halo_interpolator_update_end();
            if (cinematic_sound_sync_complete())
            {
                break;
            }
            if (++actual_ticks >= desired_ticks)
            {
                return;
            }
        }
        if (actual_ticks < desired_ticks)
        {
            game_time_discard(desired_ticks, actual_ticks, elapsed_game_dt);
        }
    }

    return;
}

void game_info_initialize_for_new_map(s_game_options* options)
{
    s_main_game_globals* game_globals = get_main_game_globals();

    game_globals->options = *options;
    game_globals->options.load_level_only = false;

    if (game_is_multiplayer() || game_globals->options.game_variant.variant_game_engine_index)
    {
        game_engine_variant_cleanup(&game_globals->options.game_variant.variant_flag);
    }
    random_math_set_seed(game_globals->options.random_seed);
    game_globals->game_is_lost = false;
    game_globals->game_is_finished = false;
    game_globals->pvs_object_is_set = 0;
    game_globals->game_ragdoll_count = 0;
    return;
}

void __cdecl game_initialize_for_new_map(s_game_options* options)
{
    s_main_game_globals* game_globals = get_main_game_globals();

    halo_interpolator_clear_buffers();
    real_math_reset_precision();
    game_globals->initializing = true;
    game_info_initialize_for_new_map(options);

    s_game_systems* g_game_systems = get_game_systems();
    for (uint32 i = 0; i < 70; i++)
    {
        if (g_game_systems[i].reset_proc)
        {
            g_game_systems[i].reset_proc();
        }
    }
    game_globals->initializing = false;
    game_globals->map_active = true;
}

real32 g_current_game_frame_delta;

typedef void(__cdecl* game_frame_t)(real32);
game_frame_t p_game_frame;
void __cdecl game_frame(real32 dt)
{
    g_current_game_frame_delta = dt;
    halo_interpolator_update_delta();
    p_game_frame(dt);
    return;
}

void game_apply_pre_winmain_patches(void)
{
    PatchCall(Memory::GetAddress(0x86BE, 0x1EB86), game_initialize_for_new_map);
    PatchCall(Memory::GetAddress(0x9802, 0x1FAED), game_initialize_for_new_map);
    PatchCall(Memory::GetAddress(0x39D2A, 0xC0C0), game_update);
    PatchCall(Memory::GetAddress(0x39E42, 0xBA4F), shell_initialize);
    
    // Get original game_frame function
    p_game_frame = Memory::GetAddress<game_frame_t>(0x48CDC, 0x41F7D);

    PatchCall(Memory::GetAddress(0x39D45, 0xC0D4), game_frame);
    return;
}
