#include "stdafx.h"
#include "screen_button_settings.h"

#include "cache/cache_files.h"
#include "input/input_abstraction.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_screen_widget_definition.h"
#include "interface/user_interface_shared_globals.h"
#include "main/game_preferences.h"
#include "saved_games/player_profile.h"

#include "tag_files/tag_loader/tag_injection.h"
#include "text/text_group.h"

/* constants */

static const char k_button_setting_list_name[] = "button settings edit list";

/* enums */

enum e_button_list_items : uint16
{
	_item_standard,
	_item_south_paw,
	_item_boxer,
	_item_green_thumb,
	
	//custom addons
	_item_jumpy,

	k_total_no_of_button_list_items
};

enum e_button_screen_panes
{
	_button_screen_pane_standard,
	_button_screen_pane_south_paw,
	_button_screen_pane_boxer,
	_button_screen_pane_green_thumb,
	//_button_screen_pane_unused4,//copy_of_standard
	_button_screen_pane_jumpy,
	_button_screen_pane_unused5,//copy_of_south_paw
	_button_screen_pane_unused6,//copy_of_boxer
	_button_screen_pane_unused7,//copy_of_green_thumb	

	k_total_no_of_button_settings_pane,
};

enum e_jumpy_pane_text_blocks
{
	_jumpy_pane_text_help,
	_jumpy_pane_text_button_throw_grenade,
	_jumpy_pane_text_button_crouch,
	_jumpy_pane_text_button_score,
	_jumpy_pane_text_button_pause,
	_jumpy_pane_text_button_fire,
	_jumpy_pane_text_button_flashlight,//_jumpy_pane_text_button_reload
	_jumpy_pane_text_button_switch_weapons,
	_jumpy_pane_text_button_reload,//_jumpy_pane_text_button_melee
	_jumpy_pane_text_button_swap_grenades,//_jumpy_pane_text_button_jump
	_jumpy_pane_text_button_jump,//_jumpy_pane_text_button_flashlight
	_jumpy_pane_text_button_melee,//_jumpy_pane_text_button_swap_grenades
	_jumpy_pane_text_button_zoom,

	//addtions
	_jumpy_pane_text_button_swap_left,
	k_jumpy_pane_texts_count,

	k_orignal_jumpy_pane_texts_count =13,
	k_number_of_jumpy_pane_text_addons = k_jumpy_pane_texts_count- k_orignal_jumpy_pane_texts_count

};

enum e_jumpy_ingame_pane_text_blocks
{
	_jumpy_ig_pane_text_help,
	_jumpy_ig_pane_text_button_throw_grenade,
	_jumpy_ig_pane_text_button_crouch,
	_jumpy_ig_pane_text_button_score,
	_jumpy_ig_pane_text_button_pause,
	_jumpy_ig_pane_text_button_fire,
	_jumpy_ig_pane_text_button_flashlight,//_jumpy_pane_text_button_reload
	_jumpy_ig_pane_text_button_switch_weapons,
	_jumpy_ig_pane_text_button_reload,//_jumpy_pane_text_button_melee
	_jumpy_ig_pane_text_button_swap_grenades,//_jumpy_pane_text_button_jump
	_jumpy_ig_pane_text_button_jump,//_jumpy_pane_text_button_flashlight
	_jumpy_ig_pane_text_button_melee,//_jumpy_pane_text_button_swap_grenades
	_jumpy_ig_pane_text_button_zoom,

	//addtions
	_jumpy_ig_pane_text_header,
	_jumpy_ig_pane_text_button_swap_left,
	k_jumpy_ig_pane_texts_count,

	k_orignal_jumpy_ig_pane_texts_count = 13,
	k_number_of_jumpy_ingame_pane_text_addons = k_jumpy_ig_pane_texts_count - k_orignal_jumpy_ig_pane_texts_count

};

enum e_button_settings_bitmap_blocks
{
	_button_settings_bitmap_ul_03,
	_button_settings_bitmap_br_05,
	_button_settings_bitmap_button_config,

	k_number_of_button_settings_bitmap_blocks
};

enum e_button_settings_ingame_bitmap_blocks //qtr_screen
{
	_button_settings_ingame_bitmap_ul_07,
	_button_settings_ingame_bitmap_br_07,
	_button_settings_ingame_bitmap_dialog1,
	_button_settings_ingame_bitmap_dialog2,
	_button_settings_ingame_bitmap_settings_framing,
	_button_settings_ingame_bitmap_settings_framing2,
	_button_settings_ingame_bitmap_arrows_up,
	_button_settings_ingame_bitmap_arrows_down,
	_button_settings_ingame_bitmap_settings_framing3,
	_button_settings_ingame_bitmap_button_config,

	k_number_of_button_settings_ingame_bitmap_blocks
};

enum e_jumpy_ingame_pane_bitmap_blocks //qtr_screen
{
	_jumpy_ig_pane_bitmap_ul_07,
	_jumpy_ig_pane_bitmap_br_07,
	_jumpy_ig_pane_bitmap_dialog1,
	_jumpy_ig_pane_bitmap_dialog2,
	_jumpy_ig_pane_bitmap_settings_framing,
	_jumpy_ig_pane_bitmap_settings_framing2,
	_jumpy_ig_pane_bitmap_arrows_up,
	_jumpy_ig_pane_bitmap_arrows_down,

	//addtions
	_jumpy_ig_pane_bitmap_settings_framing3,
	_jumpy_ig_pane_bitmap_button_config,
	k_jumpy_ig_pane_bitmaps_count,

	k_orignal_jumpy_ig_pane_bitmaps_count = 8,
	k_number_of_jumpy_ingame_pane_bitmap_addons = k_jumpy_ig_pane_bitmaps_count - k_orignal_jumpy_ig_pane_bitmaps_count

};

enum e_button_settings_multilingual_unicode_string_list
{
	_string_id_l_header,
	_string_id_l_default,
	_string_id_l_south_paw,
	_string_id_l_jumpy,
	_string_id_l_boxer,
	_string_id_l_green_thumb,
	_string_id_l_button_fire,
	_string_id_l_button_throw_grenade,
	_string_id_l_button_reload,
	_string_id_l_button_switch_weapons,
	_string_id_l_button_melee,
	_string_id_l_button_jump,
	_string_id_l_button_swap_grenades,
	_string_id_l_button_flashlight,
	_string_id_l_button_zoom,
	_string_id_l_button_crouch,
	_string_id_l_button_score,
	_string_id_l_button_pause,
	_string_id_l_button_lean_left,
	_string_id_l_button_lean_right,
	_string_id_l_standard_help,
	_string_id_l_southpaw_help,
	_string_id_l_jumpy_help,
	_string_id_l_boxer_help,
	_string_id_l_greenthumb_help,
	_string_id_l_default_ingame,
	_string_id_l_south_paw_ingame,
	_string_id_l_jumpy_ingame,
	_string_id_l_boxer_ingame,
	_string_id_l_green_thumb_ingame,
	_string_id_l_button_boxer_grenade,
	_string_id_l_button_boxer_melee,
	k_number_of_button_settings_multilingual_unicode_string_list
};

/* constants */

static char const k_main_widget_tag_path[] = "ui\\screens\\game_shell\\settings_screen\\player_profile\\button_settings";

static const wchar_t *const k_jumpy_text_button_swap_left_string[k_language_count]
{
	L"Swap Left Weapon",
	L"左武器の交換",
	L"Linke Waffe tauschen",
	L"Échanger l'arme gauche",
	L"Cambiar arma izquierda",
	L"Scambia arma sinistra",
	L"왼쪽 무기 교체",
	L"交换左武器",
	L"Trocar Arma Esquerda"
};

/* globals */

// generate string_ids on runtime instead of hardcoding them
static string_id g_strings_array[k_number_of_button_settings_multilingual_unicode_string_list] = {};

/* prototypes */

c_button_settings_edit_list::c_button_settings_edit_list(int16 user_flags) :
	c_list_widget(user_flags),
	m_slot(this, &c_button_settings_edit_list::handle_item_pressed_event),
	m_qtr_screen(false)
{
	//we dont need s_list_item_datum here as no of list items remain same
	m_list_data = ui_list_data_new(k_button_setting_list_name, k_total_no_of_button_list_items, sizeof(datum));
	data_make_valid(m_list_data);

	for (int32 i = 0; i < m_list_data->maximum_count; ++i)
	{
		datum_new(m_list_data);
	}

	linker_type2.link(&m_slot);
}

void c_button_settings_edit_list::set_using_qtr_screen(bool param)
{
	this->m_qtr_screen = param;
}

c_list_item_widget* c_button_settings_edit_list::get_list_items()
{
	//return INVOKE_TYPE(0x25D425, 0x0, c_list_item_widget*(__thiscall*)(c_button_settings_edit_list*), this);
	return this->m_list_items;
}

int32 c_button_settings_edit_list::get_list_items_count()
{
	return k_no_of_visible_items_for_button_settings;
}

void c_button_settings_edit_list::update_list_items(c_list_item_widget* item, int32 skin_index)
{
	//INVOKE_TYPE(0x25D021, 0x0, void(__thiscall*)(c_button_settings_edit_list*, c_list_item_widget*, int32), this, item, skin_index);

	ASSERT(item);
	c_text_widget* item_text = item->try_find_text_widget(_default_list_skin_text_main);

	if (item_text)
	{
		switch (DATUM_INDEX_TO_ABSOLUTE_INDEX(item->get_last_data_index()))
		{
		case _item_standard:
			item_text->set_text_from_string_id(g_strings_array[_string_id_l_default]);
			break;
		case _item_south_paw:
			item_text->set_text_from_string_id(g_strings_array[_string_id_l_south_paw]);
			break;
		case _item_boxer:
			item_text->set_text_from_string_id(g_strings_array[_string_id_l_boxer]);
			break;
		case _item_green_thumb:
			item_text->set_text_from_string_id(g_strings_array[_string_id_l_green_thumb]);
			break;
		case _item_jumpy:
			item_text->set_text_from_string_id(g_strings_array[_string_id_l_jumpy]);
			break;
		default:
			item_text->set_text_from_string_id(_string_id_invalid);
			break;
		}
	}

}


void c_button_settings_edit_list::handle_item_pressed_event(s_event_record* const& event, datum* pitem_index)
{
	//INVOKE_TYPE(0x25D0AC, 0x0, void(__thiscall*)(c_button_settings_edit_list*, s_event_record**, datum*), this, pevent, pitem_index);

	s_gamepad_input_preferences preferences;
	e_button_preset_types selected_preset;

	switch (DATUM_INDEX_TO_ABSOLUTE_INDEX(*pitem_index))
	{
	case _item_south_paw:
		selected_preset = _button_preset_south_paw;
		break;
	case _item_boxer:
		selected_preset = _button_preset_boxer;
		break;
	case _item_green_thumb:
		selected_preset = _button_preset_green_thumb;
		break;
	case _item_jumpy:
		selected_preset = _button_preset_jumpy;
		break;
	default:
		selected_preset = _button_preset_default;
	}

	s_saved_game_player_profile* player_profile = user_interface_globals_get_edit_player_profile();

	player_profile->input_preferences.controller_button_layout = selected_preset;
	input_abstraction_set_preferences_from_controller_settings(&preferences, &player_profile->input_preferences);
	input_abstraction_get_default_preferences(&preferences, player_profile->input_preferences.controller_thumbstick_layout, selected_preset, player_profile->input_preferences.keyboard_preset_type, 0);
	input_abstraction_set_controller_settings_from_preferences(&preferences, &player_profile->input_preferences);

	// apparently ingame setting changes are directly commited to disk unlike settings screen which uses (edit_player_profile)
	if (this->m_qtr_screen)
		user_interface_globals_save_profile_changes_to_disk();

	user_interface_back_out_from_channel(this->get_parent_channel(), this->get_parent_render_window());
}


//
// c_screen_button_settings_menu class starts here
// 


c_screen_button_settings_menu::c_screen_button_settings_menu(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, int16 user_flags, e_user_interface_screen_id screen_id) :
	c_screen_with_menu(screen_id, channel_type, window_index, user_flags, &m_button_settings_list),
	m_button_settings_list(user_flags),
	field_D3C(NONE)
{
}

void c_screen_button_settings_menu::update()
{
	//INVOKE_TYPE(0x25d180, 0, void(__thiscall*)(c_screen_button_settings_menu*), this);

	if (this->get_any_responding_controller() != k_no_controller)
	{
		//const int32 unk_setting = return_0x0();
		const int32 unk_setting = 0;
		if (unk_setting != this->field_D3C)
		{
			// wgit pane offset pointer ?
			this->field_9FB = unk_setting == 2 ? 5 : 0;

			datum old_data_idx = this->m_button_settings_list.get_old_data_index();
			c_screen_widget::switch_panes(&old_data_idx);
			this->field_D3C = unk_setting;
		}
		if (this->m_using_qtr_arrows)
		{
			c_bitmap_widget* bitmap_down_arrow = this->try_find_bitmap_widget(_button_settings_ingame_bitmap_arrows_down);
			c_bitmap_widget* bitmap_up_arrow = this->try_find_bitmap_widget(_button_settings_ingame_bitmap_arrows_up);
			this->set_list_arrows_widget(bitmap_up_arrow, bitmap_down_arrow);

		}
	}
	c_user_interface_widget::update();
}

bool c_screen_button_settings_menu::handle_event(s_event_record* event)
{
	return INVOKE_TYPE(0x25d2d0, 0, bool(__thiscall*)(c_screen_button_settings_menu*, s_event_record*), this, event);
}

void c_screen_button_settings_menu::post_initialize()
{
	//INVOKE_TYPE(0x25d229, 0, void(__thiscall*)(c_screen_button_settings_menu*), this);

	//This menu class is designed to handle both button_settings.wgit and button_settings_ingame.wgit together
	//m_using_qtr_arrows == true only when button_settings_ingame.wgit is being used

	if (this->m_using_qtr_arrows)
	{
		c_bitmap_widget* bitmap_down_arrow = this->try_find_bitmap_widget(_button_settings_ingame_bitmap_arrows_down);
		c_bitmap_widget* bitmap_up_arrow = this->try_find_bitmap_widget(_button_settings_ingame_bitmap_arrows_up);
		this->set_list_arrows_widget(bitmap_up_arrow, bitmap_down_arrow);

	}

	//player_profile != nullptr when button_settings.wgit is being used inside player profile settings menu
	s_saved_game_player_profile* player_profile = user_interface_globals_get_edit_player_profile();
	if (player_profile)
	{
		switch (player_profile->input_preferences.controller_button_layout)
		{
		case _button_preset_south_paw:
			this->m_button_settings_list.set_focused_item_index(_item_south_paw);
			break;
		case _button_preset_boxer:
			this->m_button_settings_list.set_focused_item_index(_item_boxer);
			break;
		case _button_preset_green_thumb:
			this->m_button_settings_list.set_focused_item_index(_item_green_thumb);
			break;
		case _button_preset_jumpy:
			this->m_button_settings_list.set_focused_item_index(_item_jumpy);
			break;
		default:
			this->m_button_settings_list.set_focused_item_index(_item_standard);

		}
	}
	c_screen_widget::post_initialize();
}

void c_screen_button_settings_menu::post_initialize_button_keys()
{
	// this function is executed once upon every pane creation
	// thus can be used as on_pane_switch hook

	if (this->m_pane_index == _button_screen_pane_jumpy
		|| DATUM_INDEX_TO_ABSOLUTE_INDEX(this->m_button_settings_list.get_old_data_index()) == _item_jumpy)
	{
		c_text_widget* swap_left_text = this->try_find_screen_text(
			m_using_qtr_arrows ? _jumpy_ig_pane_text_button_swap_left : _jumpy_pane_text_button_swap_left);

		if (swap_left_text)
		{
			swap_left_text->set_text(k_jumpy_text_button_swap_left_string[get_current_language()]);
		}

	}

	// call orignal function now
	c_screen_widget::post_initialize_button_keys();
}

const void* c_screen_button_settings_menu::load_proc() const
{
	//return INVOKE_TYPE(0x256037, 0, void*(__thiscall*)(const c_screen_button_settings_menu*), this);
	return &c_screen_button_settings_menu::load;
}


void* c_screen_button_settings_menu::load(c_screen_parameters* parameters)
{
	//return INVOKE(0x255FA4, 0x0, c_screen_button_settings_menu::load, parameters);
	c_screen_button_settings_menu* screen;

	void* pool = ui_pool_allocate_space(sizeof(c_screen_button_settings_menu), 0);
	if (pool)
	{
		screen = new (pool) c_screen_button_settings_menu(
			parameters->get_channel_type(),
			parameters->get_window_index(),
			parameters->get_user_flags(),
			_screen_pp_button_settings
		);

		screen->m_allocated = true;
		screen->m_using_qtr_arrows = false;
		user_interface_register_screen_to_channel(screen, parameters);
	}
	else
	{
		screen = NULL;
	}

	return screen;
}

void* c_screen_button_settings_menu::load_qtr(c_screen_parameters* parameters)
{
	//return INVOKE(0x259DC0, 0x0, c_screen_button_settings_menu::load_qtr, parameters);
	c_screen_button_settings_menu* screen;

	void* pool = ui_pool_allocate_space(sizeof(c_screen_button_settings_menu), 0);
	if (pool)
	{
		screen = new (pool) c_screen_button_settings_menu(
			parameters->get_channel_type(),
			parameters->get_window_index(),
			parameters->get_user_flags(),
			_screen_pp_buttons_qtr			//qtr screen is button_settings_ingame.wgit
		); 

		screen->m_allocated = true;
		screen->m_button_settings_list.set_using_qtr_screen(true);
		screen->m_using_qtr_arrows = true;
		user_interface_register_screen_to_channel(screen, parameters);
	}
	else
	{
		screen = NULL;
	}

	return screen;
}

void c_screen_button_settings_menu::apply_patches_on_ui_map_load()
{
	datum main_widget_datum_index = tag_loaded(_tag_group_user_interface_screen_widget_definition, k_main_widget_tag_path);

	if (main_widget_datum_index == NONE)
	{
		error(_error_log, "bad datum found");
		return;
	}

	s_user_interface_screen_widget_definition const* main_widget_tag = user_interface_screen_widget_definition_get(main_widget_datum_index);
	s_window_pane_reference const* standard_pane = user_interface_widget_pane_get(&main_widget_tag->panes, _button_screen_pane_standard);
	s_window_pane_reference const* south_paw_pane = user_interface_widget_pane_get(&main_widget_tag->panes, _button_screen_pane_south_paw);
	s_window_pane_reference* jumpy_pane = user_interface_widget_pane_get(&main_widget_tag->panes, _button_screen_pane_jumpy);

	s_list_reference* standard_list = user_interface_widget_pane_get_list(&standard_pane->list_block, 0);
	s_list_reference* south_paw_list = user_interface_widget_pane_get_list(&south_paw_pane->list_block, 0);

	standard_list->num_visible_items = k_no_of_visible_items_for_button_settings;
	south_paw_list->num_visible_items = k_no_of_visible_items_for_button_settings;		//_button_screen_pane_south_paw to _button_screen_pane_unused7 all use same s_list_reference tag_block

	//fixup bitmap blocks
	if (standard_pane->bitmap_blocks.count)
	{
		csmemcpy(tag_block_get_address(&jumpy_pane->bitmap_blocks), tag_block_get_address(&standard_pane->bitmap_blocks), sizeof(s_bitmap_block_reference) * standard_pane->bitmap_blocks.count);
	}

	if (standard_pane->text_blocks.count)
	{
		//addon text blocks
		tag_injection_extend_block(&jumpy_pane->text_blocks, sizeof(s_text_block_reference), k_number_of_jumpy_pane_text_addons);
		csmemcpy(
			tag_block_get_address(&jumpy_pane->text_blocks),
			tag_block_get_address(&standard_pane->text_blocks),
			sizeof(s_bitmap_block_reference) * standard_pane->bitmap_blocks.count
		);
	}


	//start adjusting texts for jumpy
	string_list_get_string_id_list(main_widget_tag->string_list_tag.index, g_strings_array, NUMBEROF(g_strings_array));
	
	s_text_block_reference* flashlight_text_block = user_interface_widget_pane_get_text(&jumpy_pane->text_blocks, _jumpy_pane_text_button_flashlight);
	s_text_block_reference* swap_grenade_text_block = user_interface_widget_pane_get_text(&jumpy_pane->text_blocks, _jumpy_pane_text_button_swap_grenades);
	s_text_block_reference* swap_left_text_block = user_interface_widget_pane_get_text(&jumpy_pane->text_blocks, _jumpy_pane_text_button_swap_left);

	user_interface_widget_pane_get_text(&jumpy_pane->text_blocks, _jumpy_pane_text_help)->string = g_strings_array[_string_id_l_jumpy_help];
	user_interface_widget_pane_get_text(&jumpy_pane->text_blocks, _jumpy_pane_text_button_reload)->string = g_strings_array[_string_id_l_button_reload];
	user_interface_widget_pane_get_text(&jumpy_pane->text_blocks, _jumpy_pane_text_button_melee)->string = g_strings_array[_string_id_l_button_melee];
	user_interface_widget_pane_get_text(&jumpy_pane->text_blocks, _jumpy_pane_text_button_jump)->string = g_strings_array[_string_id_l_button_jump];

	flashlight_text_block->string = g_strings_array[_string_id_l_button_flashlight];
	swap_grenade_text_block->string = g_strings_array[_string_id_l_button_swap_grenades];

	// Reposition
	
	flashlight_text_block->text_bounds = { -60 ,-600,-100,-266 };
	swap_grenade_text_block->text_bounds = { -280,-550,-420,-216 };

	// Additional texts to be added
	
	*swap_left_text_block = *swap_grenade_text_block;
	swap_left_text_block->string = _string_id_invalid;
	swap_left_text_block->text_bounds = { -175,296,-215,640 };
	
	return;
}

void c_screen_button_settings_menu::apply_patches_on_mp_map_load(void)
{
	const char* ingame_widget_tag_path = "ui\\screens\\game_shell\\settings_screen\\player_profile\\button_settings_ingame";
	datum main_widget_datum_index = tag_loaded(_tag_group_user_interface_screen_widget_definition, ingame_widget_tag_path);

	if (main_widget_datum_index == NONE)
	{
		error(_error_log, "bad datum found");
		return;
	}

	s_user_interface_screen_widget_definition const* main_widget_tag = user_interface_screen_widget_definition_get(main_widget_datum_index);
	s_window_pane_reference const* standard_pane = user_interface_widget_pane_get(&main_widget_tag->panes, _button_screen_pane_standard);
	s_window_pane_reference* jumpy_pane = user_interface_widget_pane_get(&main_widget_tag->panes, _button_screen_pane_jumpy);

	if (standard_pane->bitmap_blocks.count)
	{
		//add & fixup bitmap blocks
		tag_injection_extend_block(&jumpy_pane->bitmap_blocks, sizeof(s_bitmap_block_reference), k_number_of_jumpy_ingame_pane_bitmap_addons);
		csmemcpy(tag_block_get_address(&jumpy_pane->bitmap_blocks), tag_block_get_address(&standard_pane->bitmap_blocks), sizeof(s_bitmap_block_reference) * standard_pane->bitmap_blocks.count);
	}

	if (standard_pane->text_blocks.count)
	{
		//addon text blocks
		tag_injection_extend_block(&jumpy_pane->text_blocks, sizeof(s_text_block_reference), k_number_of_jumpy_ingame_pane_text_addons);
		csmemcpy(tag_block_get_address(&jumpy_pane->text_blocks), tag_block_get_address(&standard_pane->text_blocks), sizeof(s_text_block_reference) * standard_pane->text_blocks.count);
	}

	// start adjusting texts for jumpy

	string_list_get_string_id_list(main_widget_tag->string_list_tag.index, g_strings_array, NUMBEROF(g_strings_array));
	
	TAG_BLOCK_GET_ELEMENT(&jumpy_pane->text_blocks, _jumpy_ig_pane_text_help, s_text_block_reference)->string = g_strings_array[_string_id_l_jumpy_help];
	TAG_BLOCK_GET_ELEMENT(&jumpy_pane->text_blocks, _jumpy_ig_pane_text_button_reload, s_text_block_reference)->string = g_strings_array[_string_id_l_button_reload];
	TAG_BLOCK_GET_ELEMENT(&jumpy_pane->text_blocks, _jumpy_ig_pane_text_button_melee, s_text_block_reference)->string = g_strings_array[_string_id_l_button_melee];
	TAG_BLOCK_GET_ELEMENT(&jumpy_pane->text_blocks, _jumpy_ig_pane_text_button_jump, s_text_block_reference)->string = g_strings_array[_string_id_l_button_jump];
	TAG_BLOCK_GET_ELEMENT(&jumpy_pane->text_blocks, _jumpy_ig_pane_text_button_flashlight, s_text_block_reference)->string = g_strings_array[_string_id_l_button_flashlight];
	TAG_BLOCK_GET_ELEMENT(&jumpy_pane->text_blocks, _jumpy_ig_pane_text_button_swap_grenades, s_text_block_reference)->string = g_strings_array[_string_id_l_button_swap_grenades];
	TAG_BLOCK_GET_ELEMENT(&jumpy_pane->text_blocks, _jumpy_ig_pane_text_header, s_text_block_reference)->string = g_strings_array[_string_id_l_jumpy_ingame];

	// reposition
	
	TAG_BLOCK_GET_ELEMENT(&jumpy_pane->text_blocks, _jumpy_ig_pane_text_button_flashlight, s_text_block_reference)->text_bounds = { 40 ,-540, 0, -170 };
	TAG_BLOCK_GET_ELEMENT(&jumpy_pane->text_blocks, _jumpy_ig_pane_text_button_swap_grenades, s_text_block_reference)->text_bounds = { -180, -520, -320, -216 };

	// additional texts to be added

	s_text_block_reference* swap_left_reference = TAG_BLOCK_GET_ELEMENT(&jumpy_pane->text_blocks, _jumpy_ig_pane_text_button_swap_left, s_text_block_reference);
	s_text_block_reference* swap_grenade_reference = TAG_BLOCK_GET_ELEMENT(&jumpy_pane->text_blocks, _jumpy_ig_pane_text_button_swap_grenades, s_text_block_reference);

	csmemcpy(swap_left_reference, swap_grenade_reference, sizeof(s_text_block_reference));
	swap_left_reference->string = _string_id_invalid;
	swap_left_reference->text_bounds = { -75, 326, -115, 640 };

	return;
}

void c_screen_button_settings_menu::apply_instance_patches(void)
{
	//Replace orignal call with custom one inside c_controller_settings_edit_list::handle_item_pressed_event
	WriteValue(Memory::GetAddress(0x256214) + 4, c_screen_button_settings_menu::load);
	//Replace orignal call with custom one inside c_multiplayer_controller_settings_list::handle_item_pressed_event
	WriteValue(Memory::GetAddress(0x259F85) + 4, c_screen_button_settings_menu::load_qtr);
}
