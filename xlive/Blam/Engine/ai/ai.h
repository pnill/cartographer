#pragma once

enum e_ai_sound_volume : uint16
{
    ai_sound_volume_silent = 0,
    ai_sound_volume_medium = 1,
    ai_sound_volume_loud = 2,
    ai_sound_volume_shout = 3,
    ai_sound_volume_quiet = 4
};

enum e_ai_seat_type : uint16
{
    _ai_seat_type_none = 0,
    _ai_seat_type_passenger = 1,
    _ai_seat_type_gunner = 2,
    _ai_seat_type_small_cargo = 3,
    _ai_seat_type_large_cargo = 4,
    _ai_seat_type_driver = 5
};
