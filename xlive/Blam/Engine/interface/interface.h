#pragma once
#include "bitmaps/bitmap_group.h"
#include "math/real_math.h"

/* enums */

enum e_interface_tag : uint8
{
    _interface_tag_spinner,                         // bitm
    _interface_tag_obsolete,                        // bitm
    _interface_tag_screen_color_table,              // colo
    _interface_tag_hud_color_table,                 // colo
    _interface_tag_editor_color_table,              // colo
    _interface_tag_dialog_color_table,              // colo
    _interface_tag_hud_globals,                     // hudg
    _interface_tag_motion_sensor_sweep_bitmap,      // bitm 
    _interface_tag_motion_sensor_sweep_bitmap_mask, // bitm
    _interface_tag_multiplayer_hud_bitmap,          // bitm
    _interface_tag_unknown_field,
    _interface_tag_hud_digits_definition,           // hud#
    _interface_tag_motion_sensor_blip_bitmap,       // bitm
    _interface_tag_interface_goo_map_1,             // bitm
    _interface_tag_interface_goo_map_2,             // bitm
    _interface_tag_interface_goo_map_3,             // bitm
    NUMBER_OF_INTERFACE_TAGS
};

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

/* public code */

void apply_interface_hooks(void);

datum interface_get_tag_index(e_interface_tag interface_tag_index);

void __cdecl interface_draw_bitmap(
    bitmap_data* bitmap,
    const real_point2d* position,
    const real_rectangle2d* rectangle,
    real32 a4,
    real32 a5,
    real32 a6);
