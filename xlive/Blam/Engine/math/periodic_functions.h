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

real32 __cdecl transition_function_evaluate(int16 function_type, real32 function_value);
