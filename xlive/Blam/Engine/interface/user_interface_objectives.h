#pragma once

/* enums */

enum e_campaign_objective
{
	_objective_01= 0,
	_objective_02,
	_objective_03,
	_objective_04,
	_objective_05,
	k_max_number_of_objectives
};

enum e_objective_state
{
	_objective_state_first= 0,
	_objective_state_hidden = _objective_state_first,
	_objective_state_shown,
	_objective_state_completed,
	k_objective_state_count,
};

/* prototypes */

void user_interface_objectives_apply_patches(void);
void user_interface_objectives_initialize(void);
void user_interface_objectives_clear(void);
void user_interface_objectives_initialize_for_new_map(void);
void user_interface_objectives_show_up_to(e_campaign_objective index);
void user_interface_objectives_finish_up_to(e_campaign_objective index);
e_objective_state user_interface_objectives_get_state(e_campaign_objective index);
