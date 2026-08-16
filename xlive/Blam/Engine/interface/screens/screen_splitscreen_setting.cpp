#include "stdafx.h"
#include "screen_splitscreen_setting.h"

#include "interface/user_interface_memory.h"
#include "interface/user_interface_shared_globals.h"
#include "memory/data.h"
#include "render/render.h"
#include "text/text_group.h"

#include "H2MOD/Modules/Shell/Config.h"

/* enums */

enum e_splitscreen_list_items : uint16
{
	_item_automatic,
	_item_vertical,
	_item_horizontal,

	k_total_no_of_splitscreen_list_items
};

/* constants */

// TODO : currently using c_screen_safe_area_menu tags
// we should start using custom ui tags in near future
#define k_splitscreen_menu_screen_id _screen_safe_area

static const char k_splitscreen_edit_list_name[] = "splitscreen edit list";

static const wchar_t* const k_splitscreen_description_string[k_language_count] =
{
	L"Change the Display Layout when playing Splitscreen with buddies",					// English
	L"フレンドと画面分割プレイ中に表示レイアウトを変更",										// Japanese
	L"Ändere das Anzeige-Layout beim Spielen im Splitscreen mit Freunden",				// German
	L"Changer la disposition de l’affichage en jouant en écran partagé avec des amis",	// French
	L"Cambia el diseño de pantalla al jugar en pantalla dividida con amigos",			// Spanish
	L"Cambia il layout dello schermo quando giochi in splitscreen con gli amici",		// Italian
	L"친구와 화면 분할로 플레이할 때 디스플레이 레이아웃 변경",									// Korean
	L"与好友进行分屏游戏时更改显示布局",														// Chinese
	L"Altere o layout da tela ao jogar em tela dividida com os amigos"					// Portuguese
};

const wchar_t* const k_splitscreen_options_string[k_no_of_visible_items_for_splitscren][k_language_count]
{
	{
		L"Automatic",			// English
		L"自動",					// Japanese
		L"Automatisch",			// German
		L"Automatique",			// French
		L"Automático",			// Spanish
		L"Automatico",			// Italian
		L"자동",					// Korean
		L"自动",					// Chinese
		L"Automático"			// Portuguese
	},

	{
		L"Vertical",			// English
		L"垂直",					// Japanese
		L"Vertikal",			// German
		L"Vertical",			// French
		L"Vertical",			// Spanish
		L"Verticale",			// Italian
		L"수직",					// Korean
		L"垂直",					// Chinese
		L"Vertical"				// Portuguese
	},

	{
		L"Horizontal",          // English
		L"水平",					// Japanese
		L"Horizontal",          // German
		L"Horizontal",          // French
		L"Horizontal",          // Spanish
		L"Orizzontale",			// Italian
		L"수평",					// Korean
		L"水平",					// Chinese
		L"Horizontal"			// Portuguese
	}
};


const wchar_t* const k_splitscreen_header_string[k_language_count]
{
	L"Splitscreen Mode",			// English
	L"画面分割モード",					// Japanese
	L"Splitscreen-Modus",			// German
	L"Mode écran partagé",			// French
	L"Modo de pantalla dividida",	// Spanish
	L"Modalità a schermo diviso",	// Italian
	L"분할 화면 모드",				// Korean
	L"分屏模式",						// Chinese
	L"Modo de tela dividida"		// Portuguese
};

/* globals */

static c_maximum_interface_text default_text;

/* prototypes */

static const e_splitscreen_list_items get_item_type_from_split_mode(e_display_split_type mode);
static const e_display_split_type get_split_mode_from_item_type(e_splitscreen_list_items type);

/* public code */

c_splitscreen_edit_list::c_splitscreen_edit_list(uint16 user_flags) :
	c_list_widget(user_flags),
	m_slot(this, &c_splitscreen_edit_list::handle_item_pressed_event)
{
	//we dont need s_list_item_datum here as no of list items remain same
	m_list_data = ui_list_data_new(k_splitscreen_edit_list_name, k_total_no_of_splitscreen_list_items, sizeof(datum));
	data_make_valid(m_list_data);

	for (int32 i = 0; i < m_list_data->maximum_count; ++i)
	{
		datum_new(m_list_data);
	}

	linker_type2.link(&m_slot);

	const s_user_interface_shared_globals* ui_shared_globals = user_interface_shared_globals_get();

	ASSERT(ui_shared_globals->unicode_string_list_tag.index != NONE);
	
	string_list_get_normal_string(ui_shared_globals->unicode_string_list_tag.index, _string_id_default_player_profile_name, &default_text);
	return;
}

c_list_item_widget* c_splitscreen_edit_list::get_list_items()
{
	return m_list_items;
}

int32 c_splitscreen_edit_list::get_list_items_count()
{
	return k_total_no_of_splitscreen_list_items;
}

void c_splitscreen_edit_list::update_list_items(c_list_item_widget* item, int32 skin_index)
{
	ASSERT(item);

	c_text_widget* item_text = item->try_find_text_widget(_default_list_skin_text_main);
	e_splitscreen_list_items item_type = (e_splitscreen_list_items)(DATUM_INDEX_TO_ABSOLUTE_INDEX(item->get_last_data_index()));
	if (item_text)
	{
		c_maximum_interface_text value_string;
		value_string.set(k_splitscreen_options_string[item_type][get_current_language()]);
		if (item_type == _item_automatic)
		{
			value_string.append_print(L"(%s)", default_text.get_string());
		}
		item_text->set_text(value_string.get_string());
	}
}

void c_splitscreen_edit_list::handle_item_pressed_event(s_event_record* const& event, datum* pitem_index)
{
	e_splitscreen_list_items choice_type = (e_splitscreen_list_items)DATUM_INDEX_TO_ABSOLUTE_INDEX(*pitem_index);
	H2Config_split_mode = get_split_mode_from_item_type(choice_type);

	user_interface_back_out_from_channel(this->get_parent_channel(), this->get_parent_render_window());
}


//
// c_screen_vsync_menu class starts here
// 


c_screen_splitscreen_menu::c_screen_splitscreen_menu(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, uint16 user_flags) :
	c_screen_with_menu(k_splitscreen_menu_screen_id, channel_type, window_index, user_flags, &m_splitscreen_edit_list),
	m_splitscreen_edit_list(user_flags)
{
}

void c_screen_splitscreen_menu::initialize(c_screen_parameters* parameters)
{
	c_screen_with_menu::initialize(parameters);

	// we need this piece of code to update header and description texts
	// TODO : use custom tags so we dont have to override texts on the fly

	c_text_widget* header = get_screen_header_text();
	c_text_widget* subheader = get_screen_subheader_text();

	ASSERT(header && subheader);

	const e_language language = get_current_language();

	if (header)
	{
		header->set_text(k_splitscreen_header_string[language]);
	}

	if (subheader)
	{
		subheader->set_text(k_splitscreen_description_string[language]);
	}

	return;
}

void c_screen_splitscreen_menu::post_initialize()
{
	c_screen_with_menu::post_initialize();
	m_splitscreen_edit_list.set_focused_item_index(get_item_type_from_split_mode(H2Config_split_mode));
}

const void* c_screen_splitscreen_menu::load_proc() const
{
	return &c_screen_splitscreen_menu::load;
}

void* c_screen_splitscreen_menu::load(c_screen_parameters* parameters)
{
	c_screen_splitscreen_menu* screen;

	void* pool = ui_pool_allocate_space(sizeof(c_screen_splitscreen_menu), 0);
	if (pool)
	{
		screen = new (pool) c_screen_splitscreen_menu(
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

const wchar_t* const c_screen_splitscreen_menu::get_header_string()
{
	return k_splitscreen_header_string[get_current_language()];
}

const wchar_t* const c_screen_splitscreen_menu::get_option_string()
{
	return k_splitscreen_options_string[get_item_type_from_split_mode(H2Config_split_mode)][get_current_language()];
}


/* private code */

static const e_splitscreen_list_items get_item_type_from_split_mode(e_display_split_type mode)
{
	e_splitscreen_list_items ret = _item_automatic;
	switch (mode)
	{
	case _display_split_type_none:
		ret = _item_automatic;
		break;
	case _display_split_type_vertical:
		ret = _item_vertical;
		break;
	case _display_split_type_horizontal:
		ret = _item_horizontal;
		break;
	}
	return ret;
}

static const e_display_split_type get_split_mode_from_item_type(e_splitscreen_list_items item)
{
	e_display_split_type ret = _display_split_type_none;
	switch (item)
	{
	case _item_automatic:
		ret = _display_split_type_none;
		break;
	case _item_vertical:
		ret = _display_split_type_vertical;
		break;
	case _item_horizontal:
		ret = _display_split_type_horizontal;
		break;
	}
	return ret;
}