#pragma once
#include "Blam/Engine/objects/damage.h"
#include "Blam/Engine/math/color_math.h"
#include "Blam/Engine/memory/data.h"

enum e_effect_location_flags : int16
{
    _effect_location_flags_bit_15 = FLAG(15),
};


struct effect_datum
{
    int8 gap_0[2];
    int16 flags;
    int32 tag_datum;
    int8 gap_8[64];
    datum multi_purpose_origin_index;
    s_damage_owner damage_owner;
    int32 origin_local_user_index;
    int16 event_index;
    int8 gap_60[28];
    datum effect_locations[32];
    int8 gap_80[145];
};

CHECK_STRUCT_SIZE(effect_datum, 400);

struct effect_location_datum
{
    int8 gap_0[2];
    e_effect_location_flags flags;
    int32 parent_effect_location_index;
    int8 gap_8[4];
    real_point3d unk_real_point_8;
    int8 gap_18[12];
    real_point3d unk_real_point_24;
    real_point3d location;
};

CHECK_STRUCT_SIZE(effect_location_datum, 60);

datum __cdecl effect_new_from_object(
    datum effect_tag_index,
    s_damage_owner* damage_owner,
    datum object_index,
    real32 a4,
    real32 a5,
    real_rgb_color* color,
    const void* effect_vector_field);

s_data_array* get_effects_table();

s_data_array* get_effects_location_table();

effect_location_datum* __cdecl effect_location_get_next_valid_index(effect_datum* effect_datum, int32* out_index, int16 a3);
void effects_apply_patches();