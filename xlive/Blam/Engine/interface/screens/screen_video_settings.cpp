#include "stdafx.h"
#include "screen_video_settings.h"

#include "screen_display_mode.h"
#include "screen_resolution.h"
#include "screen_brightness_level.h"
#include "screen_gamma_setting.h"
#include "screen_anti_aliasing.h"
#include "screen_lod_setting.h"
#include "screen_safe_area_setting.h"
#include "screen_restore_video_defaults.h"
#include "screen_vsync_setting.h"
#include "screen_splitscreen_setting.h"

#include "cache/cache_files.h"
#include "interface/user_interface_bitmap_block.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_screen_widget_definition.h"
#include "interface/user_interface_shared_globals.h"
#include "interface/user_interface_utilities.h"
#include "main/game_preferences.h"
#include "rasterizer/rasterizer_settings.h"

#include "H2MOD/Modules/Shell/Config.h"

/* macro defines */

/* constants */

static const char* k_video_setting_list_name = "video settings list";
static const real_rgb_color k_default_text_change_color = { 1.f,1.f,1.f };
static const real_rgb_color k_list_item_disabled_text_change_color = { 0.7f,0.7f,0.7f};

/* enums */

enum e_video_settings_list_items : uint16
{
	_item_display_mode,
	_item_resolution,
	_item_vsync,//new-addon
	_item_brightness_level,
	_item_gamma_setting,
	_item_anti_aliasing,
	_item_lod_setting,
	_item_safe_area,
	_item_splitscreen,//new-addon
	_item_restore_defaults,

	k_total_no_of_video_settings_list_items
};

enum e_settings_list_skin_bitmap_hilite : uint16
{
	_bitmap_hilite_sprite_light = 0,
	_bitmap_hilite_sprite_dark,
	_bitmap_hilite_sprite_unused,
};

/* globals */

static bool g_can_use_vsync = true;

/* prototypes */

/* private code */

/* public code */

c_video_settings_list::c_video_settings_list(uint16 user_flags):
	c_list_widget(user_flags),
	m_slot(this, &c_video_settings_list::handle_item_pressed_event)
{
	//we dont need s_list_item_datum here as no of list items remain same
	m_list_data = ui_list_data_new(k_video_setting_list_name, k_total_no_of_video_settings_list_items, sizeof(datum));
	data_make_valid(m_list_data);

	for (int32 i = 0; i < m_list_data->maximum_count; ++i)
	{
		datum_new(m_list_data);
	}

	linker_type2.link(&m_slot);
}

c_video_settings_list::~c_video_settings_list()
{
	rasterizer_settings_write_to_registry();
}

c_list_item_widget* c_video_settings_list::get_list_items()
{
	return m_list_items;
}

int32 c_video_settings_list::get_list_items_count()
{
	return k_no_of_visible_items_for_video_settings;
}

void c_video_settings_list::update_list_items(c_list_item_widget* item, int32 skin_index)
{
	ASSERT(item);

	c_text_widget* primary_text = item->try_find_text_widget(_settings_list_skin_text_header);
	c_text_widget* secondary_text = item->try_find_text_widget(_settings_list_skin_text_value);
	c_bitmap_widget* hilite_bitmap = item->try_find_bitmap_widget(_settings_list_skin_bitmap_hilite);

	string_id primary_string = _string_id_empty_string;
	string_id secondary_string = _string_id_empty_string;

	s_rasterizer_settings* rasterizer_settings = rasterizer_settings_get();
	e_rasterizer_window_mode display_mode = (e_rasterizer_window_mode)rasterizer_settings->display_mode;
	const e_language language = get_current_language();

	if (primary_text)
	{
		switch (DATUM_INDEX_TO_ABSOLUTE_INDEX(item->get_last_data_index()))
		{
		case _item_display_mode:
			primary_string = _string_id_display_mode;
			if (display_mode == _rasterizer_window_mode_borderless)
			{
				secondary_string = _string_id_invalid;
				secondary_text->set_text(k_borderless_string[language]);
			}
			else
			{
				secondary_string = rasterizer_settings_get_display_mode_string(display_mode);
			}
			break;
		case _item_resolution:
			primary_string = _string_id_resolution;
			secondary_string = _string_id_invalid;

			wchar_t resolution_text[32];
			rasterizer_settings_get_display_option_resolution_string(rasterizer_settings->display_option_index, resolution_text, NUMBEROF(resolution_text));
			secondary_text->set_text(resolution_text);
			break;
		case _item_vsync:
			primary_string = _string_id_invalid;
			primary_text->set_text(k_vsync_header_string[language]);
			secondary_string = H2Config_use_vsync ? _string_id_on : _string_id_off;

			primary_text->set_change_color(g_can_use_vsync ? &k_default_text_change_color : &k_list_item_disabled_text_change_color);
			secondary_text->set_change_color(g_can_use_vsync ? &k_default_text_change_color : &k_list_item_disabled_text_change_color);			
			if (hilite_bitmap)
			{
				hilite_bitmap->verify_and_change_sprite(g_can_use_vsync ? _bitmap_hilite_sprite_light : _bitmap_hilite_sprite_dark);
			}
			item->set_item_transitioning(g_can_use_vsync);
			break;
		case _item_brightness_level:
			primary_string = _string_id_brightness_level;
			secondary_string = rasterizer_settings_get_brightness_level_string(rasterizer_settings->brightness);
			break;
		case _item_gamma_setting:
			primary_string = _string_id_gamma_setting;
			secondary_string = rasterizer_settings_get_gamma_setting_string(rasterizer_settings->gamma);
			break;
		case _item_anti_aliasing:
			primary_string = _string_id_anti_aliasing;
			secondary_string = rasterizer_settings_get_anti_aliasing_string(rasterizer_settings->anti_aliasing);
			break;
		case _item_lod_setting:
			primary_string = _string_id_level_of_detail;
			secondary_string = rasterizer_settings_get_lod_setting_string(rasterizer_settings->level_of_detail);
			break;
		case _item_safe_area:
			primary_string = _string_id_safe_area;
			secondary_string = rasterizer_settings_get_safe_area_string(rasterizer_settings->safe_area);
			break;
		case _item_splitscreen:
			primary_string = _string_id_invalid;
			secondary_string = _string_id_invalid;
			primary_text->set_text(c_screen_splitscreen_menu::get_header_string());
			secondary_text->set_text(c_screen_splitscreen_menu::get_option_string());
			break;
		case _item_restore_defaults:
			primary_string = _string_id_restore_video_defaults;
			secondary_string = _string_id_empty_string;
			break;
		default:
			vassert(false, "primary_string is undefined and will be used", NULL);
		}

		if (primary_string != _string_id_invalid)
		{
			primary_text->set_text_from_string_id(primary_string);
		}
	}
	else
	{
		vassert(secondary_text, "secondary_string is undefined and will be used", NULL);
	}

	if (secondary_text)
	{
		if (secondary_string != _string_id_invalid)
		{
			secondary_text->set_text_from_string_id(secondary_string);
		}
	}

	return;
}

void c_video_settings_list::handle_item_pressed_event(s_event_record* const& event, datum* pitem_index)
{
	//INVOKE_TYPE(0x24961B, 0x0, void(__thiscall*)(c_video_settings_list*, s_event_record**, datum*), this, pevent, pitem_index);

	if (DATUM_INDEX_TO_ABSOLUTE_INDEX(*pitem_index) == _item_vsync && !g_can_use_vsync)
	{
		//disable action when Vsync list item is pressed
		user_interface_utilities_play_sound(_user_interface_global_sound_error);
		return;
	}

	proc_ui_screen_load_cb_t load_proc = NULL;

	c_screen_parameters params;

	switch (DATUM_INDEX_TO_ABSOLUTE_INDEX(*pitem_index))
	{
	case _item_display_mode:
		load_proc = &c_screen_display_mode_menu::load;
		break;
	case _item_resolution:
		load_proc = &c_screen_resolution_menu::load;
		break;
	case _item_vsync:
		load_proc = &c_screen_vsync_menu::load;
		break;
	case _item_brightness_level:
		load_proc = &c_screen_brightness_level_menu::load;
		break;
	case _item_gamma_setting:
		load_proc = &c_screen_gamma_menu::load;
		break;
	case _item_anti_aliasing:
		load_proc = &c_screen_anti_aliasing_menu::load;
		break;
	case _item_lod_setting:
		load_proc = &c_screen_lod_menu::load;
		break;
	case _item_safe_area:
		load_proc = &c_screen_safe_area_menu::load;
		break;
	case _item_splitscreen:
		load_proc = &c_screen_splitscreen_menu::load;
		break;
	case _item_restore_defaults:
		load_proc = &c_screen_restore_video_defaults_setting_menu::load;
		break;
	default:
		unreachable();
	}

	params.initialize_default_user(FLAG(event->controller), _user_interface_channel_type_gameshell_dialog, _window_4, load_proc);

	if (load_proc!=NULL)
	{
		params.execute_load_function();
	}

	return;
}


//
// c_screen_video_settings class starts here
// 


c_screen_video_settings::c_screen_video_settings(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, uint16 user_flags) :
	c_screen_with_menu(_screen_video_settings, channel_type, window_index, user_flags, &m_video_settings_list),
	m_video_settings_list(user_flags)
{
}

void c_screen_video_settings::update()
{
	c_screen_with_menu::update();
	g_can_use_vsync = rasterizer_settings_get()->display_mode == _rasterizer_window_mode_real_fullscreen;
}

const void* c_screen_video_settings::load_proc() const
{
	return &c_screen_video_settings::load;
}

void* c_screen_video_settings::load(c_screen_parameters* parameters)
{
	//return INVOKE(0x21EDC7, 0x0, c_screen_video_settings::load, parameters);

	c_screen_video_settings* screen;

	void* pool = ui_pool_allocate_space(sizeof(c_screen_video_settings), 0);
	if (pool)
	{
		screen = new (pool) c_screen_video_settings(
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

void c_screen_video_settings::apply_patches_on_map_load()
{
	const char* main_widget_tag_path = "ui\\screens\\game_shell\\settings_screen\\video_settings\\video_settings";
	datum main_widget_datum_index = tag_loaded(_tag_group_user_interface_screen_widget_definition, main_widget_tag_path);

	if (main_widget_datum_index == NONE)
	{
		error(_error_log, "bad datum found");
		return;
	}
	s_user_interface_screen_widget_definition* main_widget_tag = (s_user_interface_screen_widget_definition*)tag_get_fast(main_widget_datum_index);
	
	//orignal value was 9 , but now that we have 10 elements to show this needs to be increased
	main_widget_tag->panes[0]->list_block[0]->num_visible_items = k_no_of_visible_items_for_video_settings;
}
