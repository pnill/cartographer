#pragma once
#include "objects/damage.h"
#include "objects/object_placement.h"
#include "math/color_math.h"
#include "math/matrix_math.h"
#include "memory/data.h"


enum e_effect_datum_flags : int16
{
    _effect_datum_flags_bit_0 = 0,
    _effect_datum_flags_bit_1 = 1,
    _effect_datum_flags_bit_2 = 2,
    _effect_datum_flags_bit_3 = 3,
    _effect_datum_flags_bit_4 = 4,
    _effect_datum_flags_created_in_violence_bit = 5,
    _effect_datum_flags_bit_6 = 6,
    _effect_potentially_first_person_bit = 7,
    _effect_datum_flags_bit_8 = 8,
    _effect_datum_flags_bit_9 = 9,
    _effect_datum_flags_bit_10 = 10,
    _effect_datum_flags_bit_11 = 11,
    _effect_datum_flags_bit_12 = 12,
    _effect_datum_flags_bit_13 = 13,
    _effect_datum_flags_bit_14 = 14,
    _effect_datum_flags_bit_15 = 15,
    k_effect_datum_flag_count,
};

enum e_effect_location_flags : uint16
{
    _effect_location_flags_bit_15 = FLAG(15),
};

struct s_effect_struct
{
    int32 field_0;
    datum particle_system;
};

struct effect_datum
{
    int8 gap_0[2];
    c_flags<e_effect_datum_flags, uint16, k_effect_datum_flag_count> flags;
    datum tag_index;
    datum object_looping_sound_index;
    string_id input_marker_name;
    string_id field_10;
    string_id field_14;
    int16 field_18;
    int8 field_1A[2];
    s_location location;
    real_rgb_color color;
    real_vector3d velocity;
    int8 gap_3C[12];
    datum object_index;
    s_damage_owner damage_owner;
    int32 first_person_user_index;
    int16 event_index;
    int16 field_5E;
    real32 field_60;
    real32 delay;
    real32 gap_68;
    real32 type_specific_scale;
    real32 scale;
    real_vector2d field_74;
    datum effect_locations[32];
    datum particle_system;
    void* particle_system_ptr;
    pixel32 pixel_1;
    pixel32 pixel_2;
    int32 field_10C;
    s_effect_struct field_110[16];
};
ASSERT_STRUCT_SIZE(effect_datum, 400);

struct effect_location_datum
{
    int8 gap_0[2];
    e_effect_location_flags flags;
    int32 parent_effect_location_index;
    real_matrix4x3 matrix;
};

ASSERT_STRUCT_SIZE(effect_location_datum, 60);

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

real_vector3d* effect_get_velocity(datum effect_index);
void __cdecl effect_update(datum effect_index, real32 dt);
void __cdecl effect_update_time(datum effect_index, real32 dt);
effect_location_datum* __cdecl effect_location_get_next_valid_index(effect_datum* effect_datum, int32* out_index, int16 a3);

void effects_apply_patches();