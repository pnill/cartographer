#include "stdafx.h"
#include "screen_xbox_live_task_progress_dialog.h"

#include "cache/cache_files.h"
#include "interface/user_interface.h"
#include "interface/user_interface_shared_globals.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_screen_widget_definition.h"
#include "networking/online/online_task_xbox.h"

c_screen_xbox_live_task_progress_dialog::c_screen_xbox_live_task_progress_dialog(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags):
	c_screen_widget(_screen_xbox_live_task_progress_dialog, channel_type, window_index, user_flags)
{
	m_update_function = nullptr;
	m_close_function = nullptr;
	m_online_task_datum = NONE;
	m_online_task_result = NULL;
}

c_screen_xbox_live_task_progress_dialog::~c_screen_xbox_live_task_progress_dialog()
{
	//todo : dispose _online_task_change_logon_users here when online_tasks are finished
	close_task_internal();
	c_screen_widget::~c_screen_widget();
}

void c_screen_xbox_live_task_progress_dialog::update()
{
	if (m_online_task_datum != NONE)
	{
		string_id header = online_task_get_header_text(m_online_task_datum);
		set_header_text(header);
		string_id display = online_task_get_display_text(m_online_task_datum);
		set_display_text(display);
	}

	if (m_update_function)
	{
		m_update_function(this);
	}
	else if ((m_current_animation.m_flags & 2) == 0) //transitioning_out ?
	{
		close_task_internal();
		start_widget_animation(3);
	}
	c_user_interface_widget::update();
}

bool c_screen_xbox_live_task_progress_dialog::handle_event(s_event_record* event)
{
	if (event->type == _user_interface_event_type_gamepad_button_pressed)
	{
		if (event->component == _controller_component_button_b
			|| event->component == _controller_component_button_back)
		{
			if (m_close_function)
			{
				m_close_function(this);
				close_task();
				return true;
			}
		}
	}
	return false;
}

void c_screen_xbox_live_task_progress_dialog::initialize(c_screen_parameters* parameters)
{
	s_interface_expected_screen_layout layout;
	csmemset(&layout, 0, sizeof(layout));
	layout.panes_count = 1;

	datum widget_tag_datum = user_interface_get_screen_tag_index_by_id(this->m_screen_id);
	if (widget_tag_datum != NONE)
	{
		this->verify_and_load_from_layout(widget_tag_datum, &layout);
	}
	this->setup_children();
}

const void* c_screen_xbox_live_task_progress_dialog::load_proc(void) const
{
	return &c_screen_xbox_live_task_progress_dialog::load;
}


void c_screen_xbox_live_task_progress_dialog::set_task_datum(datum task_datum)
{
	m_online_task_datum = task_datum;
}

void c_screen_xbox_live_task_progress_dialog::set_update_function(proc_task_cb_t function)
{
	m_update_function = function;
}

void c_screen_xbox_live_task_progress_dialog::set_close_function(proc_task_cb_t function)
{
	m_close_function = function;
}

void c_screen_xbox_live_task_progress_dialog::close_task_internal()
{
	//INVOKE_TYPE(0x2393AB, 0x0, void(__thiscall*)(c_screen_xbox_live_task_progress_dialog*), this);

	if (m_online_task_datum != NONE)
	{
		s_online_task_datum* task = online_try_and_get_task(m_online_task_datum);
		if (task)
			m_online_task_result = online_task_continue(task);
		else
			m_online_task_result = k_online_task_status_completed;

		online_task_close(m_online_task_datum);
		m_online_task_datum = NONE;
	}
}

uint8* c_screen_xbox_live_task_progress_dialog::get_data()
{
	return m_data;
}

void c_screen_xbox_live_task_progress_dialog::set_header_text(string_id header)
{
	c_text_widget* text = get_screen_header_text();
	if (text)
		text->set_text_from_string_id(header);
}

void c_screen_xbox_live_task_progress_dialog::set_display_text(string_id display)
{
	c_text_widget* text = try_find_text_widget(k_sub_header_text_block_index);
	if (text)
		text->set_text_from_string_id(display);
}

void c_screen_xbox_live_task_progress_dialog::set_display_text_raw(const wchar_t* raw_text)
{
	c_text_widget* text = try_find_text_widget(k_sub_header_text_block_index);
	if (text)
		text->set_text(raw_text);
}

void c_screen_xbox_live_task_progress_dialog::close_task()
{
	m_update_function = nullptr;
	close_task_internal();
}

void c_screen_xbox_live_task_progress_dialog::add_task(proc_task_cb_t update_function)
{
	ASSERT(update_function != nullptr);
	add_task_ex(NONE, _controller0, update_function, nullptr, nullptr);
}

void c_screen_xbox_live_task_progress_dialog::add_task_ex(datum task_datum, e_controller_index controller_index, proc_task_cb_t update_function, proc_task_cb_t close_function, uint8* data)
{
	//typedef void(__cdecl* c_screen_xbox_live_task_progress_dialog_t)(int arg_0, signed int local_player_index, void* update_function, int a4, int a5);
	//INVOKE_TYPE(0x20C776, 0x0, c_screen_xbox_live_task_progress_dialog_t, -1, 0, update_function, 0, 0);
	
	c_screen_xbox_live_task_progress_dialog* screen;
	c_screen_parameters params;

	params.initialize_default_user(
		(uint16)(controller_index < k_number_of_controllers ? FLAG(controller_index) : NONE),
		_user_interface_channel_type_game_error,
		_window_4,
		c_screen_xbox_live_task_progress_dialog::load
	);

	screen = (c_screen_xbox_live_task_progress_dialog*)params.execute_load_function();
	
	if(screen)
	{
		screen->set_task_datum(task_datum);
		screen->set_update_function(update_function);
		screen->set_close_function(close_function);
		screen->m_data = data;
	}

	return;
}

void* c_screen_xbox_live_task_progress_dialog::load(c_screen_parameters* parameters)
{
	c_screen_xbox_live_task_progress_dialog* screen = nullptr;
	uint8* pool = ui_pool_allocate_space(sizeof(c_screen_xbox_live_task_progress_dialog), 0);
	if (pool) {
		screen = new (pool) c_screen_xbox_live_task_progress_dialog(
			parameters->get_channel_type(),
			parameters->get_window_index(),
			parameters->get_user_flags()
		);
		screen->m_allocated = true;
	}
	user_interface_register_screen_to_channel(screen, parameters);
	return pool;
}


void c_screen_xbox_live_task_progress_dialog::apply_patches_on_map_load()
{
	// Get tag definition
	datum task_progress_dialog_datum_index = tag_loaded(_tag_group_user_interface_screen_widget_definition, "ui\\screens\\game_shell\\xbox_live\\task_progress_dialog\\task_progress_dialog");
	if (task_progress_dialog_datum_index == NONE) { return;	}
	s_user_interface_screen_widget_definition* task_progress_dialog_definition = user_interface_screen_widget_definition_get(task_progress_dialog_datum_index);
	
	// Sanity checks
	if (task_progress_dialog_definition == nullptr) { return; }
	if (task_progress_dialog_definition->panes.count == 0) { return; }

	s_window_pane_reference* pane_definition = user_interface_widget_pane_get(&task_progress_dialog_definition->panes, 0);
	
	// Fix the text placement
	if (pane_definition->text_blocks.count > 0)
	{
		s_text_block_reference* reference = TAG_BLOCK_GET_ELEMENT(&pane_definition->text_blocks, 0, s_text_block_reference);

		reference->text_bounds = { 144, -244, -38, 244 };
	}

	// Fix the placement of the 4 UI bitmaps
	if (pane_definition->bitmap_blocks.count == 4)
	{
		point2d fixed_bitmaps_placements[4] = { {-288, 218}, {148, -66}, {-278, 208}, {-234, 306} };

		for (int32 i = 0; i < 4; ++i)
		{
			s_bitmap_block_reference* reference = TAG_BLOCK_GET_ELEMENT(&pane_definition->bitmap_blocks, i, s_bitmap_block_reference);

			// fix the ui bitmap elements position
			reference->topleft = fixed_bitmaps_placements[i];
		}
	}

	// Fix the model viewport bounds
	if (pane_definition->model_scene_blocks.count > 0)
	{
		s_ui_model_scene_reference* scene = TAG_BLOCK_GET_ELEMENT(&pane_definition->model_scene_blocks, 0, s_ui_model_scene_reference);

		scene->ui_viewport = { 78, -110, -174, 110 };
	}
	return;
}

