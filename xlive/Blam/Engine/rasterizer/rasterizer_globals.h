#pragma once
#include "game/game_globals.h"

/* public code */

s_game_globals_rasterizer_data* rasterizer_globals_get_data(void);

void __cdecl rasterizer_get_screen_bounds(rectangle2d* screen_bounds);
void __cdecl rasterizer_get_frame_bounds(rectangle2d* frame_bounds);
