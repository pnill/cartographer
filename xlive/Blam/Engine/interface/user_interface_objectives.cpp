#include "stdafx.h"
#include "user_interface_objectives.h"

#include "saved_games/game_state.h"

/* structures */

struct s_current_objective_state
{
	e_objective_state primary_objective_states[k_max_number_of_objectives];
};

/* globals */

static s_current_objective_state* g_objectives;

/* public code */

void user_interface_objectives_apply_patches(
	void)
{
	PatchCall(Memory::GetAddress(0x2277FA), user_interface_objectives_initialize);
	PatchCall(Memory::GetAddress(0x227871), user_interface_objectives_initialize_for_new_map);
	PatchCall(Memory::GetAddress(0xF1C7A), user_interface_objectives_clear); 
	PatchCall(Memory::GetAddress(0xF1CAE), user_interface_objectives_show_up_to);
	PatchCall(Memory::GetAddress(0xF1CDF), user_interface_objectives_finish_up_to);
	return;
}

void user_interface_objectives_initialize(
	void)
{
	g_objectives = (s_current_objective_state *)game_state_malloc("pause screen objectives", NULL, sizeof(*g_objectives));
	ASSERT(g_objectives);

	return;
}

void user_interface_objectives_clear(
	void)
{
	csmemset(g_objectives, 0, sizeof(*g_objectives));
	return;
}

void user_interface_objectives_initialize_for_new_map(
	void)
{
	user_interface_objectives_clear();
	return;
}

void user_interface_objectives_show_up_to(
	e_campaign_objective index)
{
	if (VALID_INDEX(index, k_max_number_of_objectives))
	{
		for (int32 i = 0; i <= index; ++i)
		{
			if (!g_objectives->primary_objective_states[i])
			{
				g_objectives->primary_objective_states[i] = _objective_state_shown;
			}
		}
	}
	else
	{
		error(_error_immediate, __FUNCTION__"(): index out of range.");
	}

	return;
}

void user_interface_objectives_finish_up_to(
	e_campaign_objective index)
{
	if (VALID_INDEX(index, k_max_number_of_objectives))
	{
		for (int32 i = 0; i <= index; ++i)
		{
			if (!g_objectives->primary_objective_states[i])
			{
				g_objectives->primary_objective_states[i] = _objective_state_completed;
			}
		}
	}
	else
	{
		error(_error_immediate, __FUNCTION__"(): index out of range.");
	}

	return;
}

e_objective_state user_interface_objectives_get_state(
	e_campaign_objective index)
{
	e_objective_state result = _objective_state_hidden;
	if (VALID_INDEX(index, k_max_number_of_objectives))
	{
		result = g_objectives->primary_objective_states[index];
	}
	else
	{
		error(_error_immediate, __FUNCTION__"(): index out of range.");
	}

	return result;
}
