#include "stdafx.h"
#include "screen_variant_options.h"

#include "screen_variant_parameter_setting.h"
#include "cache/cache_files.h"
#include "interface/user_interface_bitmap_block.h"
#include "interface/user_interface_memory.h"
#include "networking/logic/life_cycle_manager.h"
#include "networking/session/network_session.h"
#include "text/text_group.h"


/* ------------------------ */
/* -c_variant_options_list- */
/* ------------------------ */

int32 c_variant_options_list::link_item_widgets()
{
	if (this->m_variant_setting_category_type == _variant_setting_category_type_none)
		this->m_variant_setting_category_type = g_previous_variant_setting_category;
	else
		g_previous_variant_setting_category = this->m_variant_setting_category_type;


	switch (this->m_variant_setting_category_type)
	{
	case _variant_setting_category_type_game_head_hunter:
	{
		this->link_item_widgets_headhunter();
		break;
	}
	case _variant_setting_category_type_cartographer_settings:
	{
		this->link_item_widgets_cartographer();
		break;
	}
	default:
	{
		this->link_item_widgets_default();
		break;
	}
	}

	return 1;
}

c_list_item_widget* c_variant_options_list::get_list_items()
{
	return m_items;
}

int32 c_variant_options_list::get_list_items_count()
{
	return k_variant_options_list_item_count;
}

void c_variant_options_list::update_list_items(c_list_item_widget* item, int32 skin_index)
{
	if (item->get_last_data_index() != NONE)
	{
		s_variant_options_list_item* list_item = (s_variant_options_list_item*)datum_get(this->m_list_data, item->get_last_data_index());

		if (list_item)
		{
			if (this->m_variant_setting_category_type == _variant_setting_category_type_cartographer_settings)
			{
				this->update_list_items_cartographer(item, skin_index);
			}
			else if (list_item->sily_definition && list_item->sily_definition->string_list.index != NONE)
			{
				this->update_list_items_default(item, skin_index);
			}
			else
			{
				this->update_list_items_custom(item, skin_index);
			}
		}
	}
}

void c_variant_options_list::update_list_items_cartographer(c_list_item_widget* item, int32 skin_index) const
{
	s_variant_options_list_item* list_item = (s_variant_options_list_item*)datum_get(this->m_list_data, item->get_last_data_index());

	c_text_widget* title_text = item->try_find_text_widget(0);
	c_text_widget* value_text = item->try_find_text_widget(1);

	s_game_variant* variant;

	if (!this->m_is_quick_options)
		variant = user_interface_get_variant();
	else
	{
		c_network_session* network_session;
		if (network_life_cycle_in_squad_session(&network_session))
			variant = &network_session->m_session_parameters.game_variant;
		else
			variant = nullptr;
	}

	wchar_t temp_string[512]{};

	multiplayer_variant_settings_interface_get_custom_variant_parameter_title(nullptr, list_item->parameter_type, temp_string);

	title_text->set_text(temp_string);

	int32 setting_value = multiplayer_variant_settings_interface_get_variant_parameter_value(variant, list_item->parameter_type);

	multiplayer_variant_settings_interface_get_custom_variant_parameter_label(nullptr, list_item->parameter_type, setting_value, temp_string);

	value_text->set_text(temp_string);
}

void c_variant_options_list::update_list_items_custom(c_list_item_widget* item, int32 skin_index) const
{
	s_variant_options_list_item* list_item = (s_variant_options_list_item*)datum_get(this->m_list_data, item->get_last_data_index());

	c_text_widget* title_text = item->try_find_text_widget(0);
	c_text_widget* value_text = item->try_find_text_widget(1);

	s_game_variant* variant;

	if (!this->m_is_quick_options)
		variant = user_interface_get_variant();
	else
	{
		c_network_session* network_session;
		if (network_life_cycle_in_squad_session(&network_session))
			variant = &network_session->m_session_parameters.game_variant;
		else
			variant = nullptr;
	}
	if (variant)
	{
		wchar_t temp_string[512]{};

		if (title_text)
		{
			multiplayer_variant_settings_interface_get_custom_variant_parameter_title(variant, DATUM_INDEX_TO_ABSOLUTE_INDEX(item->get_last_data_index()), temp_string);
			title_text->set_text(temp_string);
		}
		if (value_text)
		{
			int32 variant_setting_value = multiplayer_variant_settings_interface_get_variant_parameter_value(variant, g_multiplayer_variant_interface_headhunter_parameter_types[DATUM_INDEX_TO_ABSOLUTE_INDEX(item->get_last_data_index())]);

			multiplayer_variant_settings_interface_get_custom_variant_parameter_label(variant, g_multiplayer_variant_interface_headhunter_parameter_types[DATUM_INDEX_TO_ABSOLUTE_INDEX(item->get_last_data_index())], variant_setting_value, temp_string);

			value_text->set_text(temp_string);
		}
	}
}

void c_variant_options_list::update_list_items_default(c_list_item_widget* item, int32 skin_index) const
{
	s_variant_options_list_item* list_item = (s_variant_options_list_item*)datum_get(this->m_list_data, item->get_last_data_index());

	c_text_widget* title_text = item->try_find_text_widget(0);
	c_text_widget* value_text = item->try_find_text_widget(1);

	c_maximum_interface_text temp_string;
	if (title_text)
	{
		temp_string.clear();
		string_list_get_normal_string(list_item->sily_definition->string_list.index, list_item->sily_definition->title_text, &temp_string);
		title_text->set_text(temp_string.get_buffer());
	}
	if (value_text)
	{
		s_game_variant* variant;

		if (!this->m_is_quick_options)
			variant = user_interface_get_variant();
		else
		{
			c_network_session* network_session;
			if (network_life_cycle_in_squad_session(&network_session))
				variant = &network_session->m_session_parameters.game_variant;
			else
				variant = nullptr;
		}
		if (variant)
		{
			int32 variant_setting_value = multiplayer_variant_settings_interface_get_variant_parameter_value(variant, list_item->sily_definition->parameter);
			s_text_value_pair_reference_new* variant_setting_label = multiplayer_variant_settings_interface_get_variant_parameter_label(list_item->sily_definition, variant_setting_value);
			temp_string.clear();
			if (variant_setting_label)
			{
				string_list_get_normal_string(list_item->sily_definition->string_list.index, variant_setting_label->label_string, &temp_string);
				value_text->set_text(temp_string.get_buffer());
			}
		}
	}
}

void c_variant_options_list::link_item_widgets_cartographer()
{
	this->m_list_data = ui_list_data_new(k_variant_options_list_name, k_variant_setting_parameter_type_cartographer_count, sizeof(s_variant_options_list_item));
	data_make_valid(this->m_list_data);

	if (this->m_list_data->maximum_count > 0)
	{
		for (int32 i = 0; i < this->m_list_data->maximum_count; ++i)
		{
			s_variant_options_list_item* list_item = (s_variant_options_list_item*)datum_get(this->m_list_data, datum_new(this->m_list_data));

			list_item->sily_definition = nullptr;
			list_item->parameter_type = (e_variant_setting_parameter_type)(i + k_variant_setting_parameter_type_base_count + 1);
		}
	}

	this->linker_type2.link(&this->m_slot);
	c_list_widget::link_item_widgets();
}

void c_variant_options_list::link_item_widgets_default()
{
	s_variant_setting_edit_reference* variant_reference = multiplayer_variant_settings_interface_get_category_reference(this->m_variant_setting_category_type);

	if (variant_reference && variant_reference->options.count > 0)
	{
		this->m_list_data = ui_list_data_new(k_variant_options_list_name, variant_reference->options.count, sizeof(s_variant_options_list_item));
		data_make_valid(this->m_list_data);
		if (this->m_list_data->maximum_count > 0)
		{
			for (int32 i = 0; i < this->m_list_data->maximum_count; ++i)
			{
				tag_reference* sily_reference = variant_reference->options[i];
				s_variant_options_list_item* list_item = (s_variant_options_list_item*)datum_get(this->m_list_data, datum_new(this->m_list_data));

				if (sily_reference->index != NONE)
				{
					list_item->sily_definition = (s_text_value_pair_definition*)tag_get_fast(sily_reference->index);
					list_item->parameter_type = list_item->sily_definition->parameter;
				}
				else
				{
					list_item->sily_definition = nullptr;
					list_item->parameter_type = _variant_setting_parameter_type_invalid;
				}
			}
		}
		this->linker_type2.link(&this->m_slot);
		c_list_widget::link_item_widgets();
	}
	else
	{
		this->m_list_data = ui_list_data_new(k_variant_options_list_empty_name, 1, sizeof(s_variant_options_list_item));
		data_make_valid(this->m_list_data);

		s_variant_options_list_item* list_item = (s_variant_options_list_item*)datum_get(this->m_list_data, datum_new(this->m_list_data));

		list_item->sily_definition = nullptr;
		list_item->parameter_type = _variant_setting_parameter_type_invalid;

		this->linker_type2.link(&this->m_slot);
		c_list_widget::link_item_widgets();
	}
}

void c_variant_options_list::link_item_widgets_headhunter()
{
	s_variant_setting_edit_reference* variant_reference = multiplayer_variant_settings_interface_get_category_reference(this->m_variant_setting_category_type);

	this->m_list_data = ui_list_data_new(k_variant_options_list_name, k_multiplayer_variant_headhunter_parameter_count, sizeof(s_variant_options_list_item));
	data_make_valid(this->m_list_data);
	if (this->m_list_data->maximum_count > 0)
	{
		for (int32 i = 0; i < this->m_list_data->maximum_count; ++i)
		{
			s_variant_options_list_item* list_item = (s_variant_options_list_item*)datum_get(this->m_list_data, datum_new(this->m_list_data));
			list_item->parameter_type = g_multiplayer_variant_interface_headhunter_parameter_types[i];

			for (int32 k = 0; k < variant_reference->options.count; ++k)
			{
				s_text_value_pair_definition* sily_definition = (s_text_value_pair_definition*)tag_get_fast(variant_reference->options[k]->index);
				if (sily_definition->parameter == g_multiplayer_variant_interface_headhunter_parameter_types[i])
				{
					list_item->sily_definition = sily_definition;
					break;
				}
			}
		}
	}
	this->linker_type2.link(&this->m_slot);
	c_list_widget::link_item_widgets();
}

void c_variant_options_list::set_quick_options(bool state)
{
	this->m_is_quick_options = state;
}

void c_variant_options_list::set_variant_setting_category_type(e_variant_setting_category_type category)
{
	this->m_variant_setting_category_type = category;
}


void c_variant_options_list::handle_item_pressed_event(s_event_record** event, datum* pitem_index)
{
	//INVOKE_TYPE(0x2585EC, 0, void(__thiscall*)(c_variant_options_list*, s_event_record**, datum*), this, event, pitem_index);

	s_variant_options_list_item* item = (s_variant_options_list_item*)datum_get(this->m_list_data, *pitem_index);
	if (item)
	{
		if (item->parameter_type != _variant_setting_parameter_type_invalid)
		{
			c_screen_variant_parameter_setting::new_instance(
				item->parameter_type,
				_user_interface_channel_type_gameshell_dialog_history,
				_window_4,
				this->m_controllers_mask,
				this->m_is_quick_options
			);
		}
	}
}

c_variant_options_list::c_variant_options_list(uint16 user_flags) :
	c_list_widget(user_flags),
	m_variant_setting_category_type(_variant_setting_category_type_none),
	m_slot(this, &c_variant_options_list::handle_item_pressed_event),
	m_variant_setting_reference(nullptr),
	m_is_quick_options(false)
{
}


/* -------------------------- */
/* -c_screen_variant_options- */
/* -------------------------- */

void c_screen_variant_options::update()
{
	c_network_session* network_session;
	if (network_life_cycle_in_squad_session(&network_session))
	{
		s_game_variant* variant = &network_session->m_session_parameters.game_variant;
		c_bitmap_widget* bitmap_widget = this->try_find_bitmap_widget(1);
		if (bitmap_widget)
			user_interface_set_bitmap_from_variant(variant, bitmap_widget);
	}
	c_user_interface_widget::update();
}

const void* c_screen_variant_options::load_proc() const
{
	if (this->m_list.m_is_quick_options)
		return &c_screen_variant_options::load_quick_options;
	else
		return &c_screen_variant_options::load_editor;
}

void c_screen_variant_options::set_variant_setting_category_type(e_variant_setting_category_type category)
{
	this->m_list.set_variant_setting_category_type(category);
}

void* c_screen_variant_options::load_editor(s_screen_parameters* parameters)
{
	c_screen_variant_options* screen;

	void* pool = ui_pool_allocate_space(sizeof(c_screen_variant_options), 1);

	if (pool)
	{
		screen = new (pool) c_screen_variant_options(
			parameters->m_channel_type,
			parameters->m_window_index,
			parameters->m_user_flags,
			_screen_variant_editing_format
		);

		screen->m_allocated = true;
		screen->m_list.set_quick_options(false);

		user_interface_register_screen_to_channel(screen, parameters);
	}
	else
	{
		return nullptr;
	}

	return screen;
}

void* c_screen_variant_options::load_quick_options(s_screen_parameters* parameters)
{
	c_screen_variant_options* screen;

	void* pool = ui_pool_allocate_space(sizeof(c_screen_variant_options), 1);

	if (pool)
	{
		screen = new (pool) c_screen_variant_options(
			parameters->m_channel_type,
			parameters->m_window_index,
			parameters->m_user_flags,
			_screen_variant_quick_options_format
		);

		screen->m_allocated = true;
		screen->m_list.set_quick_options(true);

		user_interface_register_screen_to_channel(screen, parameters);
	}
	else
	{
		return nullptr;
	}

	return screen;
}

void c_screen_variant_options::new_instance(e_variant_setting_category_type category,
	e_user_interface_channel_type channel, e_user_interface_render_window window, int16 user_flags)
{
	proc_ui_screen_load_cb_t load_func = c_screen_variant_options::load_editor;
	for (e_variant_setting_category_type g_variant_setting_category_type_quick_option : g_variant_setting_category_type_quick_options)
	{
		if (category == g_variant_setting_category_type_quick_option)
			load_func = c_screen_variant_options::load_quick_options;
	}

	s_screen_parameters params;
	params.m_flags = 0;
	params.m_window_index = window;
	params.m_context = NULL;
	params.m_user_flags = user_flags;
	params.m_channel_type = channel;
	params.m_screen_state.field_0 = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_order = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_index = 0xFFFFFFFF;
	params.m_load_function = load_func;

	((c_screen_variant_options*)params.ui_screen_load_proc_exec())->set_variant_setting_category_type(category);
}

c_screen_variant_options::c_screen_variant_options(e_user_interface_channel_type ui_channel, e_user_interface_render_window window_index, uint16 user_flags, e_user_interface_screen_id screen_id) :
	c_screen_with_menu(screen_id, ui_channel, window_index, user_flags, &m_list),
	m_list(user_flags)
{
}