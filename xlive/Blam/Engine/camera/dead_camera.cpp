#include "stdafx.h"
#include "dead_camera.h"

#include "Blam/Engine/objects/objects.h"
#include "Util/Hooks/Hook.h"

real_point3d* __cdecl object_try_and_get_interpolated_position(datum object_index, int16 node_index, real_point3d* out_pos)
{
	real_matrix4x3 out_mat = *object_try_get_node_matrix_interpolated(object_index, node_index, &out_mat);
	*out_pos = out_mat.position;
	return out_pos;
}

__declspec(naked) void object_try_get_node_position_interpolated_intermediate()
{
#define current_stack_offset (2Ch + 4h + 4h)
	__asm
	{
		// grab the out position mem address at 0x0C
		mov eax, [esp + 4 + current_stack_offset + 0Ch]
		add eax, 4 // go to position offset
		push eax
		// grab the rest of the parameters
		mov eax, [esp + 4 + 4 + 4]
		push eax // node index
		mov eax, [esp + 4 + 4 + 4]
		push eax // object index
		call object_try_and_get_interpolated_position
		add esp, 4 * 3
		// copy ptr of the data in esi
		mov esi, [esp + 4 + current_stack_offset + 0Ch]
		retn
	}
#undef current_stack_offset
}

void apply_dead_camera_patches()
{
	// Patch call inside dead_camera update to try and get interpolated node matrix from the current target_datum
	PatchCall(Memory::GetAddress(0xCDBAE), object_try_get_node_position_interpolated_intermediate);
	// nop the rest of the unneeded instructions
	NopFill(Memory::GetAddress(0xCDBB3), 22);
}
