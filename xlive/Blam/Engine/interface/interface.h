#pragma once

enum e_split_type : int32
{
	_split_type_horizontal = 0,
	_split_type_vertical = 1
};

enum e_display_type : int32
{
	_display_type_widescreen = 0,
	_display_type_4_by_3 = 1
};

void apply_interface_hooks(void);
