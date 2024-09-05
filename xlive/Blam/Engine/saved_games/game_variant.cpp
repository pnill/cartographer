#include "stdafx.h"
#include "game_variant.h"

#include "game/game.h"

s_game_variant* get_game_variant(void)
{
    return &game_options_get()->game_variant;
}

void __cdecl game_variant_build_default(s_game_variant* variant, e_game_variant_description_index game_variant_type)
{
    INVOKE(0x5B33D, 0x3CF9D, game_variant_build_default, variant, game_variant_type);
    return;
}