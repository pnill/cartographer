#include "stdafx.h"
#include "user_interface_utilities.h"
#include "user_interface_controller.h"


datum __cdecl user_interface_get_widget_tag_index_from_screen_id(e_user_interface_screen_id screen_id)
{
	return INVOKE(0x20C701, 0x0, user_interface_get_widget_tag_index_from_screen_id, screen_id);
}

void user_interface_transition_to_offline()
{
	for (e_controller_index controller = first_controller();
		controller != k_no_controller;
		controller = next_controller(controller))
	{
		if (user_interface_controller_is_player_profile_valid(controller))
		{
			user_interface_controller_xbox_live_account_set_signed_in(controller, false);
			user_interface_controller_update_player_name(controller);
			user_interface_controller_update_network_properties(controller);
		}
	}
}
