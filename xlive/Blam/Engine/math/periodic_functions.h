#pragma once

enum e_transition_function_type : int16
{
    _transition_function_linear = 0,
    _transition_function_early = 1,
    _transition_function_very_early = 2,
    _transition_function_late = 3,
    _transition_function_very_late = 4,
    _transition_function_cosine = 5,
    _transition_function_one = 6,
    _transition_function_zero = 7,
    k_transition_functions_count
};

enum e_periodic_function_type : int16
{
    _periodic_function_one = 0,
    _periodic_function_zero = 1,
    _periodic_function_cosine = 2,
    _periodic_function_cosine_variable_period = 3,
    _periodic_function_diagonal_wave = 4,
    _periodic_function_diagonal_wave_variable_period = 5,
    _periodic_function_slide = 6,
    _periodic_function_slide_variable_period = 7,
    _periodic_function_noise = 8,
    _periodic_function_jitter = 9,
    _periodic_function_wander = 10,
    _periodic_function_spark = 11,
    k_periodic_functions_count
};

real32 __cdecl transition_function_evaluate(e_transition_function_type function_type, real32 function_value);

real32 __cdecl periodic_function_evaluate(e_periodic_function_type function_type, real32 function_value);
