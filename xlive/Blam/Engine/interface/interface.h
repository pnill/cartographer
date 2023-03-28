#pragma once

enum e_split_type : int
{
	_split_type_horizontal = 0,
	_split_type_vertical = 1
};

enum e_display_type : int
{
	_display_type_16_by_9 = 0,
	_display_type_4_by_3 = 1
};

int get_player_window_count();
e_split_type get_splitscreen_split_type();
void set_display_type(e_display_type split_type);
void draw_splitscreen_line();
void apply_interface_hooks();
