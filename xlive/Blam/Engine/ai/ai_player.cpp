#include "stdafx.h"
#include "ai_player.h"

/* constants */

enum
{
    k_ai_coop_max_player_states = 4
};

/* prototypes */

static void ai_player_increase_ai_coop_player_state_count(void);

/* public code */

void ai_player_apply_patches(void)
{
	ai_player_increase_ai_coop_player_state_count();
	return;
}

void __cdecl ai_players_initialize_for_new_map(void)
{
	INVOKE(0x30D685, 0x2B83FC, ai_players_initialize_for_new_map);
	return;
}

/* private code */

static void ai_player_increase_ai_coop_player_state_count(void)
{
	WriteValue<int8>(Memory::GetAddress(0x30E603), sizeof(ai_player_state) * k_ai_coop_max_player_states);
	WriteValue<int8>(Memory::GetAddress(0x30C537), sizeof(ai_player_state) * k_ai_coop_max_player_states);

	// count: the entry-count scans and per-entry loop trip counts.
	WriteValue<int8>(Memory::GetAddress(0x30C4C5), k_ai_coop_max_player_states);    // ai_players_reset
	WriteValue<int8>(Memory::GetAddress(0x30C588), k_ai_coop_max_player_states);    // ai_player_index_get
	WriteValue<int8>(Memory::GetAddress(0x30C5B4), k_ai_coop_max_player_states);    // ai_player_state_get
	WriteValue<int8>(Memory::GetAddress(0x30C5DE), k_ai_coop_max_player_states);    // ai_player_state_handle_deleted_object
	WriteValue<int8>(Memory::GetAddress(0x30D65F), k_ai_coop_max_player_states);    // ai_globals_initialize
	WriteValue<int8>(Memory::GetAddress(0x30D791), k_ai_coop_max_player_states);    // ai_player_state_update (dword imm, low byte)
	WriteValue<int8>(Memory::GetAddress(0x323501), k_ai_coop_max_player_states);    // ai_unit_seat_disallowed
	WriteValue<int8>(Memory::GetAddress(0x3605F9), k_ai_coop_max_player_states);    // actor_situation_update (player_looking_index bound)

	// count + 1: for_new_map trip count `lea edx,[esi+K]`, esi == -1 so K = count + 1.
	WriteValue<int8>(Memory::GetAddress(0x30D71A), (int8)(k_ai_coop_max_player_states + 1));    // ai_globals_initialize_for_new_map
	return;
}
