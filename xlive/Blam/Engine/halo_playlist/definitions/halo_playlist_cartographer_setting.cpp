#include "stdafx.h"
#include "halo_playlist_cartographer_setting.h"

constexpr wchar_t* g_halo_playlist_cartographer_setting_property_strings[k_halo_playlist_cartographer_setting_property_count]
{
    L"Engine mode",
    L"Infinite Ammo",
    L"Infinite Grenades",
    L"Explosion Physics",
    L"Force Default FoV",
    L"Force Default Weapon Offset",
    L"Force Default Crosshair Offset",
    L"Game Speed",
    L"Gravity",
    L"Spawn Protection"
};

static s_halo_playlist_item g_halo_playlist_cartographer_setting_property_items[k_halo_playlist_cartographer_setting_property_count]
{
	{g_halo_playlist_cartographer_setting_property_strings[0], _halo_playlist_cartographer_setting_thirty_tick_rate},
	{g_halo_playlist_cartographer_setting_property_strings[1], _halo_playlist_cartographer_setting_infinite_ammo},
    {g_halo_playlist_cartographer_setting_property_strings[2], _halo_playlist_cartographer_setting_infinite_grenades},
    {g_halo_playlist_cartographer_setting_property_strings[3], _halo_playlist_cartographer_setting_explosion_physics},
    {g_halo_playlist_cartographer_setting_property_strings[4], _halo_playlist_cartographer_setting_default_fov},
    {g_halo_playlist_cartographer_setting_property_strings[5], _halo_playlist_cartographer_setting_default_weapon_offsets},
    {g_halo_playlist_cartographer_setting_property_strings[6], _halo_playlist_cartographer_setting_default_cross_hair_offset},
    {g_halo_playlist_cartographer_setting_property_strings[7], _halo_playlist_cartographer_setting_game_speed},
    {g_halo_playlist_cartographer_setting_property_strings[8], _halo_playlist_cartographer_setting_gravity},
    {g_halo_playlist_cartographer_setting_property_strings[9], _halo_playlist_cartographer_setting_spawn_protection}
};

constexpr wchar_t* g_halo_playlist_cartographer_engine_mode_strings[k_halo_playlist_cartographer_engine_mode_count]
{
    L"Default",
    L"60",
    L"Legacy",
    L"30"
};

static s_halo_playlist_item g_halo_playlist_cartographer_engine_mode_items[k_halo_playlist_cartographer_engine_mode_count]
{
	{g_halo_playlist_cartographer_engine_mode_strings[0], _halo_playlist_cartographer_setting_engine_mode_default},
    {g_halo_playlist_cartographer_engine_mode_strings[1], _halo_playlist_cartographer_setting_engine_mode_default},
    {g_halo_playlist_cartographer_engine_mode_strings[2], _halo_playlist_cartographer_setting_engine_mode_legacy},
    {g_halo_playlist_cartographer_engine_mode_strings[3], _halo_playlist_cartographer_setting_engine_mode_legacy},
};

constexpr wchar_t* g_halo_playlist_cartographer_game_speed_strings[k_halo_playlist_cartographer_game_speed_count]
{
    L"Default",
    L"50%",
    L"150%",
    L"200%",
    L"Ludicrous"
};

static s_halo_playlist_item g_halo_playlist_cartographer_game_speed_items[k_halo_playlist_cartographer_game_speed_count]
{
    {g_halo_playlist_cartographer_game_speed_strings[0], _game_speed_modifier_none},
    {g_halo_playlist_cartographer_game_speed_strings[1], _game_speed_modifier_half},
    {g_halo_playlist_cartographer_game_speed_strings[2], _game_speed_modifier_hundred_fifty},
    {g_halo_playlist_cartographer_game_speed_strings[3], _game_speed_modifier_double},
    {g_halo_playlist_cartographer_game_speed_strings[4], _game_speed_modifier_ludicrous}
};

constexpr wchar_t* g_halo_playlist_cartographer_gravity_strings[k_halo_playlist_cartographer_gravity_count]
{
    L"Default",
    L"25%",
    L"50%",
    L"75%",
    L"125%",
    L"150%",
    L"175%",
    L"200%"
};

static s_halo_playlist_item g_halo_playlist_cartographer_gravity_items[k_halo_playlist_cartographer_gravity_count]
{
    {g_halo_playlist_cartographer_gravity_strings[0], _game_gravity_modifier_none},
    {g_halo_playlist_cartographer_gravity_strings[1], _game_gravity_modifier_twenty_five_percent},
    {g_halo_playlist_cartographer_gravity_strings[2], _game_gravity_modifier_fifty_percent},
    {g_halo_playlist_cartographer_gravity_strings[3], _game_gravity_modifier_seventy_five_percent},
    {g_halo_playlist_cartographer_gravity_strings[4], _game_gravity_modifier_hundred_twenty_five_percent},
    {g_halo_playlist_cartographer_gravity_strings[5], _game_gravity_modifier_hundred_fifty_percent},
    {g_halo_playlist_cartographer_gravity_strings[6], _game_gravity_modifier_hundred_seventy_five_percent},
    {g_halo_playlist_cartographer_gravity_strings[7], _game_gravity_modifier_two_hundred},
};

constexpr wchar_t* g_halo_playlist_cartographer_spawn_protection_strings[k_halo_playlist_cartographer_spawn_protection_count]
{
    L"Default",
    L"0",
    L"None",
    L"3",
    L"Three Seconds",
    L"5",
    L"Five Seconds",
    L"10",
    L"Ten Seconds"
};

static s_halo_playlist_item g_halo_playlist_cartographer_spawn_protection_items[k_halo_playlist_cartographer_spawn_protection_count]
{
    {g_halo_playlist_cartographer_spawn_protection_strings[0], _player_spawn_protection_timer_one_second},
    {g_halo_playlist_cartographer_spawn_protection_strings[1], _player_spawn_protection_timer_none},
    {g_halo_playlist_cartographer_spawn_protection_strings[2], _player_spawn_protection_timer_none},
    {g_halo_playlist_cartographer_spawn_protection_strings[3], _player_spawn_protection_timer_three_seconds},
    {g_halo_playlist_cartographer_spawn_protection_strings[4], _player_spawn_protection_timer_three_seconds},
    {g_halo_playlist_cartographer_spawn_protection_strings[5], _player_spawn_protection_timer_five_seconds},
    {g_halo_playlist_cartographer_spawn_protection_strings[6], _player_spawn_protection_timer_five_seconds},
    {g_halo_playlist_cartographer_spawn_protection_strings[7], _player_spawn_protection_timer_ten_seconds},
    {g_halo_playlist_cartographer_spawn_protection_strings[8], _player_spawn_protection_timer_ten_seconds},
};

static s_halo_playlist_item_collection g_halo_playlist_cartographer_setting_property_item_collection = { g_halo_playlist_cartographer_setting_property_items, k_halo_playlist_cartographer_setting_property_count };

static s_halo_playlist_item_collection g_halo_playlist_cartographer_engine_mode_item_collection = { g_halo_playlist_cartographer_engine_mode_items, k_halo_playlist_cartographer_engine_mode_count };

static s_halo_playlist_item_collection g_halo_playlist_cartographer_game_speed_item_collection = { g_halo_playlist_cartographer_game_speed_items, k_halo_playlist_cartographer_game_speed_count };

static s_halo_playlist_item_collection g_halo_playlist_cartographer_gravity_item_collection = { g_halo_playlist_cartographer_gravity_items, k_halo_playlist_cartographer_gravity_count };

static s_halo_playlist_item_collection g_halo_playlist_cartographer_spawn_protection_item_collection = { g_halo_playlist_cartographer_spawn_protection_items, k_halo_playlist_cartographer_spawn_protection_count };

wchar_t* halo_playlist_item_collection_cartographer_setting_get_name(e_halo_playlist_cartographer_setting_property_type value)
{
    ASSERT(IN_RANGE(value, k_halo_playlist_cartographer_setting_invalid, k_halo_playlist_cartographer_setting_count));

    return halo_playlist_item_collection_get_name(&g_halo_playlist_cartographer_setting_property_item_collection, value);
}

e_halo_playlist_cartographer_setting_property_type halo_playlist_item_collection_cartographer_setting_get_value(wchar_t* value)
{
    return (e_halo_playlist_cartographer_setting_property_type)halo_playlist_item_collection_get_value(&g_halo_playlist_cartographer_setting_property_item_collection, value);
}

e_halo_playlist_cartographer_setting_engine_mode halo_playlist_item_collection_cartographer_engine_mode_get_value(wchar_t* value)
{
    return (e_halo_playlist_cartographer_setting_engine_mode)halo_playlist_item_collection_get_value(&g_halo_playlist_cartographer_engine_mode_item_collection, value);
}

e_game_speed_modifier halo_playlist_item_collection_cartographer_game_speed_get_value(wchar_t* value)
{
    return (e_game_speed_modifier)halo_playlist_item_collection_get_value(&g_halo_playlist_cartographer_game_speed_item_collection, value);
}

e_game_gravity_modifier halo_playlist_item_collection_cartographer_gravity_get_value(wchar_t* value)
{
    return (e_game_gravity_modifier)halo_playlist_item_collection_get_value(&g_halo_playlist_cartographer_gravity_item_collection, value);
}

e_player_spawn_protection_timer halo_playlist_item_collection_cartographer_spawn_protection_get_value(wchar_t* value)
{
    return (e_player_spawn_protection_timer)halo_playlist_item_collection_get_value(&g_halo_playlist_cartographer_spawn_protection_item_collection, value);
}

static bool halo_playlist_item_collection_cartographer_setting_flag_write_to_variant(wchar_t* value, s_game_variant* variant, e_cartographer_variant_flags flag)
{
    bool result = false;
    const bool eval_result = halo_playlist_item_collection_get_boolean_value(value, &result);

    if (!eval_result)
        return eval_result;

    variant->cartographer_settings.flags.set(flag, result);

    return eval_result;
}

bool halo_playlist_item_collection_cartographer_engine_mode_write_to_variant(wchar_t* value, s_game_variant* variant)
{
    e_halo_playlist_cartographer_setting_engine_mode engine_mode = halo_playlist_item_collection_cartographer_engine_mode_get_value(value);

    if (engine_mode == k_halo_playlist_cartographer_setting_engine_mode_invalid)
        return false;

    variant->cartographer_settings.flags.set(_cartographer_variant_thirty_tick_rate, engine_mode);

	return true;
}

bool halo_playlist_item_collection_cartographer_infinite_ammo_write_to_variant(wchar_t* value, s_game_variant* variant)
{
    return halo_playlist_item_collection_cartographer_setting_flag_write_to_variant(value, variant, _cartographer_variant_infinite_ammo);
}

bool halo_playlist_item_collection_cartographer_infinite_grenades_write_to_variant(wchar_t* value, s_game_variant* variant)
{
    return halo_playlist_item_collection_cartographer_setting_flag_write_to_variant(value, variant, _cartographer_variant_infinite_grenades);
}

bool halo_playlist_item_collection_cartographer_explosion_physics_write_to_variant(wchar_t* value, s_game_variant* variant)
{
    return halo_playlist_item_collection_cartographer_setting_flag_write_to_variant(value, variant, _cartographer_variant_explosion_physics);
}

bool halo_playlist_item_collection_cartographer_default_fov_write_to_variant(wchar_t* value, s_game_variant* variant)
{
    return halo_playlist_item_collection_cartographer_setting_flag_write_to_variant(value, variant, _cartographer_variant_force_default_fov);
}

bool halo_playlist_item_collection_cartographer_default_weapon_offsets_write_to_variant(wchar_t* value, s_game_variant* variant)
{
    return halo_playlist_item_collection_cartographer_setting_flag_write_to_variant(value, variant, _cartographer_variant_force_default_weapon_offsets);
}

bool halo_playlist_item_collection_cartographer_default_crosshair_position_write_to_variant(wchar_t* value, s_game_variant* variant)
{
    return halo_playlist_item_collection_cartographer_setting_flag_write_to_variant(value, variant, _cartographer_variant_force_default_cross_hair_offset);
}

bool halo_playlist_item_collection_cartographer_game_speed_write_to_variant(wchar_t* value, s_game_variant* variant)
{
    e_game_speed_modifier game_speed = halo_playlist_item_collection_cartographer_game_speed_get_value(value);

    if (game_speed == k_game_speed_modifier_invalid)
        return false;

    variant->cartographer_settings.game_speed = game_speed;

    return true;
}

bool halo_playlist_item_collection_cartographer_gravity_write_to_variant(wchar_t* value, s_game_variant* variant)
{
    e_game_gravity_modifier gravity = halo_playlist_item_collection_cartographer_gravity_get_value(value);

    if (gravity == k_game_gravity_modifier_invalid)
        return false;

    variant->cartographer_settings.gravity = gravity;

    return true;
}

bool halo_playlist_item_collection_cartographer_spawn_protection_write_to_variant(wchar_t* value, s_game_variant* variant)
{
    e_player_spawn_protection_timer spawn_protection = halo_playlist_item_collection_cartographer_spawn_protection_get_value(value);

    if (spawn_protection == k_player_spawn_protection_timer_invalid)
        return false;

    variant->cartographer_settings.spawn_protection = spawn_protection;

    return true;
}
