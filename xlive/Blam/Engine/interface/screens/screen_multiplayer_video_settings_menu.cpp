#include "stdafx.h"
#include "screen_multiplayer_video_settings_menu.h"
#include "screen_display_mode.h"
#include "screen_resolution.h"
#include "screen_brightness_level.h"
#include "screen_gamma_setting.h"
#include "screen_anti_aliasing.h"
#include "screen_safe_area_setting.h"
#include "screen_restore_video_defaults.h"
#include "screen_vsync_setting.h"
#include "screen_splitscreen_setting.h"

#include "interface/user_interface_memory.h"
#include "interface/user_interface_controller.h"
#include "rasterizer/rasterizer_settings.h"
#include "render/render.h"
#include "saved_games/player_profile.h"

/* enums */

enum e_mp_video_settings_list_items : uint16
{
	_item_display_mode,
	_item_resolution,
	_item_vsync, //new-addon
	_item_brightness_level,
	_item_gamma_setting,
	_item_anti_aliasing,
	_item_safe_area,
	_item_splitscreen,//new-addon
	_item_restore_defaults,

	k_total_no_of_mp_video_settings_list_items
};

/* constants */

static const char k_mp_video_setting_list_name[] = "mp video settings game list";
static const s_custom_item_text_mapping k_screen_multiplayer_video_settings_menu_items_map[] =
{
	{ STRING_ID_TO_CUSTOM_ITEM_MAPPING(_string_id_display_mode)					, _item_display_mode 		},
	{ STRING_ID_TO_CUSTOM_ITEM_MAPPING(_string_id_resolution)					, _item_resolution 			},
	{ k_vsync_header_string	, _item_vsync, true 															},
	{ STRING_ID_TO_CUSTOM_ITEM_MAPPING(_string_id_brightness_level)				, _item_brightness_level	},
	{ STRING_ID_TO_CUSTOM_ITEM_MAPPING(_string_id_gamma_setting)				, _item_gamma_setting 		},
	{ STRING_ID_TO_CUSTOM_ITEM_MAPPING(_string_id_anti_aliasing)				, _item_anti_aliasing 		},
	{ STRING_ID_TO_CUSTOM_ITEM_MAPPING(_string_id_safe_area)					, _item_safe_area 			},
	{ k_splitscreen_header_string, _item_splitscreen, true 													},
	{ STRING_ID_TO_CUSTOM_ITEM_MAPPING(_string_id_restore_video_defaults)		, _item_restore_defaults 	},

};

/* globals */

/* prototypes */

/* public code */

c_multiplayer_video_settings_list::c_multiplayer_video_settings_list(uint16 user_flags):
	c_list_widget(user_flags),
	m_slot(this, &c_multiplayer_video_settings_list::handle_item_pressed_event)
{

	m_list_data = ui_list_data_new(k_mp_video_setting_list_name, k_total_no_of_mp_video_settings_list_items, sizeof(s_list_item_datum));
	
	ASSERT(m_list_data);
	data_make_valid(m_list_data);

	// yes this sucks
#define LIST_ITEM_DATUM_GET_NEW() \
		(static_cast<s_list_item_datum*>(datum_get(m_list_data, datum_new(m_list_data))))

	LIST_ITEM_DATUM_GET_NEW()->item_id = _item_display_mode;
	LIST_ITEM_DATUM_GET_NEW()->item_id = _item_resolution;
	if (rasterizer_settings_get()->display_mode == _rasterizer_window_mode_real_fullscreen)
	{
		LIST_ITEM_DATUM_GET_NEW()->item_id = _item_vsync;
	}
	LIST_ITEM_DATUM_GET_NEW()->item_id = _item_brightness_level;
	LIST_ITEM_DATUM_GET_NEW()->item_id = _item_gamma_setting;
	LIST_ITEM_DATUM_GET_NEW()->item_id = _item_anti_aliasing;
	LIST_ITEM_DATUM_GET_NEW()->item_id = _item_safe_area;
	if (get_player_window_count() > 1)
	{
		LIST_ITEM_DATUM_GET_NEW()->item_id = _item_splitscreen;
	}
	LIST_ITEM_DATUM_GET_NEW()->item_id = _item_restore_defaults;

#undef LIST_ITEM_DATUM_GET_NEW

	linker_type2.link(&m_slot);
}

c_multiplayer_video_settings_list::~c_multiplayer_video_settings_list()
{
	rasterizer_settings_write_to_registry();
}

c_list_item_widget* c_multiplayer_video_settings_list::get_list_items()
{
	return m_list_items;
}

int32 c_multiplayer_video_settings_list::get_list_items_count()
{
	return k_no_of_visible_items_for_mp_video_settings;
}

void c_multiplayer_video_settings_list::update_list_items(c_list_item_widget* item, int32 skin_index)
{
	ASSERT(item);
	this->update_list_items_from_mapping(item, skin_index, _default_list_skin_text_main, k_screen_multiplayer_video_settings_menu_items_map, k_total_no_of_mp_video_settings_list_items);
	return;
}

void c_multiplayer_video_settings_list::handle_item_pressed_event(
	s_event_record* const& event,
	datum* pitem_index)
{
	//INVOKE_TYPE(0x258F3E, 0x0, void(__thiscall*)(c_multiplayer_video_settings_list*, s_event_record**, datum*), this, pevent, pitem_index);

	proc_ui_screen_load_cb_t load_function = NULL;
	c_screen_parameters params;

	if (*pitem_index != NONE)
	{
		s_list_item_datum* item = (s_list_item_datum*)datum_try_and_get(m_list_data, *pitem_index);
		e_mp_video_settings_list_items item_type = (e_mp_video_settings_list_items)item->item_id;

		switch (item_type)
		{
		case _item_display_mode:
			load_function = &c_screen_display_mode_menu::load_mp;
			break;
		case _item_resolution:
			load_function = &c_screen_resolution_menu::load_mp;
			break;
		case _item_vsync:
			load_function = &c_screen_vsync_menu::load;
			break;
		case _item_brightness_level:
			load_function = &c_screen_brightness_level_menu::load_mp;
			break;
		case _item_gamma_setting:
			load_function = &c_screen_gamma_menu::load_mp;
			break;
		case _item_anti_aliasing:
			load_function = &c_screen_anti_aliasing_menu::load_mp;
			break;
		case _item_safe_area:
			load_function = &c_screen_safe_area_menu::load_mp;
			break;
		case _item_splitscreen:
			load_function = &c_screen_splitscreen_menu::load;
			break;
		case _item_restore_defaults:
			load_function = &c_screen_restore_video_defaults_setting_menu::load_mp;
			break;
		default:
			unreachable();
		}
	}

	params.initialize_default_user(
		FLAG(event->controller),
		get_parent_channel(),
		get_parent_render_window(),
		load_function
	);

	if (load_function!=NULL)
	{
		if (user_interface_globals_get_edit_player_profile_index() != NONE)
			user_interface_globals_finish_saving_profile_changes();

		s_saved_game_player_profile profile;
		uint32 profile_index;

		user_interface_controller_get_profile_data(this->get_any_responding_controller(), &profile, &profile_index);
		user_interface_globals_set_edit_player_profile(this->get_any_responding_controller(), profile_index, &profile);

		params.execute_load_function();
	}

	return;
};


//
// c_screen_multiplayer_video_settings class starts here
// 

c_screen_multiplayer_video_settings::c_screen_multiplayer_video_settings(
	c_screen_parameters const* parameters) :
	c_screen_multiplayer_video_settings(parameters->get_channel_type(), parameters->get_window_index(), parameters->get_user_flags())
{
	return;
}

c_screen_multiplayer_video_settings::c_screen_multiplayer_video_settings(
	e_user_interface_channel_type channel_type,
	e_user_interface_render_window window_index,
	uint16 user_flags) :
	c_screen_with_menu(_screen_video_settings_mp, channel_type, window_index, user_flags, &m_mp_video_settings_list),
	m_mp_video_settings_list(user_flags)
{
	return;
}

const void* c_screen_multiplayer_video_settings::load_proc() const
{
	return &c_screen_multiplayer_video_settings::load;
}

void c_screen_multiplayer_video_settings::apply_instance_patches()
{
	//Replace orignal call with custom one inside c_pause_settings_list::handle_item_pressed_event
	WriteValue(Memory::GetAddress(0x24E248) + 4, c_screen_multiplayer_video_settings::load);
}
