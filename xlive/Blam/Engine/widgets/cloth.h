#pragma once
#include "cloth_definitions.h"
#include "math/real_math.h"
#include "memory/data.h"
#include "tag_files/tag_group_access.h"

/* macros */

#define cloth_get(index) ((struct cloth_datum*)datum_get(cloth_data_get(), index))

/* constants */

enum
{
	k_maximum_vertices_per_cloth = 128,
	k_maximum_attachments_per_cloth = 6,

	k_cloth_minimum_relaxation_iterations = 1,
	k_cloth_maximum_relaxation_iterations = 15,
};

constexpr real32 k_cloth_teleport_distance = 0.4f;

constexpr real32 k_cloth_maximum_link_correction = 2.0f;

constexpr real32 k_cloth_frozen_object_velocity = 1.5f;

constexpr real32 k_cloth_gravity = -3.2086613f;
constexpr real32 k_cloth_gravity_time_step = 0.03125f;

constexpr real32 k_cloth_dead_object_delete_seconds = 10.f;

/* structures */

struct cloth_vertex
{
	real_point3d position;
	real_point3d last_position;
	real_vector3d normal;
};
ASSERT_STRUCT_SIZE(cloth_vertex, 36);

struct cloth_vertex_attachment
{
	int16 vertex_index;
	int16 marker_index;
	int16 pad;
};
ASSERT_STRUCT_SIZE(cloth_vertex_attachment, 6);

struct cloth_datum
{
	int16 identifier;
	int16 pad;
	datum definition_index;
	datum object_index;
	real32 time_accumulator;
	real_point3d last_attached_object_position;
	real32 object_velocity;
	cloth_vertex dynamic_vertices[k_maximum_vertices_per_cloth];
	uint32 attached_bit_vector[BIT_VECTOR_SIZE_IN_LONGS(k_maximum_vertices_per_cloth)];
	cloth_vertex_attachment attachments[k_maximum_attachments_per_cloth];
	int16 attachment_count;
	int16 pad2;
};
ASSERT_STRUCT_SIZE(cloth_datum, 4696);

/* prototypes */

void cloth_apply_patches(void);

data_array* cloth_data_get(void);

void cloth_update(real32 dt);
