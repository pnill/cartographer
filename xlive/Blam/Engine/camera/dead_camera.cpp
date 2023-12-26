#include "stdafx.h"
#include "dead_camera.h"

#include "Blam/Engine/objects/objects.h"
#include "Util/Hooks/Hook.h"

void apply_dead_camera_patches()
{
	// Patch call inside dead_camera update to try and get interpolated node matrix from the current target_datum
	PatchCall(Memory::GetAddress(0xCDBAE), object_try_get_node_matrix_interpolated);
}
