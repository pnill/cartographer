#include "H2MOD.h"
#include "InputFixes.h"
#include "Util/Hooks/Hook.h"
#include "H2MOD/Modules/OnScreenDebug/OnscreenDebug.h"
#include "Blam/Engine/Players/Players.h"
#include "SplitScreenCommon.h"

typedef DWORD(__stdcall *tsystem_get_time_internal)();
tsystem_get_time_internal system_get_time_internal;

typedef bool(_cdecl *tisWindowNotInFocus_or_XLiveEatingInput)();
tisWindowNotInFocus_or_XLiveEatingInput isWindowNotInFocus_or_XLiveEatingInput;

typedef blam_mouse_input*(__cdecl* tget_mouse_input)();
tget_mouse_input get_mouse_state;

typedef float(__cdecl* tapply_mouse_input_to_buffer)(float a1, float a2, signed int a3);
tapply_mouse_input_to_buffer apply_mouse_input_to_buffer;

typedef bool(__cdecl* tis_controller_plugged_in)(int controller_index);
tis_controller_plugged_in is_controller_plugged_in;

typedef int(__cdecl* tget_mouse_button_state)(int mkey);
tget_mouse_button_state get_mouse_button_state;

typedef int(__cdecl *tget_time_passed_since_last_input)();
tget_time_passed_since_last_input get_time_passed_since_last_input;

typedef int(__cdecl* tupdate_all_button_states)(BYTE*,WORD*,BYTE*,char,int);
tupdate_all_button_states update_all_button_states;

typedef char(__cdecl* tget_unk_biped_data)(unsigned long);
tget_unk_biped_data get_unk_biped_data;

typedef char(__cdecl* tget_unk_biped_data2)(unsigned long);
tget_unk_biped_data2 get_unk_biped_data2;

typedef char(__cdecl* tget_vehicle_input_data)();
tget_vehicle_input_data get_vehicle_input_data;

typedef int(__cdecl* tcontroller2player)();
tcontroller2player controller2player;

typedef unsigned int(__cdecl* tupdate_controller_input_states)(int a1, int a2);
tupdate_controller_input_states update_controller_input_states;

typedef short(__cdecl* tget_keyboard_states)(short);
tget_keyboard_states get_keyboard_states;

typedef bool(__cdecl* tisPlayerValid)();
tisPlayerValid isPlayervalid;

typedef BYTE*(__cdecl* tdatum_try_and_get)(BYTE* a1, int a2);
tdatum_try_and_get datum_try_and_get;

BYTE* controller_input_array = 0;
BYTE input_processed = 0;
DWORD time_passed_since_input_applied = 0; // 0x4AE86C
float const_mouse_accel = 0.0031415927;
float const_mouse_time = 16.0;
BYTE *unkInputByte;
keybind_parent *kbp2 = NULL;
int player_count = 1;

// This stuff will only be needed when mutli-controller support is added.
/*struct controller_input_state
{
BYTE triggers_analog_state[2];
BYTE old_triggers_analog_state[2];
BYTE triggers_hold_time[2];
BYTE button_hold_time[14];
};*/

/*
void  update_controller_input_hook(BYTE controller_index)
{

BYTE trigger_state = false;
bool left_trigger_state = false;
bool right_trigger_state = false;
bool were_buttons_pressed = false;

//typedef void(__cdecl* tupdate_controller_input)(int);
//tupdate_controller_input pupdate_controller_input = (tupdate_controller_input)h2mod->GetAddress(0x2FBD2);

int *InputDevicesArray = (int*)h2mod->GetAddress(0x479F00);

typedef DWORD(__thiscall* tget_xinput_state)(int* pThis, void* a2);
tget_xinput_state pxget_input_state = h2mod->GetAddress<tget_xinput_state>(0x8ACE3);


XINPUT_STATE inputs;
pxget_input_state((int*)*(InputDevicesArray), &inputs);
int trigger_index;

BYTE* bController = (BYTE*)h2mod->GetAddress<BYTE*>(0x47A5C8);
*(bController + 0x80) = 1;

DWORD cis_addr = h2mod->GetAddress(0x47A5CC)+0x40;
controller_input_state *cis = (controller_input_state*)cis_addr;
WORD *trigger_hold_state = h2mod->GetAddress<WORD*>(0x475AE0)+0x40;

for (trigger_index = 0; trigger_index < 2; trigger_index++)
{
if (trigger_index)
{
if (trigger_index == 1)
trigger_state = inputs.Gamepad.bRightTrigger;
}
else
{
trigger_state = inputs.Gamepad.bLeftTrigger;
}

left_trigger_state = trigger_state < cis->old_triggers_analog_state[trigger_index];
right_trigger_state = trigger_state == cis->old_triggers_analog_state[trigger_index];
cis->triggers_analog_state[trigger_index] = trigger_state;

if (left_trigger_state || right_trigger_state)
{
cis->triggers_hold_time[trigger_index] = 0;
*(trigger_hold_state + trigger_index) = 0;
}
else {
BYTE trigger_hold_time = cis->triggers_hold_time[trigger_index] + 1;

if (trigger_hold_time > 255)
trigger_hold_time = -1;

cis->triggers_hold_time[trigger_index] = trigger_hold_time;

int trigger_hold_state_2 = (controller_index + (BYTE)trigger_hold_state);
if (trigger_hold_state_2 > 65535)
{
trigger_hold_state_2 = -1;
}

*(trigger_hold_state + trigger_index) = trigger_hold_state_2;
were_buttons_pressed = true;
}

BYTE _triggers_analog_state = cis->triggers_analog_state[trigger_index];
BYTE triggers_analog_state_new;
if (left_trigger_state || right_trigger_state)
{
if (_triggers_analog_state <= 191)
triggers_analog_state_new = _triggers_analog_state + 64;
else
triggers_analog_state_new = -1;
if (triggers_analog_state_new < cis->old_triggers_analog_state[trigger_index])
{
LABEL_26:
cis->old_triggers_analog_state[trigger_index] = triggers_analog_state_new;
continue;
}
}
else
{
if (_triggers_analog_state >= 32)
triggers_analog_state_new = _triggers_analog_state - 32;
else
triggers_analog_state_new = 0;
if (triggers_analog_state_new > cis->old_triggers_analog_state[trigger_index])
{
goto LABEL_26;
}
}

}

int* _xinput_button_bitmask = (int*)h2mod->GetAddress(0x39DEE0);
BYTE* button_hold_time = cis->button_hold_time;
WORD* button_state_array_ptr = (WORD*)h2mod->GetAddress(0x47A5E4);
int button_index = 14;
BYTE v21;
do {
int v14 = inputs.Gamepad.wButtons & *_xinput_button_bitmask;
if (v14 <= 0)
{
*button_hold_time = 0;
*button_state_array_ptr = 0;
}
else {
int v15 = *button_hold_time +1;
if (v15 > 255)
v15 = -1;
*button_hold_time = v15;
BYTE v16 = controller_index + *button_state_array_ptr;
if (v16 > 0xFFFF)
v16 = -1;
*button_state_array_ptr = v16;
}

if (v14 > 0)
v21 = 1;
++button_state_array_ptr;
++button_hold_time;
++_xinput_button_bitmask;
--button_index;
} while (button_index);



//return pupdate_controller_input(controller_index);
}*/

int __stdcall get_controller_index(int controller_index)
{
	return controller_index;
}

#pragma optimize( "", off )

int c2p(int controller_index)
{
	__asm {
		mov eax,controller_index
	}

	return controller2player();
}
#pragma optimize( "", on )

float process_mouse_cord_input(int last_time_button_state_read, float mouse_input_read,keybind_parent *kbp)
{
	float acceleration = 0;
	float time = 0;
	float time2 = 0;
	float final_input = 0.0;

	acceleration = mouse_input_read * const_mouse_accel;
	if (acceleration < 0.0)
		acceleration = 0.0;

	time = mouse_input_read * (const_mouse_time / last_time_button_state_read);
	time2 = const_mouse_time / last_time_button_state_read;
	if (time < 0.0)
		time = 0.0;

	final_input = apply_mouse_input_to_buffer(kbp->base[0].N01E30347, 1.0, time) / time2;

	return final_input;
}

float process_mouse_scroll_wheel(float mouse_input_read, keybind_parent *kbp)
{
	//if (mouse_input_read < 0.0) breaks mouse scroll in not sure why need to do further research
	//	mouse_input_read = 0.0;

	return mouse_input_read;
}

int process_mouse_input(int mkey, keybind_parent* kbp, int last_time_button_state_read,int action_index)
{
	blam_mouse_input *mouse_input = get_mouse_state();
	float mouse_input_read = 0;
	float final_input = 0.0;
	int ret = 0;

	switch (mkey)
	{
		case 8:
			mouse_input_read = -mouse_input->mouse_lX;
			final_input = process_mouse_cord_input(last_time_button_state_read, mouse_input_read, kbp);
		break;

		case 9:
			mouse_input_read = mouse_input->mouse_lX;
			final_input = process_mouse_cord_input(last_time_button_state_read, mouse_input_read, kbp);
		break;

		case 10:
			mouse_input_read = -mouse_input->mouse_lY;
			final_input = process_mouse_cord_input(last_time_button_state_read, mouse_input_read, kbp);
		break;

		case 11:
			mouse_input_read = mouse_input->mouse_lY;
			final_input = process_mouse_cord_input(last_time_button_state_read, mouse_input_read, kbp);
		break;

		case 12:
			mouse_input_read = mouse_input->mouse_lZ;
			final_input = process_mouse_scroll_wheel(mouse_input_read, kbp);
		break;


		case 13:
			mouse_input_read = -mouse_input->mouse_lZ;
			final_input = process_mouse_scroll_wheel(mouse_input_read, kbp);
		break;
			
	}
	
	if (final_input > kbp->base[0].N01E30344 && mkey >= 8 && mkey < 12)
		ret = 1;

	if (final_input > kbp->base[0].N01E30345 && (mkey == 12 || mkey == 13))
		ret = 1;


	float last_input = kbp->input_state[action_index].Pressure;

	if (final_input > last_input)
		kbp->input_state[action_index].Pressure = final_input;

	return ret;
}


int __cdecl update_player_input_buffer(int controllerIndex, keybind_parent* kbp, controller_input_state* button_state_arrayptr, control_float_double *a5, control_float_double* a6, inputBuffer* ib)
{
	DWORD _last_time_input_processed;
	DWORD current_time = system_get_time_internal();
	DWORD last_time_button_state_read = 0;
	char button_held_state = 0;
	BYTE button_held_state2[4] = { 0,0,0,0 };
	controller_input_state* button_state_arrayptr_preserve = button_state_arrayptr;

	int result = 0;

	if (input_processed & 1)
	{
		_last_time_input_processed = time_passed_since_input_applied;
	}
	else {
		input_processed |= 1;
		_last_time_input_processed = current_time - 16;
	}


	last_time_button_state_read = current_time - _last_time_input_processed;

	if ((current_time - _last_time_input_processed) >= 1)
	{
		if ((current_time - _last_time_input_processed) > 500)
			last_time_button_state_read = 500;
	}
	else {
		last_time_button_state_read = 1;
	}


	time_passed_since_input_applied = current_time;
	button_held_state = 0;

	if (isWindowNotInFocus_or_XLiveEatingInput())
	{

		WORD last_state = ib->NFF74DD7C;
		WORD last_state2 = ib->NFF74DD7B;
		memset(ib, 0, sizeof(inputBuffer));
		result = 0;
		a5->param1 = 0.0;
		a5->param2 = 0.0;
		a6->param1 = 0.0;
		a6->param2 = 0.0;
		ib->NFF74DD7C = last_state;
		ib->NFF74DD7B = last_state2;

		return result;
	}

	if (isSplitScreen())
	{
		if (kbp2 == NULL)
		{
			kbp2 = new keybind_parent;
			memcpy(kbp2, kbp, sizeof(keybind_parent));
		}

		player_count = 2;
	}
	
	bool disable_mouse = false;
	bool disable_keyboard = false;
	bool disable_controller = false;
	for (int current_player = 0; current_player < player_count; current_player++)
	{
		if (current_player == 1)
		{

			kbp = kbp2;
			ib = (inputBuffer*)((BYTE*)ib + 0xB8);
			//ib = ib + sizeof(inputBuffer);

			disable_mouse = true;
			disable_keyboard = true;
			//disable_controller = false;
			*(DWORD*)h2mod->GetAddress(0x4AE85C) = 1;
			button_state_arrayptr = button_state_arrayptr_preserve;

			controllerIndex = 1;
		}
		else{
			if (isSplitScreen())
			{
				*(DWORD*)h2mod->GetAddress(0x4AE85C) = 0;
				button_state_arrayptr = 0;
				disable_mouse = false;
				disable_keyboard = false;
				//disable_controller = true;
			}
		}

		for (int i = 0; i < 57; i++)
		{
			float button_hold_time = 0.0f;
			kbp->input_state[i].Pressure = 0.0;

			int button_held_state = 0;
			ZeroMemory(button_held_state2, 4);


			if (kbp->base[0].action_bind[i].device_count > 0)
			{
				float button_handled = false;

				for (int dindex = 0; dindex < kbp->base[0].action_bind[i].device_count; dindex++)
				{
					float final_input = 0.0;
					float accel_calculation = 0.0;
					int key_code = kbp->base[0].action_bind[i].device[dindex].key;
					button_handled = false;

					device_index current_device = kbp->base[0].action_bind[i].device[dindex].index;

					if (current_device == device_index::mouse && !disable_mouse)
					{
						float mouse_input_read = 0;
						float acceleration = 0;
						float button_time = 0;
						float button_time2 = 0;
						float output_value = 0;


						if (key_code < 0 || key_code >= 8)
						{
							if (process_mouse_input(key_code, kbp, last_time_button_state_read, i))
							{
								button_held_state |= 1;
								button_held_state2[0] |= 1;
								button_handled = true;
							}
						}

						bool mouse_button_down = get_mouse_button_state(key_code) > kbp->base[0].action_bind[i].device[dindex].unknown;
						if (mouse_button_down)
						{
							button_held_state |= mouse_button_down;
							button_held_state2[0] |= mouse_button_down;
							final_input = 1.0;
							button_handled = true;

						}
						else {
							final_input = 0.0;
						}


						bool switch_handled = false;
						float switch_accel = 0.0;
						switch (i)
						{
						case 49:
						case 50:
							switch_accel = kbp->base[0].N01E30341;
							switch_handled = true;
							break;
						case 51:
						case 52:
							switch_accel = kbp->base[0].N01E30342;
							switch_handled = true;
							break;
						}

						if (switch_handled)
							final_input = (switch_accel * final_input) * kbp->base[0].N01E30344;
						//final_input = (final_input * accel_calculation) * kbp->base[0].N01E30344;


						float old_input_state = kbp->input_state[i].Pressure;
						if (final_input > old_input_state)
						{
							button_handled = true;
							kbp->input_state[i].Pressure = final_input;
						}
					} // end of mouse handling

					current_device = kbp->base[0].action_bind[i].device[dindex].index;
					if (current_device == device_index::controller && button_state_arrayptr && is_controller_plugged_in(0) && !disable_controller) // need to make this dynamic in the future.
					{
						key_code = kbp->base[0].action_bind[i].device[dindex].key;

						bool controller_input_handled = false;
						if (key_code >= 16 && key_code >= 2)
						{

							switch (key_code)
							{
							case 16:
								final_input = a5->param2; // walk forward
								break;

							case 17:
								final_input = -0.0 - a5->param2; // walk backward
								break;

							case 18:
								final_input = -0.0 - a5->param1;
								break;

							case 19:
								final_input = a5->param1;
								break;

							case 20:
								final_input = a6->param2;
								break;

							case 21:
								final_input = -0.0 - a6->param2;
								break;

							case 22:
								final_input = -0.0 - a6->param1;
								break;

							case 23:
								final_input = a6->param1;
								break;
							}

						}

						if (final_input < 0.0)
						{
							final_input = 0;
						}

						if (final_input > kbp->base[0].N01E30343)
						{
							button_held_state |= 1;
							button_held_state2[1] |= 1;
							float old_pressure = kbp->input_state[i].Pressure;
							if (final_input > old_pressure)
								kbp->input_state[i].Pressure = final_input;

						}
						else {
							WORD whold_time = button_state_arrayptr->wbutton_time_held[key_code];
							bool bwhold_time = (whold_time > kbp->base[0].action_bind[i].device[dindex].unknown);
							if (whold_time <= kbp->base[0].action_bind[i].device[dindex].unknown)
							{
								final_input = 0.0;
							}
							else {
								final_input = 1.0;
							}

							button_held_state |= bwhold_time;
							button_held_state2[1] |= bwhold_time;

							float value = 0.0;
							switch (i)
							{
							case button_mouselook_l:
							case button_mouselook_r:
								value = kbp->base[0].N01E30341;
								break;

							case button_mouselook_u:
							case button_mouselook_d:
								value = kbp->base[0].N01E30342;
								break;
							}

							final_input = (value * final_input) * kbp->base[0].N01E30344;

							float old_pressure = kbp->input_state[i].Pressure;
							if (final_input > old_pressure)
								kbp->input_state[i].Pressure = final_input;



						}


						if (key_code < 2)
						{
							BYTE trigger_pressure = button_state_arrayptr->trigger_pressure[key_code];
							bool btrigger_pressure_updated = (button_state_arrayptr->wbutton_time_held[key_code] > kbp->base[0].action_bind[i].device[dindex].unknown);
							button_held_state |= btrigger_pressure_updated;
							button_held_state2[1] |= btrigger_pressure_updated;

							final_input = trigger_pressure * 0.0039215689;
							float old_pressure = kbp->input_state[i].Pressure;
							if (final_input > old_pressure)
								kbp->input_state[i].Pressure = final_input;

						}
					}


					if (current_device == device_index::keyboard && !disable_keyboard)
					{
						bool input_handled = false;
						if (key_code < 0 || key_code >= 0x125)
						{
							float old_input = kbp->input_state[i].Pressure;
							if (final_input > old_input)
								kbp->input_state[i].Pressure = final_input;

							input_handled = true;
						}

						bool skip_input_held = false;
						bool skip_input_held2 = false;
						if (input_handled == false)
						{
							BYTE keyboard_state = get_keyboard_states(key_code) > kbp->base[0].action_bind[i].device[dindex].unknown;

							//get_controller_index(controllerIndex);
							//int pIndex = controller2player();
							int pIndex = c2p(controllerIndex);

							Player* game_state_player = (Player*)&game_state_players->datum[pIndex];

							if (!keyboard_state)
							{
								final_input = 0.0;
								button_held_state |= keyboard_state;
								button_held_state2[0] |= keyboard_state;
								skip_input_held = true;
							}

							if (game_state_player->is_chatting != 2 || i == f1_button)
							{
								if (keyboard_state)
								{
									final_input = 1.0f;
									button_held_state |= keyboard_state;
									button_held_state2[0] = keyboard_state;
								}
								else
								{
									final_input = 0.0;
									button_held_state |= keyboard_state;
									button_held_state2[0] |= keyboard_state;
								}
							}


							float nInput = 0.0f;
							switch (i)
							{
							case button_mouselook_l:
							case button_mouselook_r:
								nInput = kbp->base[0].N01E30341;
								nInput = (nInput * final_input) * kbp->base[0].N01E30344;
								final_input = nInput;
								break;
							case button_mouselook_u:
							case button_mouselook_d:
								nInput = kbp->base[0].N01E30342;
								nInput = (nInput * final_input) * kbp->base[0].N01E30344;
								final_input = nInput;
								break;
							}

						}

						float old_input = kbp->input_state[i].Pressure;
						if (final_input > old_input)
							kbp->input_state[i].Pressure = final_input;
					}

				}



				//keep at end of action index loop
				int time_passed = get_time_passed_since_last_input();
				update_all_button_states(&(kbp->input_state[i].hold_time1), &(kbp->input_state[i].hold_time2), 0, button_held_state, time_passed);
			}

		}

		if (button_state_arrayptr && !disable_controller)
			button_held_state2[1] |= button_state_arrayptr->wbutton_time_held[2] != 0 || button_state_arrayptr->wbutton_time_held[3] != 0;


		if (!button_held_state2[kbp->base[0].N01E30340] && !disable_controller)
		{
			int device_index = 0;
			bool skip_update = false;

			while (!(button_held_state2[device_index]))
			{
				if (++device_index >= 2)
				{
					skip_update = true;
					break;
				}
			}

			if (*(BYTE*)controller_input_array != device_index && skip_update == false)
			{
				*(BYTE*)(controller_input_array+(0x1680 * controllerIndex)) = device_index;
				update_controller_input_states((int)kbp, controllerIndex);
			}
		}

		//get_controller_index(controllerIndex);
		//int playerindex = controller2player();
		int playerindex = c2p(controllerIndex);

		unsigned long unitDatum = -1;

		if (playerindex != -1)
		{
			Player* player = (Player*)&game_state_players->datum[playerindex];
			unitDatum = player->BipedUnitDatum.ToInt();
		}

		int action_index_ = -1;
		int action_index = -1;
		int _action_index = -1;
		int __action_index = -1;

		bool swap_actions = false;

		if (unitDatum != -1)
		{
			if (kbp->base[0].N01E30346 && !kbp->base[0].N01E30340 && get_unk_biped_data2(unitDatum)) // seems to be an is dual wielding check.
			{
				if (get_unk_biped_data(unitDatum))
				{
					action_index = rt_button2;
				}
				else
				{
					action_index = get_unk_biped_data2(unitDatum) != 0 ? rt_button : lmb_button;
				}

				float old_pressure = kbp->input_state[action_index].Pressure;
				BYTE old_holdtime = kbp->input_state[action_index].hold_time1;

				_action_index = get_unk_biped_data(unitDatum) != 0 ? lt_button2 : lt_button;

				int action_index2 = 0;
				if (get_unk_biped_data(unitDatum))
				{
					action_index2 = rt_button2;
				}
				else
				{
					action_index2 = get_unk_biped_data2(unitDatum) != 0 ? rt_button : lmb_button;

				}

				kbp->input_state[action_index2].Pressure = kbp->input_state[_action_index].Pressure;
				kbp->input_state[action_index2].hold_time1 = kbp->input_state[_action_index].hold_time1;

				int __action_index = get_unk_biped_data(unitDatum) != 0 ? lt_button2 : lt_button;
				kbp->input_state[__action_index].Pressure = old_pressure;
				kbp->input_state[__action_index].hold_time1 = old_holdtime;
			}


			for (int action = 0; action < button_walkf; action++) // button_walkf is the size of the structs 0x2D.
			{
				ib->bhold_time[action] = kbp->input_state[action].hold_time1; // copy hold_time byte into input_buffer
				ib->whold_time[action] = kbp->input_state[action].hold_time2; // copy hold_time word into input_buffer
			}

			bool lmb_updated = false;
			if (unitDatum == -1)
			{
				action_index_ = lmb_button;
				ib->LMB_orRTrigger = kbp->input_state[action_index_].Pressure;
				lmb_updated = true;

			}

			if (lmb_updated == false)
			{

				if (get_unk_biped_data(unitDatum))
				{
					action_index_ = rt_button;
					ib->LMB_orRTrigger = kbp->input_state[action_index_].Pressure;
				}
				else {
					if (get_unk_biped_data2(unitDatum) == 0)
					{
						action_index_ = lmb_button;
					}
					else
					{
						action_index_ = rt_button;
					}

					swap_actions = get_unk_biped_data2(unitDatum) == 0;
					ib->LMB_orRTrigger = kbp->input_state[action_index_].Pressure;
				}

			}
		}

		int nAction = 0;
		if (unitDatum == -1 || (swap_actions = (get_unk_biped_data(unitDatum) == 0), nAction = lt_button2, swap_actions))
		{
			nAction = lt_button;
		}

		ib->RMB_or_LTrigger = kbp->input_state[nAction].Pressure;

		bool isMouseInverted = kbp->base[0].N01DA8A15;
		if (!isMouseInverted && kbp->base[0].N01DA8A16 && !disable_mouse)
		{
			get_controller_index(controllerIndex); // load result to eax
			isMouseInverted = get_vehicle_input_data(); // reads from eax.

		}

		bool isControllerInverted = kbp->base[0].N01DA8A14;
		if (!isControllerInverted && kbp->base[0].N01DA8A16 && !disable_controller)
		{
			get_controller_index(controllerIndex);
			isControllerInverted = get_vehicle_input_data();
		}

		if (!disable_mouse)
		{
			float mouse_look_input = kbp->input_state[button_mouselook_u].Pressure - kbp->input_state[button_mouselook_d].Pressure;
			if (isMouseInverted)
				mouse_look_input = -0.0 - mouse_look_input;

			ib->MouseLookupDown1 = mouse_look_input;
			ib->MouseLookupDown2 = kbp->input_state[button_mouselook_u].Pressure - kbp->input_state[button_mouselook_d].Pressure;

			ib->MouseLookLeftRight = kbp->input_state[button_mouselook_l].Pressure - kbp->input_state[button_mouselook_r].Pressure;
		}

		ib->ForwardVelocity = kbp->input_state[button_walkf].Pressure - kbp->input_state[button_walkb].Pressure;
		ib->DirectionalVelocity = kbp->input_state[button_walkl].Pressure - kbp->input_state[button_walkr].Pressure;

		if (!disable_controller)
		{
			ib->ControllerLookLeftRight = kbp->input_state[joystick_look_l].Pressure - kbp->input_state[joystick_look_r].Pressure;

			float controller_look_input = kbp->input_state[joystick_look_u].Pressure - kbp->input_state[joystick_look_d].Pressure;
			ib->Controller_LookUpDown2 = kbp->input_state[joystick_look_u].Pressure - kbp->input_state[joystick_look_d].Pressure;

			if (isControllerInverted)
				controller_look_input = -0.0 - controller_look_input;

			ib->Controller_LookUpDown1 = controller_look_input;
		}
	}
	return 0;
}
	

void InitializeInputFixes()
{
	datum_try_and_get = h2mod->GetAddress<tdatum_try_and_get>(0x6639B);
	isPlayervalid = h2mod->GetAddress<tisPlayerValid>(0x52CE9);
	get_keyboard_states = h2mod->GetAddress<tget_keyboard_states>(0x2F030);
	update_controller_input_states = h2mod->GetAddress<tupdate_controller_input_states>(0x5E717);
	controller_input_array = h2mod->GetAddress<BYTE*>(0x4AA008);
	get_mouse_state = h2mod->GetAddress<tget_mouse_input>(0x2E404);
	isWindowNotInFocus_or_XLiveEatingInput = h2mod->GetAddress<tisWindowNotInFocus_or_XLiveEatingInput>(0x2E391);
	system_get_time_internal = h2mod->GetAddress<tsystem_get_time_internal>(0x37E51);
	unkInputByte = h2mod->GetAddress<BYTE*>(0x412CE8);
	is_controller_plugged_in = h2mod->GetAddress<tis_controller_plugged_in>(0x2E186);
	get_mouse_button_state = h2mod->GetAddress<tget_mouse_button_state>(0x2E440);
	get_time_passed_since_last_input = h2mod->GetAddress<tget_time_passed_since_last_input>(0x2E180);
	update_all_button_states = h2mod->GetAddress<tupdate_all_button_states>(0x2E4C5);
	apply_mouse_input_to_buffer = h2mod->GetAddress<tapply_mouse_input_to_buffer>(0x5E31F);
	get_vehicle_input_data = h2mod->GetAddress<tget_vehicle_input_data>(0x61DD9);
	controller2player = h2mod->GetAddress<tcontroller2player>(0x51408);
	get_unk_biped_data = h2mod->GetAddress<tget_unk_biped_data>(0x13A365);
	get_unk_biped_data2 = h2mod->GetAddress<tget_unk_biped_data2>(0x13A334);
	PatchCall(h2mod->GetAddress(0x62F65), update_player_input_buffer);
}