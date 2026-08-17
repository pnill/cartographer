#include "stdafx.h"
#include "screen_pause_settings.h"

#include "screen_multiplayer_audio_settings_menu.h"
#include "screen_multiplayer_controller_settings_menu.h"
#include "screen_multiplayer_keyboard_settings_menu.h"
#include "screen_multiplayer_video_settings_menu.h"

#include "interface/user_interface_controller.h"
#include "saved_games/player_profile.h"

/* macros */

#define pause_setting_list_item_get(index)	((struct s_pause_setting_list_item*)datum_get(m_list_data, (index)))

/* structures */

struct s_pause_setting_list_item
{
	int16 field_0;
	int16 index;
};

/* public code */

c_pause_settings_list::c_pause_settings_list(
	int16 user_flags) :
	c_list_widget(user_flags),
	m_list_items(),
	m_slot(this, &c_pause_settings_list::handle_item_pressed_event)
{
	m_list_data = ui_list_data_new("pause settings list", k_pause_settings_list_item_count, sizeof(s_pause_setting_list_item));

	ASSERT(m_list_data!=NULL);

	data_make_valid(m_list_data);

	for (int16 i = 0; i < k_pause_settings_list_item_count; ++i)
	{
		pause_setting_list_item_get(datum_new(m_list_data))->index = i;
	}

	linker_type2.link(&m_slot);

	return;
}


c_list_item_widget* c_pause_settings_list::get_list_items(void)
{
	return m_list_items;
}

int32 c_pause_settings_list::get_list_items_count(void)
{
	return k_pause_settings_list_item_total_count;
}

void c_pause_settings_list::update_list_items(
	c_list_item_widget* item,
	int32 skin_index)
{
	static const s_item_text_mapping mapping[k_pause_settings_list_item_count] =
	{
		{ _pause_settings_list_item_controller, _string_id_controller_settings },
		{ _pause_settings_list_item_keyboard, _string_id_keyboard_settings_menu },
		{ _pause_settings_list_item_video, _string_id_settings_video },
		{ _pause_settings_list_item_audio, _string_id_settings_audio }
	};

	update_list_items_from_mapping(item, skin_index, _default_list_skin_text_main, mapping, NUMBEROF(mapping));

	return;
}

void c_pause_settings_list::handle_item_pressed_event(
	s_event_record* const& event,
	datum* item_index)
{
	if (*item_index != NONE)
	{
		s_pause_setting_list_item* item = pause_setting_list_item_get(*item_index);
		c_screen_parameters parameters;

		parameters.initialize_default_user(FLAG(event->controller), get_parent_channel(), get_parent_render_window(), NULL);
		
		switch (item->index)
		{
		case _pause_settings_list_item_controller:
			parameters.set_load_function(c_screen_multiplayer_controller_settings::load);
			break;
		case _pause_settings_list_item_keyboard:
			parameters.set_load_function(c_screen_multiplayer_keyboard_settings::load);
			break;
		case _pause_settings_list_item_video:
			parameters.set_load_function(c_screen_multiplayer_video_settings::load);
			break;
		case _pause_settings_list_item_audio:
			parameters.set_load_function(c_screen_multiplayer_audio_settings::load);
			break;
		default:
			break;
		}
		
		if (parameters.get_laod_function())
		{
			uint32 profile_index;
			s_saved_game_player_profile profile;

			if (user_interface_globals_get_edit_player_profile_index() != NONE)
			{
				user_interface_globals_finish_saving_profile_changes();
			}

			user_interface_controller_get_profile_data(get_any_responding_controller(), &profile, &profile_index);
			user_interface_globals_set_edit_player_profile(get_any_responding_controller(), profile_index, &profile);
			
			parameters.execute_load_function();
		}
	}

	return;
}

c_screen_pause_settings::c_screen_pause_settings(
	c_screen_parameters const* parameters) :
	c_screen_pause_settings(parameters->get_channel_type(), parameters->get_window_index(), parameters->get_user_flags())
{
	return;
}

c_screen_pause_settings::c_screen_pause_settings(
	e_user_interface_channel_type channel_type,
	e_user_interface_render_window window_index,
	int16 user_flags) :
	c_screen_with_menu(
		_screen_pause_settings,
		channel_type,
		window_index,
		user_flags,
		&m_pause_settings_list),
	m_pause_settings_list(user_flags)
{
	return;
}

bool c_screen_pause_settings::handle_event(
	s_event_record* event)
{
	if (event->type == _user_interface_event_type_gamepad_button_pressed &&
		(event->component == _controller_component_button_b || event->component == _controller_component_button_back) &&
		user_interface_globals_get_edit_player_profile_index() != NONE)
	{
		user_interface_globals_finish_saving_profile_changes();
	}

	return c_screen_widget::handle_event(event);
}

