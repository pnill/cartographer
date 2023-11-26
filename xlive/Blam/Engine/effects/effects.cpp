#include "stdafx.h"
#include "effects.h"

datum __cdecl effect_new_from_object(
    datum effect_tag_index,
    s_damage_owner* damage_owner,
    datum object_index,
    real32 a4,
    real32 a5,
    real_rgb_color* color,
    const void* effect_vector_field)
{
    return INVOKE(0xAADCE, 0x9CE4E, effect_new_from_object, effect_tag_index, damage_owner, object_index, a4, a5, color, effect_vector_field);
}
