#include "stdafx.h"
#include "screen_vsync_setting.h"

#include "interface/user_interface_memory.h"
#include "memory/data.h"
#include "rasterizer/rasterizer_globals.h"
#include "rasterizer/rasterizer_settings.h"
#include "rasterizer/dx9/rasterizer_dx9_main.h"

#include "H2MOD/Modules/Shell/Config.h"

/* enums */

enum e_vsync_list_items : uint16
{
	_item_off,
	_item_on,

	k_total_no_of_vsync_list_items
};

/* constants */

// TODO : currently using c_screen_safe_area_menu tags
// we should start using custom ui tags in near future
#define k_vsync_menu_screen_id _screen_safe_area

static const char k_vsync_edit_list_name[] = "vsync edit list";

static const wchar_t* const k_vsync_description_string[k_language_count] =
{
	L"Synchronize your FPS with your monitor's refresh rate. Prevents screen-tearing but adds input lag",
	L"モニターのリフレッシュレートとフレームレートを同期します。ティアリングは防げますが、入力遅延が発生します",
	L"Synchronisieren Sie Ihre FPS mit der Bildwiederholfrequenz Ihres Monitors. Verhindert Screen Tearing, erhöht aber die Eingangsverzögerung",
	L"Synchronisez vos images par seconde avec la fréquence de rafraîchissement de votre écran. Cela évite les déchirures d'écran, mais augmente le temps de latence",
	L"Sincroniza tus FPS con la tasa de refresco de tu monitor. Previene el desgarro de pantalla pero añade input lag",
	L"Sincronizza i tuoi FPS con la frequenza di aggiornamento del monitor. Previene lo screen tearing ma aumenta il ritardo di input",
	L"FPS를 모니터 주사율과 동기화하세요. 화면 찢김 현상은 방지하지만 입력 지연이 발생합니다",
	L"将帧率与显示器刷新率同步。避免画面撕裂，但会增加输入延迟",
	L"Sincronize os seus fps com a taxa de atualização do seu monitor. Evita ecrã rasgado, mas adiciona atraso de entrada"
};

const wchar_t* const k_vsync_header_string[k_language_count]
{
	L"VSync",
	L"垂直同期",
	L"VSync",
	L"Synchronisation V",
	L"Sincronización vertical",
	L"Sincronizzazione verticale",
	L"수직동기화",
	L"垂直同步",
	L"Sincronização V"
};

/* globals */

/* prototypes */

/* public code */

c_vsync_edit_list::c_vsync_edit_list(uint16 user_flags) :
	c_list_widget(user_flags),
	m_slot(this, &c_vsync_edit_list::handle_item_pressed_event)
{
	//we dont need s_list_item_datum here as no of list items remain same
	m_list_data = ui_list_data_new(k_vsync_edit_list_name, k_total_no_of_vsync_list_items, sizeof(datum));
	data_make_valid(m_list_data);

	for (int32 i = 0; i < m_list_data->maximum_count; ++i)
	{
		datum_new(m_list_data);
	}

	linker_type2.link(&m_slot);
}

c_list_item_widget* c_vsync_edit_list::get_list_items()
{
	return m_list_items;
}

int32 c_vsync_edit_list::get_list_items_count()
{
	return k_no_of_visible_items_for_vsync;
}

void c_vsync_edit_list::update_list_items(c_list_item_widget* item, int32 skin_index)
{
	ASSERT(item);

	c_text_widget* item_text = item->try_find_text_widget(_default_list_skin_text_main);
	if(item_text)
	{
		item_text->set_text_from_string_id(rasterizer_settings_get_safe_area_string(DATUM_INDEX_TO_ABSOLUTE_INDEX(item->get_last_data_index())));
	}
}

void c_vsync_edit_list::handle_item_pressed_event(s_event_record* const& event, datum* pitem_index)
{
	const bool choice = DATUM_INDEX_TO_ABSOLUTE_INDEX(*pitem_index) == _item_on ? true : false;
	H2Config_use_vsync = choice;

	if (rasterizer_dx9_main_globals_get()->global_d3d_device)
	{
		rasterizer_globals_get()->reset_screen = true;
		rasterizer_dx9_reset(false);
	}

	user_interface_back_out_from_channel(this->get_parent_channel(), this->get_parent_render_window());
}


//
// c_screen_vsync_menu class starts here
// 


c_screen_vsync_menu::c_screen_vsync_menu(e_user_interface_channel_type channel_type, e_user_interface_render_window window_index, uint16 user_flags) :
	c_screen_with_menu(k_vsync_menu_screen_id, channel_type, window_index, user_flags, &m_vsync_edit_list),
	m_vsync_edit_list(user_flags)
{
}

void c_screen_vsync_menu::initialize(c_screen_parameters* parameters)
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
		header->set_text(k_vsync_header_string[language]);
	}

	if (subheader)
	{
		subheader->set_text(k_vsync_description_string[language]);
	}
	return;
}

void c_screen_vsync_menu::post_initialize()
{
	c_screen_with_menu::post_initialize();
	m_vsync_edit_list.set_focused_item_index(H2Config_use_vsync ? _item_on : _item_off);
}

const void* c_screen_vsync_menu::load_proc() const
{
	return &c_screen_vsync_menu::load;
}

void* c_screen_vsync_menu::load(c_screen_parameters* parameters)
{
	c_screen_vsync_menu* screen;

	void* pool = ui_pool_allocate_space(sizeof(c_screen_vsync_menu), 0);
	if (pool)
	{
		screen = new (pool) c_screen_vsync_menu(
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