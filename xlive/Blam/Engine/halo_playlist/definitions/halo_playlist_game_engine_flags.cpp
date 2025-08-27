#include "stdafx.h"
#include "halo_playlist_game_engine_flags.h"

bool halo_playlist_item_collection_game_engine_flag_write_to_variant(wchar_t* value, s_game_variant* variant, e_game_engine_flags flag)
{
	bool result = false;
	const bool eval_result = halo_playlist_item_collection_get_boolean_value(value, &result);

	if (!eval_result)
		return eval_result;

	variant->game_engine_flags.set(flag, result);

	return eval_result;
}

bool halo_playlist_item_collection_king_engine_flag_write_to_variant(wchar_t* value, s_game_variant* variant, e_king_engine_flags flag)
{
	bool result = false;
	const bool eval_result = halo_playlist_item_collection_get_boolean_value(value, &result);

	if (!eval_result)
		return eval_result;

	variant->game_engine_variant.king.flags.set(flag, result);

	return eval_result;
}

bool halo_playlist_item_collection_headhunter_engine_flag_write_to_variant(wchar_t* value, s_game_variant* variant, e_headhunter_engine_flags flag)
{
	bool result = false;
	const bool eval_result = halo_playlist_item_collection_get_boolean_value(value, &result);

	if (!eval_result)
		return eval_result;

	variant->game_engine_variant.head_hunter.flags.set(flag, result);

	return eval_result;
}

bool halo_playlist_item_collection_team_play_write_to_variant(wchar_t* value, s_game_variant* variant)
{
	return halo_playlist_item_collection_game_engine_flag_write_to_variant(value, variant, _game_engine_teams_bit);
}

bool halo_playlist_item_collection_team_changing_write_to_variant(wchar_t* value, s_game_variant* variant)
{
	return halo_playlist_item_collection_game_engine_flag_write_to_variant(value, variant, _game_engine_changing_teams_bit);
}

bool halo_playlist_item_collection_force_even_teams_write_to_variant(wchar_t* value, s_game_variant* variant)
{
	return halo_playlist_item_collection_game_engine_flag_write_to_variant(value, variant, _game_engine_force_even_teams_bit);
}

bool halo_playlist_item_collection_king_uncontested_hill_write_to_variant(wchar_t* value, s_game_variant* variant)
{
	return halo_playlist_item_collection_king_engine_flag_write_to_variant(value, variant, _king_engine_uncontested_hill_to_score_bit);
}

bool halo_playlist_item_collection_headhunter_uncontested_hill_write_to_variant(wchar_t* value, s_game_variant* variant)
{
	return halo_playlist_item_collection_headhunter_engine_flag_write_to_variant(value, variant, _headhunter_engine_uncontested_hill_to_score_bit);
}
