#include "stdafx.h"
#include "screen_single_player_difficulty_select.h"

void* c_screen_single_player_difficulty_select::load_lobby(s_screen_parameters* parameters)
{
    return INVOKE(0x24201D, 0x0, c_screen_single_player_difficulty_select::load_lobby, parameters);

}
