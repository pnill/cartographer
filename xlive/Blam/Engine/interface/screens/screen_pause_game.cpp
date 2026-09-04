#include "stdafx.h"
#include "screen_pause_game.h"

#include "screen_pause_settings.h"

#include "achievements/achievement_manager.h"
#include "game/game.h"
#include "game/game_time.h"
#include "interface/user_interface.h"
#include "interface/user_interface_bitmap_block.h"
#include "interface/user_interface_controller.h"
#include "interface/user_interface_errors.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_objectives.h"
#include "interface/user_interface_widget_window.h"
#include "main/main.h"
#include "simulation/simulation_queue_global_events.h"
#include "scenario/scenario.h"
#include "scenario/scenario_definitions.h"
#include "sound/sound_manager.h"
#include "text/text_group.h"

/* constants */

static const int16 k_screen_pause_game_tab_stops[] = { 35 };

/* prototypes */

static bool screen_singleplayer_pause_game_handle_restart(void);
static bool screen_singleplayer_pause_game_handle_save_and_exit(void);
static string_id screen_pause_game_get_objective_stringid(e_campaign_objective index);

/* public code */

void screen_pause_game_apply_patches(
	void)
{
	WriteValue(Memory::GetAddress(0x20CD4F) + 1, c_screen_pause_game::load);
	WriteValue(Memory::GetAddress(0x20CD4F) + 1, c_screen_pause_game::load);
	
	return;
}

c_pause_game_list::c_pause_game_list(
	int16 user_flags) :
	c_list_widget(user_flags),
	m_slot(this, &c_pause_game_list::handle_item_pressed_event)
{
	m_list_data = ui_list_data_new("pause game list", k_pause_game_list_item_count, sizeof(datum));

	ASSERT(m_list_data!=NULL);

	data_make_valid(m_list_data);
	
	for (int32 i = 0; i<m_list_data->maximum_count; ++i)
	{
		datum_new(m_list_data);
	}

	linker_type2.link(&m_slot);

	return;
}

c_list_item_widget* c_pause_game_list::get_list_items(void)
{
	return m_list_items;
}

int32 c_pause_game_list::get_list_items_count(void)
{
	return NUMBEROF(m_list_items);
}

void c_pause_game_list::update_list_items(
	c_list_item_widget* item,
	int32 skin_index)
{
	ASSERT(item!=NULL);
	
	c_text_widget* text = item->try_find_text_widget(0);
	
	if (text)
	{
		string_id id;
		switch (DATUM_INDEX_TO_ABSOLUTE_INDEX(item->get_last_data_index()))
		{
		case _pause_game_list_item_continue:
			id = _string_id_continue;
			break;
		case _pause_game_list_item_revert:
			id = _string_id_revert_to_last_save;
			break;
		case _pause_game_list_item_reset:
			id = _string_id_restart_level;
			break;
		case _pause_game_list_item_settings:
			id = _string_id_controller_settings;
			break;
		case _pause_game_list_item_save_and_quit:
			id = user_interface_controller_get_signed_in_controller_count() <= 1 ? _string_id_save_and_quit : _string_id_quit;
			break;
		default:
			id = _string_id_invalid;
			break;
		}

		text->set_text_from_string_id(id);
	}

	return;
}

void c_pause_game_list::handle_item_pressed_event(
	s_event_record * const &event,
	datum* item_index)
{
	c_screen_pause_game* parent_screen = (c_screen_pause_game*)get_parent_screen();

	ASSERT(parent_screen!=NULL);
	ASSERT(parent_screen->get_id()==_screen_single_player_pause_game);

	switch (DATUM_INDEX_TO_ABSOLUTE_INDEX(*item_index))
	{
	case _pause_game_list_item_continue:
		parent_screen->continue_game(event);
		break;
	case _pause_game_list_item_revert:
	{
		int32 event_type = 11;
		achievement_manager_get()->submit_event(&event_type);
		parent_screen->revert_to_last_save();
		break;
	}
	case _pause_game_list_item_reset:
		parent_screen->restart_level(event);
		break;
	case _pause_game_list_item_settings:
		parent_screen->handle_settings(event);
		break;
	case _pause_game_list_item_save_and_quit:
		parent_screen->save_and_quit(event);
		simulation_queue_game_global_event_insert(_simulation_queue_game_global_event_type_save_and_quit);
		break;
	default:
		unreachable();
	}

	return;
}

c_screen_pause_game::c_screen_pause_game(
	const c_screen_parameters* parameters) :
	c_screen_pause_game(parameters->get_channel_type(), parameters->get_window_index(), parameters->get_user_flags())
{
	return;
}

c_screen_pause_game::c_screen_pause_game(
	e_user_interface_channel_type channel_type,
	e_user_interface_render_window window_index,
	int16 user_flags) :
	c_screen_with_menu(
		_screen_single_player_pause_game,
		channel_type,
		window_index,
		user_flags,
		&m_pause_game_list),
	m_pause_game_list(user_flags),
	m_objectives_string()
{
	if (
		!game_time_get_paused() &&
		// If we're in synchronous networking DO NOT pause the game
		!IN_RANGE(game_simulation_get(), _game_simulation_synchronous_client, _game_simulation_synchronous_server)
	)	
	{
		game_time_set_paused(true);
		sound_pause(_sound_pause_state_pausing);
	}

	return;
}

void* c_screen_pause_game::load(
	c_screen_parameters* parameters)
{
	void* pool;
	c_screen_pause_game* screen = (pool = ui_pool_allocate_space(sizeof(c_screen_pause_game), 0)) ? new (pool) c_screen_pause_game(parameters) : NULL;
	ASSERT(screen != NULL);

	screen->m_allocated = true;
	user_interface_register_screen_to_channel(screen, parameters);

	return screen;
}


void c_screen_pause_game::update(
	void)
{
	c_bitmap_widget* bitmap_widget = try_find_bitmap_widget(4);

	if (bitmap_widget && bitmap_widget->get_any_responding_controller())
	{
		bitmap_widget->set_visible(false);
	}

	c_user_interface_widget::update();
	update_objective_text();
	return;
}

const void* c_screen_pause_game::load_proc(
	void) const
{
	return c_screen_pause_game::load;
}

void c_screen_pause_game::initialize(
	c_screen_parameters const* parameters)
{
	c_screen_with_menu::initialize(parameters);
	return;
}

bool c_screen_pause_game::handle_event(
	s_event_record* event)
{
	bool result = true;

	if (event->type != _user_interface_event_type_gamepad_button_pressed)
	{
		result = c_screen_with_menu::handle_event(event);
	}
	else if (
		event->component != _controller_component_button_back &&
		event->component != _controller_component_button_b)
	{
		result = c_screen_with_menu::handle_event(event);
	}
	else
	{
		start_widget_animation(3);
	}

	return result;
}

void c_screen_pause_game::update_objective_text(void)
{
	c_text_widget* text_widget = try_find_text_widget(2);

	if (text_widget)
	{
		c_user_interface_text* text = text_widget->get_interface();

		if (text)
		{
			text->set_tab_stop_count(k_screen_pause_game_tab_stops, NUMBEROF(k_screen_pause_game_tab_stops));
			m_objectives_string.clear();
			
			const datum objectives_index = global_scenario_get()->objectives.index;

			if (objectives_index != NONE)
			{
				for (int32 i = 0; i < k_max_number_of_objectives; ++i)
				{
					const e_objective_state state = user_interface_objectives_get_state((e_campaign_objective)i);
					if (state!=_objective_state_hidden)
					{
						wchar_t state_string_unchecked[4];
						wchar_t state_string_checked[4];
						wchar_t* state_string;

						c_maximum_interface_text interface_string;

						// Shown and incomplete
						if (state==_objective_state_shown)
						{
							state_string_unchecked[0] = L'☐';
							state_string_unchecked[1] = L'\t';
							state_string_unchecked[2] = L'\0';
							state_string = state_string_unchecked;
						}
						// Completed
						else
						{
							state_string_checked[0] = L'☑';
							state_string_checked[1] = L'\t';
							state_string_checked[2] = L'\0';
							state_string = state_string_checked;
						}

						m_objectives_string.append(state_string);
						string_list_get_normal_string(objectives_index, screen_pause_game_get_objective_stringid((e_campaign_objective)i), &interface_string);
						m_objectives_string.append(interface_string.get_string());
						m_objectives_string.append(L"\r\n");
					}
				}
			}

			text_widget->set_text(m_objectives_string.get_string());
		}
	}

	return;
}


void c_screen_pause_game::revert_to_last_save(
	void)
{
	main_revert_map();
	simulation_queue_game_global_event_insert(_simulation_queue_game_global_event_type_revert_map);
	start_widget_animation(3);
	
	return;
}

void c_screen_pause_game::continue_game(
	s_event_record* const& event)
{
	start_widget_animation(3);
	return;
}

void c_screen_pause_game::restart_level(
	s_event_record* const& event)
{
	user_interface_error_ok_cancel_dialog_show_confirmation(
		_user_interface_channel_type_gameshell_dialog,
		_last_render_window,
		FLAG(event->controller),
		screen_singleplayer_pause_game_handle_restart,
		_ui_error_confirm_restart_level);
	start_widget_animation(3);

	return;
}

void c_screen_pause_game::handle_settings(
	s_event_record* const& event)
{
	c_screen_parameters params;

	start_widget_animation(3);

	params.initialize_default_user(FLAG(event->controller), get_channel(), get_render_window(), c_screen_pause_settings::load);
	params.execute_load_function();
	return;
}

void c_screen_pause_game::save_and_quit(
	s_event_record* const& event)
{
	// Show save and exit error in singleplayer and no save and exit in coop since we don't save in coop games
	const e_ui_error_types error_type = 
		user_interface_controller_get_signed_in_controller_count() <= 1 ?
		_ui_error_confirm_save_and_exit_campaign :
		_ui_error_confirm_exit_campaign;
	
	user_interface_error_ok_cancel_dialog_show_confirmation(
		_user_interface_channel_type_gameshell_dialog,
		_last_render_window,
		FLAG(event->controller),
		screen_singleplayer_pause_game_handle_save_and_exit,
		error_type);
	start_widget_animation(3);

	return;
}

/* private code */

static bool screen_singleplayer_pause_game_handle_restart(
	void)
{
	achievement_manager_get()->start_level_chosen(true);
	main_reset_map();
	simulation_queue_game_global_event_insert(_simulation_queue_game_global_event_type_reset_map);

	return true;
}

static bool screen_singleplayer_pause_game_handle_save_and_exit(
	void)
{
	main_save_and_exit();
	simulation_queue_game_global_event_insert(_simulation_queue_game_global_event_type_save_and_quit);

	return true;
}

static string_id screen_pause_game_get_objective_stringid(
	e_campaign_objective index)
{
	string_id result= _string_id_invalid;

	ASSERT(IN_RANGE(index, NONE, k_max_number_of_objectives-1));

	switch (index)
	{
	case _objective_01:
		result = _string_id_1;
		break;
	case _objective_02:
		result = _string_id_2;
		break;
	case _objective_03:
		result = _string_id_3;
		break;
	case _objective_04:
		result = _string_id_4;
		break;
	case _objective_05:
		result = _string_id_5;
		break;
	default:
		unreachable();
	}

	return result;
}
