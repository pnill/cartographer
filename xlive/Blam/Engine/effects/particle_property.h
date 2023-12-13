#pragma once
#include "Blam/Engine/math/function_definitions.h"

struct s_particle_state;

enum e_particle_property_output_modifier : int16
{
    _particle_property_output_modifier_none,
    _particle_property_output_modifier_plus,
    _particle_property_output_modifier_times
};

enum e_particle_property_variable : int16
{
    _particle_property_input_particle_age,
    _particle_property_input_particle_emit_time,
    _particle_property_input_particle_random_1,
    _particle_property_input_particle_random_2,
    _particle_property_input_emitter_age,
    _particle_property_input_emitter_random_1,
    _particle_property_input_emitter_random_2,
    _particle_property_input_system_lod,
    _particle_property_input_game_time,
    _particle_property_input_effect_a_scale,
    _particle_property_input_effect_b_scale,
    _particle_property_input_particle_rotation,
    _particle_property_input_explosion_animation,
    _particle_property_input_explosion_rotation,
    _particle_property_input_particle_random_3,
    _particle_property_input_particle_random_4,
    _particle_property_input_location_random
};

enum e_particle_system_definition_flags : int16
{
    _particle_system_definition_flags_glow = FLAG(0),
    _particle_system_definition_flags_cinematics = FLAG(1),
    _particle_system_definition_flags_looping_particle = FLAG(2),
    _particle_system_definition_flags_disabled_for_debugging = FLAG(3),
    _particle_system_definition_flags_inherit_effect_velocity = FLAG(4),
    _particle_system_definition_flags_dont_render_system = FLAG(5),
    _particle_system_definition_flags_render_when_zoomed = FLAG(6),
    _particle_system_definition_flags_spread_between_ticks = FLAG(7),
    _particle_system_definition_flags_persistent_particle = FLAG(8),
    _particle_system_definition_flags_expensive_visibility = FLAG(9)
};

enum e_particle_system_camera_mode : int16
{
    _particle_system_independent_of_camera_mode = 0,
    _particle_system_only_in_first_person = 1,
    _particle_system_only_in_third_person = 2,
    _particle_system_both_first_and_third = 3
};

class c_particle_property
{
public:
    e_particle_property_variable input_variable;
    e_particle_property_variable range_variable;
    e_particle_property_output_modifier output_modifier;
    e_particle_property_variable output_modifier_input;
    c_function_definition mapping;

    real32 get_result(s_particle_state* particle_state);
};
