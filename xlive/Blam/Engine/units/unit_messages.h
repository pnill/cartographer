#pragma once

/* enums */

enum e_unit_messages
{
    _unit_message_none = NONE,
    _unit_message_weapon_idle = 0,
    _unit_message_weapon_primary_fire,
    _unit_message_weapon_secondary_fire,
    _unit_message_weapon_primary_misfire,
    _unit_message_weapon_secondary_misfire,

    // TODO: populate with remaining flags
};
