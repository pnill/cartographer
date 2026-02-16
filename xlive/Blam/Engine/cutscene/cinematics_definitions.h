#pragma once
#include "math/color_math.h"
#include "math/integer_math.h"
#include "text/text.h"

/* structures */

// max count: k_maximum_scenario_cutscene_titles 128
struct s_scenario_cutscene_title
{
	string_id name;
	rectangle2d text_bounds_on_screen;

	e_text_justification justification;
	e_text_font font;
	pixel32 text_color;
	pixel32 shadow_color;
	real32 fade_in_time_seconds;
	real32 up_time_seconds;
	real32 fade_out_time_seconds;
};
ASSERT_STRUCT_SIZE(s_scenario_cutscene_title, 36);
