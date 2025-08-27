#include "stdafx.h"
#include "screen_variant_parameter_setting.h"

#include "interface/user_interface_memory.h"
#include "interface/user_interface_networking.h"
#include "networking/logic/life_cycle_manager.h"
#include "networking/session/network_session.h"
#include "text/text_group.h"


/* ---------------------------------- */
/* -c_variant_parameter_setting_list- */
/* ---------------------------------- */

void c_variant_parameter_setting_list::handle_item_pressed_event(s_event_record** event, datum* pitem_index)
{
	s_variant_parameter_setting_list_item* list_item = (s_variant_parameter_setting_list_item*)datum_get(this->m_list_data, *pitem_index);

	if (list_item)
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
			multiplayer_variant_settings_interface_set_variant_parameter_value(variant, this->m_variant_setting_parameter_type, list_item->value);
			if (this->m_is_quick_options)
			{
				user_interface_game_settings_set_game_variant(variant);
				if (IN_RANGE(variant->variant_game_engine_index, _game_engine_type_ctf, k_game_engine_playable_types))
				{
					user_interface_back_out_from_channel(this->get_parent_screen()->get_channel(), this->get_parent_screen()->get_render_window());
				}
			}
		}
	}
	this->get_parent_screen()->start_widget_animation(3);
}

int32 c_variant_parameter_setting_list::link_item_widgets()
{
	if (this->m_variant_setting_parameter_type == _variant_setting_parameter_type_invalid)
		this->m_variant_setting_parameter_type = g_previous_variant_setting_parameter;
	else
		g_previous_variant_setting_parameter = this->m_variant_setting_parameter_type;

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

	if (!variant)
		return 1;

	if (!multiplayer_variant_settings_interface_parameter_is_custom(variant, this->m_variant_setting_parameter_type))
	{
		this->link_item_widgets_default();
	}
	else if (IN_RANGE(this->get_variant_parameter_type(), k_variant_setting_parameter_type_base_count, k_variant_setting_parameter_type_base_count + k_variant_setting_parameter_type_cartographer_count))
	{
		this->link_item_widgets_cartographer();
	}
	else
	{
		this->link_item_widgets_custom(variant);
	}

	return 1;
}

void c_variant_parameter_setting_list::link_item_widgets_default()
{
	this->m_sily_definition = multiplayer_variant_settings_interface_get_text_value_pair_for_parameter(this->m_variant_setting_parameter_type);

	if (this->m_sily_definition && this->m_sily_definition->text_value_pairs.count)
	{
		this->m_list_data = ui_list_data_new(k_variant_parameter_setting_list_name, this->m_sily_definition->text_value_pairs.count, sizeof(s_variant_parameter_setting_list_item));
		data_make_valid(this->m_list_data);

		if (this->m_list_data->maximum_count)
		{
			for (int32 i = 0; i < this->m_list_data->maximum_count; ++i)
			{
				s_variant_parameter_setting_list_item* list_item = (s_variant_parameter_setting_list_item*)datum_get(this->m_list_data, datum_new(this->m_list_data));
				list_item->text_value_pair_reference = this->m_sily_definition->text_value_pairs[i];
				list_item->value = list_item->text_value_pair_reference->value;
			}
		}

		this->linker_type2.link(&this->m_slot);
		c_list_widget::link_item_widgets();
	}
	else
	{
		this->m_list_data = ui_list_data_new(k_variant_parameter_setting_list_empty_name, 1, sizeof(s_variant_parameter_setting_list_item));
		data_make_valid(this->m_list_data);

		s_variant_parameter_setting_list_item* list_item = (s_variant_parameter_setting_list_item*)datum_get(this->m_list_data, datum_new(this->m_list_data));
		list_item->text_value_pair_reference = nullptr;
		list_item->value = 0;

		this->linker_type2.link(&this->m_slot);
		c_list_widget::link_item_widgets();
	}
}

void c_variant_parameter_setting_list::link_item_widgets_custom(s_game_variant* variant)
{
	int32 item_count = multiplayer_variant_settings_interface_get_custom_variant_parameter_value_count(variant, this->m_variant_setting_parameter_type);
	if (item_count)
	{
		this->m_list_data = ui_list_data_new(k_variant_parameter_setting_list_name, item_count, sizeof(s_variant_parameter_setting_list_item));
		data_make_valid(this->m_list_data);

		if (this->m_list_data->maximum_count)
		{
			for (int32 i = 0; i < this->m_list_data->maximum_count; ++i)
			{
				s_variant_parameter_setting_list_item* list_item = (s_variant_parameter_setting_list_item*)datum_get(this->m_list_data, datum_new(this->m_list_data));
				list_item->value = i;
			}
		}

		this->linker_type2.link(&this->m_slot);
		c_list_widget::link_item_widgets();
	}
	else
	{
		this->m_list_data = ui_list_data_new(k_variant_parameter_setting_list_empty_name, 1, sizeof(s_variant_parameter_setting_list_item));
		data_make_valid(this->m_list_data);

		s_variant_parameter_setting_list_item* list_item = (s_variant_parameter_setting_list_item*)datum_get(this->m_list_data, datum_new(this->m_list_data));
		list_item->text_value_pair_reference = nullptr;

		this->linker_type2.link(&this->m_slot);
		c_list_widget::link_item_widgets();
	}
}

void c_variant_parameter_setting_list::link_item_widgets_cartographer()
{
	int32 item_count = multiplayer_variant_settings_interface_get_custom_variant_parameter_value_count(nullptr, this->m_variant_setting_parameter_type);

	if (item_count)
	{
		this->m_list_data = ui_list_data_new(k_variant_parameter_setting_list_name, item_count, sizeof(s_variant_parameter_setting_list_item));
		data_make_valid(this->m_list_data);

		if (this->m_list_data->maximum_count)
		{
			for (int32 i = 0; i < this->m_list_data->maximum_count; ++i)
			{
				s_variant_parameter_setting_list_item* list_item = (s_variant_parameter_setting_list_item*)datum_get(this->m_list_data, datum_new(this->m_list_data));
				list_item->value = i;
			}
		}

		this->linker_type2.link(&this->m_slot);
		c_list_widget::link_item_widgets();
	}
	else
	{
		this->m_list_data = ui_list_data_new(k_variant_parameter_setting_list_empty_name, 1, sizeof(s_variant_parameter_setting_list_item));
		data_make_valid(this->m_list_data);

		s_variant_parameter_setting_list_item* list_item = (s_variant_parameter_setting_list_item*)datum_get(this->m_list_data, datum_new(this->m_list_data));
		list_item->text_value_pair_reference = nullptr;

		this->linker_type2.link(&this->m_slot);
		c_list_widget::link_item_widgets();
	}
}

c_list_item_widget* c_variant_parameter_setting_list::get_list_items()
{
	return this->m_items;
}

int32 c_variant_parameter_setting_list::get_list_items_count()
{
	return k_variant_parameter_setting_list_count;
}

void c_variant_parameter_setting_list::update_list_items(c_list_item_widget* item, int32 skin_index)
{
	if (item->get_last_data_index() != NONE)
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

		if (!variant)
			return;

		c_maximum_interface_text temp_string;


		s_variant_parameter_setting_list_item* list_item = (s_variant_parameter_setting_list_item*)datum_get(this->m_list_data, item->get_last_data_index());
		c_text_widget* text_widget = item->try_find_text_widget(0);

		if (!multiplayer_variant_settings_interface_parameter_is_custom(variant, this->m_variant_setting_parameter_type))
		{
			if (this->m_sily_definition && this->m_sily_definition->string_list.index != NONE && text_widget && list_item->text_value_pair_reference)
			{
				string_list_get_normal_string(this->m_sily_definition->string_list.index, list_item->text_value_pair_reference->label_string, &temp_string);

				text_widget->set_text(temp_string.get_buffer());
			}
		}
		else if (IN_RANGE(this->get_variant_parameter_type(), k_variant_setting_parameter_type_base_count, k_variant_setting_parameter_type_base_count + k_variant_setting_parameter_type_cartographer_count))
		{
			multiplayer_variant_settings_interface_get_custom_variant_parameter_label(nullptr, this->m_variant_setting_parameter_type, DATUM_INDEX_TO_ABSOLUTE_INDEX(item->get_last_data_index()), temp_string.get_buffer());

			text_widget->set_text(temp_string.get_buffer());
		}
		else
		{
			multiplayer_variant_settings_interface_get_custom_variant_parameter_label(variant, this->m_variant_setting_parameter_type, DATUM_INDEX_TO_ABSOLUTE_INDEX(item->get_last_data_index()), temp_string.get_buffer());

			text_widget->set_text(temp_string.get_buffer());
		}
	}
}

void c_variant_parameter_setting_list::update_selected_list_item()
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
		int32 parameter_value = multiplayer_variant_settings_interface_get_variant_parameter_value(variant, this->m_variant_setting_parameter_type);

		data_iterator it;
		iterator_new(&it, this->m_list_data);

		s_variant_parameter_setting_list_item* current_item = (s_variant_parameter_setting_list_item*)iterator_next(&it);
		while (current_item)
		{
			if (current_item->text_value_pair_reference)
			{
				if (current_item->text_value_pair_reference->value == parameter_value)
				{
					this->set_focused_item_index(it.index);
					break;
				}
			}

			current_item = (s_variant_parameter_setting_list_item*)iterator_next(&it);
		}
	}
}

void c_variant_parameter_setting_list::set_variant_parameter_type(e_variant_setting_parameter_type type)
{
	this->m_variant_setting_parameter_type = type;
}

e_variant_setting_parameter_type c_variant_parameter_setting_list::get_variant_parameter_type() const
{
	return this->m_variant_setting_parameter_type;
}

void c_variant_parameter_setting_list::set_is_quick_options(bool state)
{
	this->m_is_quick_options = state;
}

bool c_variant_parameter_setting_list::get_is_quick_options() const
{
	return this->m_is_quick_options;
}

c_variant_parameter_setting_list::c_variant_parameter_setting_list(uint16 user_flags) :
	c_list_widget(user_flags),
	m_slot(this, &c_variant_parameter_setting_list::handle_item_pressed_event),
	m_variant_setting_parameter_type(_variant_setting_parameter_type_invalid),
	m_sily_definition(nullptr),
	m_is_quick_options(false)
{
}


/* ------------------------------------ */
/* -c_screen_variant_parameter_setting- */
/* ------------------------------------ */

void c_screen_variant_parameter_setting::post_initialize()
{

	s_game_variant* variant;

	if (!this->m_list.get_is_quick_options())
		variant = user_interface_get_variant();
	else
	{
		c_network_session* network_session;
		if (network_life_cycle_in_squad_session(&network_session))
			variant = &network_session->m_session_parameters.game_variant;
		else
			variant = nullptr;
	}

	if (!multiplayer_variant_settings_interface_parameter_is_custom(variant, this->m_list.get_variant_parameter_type()))
	{
		this->post_initialize_default();
	}
	else if (IN_RANGE(this->m_list.get_variant_parameter_type(), k_variant_setting_parameter_type_base_count, k_variant_setting_parameter_type_base_count + k_variant_setting_parameter_type_cartographer_count))
	{
		this->post_initialize_cartographer();
	}
	else
	{
		this->post_initialize_custom(variant);
	}

	this->m_list.update_selected_list_item();

	c_screen_with_menu::post_initialize();
}

void c_screen_variant_parameter_setting::post_initialize_default()
{
	c_text_widget* header_text = this->try_find_text_widget(0);
	c_text_widget* description_text = this->try_find_text_widget(2);

	s_text_value_pair_definition* value_reference = multiplayer_variant_settings_interface_get_text_value_pair_for_parameter(this->m_list.get_variant_parameter_type());

	c_maximum_interface_text temp_string;

	if (value_reference && value_reference->string_list.index != NONE)
	{
		if (header_text)
		{
			string_list_get_normal_string(value_reference->string_list.index, value_reference->header_text, &temp_string);
			header_text->set_text(temp_string.get_buffer());
		}

		if (description_text)
		{
			string_list_get_normal_string(value_reference->string_list.index, value_reference->description_test, &temp_string);
			description_text->set_text(temp_string.get_buffer());
		}
	}
}

void c_screen_variant_parameter_setting::post_initialize_custom(s_game_variant* variant)
{
	c_text_widget* header_text = this->try_find_text_widget(0);
	c_text_widget* description_text = this->try_find_text_widget(2);

	s_text_value_pair_definition* value_reference = multiplayer_variant_settings_interface_get_text_value_pair_for_parameter(this->m_list.get_variant_parameter_type());

	wchar_t temp_string[512]{};

	if (header_text)
	{
		multiplayer_variant_settings_interface_get_custom_variant_parameter_title(variant, this->m_list.get_variant_parameter_type(), temp_string);
		header_text->set_text(temp_string);
	}

	if (description_text)
	{
		multiplayer_variant_settings_interface_get_custom_variant_parameter_description(variant, this->m_list.get_variant_parameter_type(), temp_string);
		description_text->set_text(temp_string);
	}
}

void c_screen_variant_parameter_setting::post_initialize_cartographer()
{
	c_text_widget* header_text = this->try_find_text_widget(0);
	c_text_widget* description_text = this->try_find_text_widget(2);

	s_text_value_pair_definition* value_reference = multiplayer_variant_settings_interface_get_text_value_pair_for_parameter(this->m_list.get_variant_parameter_type());

	wchar_t temp_string[512]{};

	if (header_text)
	{
		multiplayer_variant_settings_interface_get_custom_variant_parameter_title(nullptr, this->m_list.get_variant_parameter_type(), temp_string);
		header_text->set_text(temp_string);
	}

	if (description_text)
	{
		multiplayer_variant_settings_interface_get_custom_variant_parameter_description(nullptr, this->m_list.get_variant_parameter_type(), temp_string);
		description_text->set_text(temp_string);
	}
}

const void* c_screen_variant_parameter_setting::load_proc() const
{
	if (this->m_list.get_is_quick_options())
		return &c_screen_variant_parameter_setting::load_quick_options;

	return &c_screen_variant_parameter_setting::load_settings;
}


void* c_screen_variant_parameter_setting::load_quick_options(s_screen_parameters* parameters)
{
	c_screen_variant_parameter_setting* screen;

	void* pool = ui_pool_allocate_space(sizeof(c_screen_variant_parameter_setting), 1);

	if (pool)
	{
		screen = new (pool) c_screen_variant_parameter_setting(
			parameters->m_channel_type,
			parameters->m_window_index,
			parameters->m_user_flags,
			_screen_variant_param_setting_format
		);

		screen->m_allocated = true;
		screen->m_list.set_is_quick_options(true);

		user_interface_register_screen_to_channel(screen, parameters);
	}
	else
		return nullptr;

	return screen;
}

void* c_screen_variant_parameter_setting::load_settings(s_screen_parameters* parameters)
{
	c_screen_variant_parameter_setting* screen;

	void* pool = ui_pool_allocate_space(sizeof(c_screen_variant_parameter_setting), 1);

	if (pool)
	{
		screen = new (pool) c_screen_variant_parameter_setting(
			parameters->m_channel_type,
			parameters->m_window_index,
			parameters->m_user_flags,
			_screen_variant_param_setting_format
		);

		screen->m_allocated = true;
		screen->m_list.set_is_quick_options(false);

		user_interface_register_screen_to_channel(screen, parameters);
	}
	else
		return nullptr;

	return screen;
}

void c_screen_variant_parameter_setting::new_instance(e_variant_setting_parameter_type parameter_type,
	e_user_interface_channel_type ui_channel, e_user_interface_render_window ui_window, uint16 user_flags,
	bool is_quick_options)
{
	proc_ui_screen_load_cb_t load_func = c_screen_variant_parameter_setting::load_settings;

	if (is_quick_options)
		load_func = c_screen_variant_parameter_setting::load_quick_options;

	s_screen_parameters params;
	params.m_flags = 0;
	params.m_window_index = ui_window;
	params.m_context = NULL;
	params.m_user_flags = user_flags;
	params.m_channel_type = ui_channel;
	params.m_screen_state.field_0 = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_order = 0xFFFFFFFF;
	params.m_screen_state.m_last_focused_item_index = 0xFFFFFFFF;
	params.m_load_function = load_func;

	((c_screen_variant_parameter_setting*)params.ui_screen_load_proc_exec())->m_list.set_variant_parameter_type(parameter_type);
}

c_screen_variant_parameter_setting::c_screen_variant_parameter_setting(e_user_interface_channel_type ui_channel, e_user_interface_render_window window_index, uint16 user_flags, e_user_interface_screen_id screen_id) :
	c_screen_with_menu(screen_id, ui_channel, window_index, user_flags, &this->m_list),
	m_list(user_flags)
{
}