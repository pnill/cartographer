#include "stdafx.h"
#include "screen_display_mode.h"
#include "screen_confirm_resolution.h"

#include "interface/user_interface_errors.h"
#include "interface/user_interface_memory.h"
#include "memory/data.h"
#include "rasterizer/rasterizer_settings.h"

/* constants */

static const char* k_dislay_mode_settings_edit_list_name = "display mode settings edit list";

const wchar_t* const k_borderless_string[k_language_count]
{
	L"Borderless",
	L"ボーダレス",
	L"Grenzenlos",
	L"Sans frontières",
	L"Sin fronteras",
	L"Senza confini",
	L"무제한",
	L"无边框",
	L"Sem fronteiras"
};

/* enums */

enum e_display_mode_settings_list_items : uint16
{
	_item_fullscreen,
	_item_windowed,
	_item_borderless,

	_item_first = _item_fullscreen,
	_item_last = _item_borderless,

	k_total_no_of_display_mode_settings_list_items
};


/* globals */

/* prototypes */

static e_rasterizer_window_mode item_type_to_display_mode(e_display_mode_settings_list_items item);
static e_display_mode_settings_list_items display_mode_to_item_type(e_rasterizer_window_mode mode);

/* public code */

c_display_mode_edit_list::c_display_mode_edit_list(uint16 user_flags):
	c_list_widget(user_flags),
	m_slot(this, &c_display_mode_edit_list::handle_item_pressed_event)
{
	m_multiplayer_menu = false;

	//we dont need s_list_item_datum here as no of list items remain same
	m_list_data = ui_list_data_new(k_dislay_mode_settings_edit_list_name, k_total_no_of_display_mode_settings_list_items, sizeof(datum));
	data_make_valid(m_list_data);

	for (int32 i = 0; i < m_list_data->maximum_count; ++i)
	{
		datum_new(m_list_data);
	}

	linker_type2.link(&m_slot);
}

void c_display_mode_edit_list::setup_children()
{
	c_list_widget::setup_item_indices();

	e_display_mode_settings_list_items item_type = _item_first;
	item_type = display_mode_to_item_type((e_rasterizer_window_mode)rasterizer_settings_get()->display_mode);
	set_focused_item_index(item_type);
}

c_list_item_widget* c_display_mode_edit_list::get_list_items()
{
	return m_list_items;
}

int32 c_display_mode_edit_list::get_list_items_count()
{
	return k_no_of_visible_items_for_display_mode;
}

void c_display_mode_edit_list::update_list_items(c_list_item_widget* item, int32 skin_index)
{
	ASSERT(item);
	c_text_widget* item_text = item->try_find_text_widget(_default_list_skin_text_main);

	if (item_text)
	{
		const e_rasterizer_window_mode mode = item_type_to_display_mode((e_display_mode_settings_list_items)DATUM_INDEX_TO_ABSOLUTE_INDEX(item->get_last_data_index()));
		if (mode == _rasterizer_window_mode_borderless)
		{
			item_text->set_text(k_borderless_string[get_current_language()]);
		}
		else
		{
			const string_id item_string = rasterizer_settings_get_display_mode_string(mode);
			item_text->set_text_from_string_id(item_string);
		}
		
	}
}


void c_display_mode_edit_list::set_using_mp_screen(bool param)
{
	m_multiplayer_menu = param;
	return;
}

void c_display_mode_edit_list::handle_item_pressed_event(
	s_event_record* const& event,
	datum* pitem_index)
{
	int16 absolute_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(*pitem_index);
	// ASSERT(absolute_index <= _rasterizer_settings_display_mode_last_user_selectable);

	absolute_index = absolute_index > _item_last ? _item_first : absolute_index;
	const e_rasterizer_window_mode display_mode = item_type_to_display_mode((e_display_mode_settings_list_items)absolute_index);

	if (absolute_index || rasterizer_settings_has_available_video_modes())
	{
		rasterizer_settings_store_current_settings_for_revert();
		rasterizer_settings_set_display_mode(&display_mode);
		int32 display_option_index = rasterizer_settings_get()->display_option_index;
		rasterizer_settings_set_display_option(&display_option_index);

		//rewrite this so this we dont have to use force_update , without which windowed to borderless transition fails
		rasterizer_settings_process_display_changes(true, false); 
		rasterizer_settings_update_window_position();

		c_screen_parameters params;

		params.initialize_default_user(FLAG(event->controller), get_parent_channel(), get_parent_render_window(), c_screen_confirm_resolution::load);
		params.execute_load_function();
	}
	else
	{
		screen_error_ok_dialog_show(_user_interface_channel_type_game_error, _ui_error_no_fullscreen_res, _window_4, (uint16)NONE, nullptr, nullptr);
	}

	return;
}


//
// c_screen_display_mode_menu class starts here
// 


c_screen_display_mode_menu::c_screen_display_mode_menu(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, uint16 user_flags):
	c_screen_with_menu(_screen_display_mode, channel_type, window_index, user_flags, &m_display_mode_edit_list),
	m_display_mode_edit_list(user_flags)
{
}

const void* c_screen_display_mode_menu::load_proc() const
{
	return &c_screen_display_mode_menu::load;
}


void* __cdecl c_screen_display_mode_menu::load(c_screen_parameters* parameters)
{
	//return INVOKE(0x2491D3, 0x0, c_screen_display_mode_menu::load, parameters);

	c_screen_display_mode_menu* screen;

	void* pool = ui_pool_allocate_space(sizeof(c_screen_display_mode_menu), 0);
	if (pool)
	{
		screen = new (pool) c_screen_display_mode_menu(
			parameters->get_channel_type(),
			parameters->get_window_index(),
			parameters->get_user_flags()
		);

		screen->m_allocated = true;
		user_interface_register_screen_to_channel(screen, parameters);
	}
	else
	{
		screen = nullptr;
	}

	return screen;
}

void* __cdecl c_screen_display_mode_menu::load_mp(c_screen_parameters* parameters)
{
	//return INVOKE(0x258C02, 0x0, c_screen_display_mode_menu::load_mp, parameters);

	c_screen_display_mode_menu* screen;

	void* pool = ui_pool_allocate_space(sizeof(c_screen_display_mode_menu), 0);
	if (pool)
	{
		screen = new (pool) c_screen_display_mode_menu(
			parameters->get_channel_type(),
			parameters->get_window_index(),
			parameters->get_user_flags()
		);

		screen->m_allocated = true;
		screen->m_display_mode_edit_list.set_using_mp_screen(true);
		user_interface_register_screen_to_channel(screen, parameters);
	}
	else
	{
		screen = nullptr;
	}

	return screen;
}



/* private code */

static e_rasterizer_window_mode item_type_to_display_mode(e_display_mode_settings_list_items item)
{
	e_rasterizer_window_mode window_type;
	switch (item)
	{
	case _item_fullscreen:
		window_type = _rasterizer_window_mode_real_fullscreen;
		break;
	case _item_windowed:
		window_type = _rasterizer_window_mode_windowed;
		break;
	case _item_borderless:
		window_type = _rasterizer_window_mode_borderless;
		break;

	default:
		window_type = _rasterizer_window_mode_funky_fullscreen;
	}
	return window_type;
}

static e_display_mode_settings_list_items display_mode_to_item_type(e_rasterizer_window_mode mode)
{
	e_display_mode_settings_list_items item_type;
	switch (mode)
	{
	case _rasterizer_window_mode_real_fullscreen:
		item_type = _item_fullscreen;
		break;
	case _rasterizer_window_mode_windowed:
		item_type = _item_windowed;
		break;
	case _rasterizer_window_mode_borderless:
		item_type = _item_borderless;
		break;
	default:
		item_type = _item_first;
	}

	return item_type;
}
