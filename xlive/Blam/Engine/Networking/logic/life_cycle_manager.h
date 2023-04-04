#pragma once
enum e_game_life_cycle : int
{
	_life_cycle_none,
	_life_cycle_pre_game,
	_life_cycle_start_game,
	_life_cycle_in_game,
	_life_cycle_post_game,
	_life_cycle_joining,
	_life_cycle_matchmaking
};

e_game_life_cycle get_game_life_cycle();
