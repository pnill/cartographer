#pragma once

/* enums */

enum e_game_team : int16
{
	// MP
	_game_team_red = 0,
	_game_team_blue = 1,
	_game_team_yellow = 2,
	_game_team_green = 3,
	_game_team_purple = 4,
	_game_team_orange = 5,
	_game_team_brown = 6,
	_game_team_pink = 7,
	_game_team_neutral = 8,

	k_game_multiplayer_team_count = 8,

	// SP
	_game_team_default = 0,
	_game_team_player = 1,
	_game_team_human = 2,
	_game_team_covenant = 3,
	_game_team_flood = 4,
	_game_team_sentinel = 5,
	_game_team_heretic = 6,
	_game_team_prophet = 7,

	// unassigned team ids
	_game_team_unused8 = 8,
	_game_team_unused9 = 9,
	_game_team_unused10 = 10,
	_game_team_unused11 = 11,
	_game_team_unused12 = 12,
	_game_team_unused13 = 13,
	_game_team_unused14 = 14,
	_game_team_unused15 = 15,

	// Shared
	_game_team_none = NONE
};