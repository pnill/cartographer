#pragma once

enum e_game_playback_type : int16
{
	_game_playback_none = 0,
	_game_playback_local = 1,
	_game_playback_network_server = 2,
	_game_playback_network_client = 3,

	k_game_playback_count
};
