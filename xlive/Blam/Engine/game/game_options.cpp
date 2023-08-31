#include "stdafx.h"
#include "game_options.h"

void game_options_new(s_game_options* game_options)
{
	typedef void(__cdecl* game_options_new_t)(s_game_options*);
	auto p_game_options_new = Memory::GetAddress<game_options_new_t>(0x48D6F, 0x42010);
	p_game_options_new(game_options);
	return;
}