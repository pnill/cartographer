#pragma once
#include "math/color_math.h"
#include "tag_files/data_reference.h"
#include "tag_files/tag_reference.h"

/* enums */

enum e_meter_definition_flags : int32
{
	// There are none :)
};

enum e_meter_interpolate_colors : int16
{
    _meter_interpolate_colors_linearly = 0,
    _meter_interpolate_colors_faster_near_empty = 1,
    _meter_interpolate_colors_faster_near_full = 2,
    _meter_interpolate_colors_through_random_noise = 3
};

enum e_meter_anchor_colors : int16
{
    _meter_anchor_colors_at_both_ends = 0,
    _meter_anchor_colors_at_empty = 1,
    _meter_anchor_colors_at_full = 2
};

/* structures */

struct meter_definition
{
	e_meter_definition_flags flags;

	// two bitmaps specifying the mask and the meter levels
	tag_reference stencil_bitmaps;	// bitm

	// optional bitmap to draw into the unmasked regions of the meter (modulated by the colors below)
	tag_reference source_bitmap;	// bitm
    int16 stencil_sequence_index;
    int16 source_sequence_index;
    real_argb_color gradient;
    int8 pad_1[4];
    
    e_meter_interpolate_colors interpolate_colors;
    e_meter_anchor_colors anchor_colors;

    int8 pad_2[8];
    real_argb_color empty_color;
    real_argb_color full_color;

    int8 pad_3[20];
    real32 unmask_distance_meter_units; // fade from fully masked to fully unmasked this distance beyond full (and below empty)
    real32 mask_distance_meter_units;   // fade from fully unmasked to fully masked this distance below full (and beyond empty)
    int8 pad_4[20];

    /****************************************
    * definition_name: meter_stencil_data_definition
    * flags: 0
    * alignment_bit: 0
    * byteswap_proc: 0x00560bc0
    ****************************************/
    // DataSize(65536)
    data_reference encoded_stencil;
};
ASSERT_STRUCT_SIZE(meter_definition, 144);
