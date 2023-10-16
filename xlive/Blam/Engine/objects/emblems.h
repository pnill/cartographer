#pragma once

enum e__emblem_foreground : uint8
{
	_emblem_foreground_seventh_column = 0,
	_emblem_foreground_bullseye = 1,
	_emblem_foreground_vortex = 2,
	_emblem_foreground_halt = 3,
	_emblem_foreground_spartan = 4,
	_emblem_foreground_da_bomb = 5,
	_emblem_foreground_trinity = 6,
	_emblem_foreground_delta = 7,
	_emblem_foreground_rampancy = 8,
	_emblem_foreground_sergeant = 9,
	_emblem_foreground_phenoix = 10,
	_emblem_foreground_champion = 11,
	_emblem_foreground_jolly_roger = 12,
	_emblem_foreground_marathon = 13,
	_emblem_foreground_cube = 14,
	_emblem_foreground_radioactive = 15,
	_emblem_foreground_smiley = 16,
	_emblem_foreground_frowney = 17,
	_emblem_foreground_spearhead = 18,
	_emblem_foreground_sol = 19,
	_emblem_foreground_waypoint = 20,
	_emblem_foreground_ying_yang = 21,
	_emblem_foreground_helmet = 22,
	_emblem_foreground_triad = 23,
	_emblem_foreground_grunt_symbol = 24,
	_emblem_foreground_cleave = 25,
	_emblem_foreground_thor = 26,
	_emblem_foreground_skull_king = 27,
	_emblem_foreground_triplicate = 28,
	_emblem_foreground_subnova = 29,
	_emblem_foreground_flaming_ninja = 30,
	_emblem_foreground_doubleCresent = 31,
	_emblem_foreground_spades = 32,
	_emblem_foreground_clubs = 33,
	_emblem_foreground_diamonds = 34,
	_emblem_foreground_hearts = 35,
	_emblem_foreground_wasp = 36,
	_emblem_foreground_mark_of_shame = 37,
	_emblem_foreground_snake = 38,
	_emblem_foreground_hawk = 39,
	_emblem_foreground_lips = 40,
	_emblem_foreground_capsule = 41,
	_emblem_foreground_cancel = 42,
	_emblem_foreground_gas_mask = 43,
	_emblem_foreground_grenade = 44,
	_emblem_foreground_tsanta = 45,
	_emblem_foreground_race = 46,
	_emblem_foreground_valkyire = 47,
	_emblem_foreground_drone = 48,
	_emblem_foreground_grunt = 49,
	_emblem_foreground_grunt_head = 50,
	_emblem_foreground_brute_head = 51,
	_emblem_foreground_runes = 52,
	_emblem_foreground_trident = 53,
	_emblem_foreground_number0 = 54,
	_emblem_foreground_number1 = 55,
	_emblem_foreground_number2 = 56,
	_emblem_foreground_number3 = 57,
	_emblem_foreground_number4 = 58,
	_emblem_foreground_number5 = 59,
	_emblem_foreground_number6 = 60,
	_emblem_foreground_number7 = 61,
	_emblem_foreground_number8 = 62,
	_emblem_foreground_number9 = 63
};

enum e__emblem_background : uint8
{
	_emblem_background_solid = 0,
	_emblem_background_vertical_split = 1,
	_emblem_background_horizontal_split1 = 2,
	_emblem_background_horizontal_split2 = 3,
	_emblem_background_vertical_gradient = 4,
	_emblem_background_horizontal_gradient = 5,
	_emblem_background_triple_column = 6,
	_emblem_background_triple_row = 7,
	_emblem_background_quadrants1 = 8,
	_emblem_background_quadrants2 = 9,
	_emblem_background_diagonal_slice = 10,
	_emblem_background_cleft = 11,
	_emblem_background_x1 = 12,
	_emblem_background_x2 = 13,
	_emblem_background_dircle = 14,
	_emblem_background_diamond = 15,
	_emblem_background_cross = 16,
	_emblem_background_square = 17,
	_emblem_background_dual_half_circle = 18,
	_emblem_background_triangle = 19,
	_emblem_background_diagonal_quadrant = 20,
	_emblem_background_three_quaters = 21,
	_emblem_background_quarter = 22,
	_emblem_background_four_rows1 = 23,
	_emblem_background_four_rows2 = 24,
	_emblem_background_split_circle = 25,
	_emblem_background_one_third = 26,
	_emblem_background_two_thirds = 27,
	_emblem_background_upper_field = 28,
	_emblem_background_top_and_bottom = 29,
	_emblem_background_center_stripe = 30,
	_emblem_background_left_and_right = 31
};

enum e_emblem_flags : uint8
{

};

struct s_emblem_info
{
	e__emblem_foreground foreground_emblem;
	e__emblem_background background_emblem;
	e_emblem_flags emblem_flags;
};
CHECK_STRUCT_SIZE(s_emblem_info, 3);
