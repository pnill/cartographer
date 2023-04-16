#pragma once
#include "Blam/Engine/camera/camera.h"

void __cdecl render_camera_build_projection_hook(s_camera* camera, float* frustum_bounds, real_matrix4x3* out_projection);