#pragma once
#include "antenna_definitions.h"
#include "math/real_math.h"
#include "memory/data.h"
#include "tag_files/tag_group_access.h"

/* macros */

#define antenna_definition_get(index) ((struct antenna_definition*)tag_get(_tag_group_antenna, (index)))

#define antenna_get(index) ((struct antenna_datum*)datum_get(antenna_data_get(), (index)))

/* constants */

enum
{
	k_antenna_maximum_vertex_datum_count = MAXIMUM_NUMBER_OF_ANTENNA_VERTICES + 1,

	k_antenna_render_catch_up_update_threshold = 5,
	k_antenna_render_catch_up_update_count = 3,
};

constexpr real32 k_antenna_render_catch_up_dt = 0.05f;

constexpr real32 k_antenna_render_projected_pixels = 100.f;

/* structures */

struct antenna_vertex_datum
{
	real_point3d position;
	real_vector3d velocity;
	real32 sprite_scale;
	int16 sprite_index;
	int16 pad;
};
ASSERT_STRUCT_SIZE(antenna_vertex_datum, 32);

struct antenna_datum
{
	int16 identifier;
	int16 pad;
	bool unused;
	bool invalid;
	int16 updates_since_last_render;
	datum definition_index;
	datum object_index;
	real_point3d last_attachment_location;
	antenna_vertex_datum vertices[k_antenna_maximum_vertex_datum_count];
};
ASSERT_STRUCT_SIZE(antenna_datum, 700);

/* prototypes */

void antenna_apply_patches(void);

data_array* antenna_data_get(void);

void __cdecl antenna_update(antenna_datum* antenna, const antenna_definition* definition, real32 dt);

void __cdecl antenna_render_proper(datum object_index, datum antenna_index);
