#pragma once
#include "math/color_math.h"
#include "tag_files/string_id.h"
#include "tag_files/tag_block.h"
#include "tag_files/tag_reference.h"

#define MAXIMUM_NUMBER_OF_ANTENNA_VERTICES 20

// max: MAXIMUM_NUMBER_OF_ANTENNA_VERTICES
struct antenna_vertex
{
	/// strength of the spring (larger values make the spring stronger)
	real32 spring_strength_coefficient;

	int8 pad_1[24];
	/// direction toward next vertex
	real_euler_angles2d direction;
	/// distance between this vertex and the next in world units
	real32 length;
	/// bitmap group sequence index for this vertex's texture
	uint16 sequence_index;

	int8 pad_2[2];
	/// color at this vertex
	real_argb_color color;
	/// color at this vertex for the low-LOD line primitives
	real_argb_color lod_color;

	int8 pad_3[52];
};
ASSERT_STRUCT_SIZE(antenna_vertex, 128);

struct antenna_definition
{
	/// the marker name where the antenna should be attached
	string_id marker_name;

	tag_reference bitmap; // bitm

	tag_reference physics; // pphy

	int8 pad_1[80];

	real32 spring_strength_coefficient;
	real32 falloff_pixels;
	real32 cutoff_pixels;

	int8 pad_2[40];

	tag_block<antenna_vertex> vertices;
};
ASSERT_STRUCT_SIZE(antenna_definition, 160);
