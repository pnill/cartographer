#include "stdafx.h"
#include "game_engine_util.h"

#include "game/game.h"
#include "game/game_engine.h"
#include "networking/network_game_definitions.h"

/* public code */

bool game_engine_in_round(void)
{
	return current_game_engine() != NULL
		&& game_engine_globals_get()->field_6C == 1
		&& (game_is_predicted() || game_engine_globals_get()->field_C44 == 1);
}

void __cdecl game_engine_check_for_round_winner(void)
{
	INVOKE(0x70F49, 0x6FA4A, game_engine_check_for_round_winner);
	return;
}

void __cdecl game_engine_end_round_with_winner(
	int32 player_datum_or_team_index,
	bool go_to_next_round)
{
	INVOKE(0x70A6F, 0x6F570, game_engine_end_round_with_winner, player_datum_or_team_index, go_to_next_round);
	return;
}

bool game_engine_has_teams(void)
{
	bool result = false;

	if (current_game_engine())
	{
		result = current_game_variant()->game_engine_flags.test(_game_engine_teams_bit);
	}

	return result;
}

e_network_game_simulation_protocol game_engine_get_simulation_protocol(
	struct s_game_variant const* variant)
{
	e_network_game_simulation_protocol result = _network_game_simulation_protocol_synchronous;

	switch (variant->variant_game_engine_index)
	{
	case _game_engine_type_ctf:
	case _game_engine_type_slayer:
	case _game_engine_type_oddball:
	case _game_engine_type_koth:
	case _game_engine_type_juggernaut:
	case _game_engine_type_territories:
	case _game_engine_type_assault:
		result = _network_game_simulation_protocol_distributed;
		break;
	default:
		unreachable();
	}

	return result;
}

bool __cdecl sub_4701B6(datum player_index)
{
	return INVOKE(0x701B6, 0x0, sub_4701B6, player_index);
}

e_game_variant_description_index game_engine_type_get_variant_description_index(e_game_engine_type type)
{
	e_game_variant_description_index result = k_game_variant_description_invalid;
	switch (type)
	{
		case _game_engine_type_ctf:
			result = _game_variant_description_ctf;
			break;
		case _game_engine_type_slayer:
			result = _game_variant_description_slayer;
			break;
		case _game_engine_type_oddball:
			result = _game_variant_description_oddball;
			break;
		case _game_engine_type_koth:
			result = _game_variant_description_king;
			break;
		case _game_engine_type_race:
			result = k_game_variant_description_invalid;
			break;
		case _game_engine_type_headhunter:
			result = k_game_variant_description_invalid;
			break;
		case _game_engine_type_juggernaut:
			result = _game_variant_description_juggernaut;
			break;
		case _game_engine_type_territories:
			result = _game_variant_description_territories;
			break;
		case _game_engine_type_assault:
			result = _game_variant_description_invasion;
			break;
	}

	return result;
}