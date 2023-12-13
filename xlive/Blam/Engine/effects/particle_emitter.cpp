#include "stdafx.h"
#include "particle_emitter.h"

#include "Blam/Engine/camera/camera.h"

void c_particle_emitter::adjust_matrix_and_vector_to_effect_camera(bool use_effect_camera, real_matrix3x3* out_matrix, real_point3d* out_point) const
{
	if(use_effect_camera)
	{
		s_camera* effect_camera = get_effect_camera();
		real_matrix3x3 effect_camera_matrix;
		real_matrix3x3 temp_matrix;

		memcpy(&temp_matrix, &this->matrix, sizeof(real_matrix3x3));
		real_point3d temp_point = this->position;
		matrix3x3_from_forward_and_up(&effect_camera_matrix, &effect_camera->forward, &effect_camera->up);
		matrix3x3_multiply(&effect_camera_matrix, &temp_matrix, &temp_matrix);
		matrix3x3_transform_vector(&effect_camera_matrix, &temp_point, &temp_point);

		temp_point.x += effect_camera->point.x;
		temp_point.y += effect_camera->point.y;
		temp_point.z += effect_camera->point.z;

		*out_matrix = temp_matrix;
		*out_point = temp_point;
	}
	else
	{
		*out_matrix = this->matrix;
		*out_point = this->position;
	}
}

s_data_array* get_particle_emitter_table()
{
	//TODO Server offset
	return *Memory::GetAddress<s_data_array**>(0x4DD090, 0x0);
}
