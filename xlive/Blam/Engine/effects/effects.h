#pragma once
#include "Blam/Engine/objects/damage.h"
#include "Blam/Math/real_math.h"

datum __cdecl effect_new_from_object(
    datum effect_tag_index,
    s_damage_owner* damage_owner,
    datum object_index,
    real32 a4,
    real32 a5,
    real_color_rgb* color,
    const void* effect_vector_field);