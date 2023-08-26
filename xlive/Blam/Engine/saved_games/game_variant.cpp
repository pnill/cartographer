#include "stdafx.h"
#include "game_variant.h"

void game_variant_build_default(s_game_variant* variant, e_game_variant_description_index game_variant_type)
{
    typedef void(__stdcall* game_variant_build_default_t)(s_game_variant*, e_game_variant_description_index game_variant_type);
    auto p_game_variant_build_default = Memory::GetAddress<game_variant_build_default_t>(0x5B33D, 0x3CF9D);
    p_game_variant_build_default(variant, game_variant_type);
    return;
}