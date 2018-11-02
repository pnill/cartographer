#include "H2MOD.h"
#include "../Camera.h"

#define _USE_MATH_DEFINES
#include "math.h"

void Camera::Tweaks::SetFOV(float field_of_view_degrees) 
{
	if (h2mod->Server)
		return;

	if (field_of_view_degrees > 0.f && field_of_view_degrees <= 110.f)
	{
		float current_FOV = *reinterpret_cast<float*>(h2mod->GetBase() + 0x41D984);

		//int res_width = *(int*)(H2BaseAddr + 0xA3DA00); //wip
		//int res_height = *(int*)(H2BaseAddr + 0xA3DA04);

		const double default_radians_FOV = 70.0 * M_PI / 180.0;

		float calculated_radians_FOV = (field_of_view_degrees * M_PI / 180.0f) / static_cast<float>(default_radians_FOV);
		WriteValue(h2mod->GetBase() + 0x41D984, calculated_radians_FOV); // First Person
		WriteValue(h2mod->GetBase() + 0x413780, calculated_radians_FOV + 0.22f); // Third Person
	}
}