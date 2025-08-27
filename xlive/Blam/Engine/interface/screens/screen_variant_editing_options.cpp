#include "stdafx.h"
#include "screen_variant_editing_options.h"

#include "screen_game_engine_category.h"
#include "screen_variant_options.h"
#include "cache/cache_files.h"
#include "interface/multiplayer_variant_settings_interface_definition.h"
#include "interface/user_interface_controller.h"
#include "interface/user_interface_memory.h"
#include "interface/user_interface_screen_widget_definition.h"
#include "tag_files/tag_loader/tag_injection.h"

/* typedefs */
proc_ui_screen_load_cb_t p_c_screen_variant_options_new;



/* -------------------------------- */
/* -c_variant_editing_options_list- */
/* -------------------------------- */

void c_variant_editing_options_list::handle_item_pressed_event(s_event_record** pevent, datum* pitem_index)
{
	s_list_item_datum* item = (s_list_item_datum*)datum_try_and_get(this->m_list_data, *pitem_index);
	if (item)
	{
		s_game_variant* ui_variant = user_interface_get_variant();

		e_variant_setting_category_type menu_type;
		switch (item->item_id)
		{
		case _variant_editing_options_item_match:
			menu_type = g_variant_setting_category_type_match[ui_variant->variant_game_engine_index];
			break;
		case _variant_editing_options_item_player:
			menu_type = _variant_setting_category_type_players;
			break;
		case _variant_editing_options_item_team:
			menu_type = g_variant_setting_category_type_team[ui_variant->variant_game_engine_index];
			break;
		case _variant_editing_options_item_game_type:
			menu_type = g_variant_setting_category_type_game_type[ui_variant->variant_game_engine_index];
			break;
		case _variant_editing_options_item_vehicle:
			menu_type = _variant_setting_category_type_vehicles;
			break;
		case _variant_editing_options_item_equipment:
			menu_type = _variant_setting_category_type_equipment;
			break;
		case _variant_editing_options_item_cartographer:
			menu_type = _variant_setting_category_type_cartographer_settings;
			break;
		default:
			return;
		}
		c_screen_variant_options::new_instance(menu_type, _user_interface_channel_type_gameshell_screen, _window_4, 1 << (*pevent)->controller);
	}
}

c_list_item_widget* c_variant_editing_options_list::get_list_items()
{
	return this->m_items;
}

int32 c_variant_editing_options_list::get_list_items_count()
{
	return k_variant_editing_options_item_count;
}

void c_variant_editing_options_list::update_list_items(c_list_item_widget* item, int32 skin_index)
{
	const static s_item_text_mapping items_map[k_variant_editing_options_item_count] =
	{
		{_variant_editing_options_item_match, _string_id_match_options},
		{_variant_editing_options_item_player, _string_id_player_options},
		{_variant_editing_options_item_team, _string_id_team_options},
		{_variant_editing_options_item_game_type, _string_id_gametype_settings},
		{_variant_editing_options_item_vehicle, _string_id_vehicle_options},
		{_variant_editing_options_item_equipment, _string_id_equipment_options}
	};

	const static wchar_t* head_hunter_strings[k_language_count]
	{
		L"Headhunter Options",
		L"ヘッドハンターのオプション",
		L"Headhunter-Optionen",
		L"Options du chasseur de têtes",
		L"Opciones de cazatalentos",
		L"Opzioni del cacciatore di teste",
		L"헤드헌터 옵션",
		L"猎头选项",
		L"Opções do caça-talentos"
	};

	c_text_widget* item_text = item->try_find_text_widget(0);
	if (item_text)
	{
		s_list_item_datum* item_datum = (s_list_item_datum*)datum_try_and_get(this->m_list_data, item->get_last_data_index());
		switch ((e_screen_game_engine_items)item_datum->item_id)
		{
		case _variant_editing_options_item_game_type:
		{
			//s_game_variant* ui_variant = user_interface_get_variant();
			//// TODO: REVERSE STRING_ID PARSING TO ADD THE ABILITY TO ADD NEW SLUGS
			//// SO WE DO NOT HAVE TO DO THIS
			//if(ui_variant->variant_game_engine_index == _game_engine_type_headhunter)
			//{
			//	const e_language language = get_current_language();
			//	item_text->set_text(head_hunter_strings[language]);
			//}
			//else
			item_text->set_text_from_string_id(items_map[item_datum->item_id].item_text);
			break;
		}
		case _variant_editing_options_item_cartographer:
		{
			item_text->set_text(L"Extra settings");
			break;
		}
		default:
			item_text->set_text_from_string_id(items_map[item_datum->item_id].item_text);
			break;
		}
	}
}

c_variant_editing_options_list::c_variant_editing_options_list(uint16 user_flags) :
	c_list_widget(user_flags),
	m_slot(this, &c_variant_editing_options_list::handle_item_pressed_event)
{
	this->m_list_data = ui_list_data_new(k_variant_editing_options_list_name, k_variant_editing_options_item_count, sizeof(s_list_item_datum));
	data_make_valid(this->m_list_data);
	for (int32 i = 0; i < this->m_list_data->maximum_count; ++i)
	{
		((s_list_item_datum*)datum_get(this->m_list_data, datum_new(this->m_list_data)))->item_id = (int16)i;
	}

	this->linker_type2.link(&this->m_slot);
}


/* ---------------------------------- */
/* -c_screen_variant_editing_options- */
/* ---------------------------------- */


void* c_screen_variant_editing_options::load_editor(s_screen_parameters* parameters)
{
	c_screen_variant_editing_options* screen;

	void* pool = ui_pool_allocate_space(sizeof(c_screen_variant_editing_options), 1);

	if (pool)
	{
		screen = new (pool) c_screen_variant_editing_options(
			parameters->m_channel_type,
			parameters->m_window_index,
			parameters->m_user_flags,
			_screen_variant_editing_options_menu
		);

		screen->m_allocated = true;

		user_interface_register_screen_to_channel(screen, parameters);
	}
	else
	{
		return nullptr;
	}

	return screen;
}

const void* c_screen_variant_editing_options::load_proc() const
{
	return &c_screen_variant_editing_options::load_editor;
}

void c_screen_variant_editing_options::apply_patches_on_map_load()
{
	constexpr char* tag_path = "ui\\screens\\game_shell\\settings_screen\\variant_settings\\variant_editing_options";
	datum variant_editing_options_index = tag_loaded(_tag_group_user_interface_screen_widget_definition, tag_path);

	if (variant_editing_options_index == NONE)
	{
		error(_error_immediate, "screen variant editing options tag not found");
		return;
	}

	s_user_interface_screen_widget_definition* screen_definition = (s_user_interface_screen_widget_definition*)tag_get_fast(variant_editing_options_index);

	if (screen_definition)
	{
		s_window_pane_reference* match_pane = screen_definition->panes[0];

		s_window_pane_reference* cartographer_pane = (s_window_pane_reference*)tag_injection_extend_block(&screen_definition->panes, sizeof(s_window_pane_reference), 1);
		csmemcpy(cartographer_pane, match_pane, sizeof(s_window_pane_reference));

		uint32 out_data_offset = 0;

		if (match_pane->list_block.count)
		{
			s_list_reference* cartographer_list_references = (s_list_reference*)tag_injection_reserve_cache_memory(sizeof(s_list_reference) * match_pane->list_block.count, &out_data_offset);
			cartographer_pane->list_block.data = out_data_offset;
			csmemcpy(cartographer_list_references, match_pane->list_block[0], sizeof(s_list_reference) * match_pane->list_block.count);
		}

		if (match_pane->text_blocks.count)
		{
			s_text_block_reference* cartographer_text_references = (s_text_block_reference*)tag_injection_reserve_cache_memory(sizeof(s_text_block_reference) * match_pane->text_blocks.count, &out_data_offset);
			cartographer_pane->text_blocks.data = out_data_offset;
			csmemcpy(cartographer_text_references, match_pane->text_blocks[0], sizeof(s_text_block_reference) * match_pane->text_blocks.count);
		}

		if (match_pane->bitmap_blocks.count)
		{
			s_bitmap_block_reference* cartographer_bitmap_references = (s_bitmap_block_reference*)tag_injection_reserve_cache_memory(sizeof(s_bitmap_block_reference) * match_pane->bitmap_blocks.count, &out_data_offset);
			cartographer_pane->bitmap_blocks.data = out_data_offset;
			csmemcpy(cartographer_bitmap_references, match_pane->bitmap_blocks[0], sizeof(s_bitmap_block_reference) * match_pane->bitmap_blocks.count);
		}

		cartographer_pane->text_blocks[0]->string = _string_id_filter_custom_help;

		cartographer_pane->bitmap_blocks[4]->initial_sprite_frame = 3;
	}
}

void c_screen_variant_editing_options::apply_patches()
{
	DETOUR_ATTACH(p_c_screen_variant_options_new, Memory::GetAddress<proc_ui_screen_load_cb_t>(0x23CB1A), c_screen_variant_editing_options::load_editor);
}

c_screen_variant_editing_options::c_screen_variant_editing_options(e_user_interface_channel_type ui_channel, e_user_interface_render_window window_index, uint16 user_flags, e_user_interface_screen_id screen_id) :
	c_screen_with_menu(screen_id, ui_channel, window_index, user_flags, &m_list),
	m_list(user_flags)
{

}