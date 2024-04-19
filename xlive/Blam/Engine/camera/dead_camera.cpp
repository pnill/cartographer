#include "stdafx.h"
#include "dead_camera.h"

#include "objects/objects.h"
#include "input/input_abstraction.h"

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

void* __cdecl death_cam_get_controller_input(e_controller_index controller)
{
	//orignally 
	// return input_get_gamepad_state(controller);

	// instead we return abstracted_input_state
	// then we test for abstracted _button_jump in the caller
	return &input_abstraction_globals->input_states[controller];
}

// allows keyboards/gamepads/any device to switch death b/w targets
void dead_camera_switch_patch()
{
	/*
	*	we change from
	*	input_get_gamepad_state(controller_index)->button_frames_down[_xinput_gamepad_a];
	*		to
	*	input_abstraction_globals->input_states[controller_index].m_down_frames[_button_jump];
	*
	*/

	// Patch call inside dead_camera update that gets input_get_gamepad_state
	PatchCall(Memory::GetAddress(0xCDEF3), death_cam_get_controller_input);
	//uint8 orignal_opcodes[] = { 0x80 ,0x78 , _xinput_gamepad_a ,0x01 };


	uint8 opcodes[] = { 0x80 ,0x78 ,_button_jump ,0x01 };
	WriteBytes(Memory::GetAddress(0xCDEFF), opcodes, NUMBEROF(opcodes));
}

void apply_dead_camera_patches()
{
	// Patch call inside dead_camera update to try and get interpolated node matrix from the current target_datum
	PatchCall(Memory::GetAddress(0xCDBAE), object_try_get_node_position_interpolated_intermediate);
	// nop the rest of the unneeded instructions
	NopFill(Memory::GetAddress(0xCDBB3), 22);

	dead_camera_switch_patch();
}
