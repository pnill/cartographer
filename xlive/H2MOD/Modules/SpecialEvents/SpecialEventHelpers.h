#pragma once

/* structs */

struct s_event_scenery_placement
{
	int16 type;
	int16 variant_id;
	real_point3d position;
	real_euler_angles3d rotation;
	real32 scale;
};

/* globals */

const string_id new_elite_head_marker = (string_id)0xFEE01234;

/* prototypes */

void add_special_event_markers(void);
void replace_fp_and_3p_models_from_weapon(datum weapon_datum, datum fp_model_datum, datum _3p_model_datum);
void add_hat_and_beard_to_model(datum player_hlmt_datum, datum hat_scenery_datum, datum beard_scenery_datum, bool is_elite = false);
