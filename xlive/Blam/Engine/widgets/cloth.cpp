#include "stdafx.h"
#include "cloth.h"

#include "game/game_time.h"
#include "math/random_math.h"
#include "memory/data.h"
#include "objects/objects.h"
#include "objects/widgets/widgets.h"
#include "render/render.h"

/* typedefs */

typedef void(__cdecl* t_cloth_update)(real32 dt);
t_cloth_update p_cloth_update;

/* prototypes */

static real32 cloth_approximate_square_root(real32 f);

static void __cdecl cloth_enforce_attachments(datum cloth_index);

static void __cdecl cloth_reset_flag_to_default_positions(datum cloth_index);

static void __cdecl cloth_update_positions_verlet(datum cloth_index);

static void __cdecl cloth_constrain_positions_relaxation(datum cloth_index);

static void __cdecl cloth_calculate_normals(datum cloth_index);

static void __cdecl cloth_check_for_delete(datum cloth_index);

/* public code */

void cloth_apply_patches(void)
{
	// disable cloth debugging that writes to cloth.txt
	*Memory::GetAddress<bool*>(0x41F650) = false;

	DETOUR_ATTACH(p_cloth_update, Memory::GetAddress<t_cloth_update>(0x18F6D0), cloth_update);

	PatchCall(Memory::GetAddress(0x18F6B7, 0x0), cloth_constrain_positions_relaxation);

	PatchCall(Memory::GetAddress(0x18E19C, 0x0), object_get_markers_by_string_id);

	return;
}


data_array* cloth_data_get(void)
{
	return *Memory::GetAddress<data_array**>(0x4E66A8);
}

void cloth_update(real32 dt)
{
	data_array* cloth_array = cloth_data_get();

	for (datum index = data_next_index(cloth_array, NONE); index != NONE; index = data_next_index(cloth_array, index))
	{
		if (game_time_initialized())
		{
			cloth_datum* cloth = cloth_get(index);
			cloth->time_accumulator += dt;

			if (cloth->time_accumulator > game_tick_length())
			{
				cloth->time_accumulator -= (int32)(cloth->time_accumulator / game_tick_length()) * game_tick_length();
				cloth_constrain_positions_relaxation(index);
				cloth_update_positions_verlet(index);
				cloth_calculate_normals(index);
			}
			cloth_check_for_delete(index);
		}
	}
}

/* private code */

static real32 cloth_approximate_square_root(real32 f)
{
	int32 bits = *(int32*)&f;
	bits = (bits >> 1) + 0x1FC00000;
	return *(real32*)&bits;
}

void __cdecl cloth_enforce_attachments(datum cloth_index)
{
	cloth_datum* cloth = cloth_get(cloth_index);
	const cloth_definition* definition = cloth_definition_get(cloth->definition_index);
	const object_datum* object = object_get(cloth->object_index);

	object_marker markers[k_cloth_maximum_attachment_count];
	const bool markers_valid = cloth->attachment_count > 0
		&& object_get_markers_by_string_id(cloth->object_index, definition->marker_attachment_name, markers, NUMBEROF(markers)) > 0;

	real_point3d attached_position = object->object.center;
	if (markers_valid)
	{
		attached_position = markers[cloth->attachments[0].marker_index].matrix.position;
	}

	if (distance_squared3d(&cloth->last_attached_object_position, &attached_position) > k_cloth_teleport_distance * k_cloth_teleport_distance)
	{
		real_vector3d delta;
		vector_from_points3d(&cloth->last_attached_object_position, &attached_position, &delta);

		for (int32 vertex_index = 0; vertex_index < definition->verticies.count; vertex_index++)
		{
			cloth_vertex* vertex = &cloth->dynamic_vertices[vertex_index];
			add_vectors3d((real_vector3d*)&vertex->position, &delta, (real_vector3d*)&vertex->position);
			vertex->last_position = vertex->position;
		}
	}

	real_vector3d translational_velocity;
	object_get_velocities(cloth->object_index, &translational_velocity, NULL);

	cloth->last_attached_object_position = attached_position;
	cloth->object_velocity = magnitude3d(&translational_velocity);

	if (markers_valid)
	{
		for (int32 attachment_index = 0; attachment_index < cloth->attachment_count; attachment_index++)
		{
			const cloth_vertex_attachment* attachment = &cloth->attachments[attachment_index];
			cloth->dynamic_vertices[attachment->vertex_index].position = markers[attachment->marker_index].matrix.position;
			BIT_VECTOR_SET_FLAG(cloth->attached_bit_vector, attachment->vertex_index, true);
		}
	}

	return;
}

void __cdecl cloth_reset_flag_to_default_positions(datum cloth_index)
{
	cloth_datum* cloth = cloth_get(cloth_index);
	const cloth_definition* definition = cloth_definition_get(cloth->definition_index);

	for (int32 vertex_index = 0; vertex_index < definition->verticies.count; vertex_index++)
	{
		const cloth_vertex_definition* vertex_definition = TAG_BLOCK_GET_ELEMENT(&definition->verticies, vertex_index, cloth_vertex_definition);
		cloth_vertex* vertex = &cloth->dynamic_vertices[vertex_index];

		vertex->position = vertex_definition->initial_position;
		vertex->last_position = vertex_definition->initial_position;
		vertex->normal = *global_left3d;
	}

	cloth->last_attached_object_position = *global_origin3d;

	cloth_enforce_attachments(cloth_index);
	return;
}

void __cdecl cloth_constrain_positions_relaxation(datum cloth_index)
{
	cloth_datum* cloth = cloth_get(cloth_index);
	const cloth_definition* definition = cloth_definition_get(cloth->definition_index);

	cloth_enforce_attachments(cloth_index);

	int32 iteration_count = (int32)((real32)definition->properties.number_iterations + cloth->object_velocity * 3.0f);
	iteration_count = PIN(iteration_count, k_cloth_minimum_relaxation_iterations, k_cloth_maximum_relaxation_iterations);

	bool reset_this_update = false;

	for (int32 iteration = 0; iteration < iteration_count; iteration++)
	{
		bool restart_iteration = false;

		for (int32 link_index = 0; link_index < definition->links.count && !restart_iteration; link_index++)
		{
			const cloth_link_definition* link = TAG_BLOCK_GET_ELEMENT(&definition->links, link_index, cloth_link_definition);

			cloth_vertex* vertex_1 = &cloth->dynamic_vertices[link->index_1];
			cloth_vertex* vertex_2 = &cloth->dynamic_vertices[link->index_2];

			const bool vertex_1_attached = BIT_VECTOR_TEST_FLAG(cloth->attached_bit_vector, link->index_1);
			const bool vertex_2_attached = BIT_VECTOR_TEST_FLAG(cloth->attached_bit_vector, link->index_2);

			real_vector3d delta;
			vector_from_points3d(&vertex_1->position, &vertex_2->position, &delta);

			real32 distance_squared = (delta.k * delta.k + delta.j * delta.j) + delta.i * delta.i;
			real32 distance = cloth_approximate_square_root(distance_squared);
			if (distance < k_real_epsilon)
			{
				distance = k_real_epsilon;
			}

			real32 default_distance_squared = link->default_distance * link->default_distance;
			real32 correction = (default_distance_squared / (distance * distance + default_distance_squared) - 0.5f) * distance;

			real_vector3d direction;
			real_vector3d correction_vector;
			scale_vector3d(&delta, 1.0f / distance, &direction);
			scale_vector3d(&direction, correction, &correction_vector);

			real_vector3d* position_1 = (real_vector3d*)&vertex_1->position;
			real_vector3d* position_2 = (real_vector3d*)&vertex_2->position;

			if (!vertex_1_attached)
			{
				subtract_vectors3d(position_1, &correction_vector, position_1);
				if (vertex_2_attached)
				{
					subtract_vectors3d(position_1, &correction_vector, position_1);
				}
			}

			if (!vertex_2_attached)
			{
				add_vectors3d(position_2, &correction_vector, position_2);
				if (vertex_1_attached)
				{
					add_vectors3d(position_2, &correction_vector, position_2);
				}
			}

			if (::abs(correction) > k_cloth_maximum_link_correction && !reset_this_update)
			{
				cloth_reset_flag_to_default_positions(cloth_index);
				reset_this_update = true;
				restart_iteration = true;
			}
		}

		if (restart_iteration)
		{
			iteration--;
			continue;
		}

		cloth_enforce_attachments(cloth_index);
	}

	return;
}

void cloth_wind_exclude_from_pole_by_angle(real_vector3d* direction, real32 angle)
{
	const real32 cosine_angle = cosine(angle);
	if (::abs(direction->k) > cosine_angle)
	{
		direction->k = (direction->k >= 0.f) ? cosine_angle : -cosine_angle;
	}

	real_vector2d* horizontal = (real_vector2d*)direction;
	if ((::abs(horizontal->i) < 0.01f && ::abs(horizontal->j) < 0.01f) || ::abs(normalize2d(horizontal)) < k_real_epsilon)
	{
		horizontal->i = 1.f;
		horizontal->j = 0.f;
	}
	normalize2d(horizontal);

	const real32 horizontal_scale = square_root(1.f - direction->k * direction->k);
	scale_vector2d(horizontal, horizontal_scale, horizontal);
	return;
}

void __cdecl cloth_update_positions_verlet(datum cloth_index)
{
	void* func = Memory::GetAddress<void*>(0x18E82F);
	__asm
	{
		lea eax, cloth_index
		mov eax, dword ptr[eax]
		call func
	}
}

void __cdecl cloth_calculate_normals(datum cloth_index)
{
	cloth_datum* cloth = cloth_get(cloth_index);
	const cloth_definition* definition = cloth_definition_get(cloth->definition_index);

	for (int32 index = 0; index < definition->indices.count; index += 3)
	{
		cloth_vertex* vertex_0 = &cloth->dynamic_vertices[TAG_BLOCK_GET_ELEMENT(&definition->indices, index + 0, cloth_index_definition)->index];
		cloth_vertex* vertex_1 = &cloth->dynamic_vertices[TAG_BLOCK_GET_ELEMENT(&definition->indices, index + 1, cloth_index_definition)->index];
		cloth_vertex* vertex_2 = &cloth->dynamic_vertices[TAG_BLOCK_GET_ELEMENT(&definition->indices, index + 2, cloth_index_definition)->index];

		real_vector3d edge_1;
		real_vector3d edge_2;
		real_vector3d normal;
		vector_from_points3d(&vertex_0->position, &vertex_1->position, &edge_1);
		vector_from_points3d(&vertex_0->position, &vertex_2->position, &edge_2);
		cross_product3d(&edge_1, &edge_2, &normal);

		normalize3d(&normal);

		add_vectors3d(&vertex_0->normal, &normal, &vertex_0->normal);
		add_vectors3d(&vertex_1->normal, &normal, &vertex_1->normal);
		add_vectors3d(&vertex_2->normal, &normal, &vertex_2->normal);
	}

	for (int32 vertex_index = 0; vertex_index < definition->verticies.count; vertex_index++)
	{
		cloth_vertex* vertex = &cloth->dynamic_vertices[vertex_index];
		if (normalize3d(&vertex->normal) == 0.f)
		{
			vertex->normal = *global_left3d;
		}
	}

	return;
}

void __cdecl cloth_check_for_delete(datum cloth_index)
{
	const cloth_datum* cloth = cloth_get(cloth_index);
	if (cloth->object_index == NONE)
	{
		return;
	}

	if (object_get_type(cloth->object_index) == _object_type_biped)
	{
		const object_datum* object = object_get(cloth->object_index);
		if (object->object.object_damage_flags.test(_object_is_dead_bit))
		{
			const real32 ticks_since_motion = (real32)(int32)(game_time_get() - object->object.last_motion_time);
			if (game_ticks_to_seconds(ticks_since_motion) > k_cloth_dead_object_delete_seconds)
			{
				widget_delete_type_datum(cloth->object_index, cloth_index);
			}
		}
	}

	return;
}